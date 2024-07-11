import re
import time
import sys

import cotengra as ctg
from cgreedy import CGreedy

from tools.Scripts.import_file import import_tensor_train
from tools.Scripts.contraction_list import generate_contraction_list


def node_id_from_ctg(node_id, dim, dmin, dmax):
    len = dmax - dmin + 1
    row = node_id // len
    column = node_id % len + dmin

    return row * dim + column


def node_id_from_ctg_ij(node_id, dim, i_min, j_min, i_max, j_max):
    upper_length = i_max - i_min + 1
    if node_id < upper_length:
        return node_id + i_min
    else:
        return (node_id - upper_length) + dim + j_min


def rename_nodes(contraction_tree, dim, dmin, dmax, input_node_included):
    if isinstance(contraction_tree, tuple):
        return (rename_nodes(contraction_tree[0], dim, dmin, dmax, input_node_included), rename_nodes(contraction_tree[1], dim, dmin, dmax, input_node_included))
    else:
        if not input_node_included:
            return node_id_from_ctg(contraction_tree, dim, dmin, dmax)
        else:
            if contraction_tree == 0:
                return '#'
            else:
                return node_id_from_ctg(contraction_tree - 1, dim, dmin, dmax)
            

def rename_nodes_ij(contraction_tree, dim, i_min, j_min, i_max, j_max, input_node_included):
    if isinstance(contraction_tree, tuple):
        return (rename_nodes(contraction_tree[0], dim, i_min, j_min, i_max, j_max, input_node_included), rename_nodes(contraction_tree[1], dim, i_min, j_min, i_max, j_max, input_node_included))
    else:
        if not input_node_included:
            return node_id_from_ctg_ij(contraction_tree, dim, i_min, j_min, i_max, j_max)
        else:
            if contraction_tree == 0:
                return '#'
            else:
                return node_id_from_ctg_ij(contraction_tree - 1, dim, i_min, j_min, i_max, j_max)


def cotengra_wrapper_solve_with_args(algorithm, inputs, output, sizes_dict, dim, dim_min, dim_max, input_node_included):
    # Compute the contraction ordering for given arguments
    algorithm_str = algorithm
    if algorithm == 'cgreedy':
        algorithm = CGreedy(seed=1, minimize="flops", max_repeats=1024, max_time=1.0, progbar=False, threshold_optimal=12, threads=1)
    elif algorithm == 'hyper-greedy':
        algorithm = ctg.HyperOptimizer(methods=["greedy"], minimize="flops", parallel=False)
    elif algorithm == 'hyper-kahypar':
        algorithm = ctg.HyperOptimizer(methods=["kahypar"], minimize="flops", parallel=False)
    elif algorithm == 'quickbb-2':
        algorithm = ctg.QuickBBOptimizer()
    elif algorithm == 'flowcutter':
        algorithm = ctg.FlowCutterOptimizer()

    if ctg.__version__ != "0.2.0":
        tree = ctg.array_contract_tree(inputs, output, sizes_dict, optimize=algorithm)
    else:
        if algorithm_str == 'quickbb-2' or algorithm_str == 'flowcutter':
            inputs = [set(node_indices_str) for node_indices_str in inputs]
            tree = algorithm.build_tree(inputs, output, sizes_dict)
        else:
            tree = algorithm.search(inputs, output, sizes_dict)

    # Rename the nodes to match the original tensor train
    contraction_tree = tree.flat_tree()
    contraction_tree = rename_nodes(contraction_tree, dim, dim_min, dim_max, input_node_included)
    path_str = str(contraction_tree)

    #print(tree.contraction_cost(), path_str)
    return (int(tree.contraction_cost()), path_str)


def cotengra_wrapper_solve_ij(algorithm, inputs, output, sizes_dict, dim, i_min, j_min, i_max, j_max, input_node_included):
    # Compute the contraction ordering for given arguments
    algorithm_str = algorithm
    if algorithm == 'cgreedy':
        algorithm = CGreedy(seed=1, minimize="flops", max_repeats=1024, max_time=1.0, progbar=False, threshold_optimal=12, threads=1)
    elif algorithm == 'hyper-greedy':
        algorithm = ctg.HyperOptimizer(methods=["greedy"], minimize="flops", parallel=False)
    elif algorithm == 'hyper-kahypar':
        algorithm = ctg.HyperOptimizer(methods=["kahypar"], minimize="flops", parallel=False)
    elif algorithm == 'quickbb-2':
        algorithm = ctg.QuickBBOptimizer()
    elif algorithm == 'flowcutter':
        algorithm = ctg.FlowCutterOptimizer()

    if ctg.__version__ != "0.2.0":
        tree = ctg.array_contract_tree(inputs, output, sizes_dict, optimize=algorithm)
    else:
        if algorithm_str == 'quickbb-2' or algorithm_str == 'flowcutter':
            inputs = [set(node_indices_str) for node_indices_str in inputs]
            tree = algorithm.build_tree(inputs, output, sizes_dict)
        else:
            tree = algorithm.search(inputs, output, sizes_dict)

    # Rename the nodes to match the original tensor train
    contraction_tree = tree.flat_tree()
    contraction_tree = rename_nodes_ij(contraction_tree, dim, i_min, j_min, i_max, j_max, input_node_included)
    path_str = str(contraction_tree)

    #print(tree.contraction_cost(), path_str)
    return (int(tree.contraction_cost()), path_str)


#print(cotengra_wrapper_solve("/home/pdominik/Tensor_experiments/Tests2/xy/random/high/d_006_v_008.txt", 0, 5, 6, False))