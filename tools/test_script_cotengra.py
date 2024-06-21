import os
import sys
import re
import time
import cotengra as ctg
import opt_einsum as oe
import multiprocessing
import threading
import configparser
from cgreedy import CGreedy

from Scripts.import_file import import_tensor_train

lock = threading.Lock()
result_file = None

def benchmark(algorithm, input_file):
    inputs, output, sizes_dict, _ = import_tensor_train(input_file)

    tokens = re.split("\W+|_", input_file)
    size = int(tokens[-3])
    instance = tokens[-2]

    #if algorithm == 'hyper-kahypar' and size > 19:
    #    return None
    if algorithm == 'cgreedy':
        algorithm = CGreedy(seed=1, minimize="size", max_repeats=1024, max_time=1.0, progbar=True, threshold_optimal=12, threads=0)

    start_time = time.time()
    tree = ctg.array_contract_tree(inputs, output, sizes_dict, optimize=algorithm)
    end_time = time.time()
    ex_time = end_time - start_time

    print(f"{algorithm};{int(size)};{int(instance)};{input_file};{tree.contraction_cost()};{ex_time}")

    lock.acquire()
    with open(result_file, 'a') as output_file:
        output_file.write(f"{algorithm};{int(size)};{int(instance)};{input_file};{tree.contraction_cost()};{ex_time}\n")
    lock.release()

if __name__ == '__main__':
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

    with open(result_file, 'a') as output_file:
        output_file.write(f"Algorithm;Size;Instance;Test_file;Cost;Execution_time\n")

    if not parallel:
        print("Sequential execution")
        for algorithm in algorithms:
            for file in files:
                benchmark(algorithm, file)
    else:
        print("Parallel execution")
        for algorithm in algorithms:
            pool = multiprocessing.Pool()
            input_args = [(algorithm, file) for file in files]
            pool.starmap(benchmark, input_args)
