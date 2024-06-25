import multiprocessing
import os
import sys
import time
import subprocess
import configparser

from matplotlib import pyplot as plt
import pandas as pd
import seaborn as sns

import cotengra as ctg
from cgreedy import CGreedy

from alive_progress import alive_bar

from Scripts.naming import get_algorithm_name, get_dir, get_test_filename, get_result_filename
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
    #print("\t\t", output_str, end='')


def run_algorithm_python(algorithm, test_filename, result_file, tt_dim, dimension, instance):
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

    # Write results to file
    output_str = f"{algorithm_str};{dimension};{instance};{test_filename};{cost};{execution_time}\n"
    result_file.write(output_str)
    #print("\t\t", output_str, end='')


# Wrapper function to run computations for given test using either
# C++ (our algorithms), or Python (cotengra and other algorithms)
def run_algorithm(algorithm, test_filename, result_file, tt_dim, dimension, instance, delta):
    if algorithm in ['OneSidedOneDim', 'TwoSidedDeltaDim']:
        run_algorithm_cpp(algorithm, test_filename, result_file, tt_dim, dimension, instance, delta)
    else:
        run_algorithm_python(algorithm, test_filename, result_file, tt_dim, dimension, instance)

def plot_test_case(plot_algorithms, normalization_algorithm, result_dir_path, plot_dir_path, nb_instances):
    # Import CSV files with results
    results = {}
    algorithms = []
    normalization_algorithm = get_algorithm_name(normalization_algorithm[0], normalization_algorithm[1])

    for algorithm_tuple in plot_algorithms:
        # Prepare algorithm name for plot
        algorithm = get_algorithm_name(algorithm_tuple[0], algorithm_tuple[1])

        # Import the results from file
        result_filename = get_result_filename(result_dir_path, algorithm_tuple[0], algorithm_tuple[1])
        if not os.path.exists(result_filename):
            print(f"[Error ❌] File {result_filename} does not exist. Algorithm {algorithm} will be skipped.")
            continue
        else:
            algorithms.append(algorithm)
        results[algorithm] = pd.read_csv(result_filename, sep=';')

        # Sort the results by size and instance
        results[algorithm].sort_values(by=['Size', 'Instance'], inplace=True)

        # Drop rows with any size for which there is any instance missing
        sizes = results[algorithm]['Size'].unique()
        for size in sizes:
            if len(results[algorithm].loc[(results[algorithm]['Size'] == size)]) != nb_instances:
                print(f"[Warning ❗] In import of file {result_filename} dropped size {size} due to missing instances.", )
                results[algorithm] = results[algorithm].loc[(results[algorithm]['Size'] != size)]

    # Normalize the results using results from given normalization algorithm
    if normalization_algorithm in results:
        results_cmp = {}
        for algorithm in algorithms:
            results_cmp[algorithm] = results[algorithm].merge(results[normalization_algorithm], on=['Size', 'Instance'], how='left', sort=False, suffixes=(None, '_Norm'))
            results_cmp[algorithm]['Normalized_cost'] = results_cmp[algorithm]['Cost'] / results_cmp[algorithm]['Cost_Norm']

        # Plot the normalized contraction cost
        for algorithm in algorithms:
            sns.lineplot(data=results_cmp[algorithm], x="Size", y="Normalized_cost", label=algorithm)
        plt.xlabel('Size of the dataset')
        plt.ylabel('Contraction cost (mean of 50 instances)')
        plt.title('Comparison of the contraction cost (normalized to 2SΔD)')
        plt.legend(loc='upper right')

        plt.savefig(f'{plot_dir_path}/contraction_cost_normalized.pdf')

        plt.axis([None, None, 0.95, 1.25])
        plt.savefig(f'{plot_dir_path}/contraction_cost_normalized_zoom.pdf')

        plt.close()
    else:
        print(f"[Warning ❗] Normalization algorithm {normalization_algorithm} not found. Skipping normalization plot.")

    # Plot the mean execution time
    for algorithm in algorithms:
        line = '--'
        if algorithm == 'optimal':
            line = 'r+'
        plt.plot(results[algorithm].groupby('Size')['Execution_time'].mean(), line, label=algorithm)
    plt.xlabel('Size of the dataset')
    plt.ylabel('Execution time [s] (mean of 50 instances)')
    plt.title('Comparison of the execution time')
    plt.yscale('log')
    plt.legend(loc='upper right')
    plt.savefig(f'{plot_dir_path}/execution_time.pdf')
    plt.close()

    print(f"[Plot ✅] Plot generation for test case {result_dir_path} \t ({plot_dir_path}) \t completed.")

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

    # Prepare test, result and plot directory path
    test_dir = config['Tests']['test_dir']
    result_dir = config['Results']['result_dir']
    plot_dir = config['Plots']['plot_dir']

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

    # Plotting of each test case
    cores = min(int(config['Plots']['max_cores']), multiprocessing.cpu_count() // 2)
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
                    # Prepare directory for input files and output plots
                    test_dir_path = get_dir(test_dir, tt_dim, y_eq_xT, type, rank_type)
                    result_dir_path = get_dir(result_dir, tt_dim, y_eq_xT, type, rank_type)
                    plot_dir_path = get_dir(plot_dir, tt_dim, y_eq_xT, type, rank_type)

                    # Plot results for each algorithm and test file
                    print(f"[Info {case_no}] Preparing plot for: TT-dim={tt_dim}, (y=xT)={y_eq_xT}, type={type}, rank_type={rank_type}")
                    case_no += 1

                    plot_algorithms = []
                    for algorithm in algorithms:
                        if algorithm != "TwoSidedDeltaDim1123":
                            plot_algorithms.append((algorithm, None))
                        else:
                            for delta in deltas:
                                plot_algorithms.append((algorithm, delta))
                    normalization_algorithm = ("TwoSidedDeltaDim", None)
                    parallel_input.append((plot_algorithms, normalization_algorithm, result_dir_path, plot_dir_path, nb_instances))


    # Execute tasks in parallel
    print(f"Executing plot generation in parallel using {cores} cores")
    pool.starmap(plot_test_case, parallel_input)
