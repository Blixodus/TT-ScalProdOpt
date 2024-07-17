import ast
import multiprocessing
import os
import re
import sys
import time
import subprocess
import configparser

import cotengra as ctg
from cgreedy import CGreedy

import pandas as pd
from alive_progress import alive_bar

from Scripts.naming import get_dir, get_dir_ratio, get_test_filename, get_result_filename
from Scripts.contraction_list import generate_contraction_list
from Scripts.import_file import import_tensor_train

file_lock = {}
single_file_lock = multiprocessing.Lock()
validate_results = False

# ------------------------------- Utility functions --------------------------------
def run_algorithm_cpp(algorithm, test_filename, tt_dim, delta, ctg_algorithm=None):
    # Prepare command line arguments
    algorithm_arguments = f"main_alg {algorithm}"

    if tt_dim is not None:
        algorithm_arguments += f" tt_dim {tt_dim}"

    if delta is not None:
        algorithm_arguments += f" delta {delta}"

    if ctg_algorithm is not None:
        algorithm_arguments += f" ctg_algorithm {ctg_algorithm}"

    args = f"./build/OptiTenseurs -a \"{algorithm_arguments}\" -f {test_filename}"

    # Run the C++ program and retrieve output of the algorithm
    result = subprocess.run(args=args, capture_output=True, text=True, shell=True)
    
    # Print the stderr output of the algorithm to inform about warning or errors
    if result.stderr:
        print(f"🚨 Algorithm {algorithm} returned warnings and/or errors on {test_filename}.")
        print("Args: ", args)
        print(result.stderr)

    # Parse cost of the contraction and execution time of the algorithm
    cost = 0
    order = ""
    execution_time = 0.0
    for line in result.stdout.split("\n"):
        if line.startswith("Best cost"):
            cost = float(line.split(":")[-1])
        if line.startswith("Best order"):
            order = line.split(":")[-1]
        if line.startswith("Execution time"):
            execution_time = float(line.split(":")[-1][:-1])

    # Return results
    return cost, execution_time, order


def run_algorithm_python(algorithm, test_filename):
    # Import tensor train from file
    inputs, output, sizes_dict, _ = import_tensor_train(test_filename)

    # Initialize the cgreedy object (which is not part of cotengra)
    if algorithm == 'cgreedy':
        algorithm = CGreedy(seed=1, minimize="flops", max_repeats=1024, max_time=1.0, progbar=False, threshold_optimal=12, threads=1)
    elif algorithm == 'hyper-greedy':
        algorithm = ctg.HyperOptimizer(methods=["greedy"], minimize="flops", parallel=False)
    elif algorithm == 'hyper-kahypar':
        algorithm = ctg.HyperOptimizer(methods=["kahypar"], minimize="flops", parallel=False)

    # Run the algorithm and retrieve the output
    start_time = time.time()
    tree = ctg.array_contract_tree(inputs, output, sizes_dict, optimize=algorithm)
    end_time = time.time()

    # Calculate cost and execution time
    cost = tree.contraction_cost()
    order = tree.flat_tree()
    execution_time = end_time - start_time

    # Return results
    return cost, execution_time, order


def run_algorithm_naive(test_filename, tt_dim, dim):
    if tt_dim == 2:
        # Parse naive cost from test file
        with open(test_filename, 'r') as text_file:
            lines = text_file.readlines()

            # Locate the naive cost(s) of contracting the tensor train
            naive_costs = []
            for line in lines:
                if line.startswith('c'):
                    cost = float(re.split(r'\W+', line)[3])
                    naive_costs.append(cost)

            cost = min(naive_costs)

            # Return results
            return cost, 0.0, "naive_order_NA"
    elif tt_dim == 3:
        # Generate naive order and evaluate its cost
        order = []
        for i in range(dim):
            order.append((i, dim + i))
            order.append((i, 2 * dim + i))
            if i < dim - 1:
                order.append((i, dim + (i + 1)))

        # Retrieve cost of naive order
        cost = run_validation_on_test_case(tt_dim, test_filename, order)

        return cost, 0.0, "naive_order_NA"
    else:
        exit(f"Error! Naive algorithm is not implemented for given tensor train dimension {tt_dim}.")


