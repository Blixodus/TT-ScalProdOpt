import sys
import os
import pickle
import timeit
from pathlib import Path
from collections import OrderedDict
import cotengra as ctg
import opt_einsum as oe
from import_file import import_tensor_train


if __name__ == "__main__":
    if len(sys.argv) > 3:
        result_dir = sys.argv[1]
        input_dir = sys.argv[2]
        for_plot_dir = sys.argv[3]
        if not os.path.exists(result_dir) or not os.path.exists(input_dir):
            exit("Error! Provided directory does not exist.")
    else:
        exit("Error! No directory provided.")
    
    for file_path in Path(result_dir).rglob('*'):
        if not file_path.is_file():
            continue
        relative_path = file_path.relative_to(result_dir)
        for_plot_filename = os.path.join(for_plot_dir, relative_path.parent)
        for_plot_filename = os.path.join(for_plot_filename, "rl-tnco.txt")
        num_nodes = int(os.path.splitext(file_path.name)[0].split('_')[1])
        version = int(os.path.splitext(file_path.name)[0].split('_')[3])
        os.makedirs(os.path.dirname(for_plot_filename), exist_ok=True)
        with open(for_plot_filename, 'w') as f:
            print("Algorithm;Size;Instance;Test_file;Cost;Execution_time;Order", file=f)
            
    for file_path in Path(result_dir).rglob('*'):
        if not file_path.is_file():
            continue
        time, path = pickle.load(open(file_path, 'rb'))
        relative_path = file_path.relative_to(result_dir)
        input_filename = os.path.join(input_dir, relative_path)
        input_filename = os.path.splitext(input_filename)[0] + '.txt'
        inputs, output, sizes_dict, _ = import_tensor_train(input_filename)
        path_ctg = ctg.ContractionTree.from_path(inputs, output, sizes_dict, path = path)
        for_plot_filename = os.path.join(for_plot_dir, relative_path.parent)
        for_plot_filename = os.path.join(for_plot_filename, "rl-tnco.txt")
        num_nodes = int(os.path.splitext(file_path.name)[0].split('_')[1])
        version = int(os.path.splitext(file_path.name)[0].split('_')[3])
        with open(for_plot_filename, 'a') as f:
            print(f"rl-tnco;{num_nodes};{version};TBD;{path_ctg.contraction_cost()};{time};TBD", file=f)
