import re
import time
import sys
import cotengra as ctg

from tools.Scripts.import_file import import_tensor_train


def node_id_from_ctg(node_id, tt_dim, dim, dmin, input_node_included):
    if input_node_included:
        if node_id == 0:
            return '#'
        else:
            node_id -= 1

    column = node_id // tt_dim
    row = node_id % tt_dim
    return row * dim + dmin + column

def rename_nodes_from_ctg(contraction_tree, tt_dim, dim, dmin, input_node_included):
    if isinstance(contraction_tree, tuple):
        return (rename_nodes_from_ctg(contraction_tree[0], tt_dim, dim, dmin, input_node_included), rename_nodes_from_ctg(contraction_tree[1], tt_dim, dim, dmin, input_node_included))
    else:
        return node_id_from_ctg(contraction_tree, tt_dim, dim, dmin, input_node_included)

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
    inputs, output, sizes_dict, input_node_included = import_tensor_train(input_file, dim_min, dim_max + 1, reversed)

    #print(inputs, output, sizes_dict)

    #fig, ax = ctg.HyperGraph(inputs, output, sizes_dict).plot()
    #fig.savefig("graph_4.pdf")
    tree = ctg.array_contract_tree(inputs, output, sizes_dict, optimize='optimal')

    #fig, ax = tree.plot_rubberband()
    #fig.savefig("tree_4.pdf")

    #print(tree.flat_tree())
    #contraction_list, _ = generate_contraction_list(tree.flat_tree())
    #print(contraction_list)

    contraction_list_renamed = rename_nodes_from_ctg(tree.flat_tree(), 2, dim, dim_min, input_node_included)
    #print(contraction_list_renamed)

    #tree2 = ctg.array_contract_tree(inputs, output, sizes_dict, optimize=tree.flat_tree())
    #print(tree2.contraction_cost())

    path_str = str(contraction_list_renamed)
    #print(path_str)

    #print(sizes_dict, output)
    #print("[Cotengra wrapper PY]", input_file, dim_min, "...", dim_max, tree.contraction_cost() * outer_edges_cost, tree.contraction_cost(), outer_edges_cost)
    
    return (tree.contraction_cost(), path_str)

#print(cotengra_wrapper_solve("/home/pdominik/Tensor_experiments/OptiTenseurs/instances/test/uniform_all2/instance_006_01.txt", 0, 5, 6, False))