# Wrapper function to run computations for given test using either
# C++ (our algorithms), or Python (cotengra and other algorithms)
def run_algorithm(algorithm, test_filename, tt_dim, dim, delta):
    if algorithm in ['OneSidedOneDim', 'TwoSidedDeltaDim']:
        return run_algorithm_cpp(algorithm, test_filename, tt_dim, delta)
    elif algorithm == 'naive':
        return run_algorithm_naive(test_filename, tt_dim, dim)
    else:
        return run_algorithm_cpp("CotengraWrapper", test_filename, tt_dim, None, algorithm)


def run_validation_on_test_case(tt_dim, test_filename, order):
    # Prepare command line arguments
    args = f"./build/Compute"

    # Prepare input (list of contractions in the order)
    inputs = f"{tt_dim}\n{test_filename}\n{len(order)}\n"
    for contraction in order:
        inputs += f"{contraction[0]} {contraction[1]}\n"
    print(inputs)

    # Run the C++ validator and retrieve the output
    result = subprocess.run(args=args, input=inputs, text=True, shell=True)

    # Parse cost of the contraction and execution time of the algorithm
    cost = 0
    for line in result.stdout.split("\n"):
        if line.startswith("Cost"):
            cost = float(line.split(":")[-1])
            break

    # Return result
    return cost


def run_algorithm_on_test_case(input):
    # Unpack the input arguments
    algorithm, test_filename, result_filename, tt_dim, delta, dimension, instance = input[0]

    # Run the algorith on the test case
    cost, execution_time, order = run_algorithm(algorithm, test_filename, tt_dim, dimension, delta)
    output_str = f"{algorithm};{dimension};{instance};{test_filename};{cost};{execution_time};{order}"

    # Validate the contraction cost for given order if requested
    if validate_results and order != "naive_order_NA":
        contraction_recursive = ast.literal_eval(order)
        contraction_flat, _ = generate_contraction_list(contraction_recursive)
        cost_validation = run_validation_on_test_case(tt_dim, test_filename, contraction_flat)
        result_validation = "Correct"
        if cost_validation != cost:
            result_validation = "Invalid"
            print(f"❌ Validation failed for {algorithm} on {test_filename}. Expected cost: {cost}, validated cost: {cost_validation}")
        output_str += f";{cost_validation};{result_validation}"
    elif validate_results and order == "naive_order_NA":
        output_str += ";0;Correct"

    # Save the results to the result file
    single_file_lock.acquire()
    with open(result_filename, 'a') as result_file:
        result_file.write(output_str + "\n")
    single_file_lock.release()


def generate_arguments_for_test_case(type, algorithm, test_dir_path, result_dir_path, min_size, max_size, step_size, max_size_optimal, tt_dim, delta):
    global file_lock

    # Prepare the lock for the result file
    result_filename = get_result_filename(result_dir_path, algorithm, delta)
    file_lock[result_filename] = multiprocessing.Lock()

    # Prepare header of result file if we start with the first test case
    if not os.path.exists(result_filename):
        with open(result_filename, 'w') as result_file:
            if not validate_results:
                result_file.write(f"Algorithm;Size;Instance;Test_file;Cost;Execution_time;Order\n")
            else:
                result_file.write(f"Algorithm;Size;Instance;Test_file;Cost;Execution_time;Order;Validated_cost;Validation_result\n")

    # Prepare max size for optimal algorithm (which we may not run for large sizes)
    if algorithm == "optimal":
        max_size = min(max_size, max_size_optimal)

    # Prepare arguments for each test case
    arguments = []
    if type != "ratio":
        # Open and parse the file with results to check which test cases have already been computed
        existing_results = pd.read_csv(result_filename, sep=';')
        existing_results.sort_values(by=['Size', 'Instance'], inplace=True)

        # Generate test cases to be computed
        for dimension in range(min_size, max_size + 1):
            for instance in range(1, nb_instances + 1):
                test_filename = get_test_filename(test_dir_path, dimension, instance)
                if existing_results[existing_results['Test_file'] == test_filename].empty or not existing_results[existing_results['Test_file'] == test_filename]['Cost'].any():
                    #print(f"Calculating test case {test_filename} for {algorithm} as it has not been calculated")
                    arguments.append([(algorithm, test_filename, result_filename, tt_dim, delta, dimension, instance)])
                    
    else:
        ratio_list = ast.literal_eval(config['General']['ratio_list'])
        for rank_const, dim_const in ratio_list:
            ratio_test_dir = get_dir_ratio(test_dir_path, rank_const, dim_const)
            ratio_result_dir = get_dir_ratio(result_dir_path, rank_const, dim_const)
            result_filename = get_result_filename(ratio_result_dir, algorithm, delta)

            if not os.path.exists(result_filename):
                with open(result_filename, 'w') as result_file:
                    if not validate_results:
                        result_file.write(f"Algorithm;Size;Instance;Test_file;Cost;Execution_time;Order\n")
                    else:
                        result_file.write(f"Algorithm;Size;Instance;Test_file;Cost;Execution_time;Validated_cost;Validation_result\n")

            # Open and parse the file with results to check which test cases have already been computed
            existing_results = pd.read_csv(result_filename, sep=';')
            existing_results.sort_values(by=['Size', 'Instance'], inplace=True)

            # Generate test cases to be computed
            for dimension in range(min_size, max_size + 1, step_size):
                for instance in range(1, nb_instances + 1):
                    test_filename = get_test_filename(ratio_test_dir, dimension, instance)
                    if existing_results[existing_results['Test_file'] == test_filename].empty or not existing_results[existing_results['Test_file'] == test_filename]['Cost'].any():
                        #print(f"Calculating test case {test_filename} for {algorithm} as it has not been calculated")
                        arguments.append([(algorithm, test_filename, result_filename, tt_dim, delta, dimension, instance)])

            rank_const *= 2
            dim_const //= 2

    return arguments


