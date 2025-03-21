import sys
import os
import pickle
import timeit
import argparse
from pathlib import Path
from collections import OrderedDict
import cotengra as ctg
import opt_einsum as oe
from import_file import import_tensor_train


def solve_eq(eq, shapes, baseline, max_time=300):
    start = timeit.default_timer()
    if baseline == 'ctg_kahypar':
        opt = ctg.HyperOptimizer(methods=['kahypar'])#, max_repeats=1000, max_time=max_time)
        path, info = oe.contract_path(eq, *shapes, shapes=True, optimize=opt)
    elif baseline == 'ctg_greedy':
        opt = ctg.HyperOptimizer(methods=['greedy'])#, max_repeats=10, max_time=max_time)
        path, info = oe.contract_path(eq, *shapes, shapes=True, optimize=opt)
    elif baseline == 'oe_greedy':
        path, info = oe.contract_path(eq, *shapes, shapes=True, optimize='greedy')
    elif baseline == 'oe_dp':
        path, info = oe.contract_path(eq, *shapes, shapes=True, optimize='dp')
    elif baseline == 'oe_optimal':
        path, info = oe.contract_path(eq, *shapes, shapes=True, optimize='optimal')
    elif baseline == 'oe_branch-1':
        path, info = oe.contract_path(eq, *shapes, shapes=True, optimize='branch-1')
    elif baseline == 'oe_branch-2':
        path, info = oe.contract_path(eq, *shapes, shapes=True, optimize='branch-2')
    elif baseline == 'oe_branch-all':
        path, info = oe.contract_path(eq, *shapes, shapes=True, optimize='branch-all')
    else:
        raise  ValueError('undefined baseline')
    stop = timeit.default_timer()
    total_time = stop - start
    contraction_cost = info.opt_cost
    print('solving with {} yielded contraction_cost={} in {} sec.'
          .format(baseline, contraction_cost, total_time))
    return contraction_cost, total_time, info, path

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Process arguments.")
    parser.add_argument("input", help="Path to the input directory")
    parser.add_argument("-d", "--dir", nargs="?", help="Path to the output directory (evaluation dataset)")
    parser.add_argument("-f", "--file", nargs="?", help="Path to the output file (training dataset)")

    args = parser.parse_args()

    # Validate input directory
    if not os.path.exists(args.input):
        sys.exit("Error! Provided input directory does not exist.")

    # Ensure that either --dir or --file is provided
    if not args.dir and not args.file or args.dir and args.file:
        sys.exit("Error! You must provide either an output directory (--dir) or an output file (--file) but not both.")

    # If output directory is provided, create it if it doesn't exist
    if args.dir:
        os.makedirs(args.dir, exist_ok=True)
        print(f"Output directory set to: {args.dir}")

    counter = 0
    eq_list = []
    baselines = [ 'oe_greedy', 'ctg_greedy', 'ctg_kahypar' ]
    solution_dict = { baseline: [] for baseline in baselines }
    for file_path in Path(args.input).rglob('*'):
        
        # Skip directories and non-file paths
        if not file_path.is_file():
            continue
        # If writing for evaluation, check if the processed file already exists,
        # and that it's index is not over 25
        if args.dir:
            relative_path = file_path.relative_to(args.input)
            filename = os.path.join(args.dir, relative_path)
            filename = os.path.splitext(filename)[0] + '.p'
            # Skip if the file already exists
            if os.path.isfile(filename):
                continue
            # Skip if index too big
            try:
                file_index = int(os.path.splitext(file_path.name)[0].split('_')[3])
                if file_index > 25:
                    continue
            except IndexError:
                # If the filename format is unexpected, just continue to next file
                continue
        
        counter += 1
        # Import tensor train from file (output is empty list in scalar product)
        inputs, output, sizes_dict, input_node_included = import_tensor_train(file_path)
        # Convert the tensor train description into RL-TNCO input format
        equation = ",".join("".join(sublist) for sublist in inputs)
        eq = f'{equation}->'
        # Prepare shape and size information
        shapes = [tuple(sizes_dict[char] for char in sublist) for sublist in inputs]
        size_dict = OrderedDict(sorted(sizes_dict.items()))
        eq_list.append((eq, shapes, size_dict))
        # Solve equation using the baselines
        for baseline in baselines:
            contraction_cost, solver_time, info, path = solve_eq(eq, shapes, baseline=baseline)
            solution_dict[baseline].append((contraction_cost, solver_time, info, path))
        # If output directory is specified, save the results to a pickle file
        if args.dir:
            # Create the output file path
            relative_path = file_path.relative_to(args.input)
            filename = os.path.join(args.dir, relative_path)
            filename = os.path.splitext(filename)[0] + '.p'
            # Create output directory if it doesn't exist
            os.makedirs(os.path.dirname(filename), exist_ok=True)
            # Save the equation and solution information to a pickle file
            pickle.dump((eq_list, solution_dict, ""), open(filename, "wb"))
            # Reset eq_list and solution_dict for the next set of files
            eq_list = []
            solution_dict = {b: [] for b in baselines}
                
    if args.file:
        # Create output directory if it doesn't exist
        output_dir = os.path.dirname(args.file)
        if output_dir:
            os.makedirs(output_dir, exist_ok=True)
        pickle.dump((eq_list, solution_dict, ""), open(args.file, "wb"))
