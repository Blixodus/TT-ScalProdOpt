import os
import sys
import re
import ast
import time
import subprocess
import multiprocessing
import threading
import configparser

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

    tokens = re.split("\W+|_", input_file)
    size = tokens[-3]
    instance = tokens[-2]
    ex_time = end_time - start_time

    print(f"{algorithm};{int(size)};{int(instance)};{input_file};{cost};{ex_time}")

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

algorithm = "TwoSidedDeltaDim"
file = "/home/pdominik/Tensor_experiments/OptiTenseurs/instances/test/uniform_all2/instance_006_01.txt"
dmax = 4
cost, ex_time, order_str = run_program(algorithm=algorithm, input_file=file, dmax=dmax)

# Retrieve the flat list of contractions
contraction_recursive = ast.literal_eval(order_str)
contraction_flat, _ = generate_contraction_list(contraction_recursive)

# Calculate the cost of the contraction order
cost_validation = run_validation(input_file=file, order=contraction_flat)
print(f"Validation cost: {cost_validation}")
print(f"OptiTenseurs cost: {cost}")



        