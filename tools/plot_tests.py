import ast
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

from Scripts.naming import get_algorithm_name, get_dir, get_dir_ratio, get_test_filename, get_result_filename
from Scripts.import_file import import_tensor_train

# ------------------------------- Plot function --------------------------------
def plot_test_case(plot_algorithms, normalization_algorithm, result_dir_path, plot_dir_path, nb_instances):
    completed_successfully = True
    error_message = ""

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
            completed_successfully = False
            error_message += f"missing file ({algorithm}\t"
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
                if algorithm != 'optimal':
                    completed_successfully = False
                    error_message += f"missing instances ({algorithm}\t"
                else:
                    completed_successfully = False
                    error_message += f"missing optimal instances\t"
        
        if algorithm == 'optimal' and max(sizes) < 30:
            completed_successfully = False
            error_message += f"missing optimal sizes {max(sizes)}/30\t"

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
        completed_successfully = False
        error_message += "normalization algorithm missing\t"

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
    return (plot_dir_path, completed_successfully, error_message)

def plot_ratio(ratio_list, plot_algorithms, normalization_algorithm, result_dir_path, plot_dir_path):
    completed_successfully = True
    error_message = ""

    # Import CSV files with results
    results = {}
    algorithms = []
    normalization_algorithm = get_algorithm_name(normalization_algorithm[0], normalization_algorithm[1])

    for algorithm_tuple in plot_algorithms:
        # Prepare algorithm name for plot
        algorithm = get_algorithm_name(algorithm_tuple[0], algorithm_tuple[1])
        algorithms.append(algorithm)

        for rank_const, dim_const in ratio_list:
            # Import the results from file
            local_dir_path = get_dir_ratio(result_dir_path, rank_const, dim_const)
            result_filename = get_result_filename(local_dir_path, algorithm_tuple[0], algorithm_tuple[1])
            if not os.path.exists(result_filename):
                print(f"[Error ❌] File {result_filename} does not exist. Algorithm {algorithm} will be skipped.")
                completed_successfully = False
                error_message += f"missing file ({algorithm}\t"
                continue
            
            print(result_filename)
            df = pd.read_csv(result_filename, sep=';')
            df['Ratio'] = dim_const / rank_const
            if algorithm in results:
                results[algorithm] = pd.concat([results[algorithm], df], ignore_index=True)
            else:
                results[algorithm] = df

    # Normalize the results using results from given normalization algorithm
    if normalization_algorithm in results:
        results_cmp = {}
        for algorithm in algorithms:
            results_cmp[algorithm] = results[algorithm].merge(results[normalization_algorithm], on=['Ratio', 'Size', 'Instance'], how='left', sort=False, suffixes=(None, '_Norm'))
            results_cmp[algorithm]['Normalized_cost'] = results_cmp[algorithm]['Cost'] / results_cmp[algorithm]['Cost_Norm']

        # Plot the normalized contraction cost for each size
        print(results_cmp[algorithms[0]]['Size'].unique())
        for size in results_cmp[algorithms[0]]['Size'].unique():
            for algorithm in algorithms:
                print(size, algorithm)
                sns.lineplot(data=results_cmp[algorithm].loc[(results_cmp[algorithm]['Size'] == size)], x="Ratio", y="Normalized_cost", label=algorithm)
            plt.xlabel('Ratio (dim_const / rank_const)')
            plt.ylabel('Normalized contraction cost')
            plt.title(f'Comparison of the contraction cost (size = {size})')
            plt.legend(loc='upper right')
            plt.xscale('log')

            os.makedirs(f'{plot_dir_path}/ratio_test/{size}', exist_ok=True)
            print(f"{plot_dir_path}/ratio_test/{size}")

            plt.savefig(f'{plot_dir_path}/ratio_test/{size}/contraction_cost_normalized.pdf')

            plt.axis([None, None, 0.95, 1.25])
            plt.savefig(f'{plot_dir_path}/ratio_test/{size}/contraction_cost_normalized_zoom.pdf')

            plt.close()
    else:
        print(f"[Warning ❗] Normalization algorithm {normalization_algorithm} not found. Skipping normalization plot.")
        completed_successfully = False
        error_message += "normalization algorithm missing\t"

    print(f"[Plot ✅] Plot generation for test case {result_dir_path} \t ({plot_dir_path}) \t completed.")
    return (plot_dir_path, completed_successfully, error_message)

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

    # Retrieve list of algorithms to plot
    algorithms = config['Algorithms']['algorithms'].split(',')
    deltas = [int(delta) for delta in config['Algorithms']['deltas'].split(',')]
    print(deltas)

    # Retrieve normalization algorithm
    normalization_algorithm = (config['Plots']['normalization_algorithm'], config['Plots']['normalization_delta'])

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
                if type == "ratio" and y_eq_xT:
                    continue

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
                        if algorithm != "TwoSidedDeltaDim":
                            plot_algorithms.append((algorithm, None))
                        else:
                            for delta in deltas:
                                plot_algorithms.append((algorithm, delta))

                    if type != 'ratio':
                        parallel_input.append((plot_algorithms, normalization_algorithm, result_dir_path, plot_dir_path, nb_instances))
                    else:
                        ratio_list = ast.literal_eval(config['General']['ratio_list'])
                        for rank_const, dim_const in ratio_list:
                            parallel_input.append((plot_algorithms, normalization_algorithm, get_dir_ratio(result_dir_path, rank_const, dim_const),  get_dir_ratio(plot_dir_path, rank_const, dim_const), nb_instances))
                        plot_ratio(ratio_list, plot_algorithms, normalization_algorithm, result_dir_path, plot_dir_path)


    # Execute tasks in parallel
    print(f"Executing plot generation in parallel using {cores} cores")
    execution_results = pool.starmap(plot_test_case, parallel_input)
    for result in execution_results:
        if result[1] == True:
            print(f"🟢 Test case {result[0]}")
        else:
            print(f"🟡 Test case {result[0]}: {result[2]}")
