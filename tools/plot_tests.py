import ast
import multiprocessing
import os
import sys
import time
import subprocess
import configparser
from dataclasses import dataclass

from matplotlib import pyplot as plt
import pandas as pd
import seaborn as sns

from alive_progress import alive_bar

from Scripts.naming import get_test_case_name, get_test_case_name2, get_test_case_name3, get_algorithm_name, get_dir, get_dir_ratio, get_test_filename, get_result_filename



# ----------------------------- Utility functions ------------------------------
def get_label(algorithm, optimal_only=False):
    label_dict = {
        'optimal': 'Optimal / Best',
        'hyper-greedy': 'Hyper-Greedy',
        'hyper-kahypar': 'Hyper-Kahypar',
        'cgreedy': 'Cgreedy',
        'naive': "Naive",
        'OneSidedOneDim': "1-sided 1-dim",
        'quickbb-2': 'QuickBB',
        'flowcutter': 'FlowCutter',
        'TwoSidedSweeping': 'Sweeping'
    }

    if algorithm == 'optimal' and optimal_only:
        return 'Optimal'

    if algorithm in label_dict:
        return label_dict[algorithm]
    elif algorithm.startswith('TwoSidedDeltaDim'):
        return f"Δ-opt (Δ={algorithm.split('_')[1]})"
    else:
        return algorithm


def get_color(algorithm):
    color_dict = {
        'optimal': 'tab:red',
        'hyper-greedy': 'tab:cyan',
        'hyper-kahypar': 'tab:pink',
        'cgreedy': 'tab:olive',
        'naive': "tab:gray",
        'OneSidedOneDim': "greenyellow",
        'quickbb-2': 'tab:purple',
        'flowcutter': 'tab:brown',
        'TwoSidedSweeping': 'seagreen'
    }

    if algorithm in color_dict:
        return color_dict[algorithm]
    elif algorithm == "TwoSidedDeltaDim_4":
        return "tab:blue"
    elif algorithm == "TwoSidedDeltaDim_6":
        return "salmon"
    elif algorithm == "TwoSidedDeltaDim_8":
        return "tab:orange"
    else:
        return None
        
@dataclass
class PlotInfo:
    tt_dim: str
    y_eq_xT: bool
    tt_type: str
    ranks_val: str
    const_dim: int

def get_text_of_plot_info(plot_info):
    test_case_id = get_test_case_name(plot_info.tt_dim, plot_info.y_eq_xT)
    const_dim = "-"
    if plot_info.const_dim > 0:
        const_dim = plot_info.const_dim
    return f"Problem: {test_case_id}\nTT: {plot_info.tt_type}\nRanks: {plot_info.ranks_val}\nConst dim: {const_dim}"

def get_title(plot_info):
    test_case_id = get_test_case_name2(plot_info.tt_dim, plot_info.y_eq_xT)
    type_dict = {
        'random': 'rand-rand',
        'quantized': 'quant-rand',
        'increasing': 'quant-incr',
    }
    return f"Problem: {test_case_id}, TT: {type_dict[plot_info.tt_type]} {plot_info.ranks_val}"

def get_plot_name(plot_info):
    test_case_id = get_test_case_name3(plot_info.tt_dim, plot_info.y_eq_xT)
    return f"{test_case_id}_{plot_info.tt_type}_{plot_info.ranks_val}"

