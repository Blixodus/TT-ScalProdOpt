import re
import time
import sys
import cotengra as ctg

from tools.Scripts.import_file import import_tensor_train


def node_id_from_ctg(node_id, dim, dmin, dmax):
    len = dmax - dmin + 1
    row = node_id // len
    column = node_id % len + dmin

    #print(node_id, "--->", row, column, row * dim + column)
    return row * dim + column


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
        

def cotengra_wrapper_solve(input_file, dim_min, dim_max, dim, reversed):
    inputs, output, sizes_dict, input_node_included = import_tensor_train(input_file, dim_min, dim_max, reversed)

    #print(inputs, output, sizes_dict)

    #fig, ax = ctg.HyperGraph(inputs, output, sizes_dict).plot()
    #fig.savefig("graph_2d.pdf")
    tree = ctg.array_contract_tree(inputs, output, sizes_dict, optimize='optimal')

    #fig, ax = tree.plot_rubberband()
    #fig.savefig("tree_2d.pdf")

    #print(tree.flat_tree())
    #contraction_list, _ = generate_contraction_list(tree.flat_tree())
    #print(contraction_list)

    #contraction_list_renamed = rename_nodes_from_ctg(tree.flat_tree(), 2, dim, dim_min, input_node_included)
    #print(contraction_list_renamed)

    #tree2 = ctg.array_contract_tree(inputs, output, sizes_dict, optimize=tree.flat_tree())
    #print(tree2.contraction_cost())

    contraction_tree = tree.flat_tree()
    #print(contraction_tree)
    contraction_tree = rename_nodes(contraction_tree, dim, dim_min, dim_max, input_node_included)
    #print(contraction_tree)
    path_str = str(contraction_tree) #str(contraction_list_renamed)
    #print(path_str)

    #print(tree.flat_tree())

    #print(sizes_dict, output)
    #print("[Cotengra wrapper PY]", input_file, dim_min, "...", dim_max, tree.contraction_cost() * outer_edges_cost, tree.contraction_cost(), outer_edges_cost)
    
    #print("[Cotengra wrapper PY]", input_file, dim_min, "...", dim_max, dim, tree.contraction_cost(), path_str)
    return (tree.contraction_cost(), path_str)

#print(cotengra_wrapper_solve("/home/pdominik/Tensor_experiments/OptiTenseurs_2d/instances/test/xAxt/instance_005_001.txt", 1, 4, 5, False))