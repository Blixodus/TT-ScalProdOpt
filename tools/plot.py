import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns
import configparser
import os
import sys

# Import configuration
if len(sys.argv) > 1:
    config_file = sys.argv[1]
    if not os.path.exists(config_file):
        exit("Error! Provided configuration file does not exist.")
else:
    exit("Error! No configuration file provided.")

config = configparser.ConfigParser()
config.read(config_file)

input_dir = config['Input']['input_dir']
input_prefix = config['Input']['input_prefix']

output_dir = config['Output']['output_dir']
if 'output_date' in config['Output'] and bool(config['Output']['output_date']) == True:
    output_dir += f" - {str(pd.Timestamp.now().floor('S'))}"

algorithms = config['Algorithms']['algorithms'].split(',')

if not os.path.exists(output_dir):
    os.makedirs(output_dir)

include_naive_cost = False
if 'test_include_naive' in config['Tests']:
    include_naive_cost = bool(config['Tests']['test_include_naive'])
    algorithms.append('naive')

# Import CSV files
results = {}

for algorithm in algorithms:
    results[algorithm] = pd.read_csv(f"{input_dir}/{input_prefix}_{algorithm}.txt", sep=';')

    # Sort the results by size and instance
    results[algorithm].sort_values(by=['Size', 'Instance'], inplace=True)

    # Drop sizes larger than measured for the optimal algorithm
    #results[algorithm] = results[algorithm].loc[results[algorithm]['Size'] <= 30]

# Plot the contraction cost for the first instance of each size
for algorithm in algorithms:
    line = '--'
    if algorithm == 'optimal':
        line = 'r+'
    plt.plot(results[algorithm].loc[results[algorithm]['Instance'] == 0]['Size'], results[algorithm].loc[results[algorithm]['Instance'] == 0]['Cost'], line, label=algorithm)

plt.xlabel('Size of the dataset')
plt.ylabel('Contraction cost')
plt.title('Comparison of the contraction cost for 1st instances')

plt.legend()
plt.savefig(f'{output_dir}/contraction_cost_1st_instance.pdf')
plt.close()

# Normalize the contraction cost by the cost of the optimal algorithm
results_normalized = {}
for algorithm in algorithms:
    results_normalized[algorithm] = results[algorithm].copy()
    results_normalized[algorithm]['Cost'] = results_normalized[algorithm]['Cost'] / results['optimal']['Cost']

# Plot the average normalized contraction cost for all instances of each size
for algorithm in algorithms:
    sns.lineplot(data=results_normalized[algorithm], x="Size", y="Cost", label=algorithm)
plt.legend()
plt.xlabel('Size of the dataset')
plt.ylabel('Contraction cost (mean of 50 instances)')
plt.yscale('log')
plt.title('Comparison of the normalized contraction cost for all instances')
plt.savefig(f'{output_dir}/contraction_cost_normalized_all_instances_with_kahypar.pdf')
plt.close()

for algorithm in algorithms:
    if algorithm == 'hyper-kahypar':
        continue
    sns.lineplot(data=results_normalized[algorithm], x="Size", y="Cost", label=algorithm)
plt.legend()
plt.xlabel('Size of the dataset')
plt.ylabel('Contraction cost (mean of 50 instances)')
plt.title('Comparison of the normalized contraction cost for all instances')
plt.savefig(f'{output_dir}/contraction_cost_normalized_all_instances.pdf')
plt.close()

#%%
# Normalize the contraction cost by the cost of our algorithm
results_cmp = {}
for algorithm in algorithms:
    results_cmp[algorithm] = results[algorithm].copy()
    results_cmp[algorithm]['Cost'] = results_cmp[algorithm]['Cost'] / results['OneSideDimByDim']['Cost']

# Plot the average normalized contraction cost for all instances of each size
for algorithm in algorithms:
    if algorithm == 'hyper-kahypar':
        continue
    sns.lineplot(data=results_cmp[algorithm], x="Size", y="Cost", label=algorithm)
plt.xlabel('Size of the dataset')
plt.ylabel('Contraction cost (mean of 50 instances)')
plt.title('Comparison of the normalized (to 1SideDim) contraction cost')
plt.legend()
plt.savefig(f'{output_dir}/contraction_cost_normalized_to_our_all_instances.pdf')
plt.close()

for algorithm in algorithms:
    sns.lineplot(data=results_cmp[algorithm], x="Size", y="Cost", label=algorithm)
plt.xlabel('Size of the dataset')
plt.ylabel('Contraction cost (mean of 50 instances)')
plt.title('Comparison of the normalized (to 1SideDim) contraction cost')
plt.legend()
plt.savefig(f'{output_dir}/contraction_cost_normalized_to_our_all_instances_with_kahypar.pdf')
plt.close()

# Plot the mean execution time for all instances of each size
for algorithm in algorithms:
    line = '--'
    if algorithm == 'optimal':
        line = 'r+'
    plt.plot(results[algorithm].groupby('Size')['Execution_time'].mean(), line, label=algorithm)
plt.xlabel('Size of the dataset')
plt.ylabel('Execution time [s] (mean of 50 instances)')
plt.title('Comparison of the execution time')
plt.yscale('log')
plt.legend()
plt.savefig(f'{output_dir}/execution_time.pdf')
plt.close()
#%%
