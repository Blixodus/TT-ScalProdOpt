import os
import sys
import re
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
    for line in result.stdout.split("\n"):
        if "Best cost" in line:
            cost = float(line.split(":")[1])

    tokens = re.split("\W+|_", input_file)
    size = tokens[-3]
    instance = tokens[-2]
    ex_time = end_time - start_time

    print(f"{algorithm};{int(size)};{int(instance)};{input_file};{cost};{ex_time}")

    lock.acquire()
    with open(result_file, 'a') as output_file:
        output_file.write(f"{algorithm};{int(size)};{int(instance)};{input_file};{cost};{ex_time}\n")
    lock.release()

    return cost, end_time - start_time


if len(sys.argv) > 1:
    config_file = sys.argv[1]
    if not os.path.exists(config_file):
        exit("Error! Provided configuration file does not exist.")
else:
    exit("Error! No configuration file provided.")

config = configparser.ConfigParser()
config.read(config_file)

parallel = bool(int(config['Tests']['test_parallel']))

test_dir = config['Tests']['test_dir']
files = os.listdir(test_dir)
files = [os.path.join(test_dir, file) for file in files]
files.sort()

algorithms = []
result_file = f"{config['Input']['input_dir']}/{config['Input']['input_prefix']}"

if len(sys.argv) > 2:
    algorithms = [sys.argv[2]]
    result_file += f"_{sys.argv[2]}.txt"

dmax = None
if len(sys.argv) > 3:
    dmax = sys.argv[3]
    result_file = f"{config['Input']['input_dir']}/{config['Input']['input_prefix']}_{sys.argv[2]}_{sys.argv[3]}.txt"


with open(result_file, 'a') as output_file:
    output_file.write(f"Algorithm;Size;Instance;Test_file;Cost;Execution_time\n")

if not parallel:
    print("Sequential execution")
    for algorithm in algorithms:
        for file in files:
            print(f"Algorithm: {algorithm} \t Evaluating {file}")
            cost, ex_time = run_program(algorithm=algorithm, input_file=file, dmax=dmax)
else:
    print("Parallel execution")
    for algorithm in algorithms:
        pool = multiprocessing.Pool()
        input = [(algorithm, file) for file in files]
        pool.starmap(run_program, input)
