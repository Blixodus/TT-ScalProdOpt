import os
import sys
import re
import ast
import time
import subprocess
import multiprocessing
import threading
import configparser

import pandas as pd

lock = threading.Lock()
result_file = None

def run_program(algorithm, input_file, dmax=None):
    global result_file
    global lock

    dmax_str = ""
    if dmax is not None:
        dmax_str = f"dmax {dmax}"

    args = f"source ~/.xmake/profile && cd /home/pdominik/Tensor_experiments/OptiTenseurs_dev && xmake run -w . OptiTenseurs -a \"main_alg {algorithm} {dmax_str}\" -f {input_file}"
    print(args)

    start_time = time.time()
    result = subprocess.run(args=args, capture_output=True, text=True, shell=True)
    end_time = time.time()

    cost = 0
    order = ""
    for line in result.stdout.split("\n"):
        if "Best cost" in line:
            cost = float(line.split(":")[1])
        if "Best order" in line:
            order = line.split(":")[1]

    return cost, end_time - start_time, order

def run_validation(input_file, order):
    global result_file
    global lock

    args = f"source ~/.xmake/profile && cd /home/pdominik/Tensor_experiments/OptiTenseurs_dev && xmake run -w . Compute"
    inputs = f"{input_file}\n{len(order)}\n"
    for contraction in order:
        inputs += f"{contraction[0]} {contraction[1]}\n"

    start_time = time.time()
    result = subprocess.run(args=args, input=inputs, capture_output=True, text=True, shell=True)
    end_time = time.time()

    cost = 0
    for line in result.stdout.split("\n"):
        if "Cost" in line:
            cost = float(line.split(":")[1])

    return cost

def generate_contraction_list(contraction_tree):
    if len(contraction_tree) > 2:
        exit("Error! The contraction tree is not binary.")
    
    contraction_list = []
    ids = []

    for i in range(len(contraction_tree)):
        if isinstance(contraction_tree[i], tuple):
            local_list, min_id = generate_contraction_list(contraction_tree[i])
            ids.append(min_id)
            contraction_list += local_list
        else:
            ids.append(contraction_tree[i])

    for i in range(len(ids) - 1):
        contraction_list.append((ids[i], ids[i + 1]))

    return contraction_list, min(ids)

# Parse configuration file and initialize variables
if len(sys.argv) > 1:
    config_file = sys.argv[1]
    if not os.path.exists(config_file):
        exit("Error! Provided configuration file does not exist.")
else:
    exit("Error! No configuration file provided.")

config = configparser.ConfigParser()
config.read(config_file)

test_dir = config['Tests']['test_dir']
files = os.listdir(test_dir)
files = [os.path.join(test_dir, file) for file in files]
files.sort()

algorithms = []
result_file = f"{config['Input']['input_dir']}/{config['Input']['input_prefix']}"

if len(sys.argv) > 2:
    algorithms = [sys.argv[2]]
    result_file += f"_{sys.argv[2]}"
result_file += "_validation.txt"

dmax = None
if len(sys.argv) > 3:
    dmax = sys.argv[3]
    result_file = f"{config['Input']['input_dir']}/{config['Input']['input_prefix']}_{sys.argv[2]}_{sys.argv[3]}_validation.txt"

# Load the file with the exact results
exact_results = pd.read_csv(f"{config['Input']['input_dir']}/{config['Input']['input_prefix']}_optimal.txt", sep=';')

# Write the header to the output file
output_file = open(result_file, 'a')
output_file.write(f"Algorithm;Size;Instance;Test_file;Cost;Validated_cost;Execution_time;Result\n")

for algorithm in algorithms:
    for file in files:
        # Run the algorithm on the input file
        cost, ex_time, order_str = run_program(algorithm=algorithm, input_file=file, dmax=dmax)

        # Retrieve the flat list of contractions
        contraction_recursive = ast.literal_eval(order_str)
        contraction_flat, _ = generate_contraction_list(contraction_recursive)

        # Calculate the cost of the contraction order
        cost_validation = run_validation(input_file=file, order=contraction_flat)
        
        # Save the result to the output file
        tokens = re.split("\W+|_", file)
        size = tokens[-3]
        instance = tokens[-2]

        result = "OK"
        summary = "✅"

        # Check if the reported cost is equal to the validated cost
        if cost != cost_validation:
            result = "NOT_EQUAL"
            summary = "❌"

        # Check if the reported cost is greater or equal to the exact cost
        if cost < exact_results[(exact_results['Size'] == int(size)) & (exact_results['Instance'] == int(instance))]['Cost'].values[0]:
            result = "LESS_THAN_EXACT"
            summary = "❌"

        output_str = f"{algorithm};{int(size)};{int(instance)};{file};{cost};{cost_validation};{ex_time};{result}\n"
        output_file.write(output_str)
        print(output_str, summary)
    
output_file.close()
        



        