# ------------------------------- Plot function --------------------------------
def plot_test_case(plot_algorithms, normalization_algorithm, result_dir_path, plot_dir_path, nb_instances, plot_info, root_dir):
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

        # Drop rows with cost equal to 0
        results[algorithm] = results[algorithm].loc[(results[algorithm]['Cost'] != 0)]

        # Drop rows with any size for which there is any instance missing
        sizes = results[algorithm]['Size'].unique()
        for size in sizes:
            if len(results[algorithm].loc[(results[algorithm]['Size'] == size)]) < nb_instances:
                print(f"[Warning ❗] In import of file {result_filename} dropped size {size} due to missing instances.", )
                results[algorithm] = results[algorithm].loc[(results[algorithm]['Size'] != size)]
                if algorithm != 'optimal':
                    completed_successfully = False
                    error_message += f"missing instances ({algorithm}\t"
                else:
                    completed_successfully = False
                    error_message += f"missing optimal instances\t"

    # Find cut-off line for optimal algorithm, when result calculation
    # takes more than 1 hour
    cutoff_limits = {60 : "silver", 600 : "grey", 1800: "dimgrey", 3600: "black"}
    cutoff_sizes = {}
    max_optimal_size = 0
    if 'optimal' in results:
        sizes = results['optimal']['Size'].unique().tolist()
        print(result_dir_path, len(sizes), sizes)
        if sizes is not None and len(sizes) > 0:
            max_optimal_size = max(sizes)
            for time_limit in cutoff_limits:
                for size in sizes:
                    if results['optimal'].loc[(results['optimal']['Size'] == size)]['Execution_time'].mean() > time_limit:
                        cutoff_sizes[time_limit] = size
                        break

    # Normalize the results using results from given normalization algorithm
    if normalization_algorithm in results:
        results_cmp = {}
        # Merge the results up to size of cutoff 1h with the normalization algorithm above the cutoff
        # minimum cost for all sizes
        # Merge cost columns for each algorithm
        #for algorithm in algorithms:
        #    results[algorithm].set_index(['Size', 'Instance'], inplace=True)
        #results_combined = pd.concat([results[algorithm] for algorithm in algorithms], axis=1, keys=algorithms)
        sizes = range(3, 101)
        instances = range(1, 51)
        base = pd.DataFrame([(size, instance) for size in sizes for instance in instances], columns=['Size', 'Instance'])

        
        results_norm = pd.merge(base, results['optimal'][['Size', 'Instance', 'Cost']], on=['Size', 'Instance'], how='left')
        # Merge the current algorithm's results with the current optimal results on size and instance
        for algorithm in algorithms:
            results_merged = pd.merge(
                results_norm[['Size', 'Instance', 'Cost']],  # current optimal results
                results[algorithm][['Size', 'Instance', 'Cost']],  # new algorithm's results
                on=['Size', 'Instance'],  # merging on Size and Instance columns
                how='left',
                suffixes=('_mini', f'_{algorithm}')  # distinguish between columns
            )
        
            # Compute the minimum cost between the two
            results_merged['Cost'] = results_merged[['Cost_mini', f'Cost_{algorithm}']].min(axis=1)
            
            # Update the normalized results with the newly computed minimum costs
            results_norm = results_merged[['Size', 'Instance', 'Cost']]

        # Find minimum cost for each size
        #results_norm = pd.DataFrame()
        #results_norm['Size'] = results_combined.index.get_level_values(0)
        #results_norm['Instance'] = results_combined.index.get_level_values(1)
        #results_norm['Cost'] = results_combined.min(axis=1)

        for algorithm in algorithms:
            results_cmp[algorithm] = results[algorithm].merge(results_norm, on=['Size', 'Instance'], how='left', sort=False, suffixes=(None, '_Norm'))
            results_cmp[algorithm]['Normalized_cost'] = results_cmp[algorithm]['Cost'] / results_cmp[algorithm]['Cost_Norm']

            # Drop rows with cost more than 10 times higher than the optimal
            #results_cmp[algorithm] = results_cmp[algorithm].loc[(results_cmp[algorithm]['Normalized_cost'] < 10)]

        results_cmp['optimal'] = results_norm.copy()
        results_cmp['optimal']['Normalized_cost'] = 1

        if plot_dir_path == "/gpfs/workdir/torria/pdominik/Plots2/Plots_new_increasing/xAxT/increasing/high":
            for algorithm in algorithms:
                print(algorithm, results_cmp[algorithm].loc[(results_cmp[algorithm]['Size'] == 40)].head(n=5))

        # Add information about the test case
        #plt.text(0.58, 0.88, get_text_of_plot_info(plot_info), transform=plt.gca().transAxes, ha='right', va='bottom', style='italic', fontsize="6") #bbox={'facecolor':'white', 'alpha':0.5, 'pad':10}

        # Plot the normalized contraction cost
        plt.plot([0]) # to shift color map
        plt.plot([0])
        plt.plot([0])
        plt.plot([0])
        for algorithm in algorithms:
            if algorithm == "optimal":
                continue
            sns_plot = sns.lineplot(data=results_cmp[algorithm], x="Size", y="Normalized_cost", label=get_label(algorithm), color=get_color(algorithm)) #estimator="median"
            #sns.lineplot(data=results_cmp[algorithm], x="Size", y="Normalized_cost", label=get_label(algorithm))
        sns_plot = sns.lineplot(data=results_cmp['optimal'], x="Size", y="Normalized_cost", label=get_label(algorithm), color=get_color(algorithm), linestyle="dotted")

        # Plot vertical lines for time limit cutoffs
        #for time_limit in cutoff_sizes:
        #    plt.axvline(x=cutoff_sizes[time_limit], color=cutoff_limits[time_limit], linestyle=':')

        # Plot line for end of optimal results
        if max_optimal_size > 0 and max_optimal_size < 100:
            plt.axvline(x=max_optimal_size, color='black', linestyle='solid', linewidth=1)

        plt.xlabel('#dimensions')
        plt.ylabel('Contraction cost (relative to best)')
        #plt.ylabel(plot_dir_path.replace("/gpfs/workdir/torria/pdominik/Plots/Plots_", ""))
        plt.title(get_title(plot_info))
        plt.legend(loc='upper right')

        # Unbounded scale plots
        plt.savefig(f'{plot_dir_path}/contraction_cost_normalized_1.pdf', bbox_inches='tight')

        plt.yscale('log')
        y_min, y_max = plt.ylim()
        if y_max > 10:
            plt.ylim(0.875, 10)

        plt.savefig(f'{plot_dir_path}/contraction_cost_normalized_1_log.pdf', bbox_inches='tight')
        plt.savefig(f'{root_dir}/cost_{get_plot_name(plot_info)}.pdf', bbox_inches='tight')

        # Non-logarithim scale plots
        plt.yscale('linear')

        plt.axis([None, None, 0.95, 1.25])
        plt.savefig(f'{plot_dir_path}/contraction_cost_normalized_2.pdf', bbox_inches='tight')

        plt.axis([None, None, 0.95, 1.5])
        plt.savefig(f'{plot_dir_path}/contraction_cost_normalized_3.pdf', bbox_inches='tight')

        plt.axis([None, None, 0.95, 2])
        plt.savefig(f'{plot_dir_path}/contraction_cost_normalized_4.pdf', bbox_inches='tight')

        plt.axis([None, None, 0.5, 1.5])
        plt.savefig(f'{plot_dir_path}/contraction_cost_normalized_5.pdf', bbox_inches='tight')

        plt.axis([None, None, 0.0, 2.0])
        plt.savefig(f'{plot_dir_path}/contraction_cost_normalized_6.pdf', bbox_inches='tight')

        # Logarithimic scale plots
        plt.yscale('log')

        plt.axis([None, None, 0.5, 2])
        plt.savefig(f'{plot_dir_path}/contraction_cost_normalized_log.pdf', bbox_inches='tight')


        plt.close()
    else:
        print(f"[Warning ❗] Normalization algorithm {normalization_algorithm} not found. Skipping normalization plot.")
        completed_successfully = False
        error_message += "normalization algorithm missing\t"

    # Plot the mean execution time
    plt.plot([0])
    plt.plot([0])  
    plt.plot([0])
    plt.plot([0])
    for algorithm in algorithms:
        if algorithm == 'naive':
            continue
        sns_plot = sns.lineplot(data=results[algorithm], x="Size", y="Execution_time", label=get_label(algorithm, True), color=get_color(algorithm))    
        #plt.plot(results[algorithm].groupby('Size')['Execution_time'].mean(), line, label=get_label(algorithm), color=get_color(algorithm))
        #plt.plot(results[algorithm].groupby('Size')['Execution_time'].mean(), line, label=get_label(algorithm))
    plt.xlabel('#dimensions')
    plt.ylabel('Mean execution time [s]')
    plt.title(get_title(plot_info))
    plt.yscale('log')
    plt.legend(loc='upper left')
    plt.savefig(f'{plot_dir_path}/execution_time.pdf', bbox_inches='tight')
    plt.savefig(f'{root_dir}/time_{get_plot_name(plot_info)}.pdf', bbox_inches='tight')
    plt.close()

    # Plot the stack chart of mean execution time for wrapper, optimal and DP
    # check if 'Wrapper_time' column exists
    for algorithm in algorithms:
        sizes = results[algorithm]['Size'].unique().tolist()
        if algorithm.startswith('TwoSidedDeltaDim') and len(sizes) > 0 and 'Wrapper_time' in results[algorithm].columns:
            # Create dataframe with mean time for each size
            mean_wrapper_time = results[algorithm].groupby('Size')['Wrapper_time'].mean()
            mean_optimal_time = results[algorithm].groupby('Size')['Optimal_time'].mean()
            mean_dp_time = results[algorithm].groupby('Size')['DP_time'].mean()

            # Fill missing sizes with 0
            for size in sizes:
                if size not in mean_wrapper_time:
                    mean_wrapper_time[size] = 0
                if size not in mean_optimal_time:
                    mean_optimal_time[size] = 0
                if size not in mean_dp_time:
                    mean_dp_time[size] = 0

            # Plot the stack chart
            plt.stackplot(sizes, [mean_wrapper_time, mean_optimal_time, mean_dp_time], labels=['Wrapper', 'Optimal', 'DP'], colors=['tab:blue', 'tab:green', 'tab:red'])
            plt.legend(loc='upper right')

            plt.xlabel('Size of the dataset')
            plt.ylabel('Execution time [s] (mean of 50 instances)')
            plt.title(f'Composition of the execution time for {get_label(algorithm)}')

            plt.savefig(f'{plot_dir_path}/execution_time_{algorithm}.pdf', bbox_inches='tight')

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
        for size in results_cmp[algorithms[0]]['Size'].unique():
            for algorithm in algorithms:
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
    print(config_file, result_dir, test_dir, plot_dir)

    # Retrieve test cases parameters
    tt_dims = [int(tt_dim) for tt_dim in config['General']['tt_dims'].split(',')]
    max_size = int(config['Tests']['max_size'])
    nb_instances = int(config['Tests']['nb_instances'])

    types = config['General']['types'].split(',')
    rank_types = config['General']['rank_types'].split(',')
    const_dim = int(config['Tests']['const_dim'])

    # Retrieve list of algorithms to plot
    algorithms = config['Algorithms']['algorithms'].split(',')
    deltas = [int(delta) for delta in config['Algorithms']['deltas'].split(',')]
    print(deltas)

    # Retrieve normalization algorithm
    normalization_algorithm = (config['Plots']['normalization_algorithm'], config['Plots']['normalization_delta'])

    # Plotting of each test case
    cores = min(int(config['Plots']['max_cores']), multiprocessing.cpu_count())
    pool = multiprocessing.Pool(processes=4)
    parallel_input = []

    case_no = 1
    for tt_dim in tt_dims:
        # Consider case y=xT for smaller TT dimensions
        y_cases = [False]
        #if tt_dim <= 3:
        #    y_cases = [True, False]

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
                        if algorithm == "OneSidedOneDim" and tt_dim != 2:
                            continue
                        if algorithm != "TwoSidedDeltaDim":
                            plot_algorithms.append((algorithm, None))
                        else:
                            for delta in deltas:
                                plot_algorithms.append((algorithm, delta))

                    const_dim_val = const_dim
                    if type == "random":
                        const_dim_val = 0
                    plot_info = PlotInfo(tt_dim, y_eq_xT, type, rank_type, const_dim_val)

                    if type != 'ratio':
                        parallel_input.append((plot_algorithms, normalization_algorithm, result_dir_path, plot_dir_path, nb_instances, plot_info, plot_dir))
                    else:
                        ratio_list = ast.literal_eval(config['General']['ratio_list'])
                        for rank_const, dim_const in ratio_list:
                            parallel_input.append((plot_algorithms, normalization_algorithm, get_dir_ratio(result_dir_path, rank_const, dim_const),  get_dir_ratio(plot_dir_path, rank_const, dim_const), nb_instances, plot_info, plot_dir))
                        plot_ratio(ratio_list, plot_algorithms, normalization_algorithm, result_dir_path, plot_dir_path)


    # Execute tasks in parallel
    print(f"Executing plot generation in parallel using {cores} cores")
    execution_results = pool.starmap(plot_test_case, parallel_input)
    for result in execution_results:
        if result[1] == True:
            print(f"🟢 Test case {result[0]}")
        else:
            print(f"🟡 Test case {result[0]}: {result[2]}")