# ------------------------------- Main function --------------------------------
if __name__ == "__main__":
    # Load configuration file
    if len(sys.argv) > 1:
        config_file = sys.argv[1]
        if not os.path.exists(config_file):
            exit("Error! Provided configuration file does not exist.")
    else:
        exit("Error! No configuration file provided.")

    # Read configuration file
    print(f"Reading configuration file {config_file}")
    config = configparser.ConfigParser()
    config.read(config_file)

    # Prepare test and result directory path
    test_dir = config['Tests']['test_dir']
    result_dir = config['Results']['result_dir']

    # Retrieve test cases parameters
    tt_dims = [int(tt_dim) for tt_dim in config['General']['tt_dims'].split(',')]
    min_size = int(config['Results']['min_size'])
    max_size = int(config['Results']['max_size'])
    step_size = int(config['Results']['step_size'])
    max_size_optimal = int(config['Results']['max_size_optimal'])
    nb_instances = int(config['Tests']['nb_instances'])

    types = config['General']['types'].split(',')
    rank_types = config['General']['rank_types'].split(',')

    # Retrieve list of algorithms to run
    algorithms = config['Algorithms']['algorithms'].split(',')
    deltas = [int(delta) for delta in config['Algorithms']['deltas'].split(',')]

    # Retrieve flag for validation of the results
    validate_results = int(config['Results']['validate_order'])

    # Computation of the test cases
    cores = min(int(config['Results']['max_cores']), multiprocessing.cpu_count())
    pool = multiprocessing.Pool(processes=cores)
    parallel_input = []

    case_no = 1
    for tt_dim in tt_dims:
        # Consider case y=xT for smaller TT dimensions
        y_cases = [False]
        if tt_dim <= 3:
            y_cases = [True, False]

        for y_eq_xT in y_cases:
            for type in types:
                if type == "ratio" and y_eq_xT:
                    continue

                for rank_type in rank_types:
                    # Prepare directory for result files
                    test_dir_path = get_dir(test_dir, tt_dim, y_eq_xT, type, rank_type)
                    result_dir_path = get_dir(result_dir, tt_dim, y_eq_xT, type, rank_type)

                    # Compute results for each algorithm and test file
                    print(f"[Info {case_no}] Preparing test cases: TT-dim={tt_dim}, (y=xT)={y_eq_xT}, type={type}, rank_type={rank_type}")
                    case_no += 1

                    for algorithm in algorithms:
                        if algorithm != "TwoSidedDeltaDim":
                            delta = None
                            parallel_input += generate_arguments_for_test_case(type, algorithm, test_dir_path, result_dir_path, min_size, max_size, step_size, max_size_optimal, tt_dim, None)
                        else:
                            for delta in deltas:
                                parallel_input += generate_arguments_for_test_case(type, algorithm, test_dir_path, result_dir_path, min_size, max_size, step_size, max_size_optimal, tt_dim, delta)


    # Execute tasks in parallel
    print(f"Executing {len(parallel_input)} test cases in parallel using {cores} cores")
    with alive_bar(len(parallel_input)) as bar:
        for i in pool.imap_unordered(run_algorithm_on_test_case, parallel_input):
            bar()
