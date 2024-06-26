import multiprocessing
import os
import re
import sys
import time
import subprocess
import configparser

import cotengra as ctg
from cgreedy import CGreedy

from alive_progress import alive_bar

from Scripts.naming import get_dir, get_test_filename, get_result_filename
from Scripts.import_file import import_tensor_train

file_lock = {}
single_file_lock = multiprocessing.Lock()

# ------------------------------- Utility functions --------------------------------
def run_algorithm_cpp(algorithm, test_filename, tt_dim, delta):
    # Prepare command line arguments
    delta_str = ""
    if delta is not None:
        delta_str = f"delta {delta}"

    tt_dim_str = f"tt_dim {tt_dim}"

    args = f"source ~/.xmake/profile && xmake run -w . OptiTenseurs -a \"main_alg {algorithm} {tt_dim_str} {delta_str}\" -f {test_filename}"

    # Run the C++ program and retrieve output of the algorithm
    result = subprocess.run(args=args, capture_output=True, text=True, shell=True)

    # Parse cost of the contraction and execution time of the algorithm
    cost = 0
    execution_time = 0.0
    for line in result.stdout.split("\n"):
        if line.startswith("Best cost"):
            cost = float(line.split(":")[-1])
        if line.startswith("Execution time"):
            execution_time = float(line.split(":")[-1][:-1])

    # Return results
    return cost, execution_time


def run_algorithm_python(algorithm, test_filename):
    # Import tensor train from file
    inputs, output, sizes_dict, _ = import_tensor_train(test_filename)

    # Initialize the cgreedy object (which is not part of cotengra)
    algorithm_str = algorithm
    if algorithm == 'cgreedy':
        algorithm = CGreedy(seed=1, minimize="flops", max_repeats=1024, max_time=1.0, progbar=False, threshold_optimal=12, threads=1)
    elif algorithm == 'hyper-greedy':
        algorithm = ctg.HyperOptimizer(methods=["greedy"], minimize="flops", parallel=False)

    # Run the algorithm and retrieve the output
    start_time = time.time()
    tree = ctg.array_contract_tree(inputs, output, sizes_dict, optimize=algorithm)
    end_time = time.time()

    # Calculate cost and execution time
    cost = tree.contraction_cost()
    execution_time = end_time - start_time

    # Return results
    return cost, execution_time

def run_algorithm_naive(test_filename):
    # Open test file and read lines
    with open(test_filename, 'r') as text_file:
        lines = text_file.readlines()

        # Locate the naive cost(s) of contracting the tensor train
        naive_costs = []
        for line in lines:
            if line.startswith('c'):
                cost = int(re.split('\W+', line)[3])
                naive_costs.append(cost)

        cost = min(naive_costs)
        execution_time = 0.0

        # Return results
        return cost, execution_time


# Wrapper function to run computations for given test using either
# C++ (our algorithms), or Python (cotengra and other algorithms)
def run_algorithm(algorithm, test_filename, tt_dim, delta):
    if algorithm in ['OneSidedOneDim', 'TwoSidedDeltaDim']:
        return run_algorithm_cpp(algorithm, test_filename, tt_dim, delta)
    elif algorithm == 'naive':
        return run_algorithm_naive(test_filename)
    else:
        return run_algorithm_python(algorithm, test_filename)


def run_algorithm_on_test_case(input):
    # Unpack the input arguments
    algorithm, test_filename, result_filename, tt_dim, delta, dimension, instance = input[0]

    # Run the algorith on the test case
    cost, execution_time = run_algorithm(algorithm, test_filename, tt_dim, delta)
    output_str = f"{algorithm};{dimension};{instance};{test_filename};{cost};{execution_time}\n"

    # Save the results to the result file
    single_file_lock.acquire()
    with open(result_filename, 'a') as result_file:
        result_file.write(output_str)
    single_file_lock.release()


def generate_arguments_for_test_case(algorithm, test_dir_path, result_dir_path, min_size, max_size, max_size_optimal, tt_dim, delta):
    global file_lock

    # Prepare the lock for the result file
    result_filename = get_result_filename(result_dir_path, algorithm, delta)
    file_lock[result_filename] = multiprocessing.Lock()

    # Prepare header of result file if we start with the first test case
    if not os.path.exists(result_filename):
        with open(result_filename, 'w') as result_file:
            result_file.write(f"Algorithm;Size;Instance;Test_file;Cost;Execution_time\n")

    # Prepare max size for optimal algorithm (which we may not run for large sizes)
    if algorithm == "optimal":
        max_size = min(max_size, max_size_optimal)

    # Prepare arguments for each test case
    arguments = []
    for dimension in range(min_size, max_size + 1):
        for instance in range(1, nb_instances + 1):
            test_filename = get_test_filename(test_dir_path, dimension, instance)
            arguments.append([(algorithm, test_filename, result_filename, tt_dim, delta, dimension, instance)])

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
    config = configparser.ConfigParser()
    config.read(config_file)

    # Prepare test and result directory path
    test_dir = config['Tests']['test_dir']
    result_dir = config['Results']['result_dir']

    # Retrieve test cases parameters
    tt_dims = [int(tt_dim) for tt_dim in config['General']['tt_dims'].split(',')]
    min_size = int(config['Results']['min_size'])
    max_size = int(config['Results']['max_size'])
    max_size_optimal = int(config['Results']['max_size_optimal'])
    nb_instances = int(config['Tests']['nb_instances'])

    types = config['General']['types'].split(',')
    rank_types = config['General']['rank_types'].split(',')

    # Retrieve list of algorithms to run
    algorithms = config['Algorithms']['algorithms'].split(',')
    deltas = [int(delta) for delta in config['Algorithms']['deltas'].split(',')]
    print(deltas)

    # Generation of the test cases
    cores = min(int(config['Results']['max_cores']), multiprocessing.cpu_count() // 2)
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
                            parallel_input += generate_arguments_for_test_case(algorithm, test_dir_path, result_dir_path, min_size, max_size, max_size_optimal, tt_dim, None)
                        else:
                            for delta in deltas:
                                parallel_input += generate_arguments_for_test_case(algorithm, test_dir_path, result_dir_path, min_size, max_size, max_size_optimal, tt_dim, delta)


    # Execute tasks in parallel
    print(f"Executing test cases in parallel using {cores} cores")
    #pool.starmap(run_algorithm_on_test_case, parallel_input)
    with alive_bar(len(parallel_input)) as bar:
        for i in pool.imap_unordered(run_algorithm_on_test_case, parallel_input):
            bar()
