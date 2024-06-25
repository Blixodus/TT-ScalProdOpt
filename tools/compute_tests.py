import os
import sys
import time
import subprocess
import configparser

import cotengra as ctg
from cgreedy import CGreedy

from Scripts.naming import get_dir, get_test_filename, get_result_filename
from Scripts.import_file import import_tensor_train




# ------------------------------- Utility functions --------------------------------
def run_algorithm_cpp(algorithm, test_filename, result_file, tt_dim, dimension, instance, delta):
    # Prepare command line arguments
    delta_str = ""
    if delta is not None:
        delta = f"delta {delta}"

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

    # Write results to file
    output_str = f"{algorithm};{dimension};{instance};{test_filename};{cost};{execution_time}\n"
    result_file.write(output_str)
    print("\t\t", output_str, end='')


def run_algorithm_python(algorithm, test_filename, result_file, tt_dim, dimension, instance):
    # Import tensor train from file
    inputs, output, sizes_dict, _ = import_tensor_train(test_filename)

    # Initialize the cgreedy object (which is not part of cotengra)
    algorithm_str = algorithm
    if algorithm == 'cgreedy':
        algorithm = CGreedy(seed=1, minimize="flops", max_repeats=1024, max_time=1.0, progbar=False, threshold_optimal=12)

    # Run the algorithm and retrieve the output
    start_time = time.time()
    tree = ctg.array_contract_tree(inputs, output, sizes_dict, optimize=algorithm)
    end_time = time.time()

    # Calculate cost and execution time
    cost = tree.contraction_cost()
    execution_time = end_time - start_time

    # Write results to file
    output_str = f"{algorithm_str};{dimension};{instance};{test_filename};{cost};{execution_time}\n"
    result_file.write(output_str)
    print("\t\t", output_str, end='')


# Wrapper function to run computations for given test using either
# C++ (our algorithms), or Python (cotengra and other algorithms)
def run_algorithm(algorithm, test_filename, result_file, tt_dim, dimension, instance, delta):
    if algorithm in ['OneSidedOneDim', 'TwoSidedDeltaDim']:
        run_algorithm_cpp(algorithm, test_filename, result_file, tt_dim, dimension, instance, delta)
    else:
        run_algorithm_python(algorithm, test_filename, result_file, tt_dim, dimension, instance)

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
    max_size = int(config['Tests']['max_size'])
    nb_instances = int(config['Tests']['nb_instances'])

    types = config['General']['types'].split(',')
    rank_types = config['General']['rank_types'].split(',')

    # Retrieve list of algorithms to run
    algorithms = config['Algorithms']['algorithms'].split(',')
    deltas = [int(delta) for delta in config['Algorithms']['deltas'].split(',')]
    print(deltas)

    # Generation of the test cases
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
                    print(f"[Info {case_no}] Computing test cases: TT-dim={tt_dim}, (y=xT)={y_eq_xT}, type={type}, rank_type={rank_type}")
                    case_no += 1

                    for algorithm in algorithms:
                        result_filename = get_result_filename(result_dir_path, algorithm)
                        with open(result_filename, 'w') as result_file:
                            result_file.write(f"Algorithm;Size;Instance;Test_file;Cost;Execution_time\n")

                            max_size_local = max_size
                            if algorithm == "optimal":
                                max_size_local = 30

                            for dimension in range(3, max_size_local + 1):
                                print(f"\t Algorithm: {algorithm} \t\t Dimension: {dimension}")
                                for instance in range(1, nb_instances + 1):
                                    test_filename = get_test_filename(test_dir_path, dimension, instance)
                                    run_algorithm(algorithm, test_filename, result_file, tt_dim, dimension, instance, deltas[0])
