import sys
import os
import pickle
import timeit
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
    # Load configuration file
    input_dir = False
    output_dir = False
    if len(sys.argv) > 2:
        input_dir = sys.argv[1]
        output_dir = sys.argv[2]
        os.makedirs(output_dir, exist_ok=True)
        if not os.path.exists(input_dir):
            exit("Error! Provided directories do not exist.")
    elif len(sys.argv) > 1:
        input_dir = sys.argv[1]
        if not os.path.exists(input_dir):
            exit("Error! Provided directory does not exist.")
    else:
        exit("Error! No directory provided.")

    counter = 0
    eq_list = []
    baselines = ['ctg_greedy', 'oe_greedy', 'ctg_kahypar']
    solution_dict = {b: [] for b in baselines}
    for file_path in Path(input_dir).rglob('*'):
        if not file_path.is_file():
            continue
        if output_dir:
            relative_path = file_path.relative_to(input_dir)
            filename = os.path.join(output_dir, relative_path)
            filename = os.path.splitext(filename)[0] + '.p'
            if os.path.isfile(filename):
                continue
        if int(os.path.splitext(file_path.name)[0].split('_')[3]) > 25:
            continue
        counter += 1
        # Import tensor train from file (note : output is empty list in scalar product)
        inputs, output, sizes_dict, input_node_included = import_tensor_train(file_path)
        # Convert the tensor train description into rl-tnco input
        equation = ",".join("".join(sublist) for sublist in inputs)
        eq = f'{equation}->'
        shapes = [tuple(sizes_dict[char] for char in sublist) for sublist in inputs]
        size_dict = OrderedDict(sorted(sizes_dict.items()))
        eq_list.append((eq, shapes, size_dict))
        for baseline in baselines:
            contraction_cost, solver_time, info, path = solve_eq(eq, shapes, baseline=baseline)
            solution_dict[baseline].append((contraction_cost, solver_time, info, path))
        if output_dir:
            relative_path = file_path.relative_to(input_dir)
            filename = os.path.join(output_dir, relative_path)
            filename = os.path.splitext(filename)[0] + '.p'
            os.makedirs(os.path.dirname(filename), exist_ok=True)
            pickle.dump((eq_list, solution_dict, ""), open(filename, "wb"))
            eq_list = []
            solution_dict = {b: [] for b in baselines}
                
    if not output_dir:
        filename = f"scalar_product_dataset_num_eqs_{counter}_num_node_{100}_mean_conn_{3}.p"
        pickle.dump((eq_list, solution_dict, ""), open(filename, "wb"))
