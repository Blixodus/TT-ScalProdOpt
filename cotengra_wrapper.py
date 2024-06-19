import re
import time
import sys
import cotengra as ctg

def import_tensor_train(filename, dim_min=None, dim_max=None, reversed=False):
    # Import text file with tensor train description
    text_file = open(filename, "r")
    lines = text_file.readlines()
    text_file.close()

    # Locate the dimension of the tensor train
    n = 0
    for line in lines:
        if line.startswith('d'):
            n = int(re.split('\W+', line)[1])
            break

    # Parse the ranks of each dimension
    ranks = {}
    for line in lines:
        if line.startswith('e'):
            elements = re.split(' |\t', line)
            a = int(elements[1])
            b = int(elements[2])
            rank = int(elements[3])
            ranks[(a, b)] = rank
            ranks[(b, a)] = rank

    #print(ranks)

    # Locate node names for upper and lower indices
    line_up = ""
    line_down = ""
    for i in range(len(lines) - 2):
        if lines[i].startswith('v') and lines[i + 1].startswith('v') and lines[i + 2].startswith('v'):
            line_up = lines[i]
            line_down = lines[i + 2]

    # Parse the node names for each tensor train
    nodes_up = []
    nodes_down = []
    for node in re.split(' |\t', line_up):
        #print("!!!", node, "!!!")
        if not node.startswith('*') and not node.startswith('v') and node != '':
            nodes_up.append(int(node))

    for node in re.split(' |\t', line_down):
        if not node.startswith('*') and not node.startswith('v') and node != '':
            nodes_down.append(int(node))


    # Generate tensor train 
    inputs = []
    output = []
    sizes_dict = {}
    input_node_included = False

    # Generate the input node (representing the part of TT which is already
    # contracted prior to this subproblem) and the output node (representing 
    # the part of TT which is result of this computation)

    # Add outer edges from the left size to include them in the cost
    if dim_min > 0:
        i = dim_min
        j = 3 * i
        ul, ur, m, ll, lr = (ctg.get_symbol(i) for i in (j - 1, j + 2, j, j - 2, j + 1))
        if reversed == False:
            inputs.append([ul, ll])
            input_node_included = True
        else:
            output.append(ul)
            output.append(ll)

    # Add outer edges from the right size to include them in the cost
    if dim_max < n:
        i = dim_max - 1
        j = 3 * i
        ul, ur, m, ll, lr = (ctg.get_symbol(i) for i in (j - 1, j + 2, j, j - 2, j + 1))
        if reversed == False:
            output.append(ur)
            output.append(lr)
        else:
            inputs.append([ur, lr])
            input_node_included = True

    for i in range(dim_min, dim_max):
        # set the upper left/right, middle and lower left/right indices
        # --O--
        #   |
        # --O--
        j = 3 * i
        ul, ur, m, ll, lr = (ctg.get_symbol(i)
                             for i in (j - 1, j + 2, j, j - 2, j + 1))
        # Tensor train of dimension 1 with no outer edges
        if i == dim_min and i == dim_max and dim_min == 0:
            inputs = [[m]]
            sizes_dict[m] = ranks[(nodes_up[i], nodes_down[i])]
        # Left size of the tensor train with no outer edges
        elif i == dim_min and dim_min == 0:
            inputs.append([m, ur])
            inputs.append([m, lr])

            sizes_dict[m] = ranks[(nodes_up[i], nodes_down[i])]
            sizes_dict[ur] = ranks[(nodes_up[i], nodes_up[i + 1])]
            sizes_dict[lr] = ranks[(nodes_down[i], nodes_down[i + 1])]
        # Right size of the tensor train with no outer edges
        elif i == dim_max - 1 and dim_max == n:
            inputs.append([m, ul])
            inputs.append([m, ll])

            sizes_dict[m] = ranks[(nodes_up[i], nodes_down[i])]
            sizes_dict[ul] = ranks[(nodes_up[i], nodes_up[i - 1])]
            sizes_dict[ll] = ranks[(nodes_down[i], nodes_down[i - 1])]
        else:
            inputs.append([m, ul, ur])
            inputs.append([m, ll, lr])

            sizes_dict[m] = ranks[(nodes_up[i], nodes_down[i])]

            sizes_dict[ul] = ranks[(nodes_up[i], nodes_up[i - 1])]
            sizes_dict[ur] = ranks[(nodes_up[i], nodes_up[i + 1])]

            sizes_dict[ll] = ranks[(nodes_down[i], nodes_down[i - 1])]
            sizes_dict[lr] = ranks[(nodes_down[i], nodes_down[i + 1])]

    return inputs, output, sizes_dict, input_node_included

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
    print(contraction_list_renamed)

    #tree2 = ctg.array_contract_tree(inputs, output, sizes_dict, optimize=tree.flat_tree())
    #print(tree2.contraction_cost())

    path_str = str(contraction_list_renamed)
    #print(path_str)

    #print(sizes_dict, output)
    #print("[Cotengra wrapper PY]", input_file, dim_min, "...", dim_max, tree.contraction_cost() * outer_edges_cost, tree.contraction_cost(), outer_edges_cost)
    
    return (tree.contraction_cost(), path_str)

#print(cotengra_wrapper_solve("/home/pdominik/Tensor_experiments/OptiTenseurs/instances/test/uniform_all2/instance_006_01.txt", 0, 5, 6, False))