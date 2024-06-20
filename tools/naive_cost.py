import configparser
import os
import sys
import re

# Import configuration
if len(sys.argv) > 1:
    config_file = sys.argv[1]
    if not os.path.exists(config_file):
        exit("Error! Provided configuration file does not exist.")
else:
    exit("Error! No configuration file provided.")

config = configparser.ConfigParser()
config.read(config_file)

test_dir = config['Tests']['test_dir']

input_dir = config['Input']['input_dir']
input_prefix = config['Input']['input_prefix']
result_file = f"{input_dir}/{input_prefix}_naive.txt"

# Import naive costs
files = os.listdir(test_dir)
files = [os.path.join(test_dir, file) for file in files]
files.sort()

output_file = open(result_file, 'a')
output_file.write(f"Algorithm;Size;Instance;Test_file;Cost;Execution_time\n")

for file in files:
    text_file = open(file, "r")
    lines = text_file.readlines()
    text_file.close()

    tokens = re.split("\W+|_", file)
    size = int(tokens[-3])
    instance = int(tokens[-2])

    # Locate the naive cost of contracting the tensor train
    naive_costs = []
    for line in lines:
        if line.startswith('c'):
            cost = int(re.split('\W+', line)[3])
            naive_costs.append(cost)

    print(f"{file};{naive_costs};{min(naive_costs)}")
    output_file.write(f"naive;{size};{instance};{file};{min(naive_costs)};0.0\n")

