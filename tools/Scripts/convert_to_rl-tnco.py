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
    if len(sys.argv) > 1:
        training_dir = sys.argv[1]
        if not os.path.exists(training_dir):
            exit("Error! Provided training directory does not exist.")
    else:
        exit("Error! No training directory provided.")

    counter = 0
    eq_list = []
    for file_path in Path(training_dir).rglob('*'):
        if file_path.is_file():
            counter += 1
            # Import tensor train from file (note : output is empty list in scalar product)
            inputs, output, sizes_dict, input_node_included = import_tensor_train(file_path)
            # Convert the tensor train description into rl-tnco input
            equation = ",".join("".join(sublist) for sublist in inputs)
            list_of_sizes = [tuple(sizes_dict[char] for char in sublist) for sublist in inputs]
            ordered_size_dict = OrderedDict(sorted(sizes_dict.items()))
            eq_list.append((f'{equation}->', list_of_sizes, ordered_size_dict))

    baselines = ['ctg_greedy']
    solution_dict = {b: [] for b in baselines}
    for (eq, shapes, size_dict) in eq_list:
        for baseline in baselines:
            contraction_cost, solver_time, info, path = solve_eq(eq, shapes, baseline=baseline)
            solution_dict[baseline].append((contraction_cost, solver_time, info, path))

    filename = f"scalar_product_2D_dataset_num_eqs_{counter}_num_node_{100}_mean_conn_{3}.p"
    pickle.dump((eq_list, solution_dict, ""), open(filename, "wb"))
    
    print(eq_list)
    print(solution_dict)
    print(counter)
