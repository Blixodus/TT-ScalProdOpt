import re
import time
import sys
import cotengra as ctg

def import_tensor_train(filename, dim_min=None, dim_max=None):
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

    print(ranks)

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


    # Limit the tensor network to given dimension
    output = []
    sizes_dict = {}
    if False and dim_min is not None and dim_max is not None:
        # Add outer edges from the left size to include them in the cost
        if dim_min > 0:
            i = dim_min - 1
            j = 3 * i
            ul, m, ll, =  (ctg.get_symbol(i) for i in (j - 1, j, j - 2))
            output.append(ul)
            output.append(ll)

            sizes_dict[ll] = ranks[(nodes_up[dim_min - 1], nodes_up[dim_min])]
            sizes_dict[ul] = ranks[(nodes_down[dim_min - 1], nodes_down[dim_min])]
        
        # Add outer edges from the right size to include them in the cost
        if dim_max < n:
            i = dim_max
            j = 3 * i
            ul, m, ll, =  (ctg.get_symbol(i) for i in (j - 1, j, j - 2))
            output.append(ul)
            output.append(ll)

            sizes_dict[ll] = ranks[(nodes_up[dim_max - 1], nodes_up[dim_max])]
            sizes_dict[ul] = ranks[(nodes_down[dim_max - 1], nodes_down[dim_max])]

        #nodes_up = nodes_up[dim_min:dim_max]
        #nodes_down = nodes_down[dim_min:dim_max]
        #n = dim_max - dim_min

    # Left side of tensor train
    # O--
    # |
    # O--
    #j = 3 * dim_min
    #ul, ur, m, ll, lr = (ctg.get_symbol(i) for i in (j - 1, j + 2, j, j - 2, j + 1))
    #inputs = [[m, ur], [m,lr]]
    
    #if n == 1:
    #    inputs = [[m], [m]]

    #print(nodes_up, nodes_down)
    #sizes_dict['a'] = ranks[(nodes_up[0], nodes_down[0])]

    # Middle part of tensor train
    inputs = []
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
            sizes_dict[ur] = ranks[(nodes_down[i], nodes_down[i + 1])]
            sizes_dict[lr] = ranks[(nodes_up[i], nodes_up[i + 1])]
        # Right size of the tensor train with no outer edges
        elif i == dim_max - 1 and dim_max == n:
            inputs.append([m, ul])
            inputs.append([m, ll])

            sizes_dict[m] = ranks[(nodes_up[i], nodes_down[i])]
            sizes_dict[ul] = ranks[(nodes_down[i], nodes_down[i - 1])]
            sizes_dict[ll] = ranks[(nodes_up[i], nodes_up[i - 1])]
        else:
            inputs.append([m, ul, ur])
            inputs.append([m, ll, lr])

            sizes_dict[m] = ranks[(nodes_up[i], nodes_down[i])]

            sizes_dict[ul] = ranks[(nodes_down[i], nodes_down[i - 1])]
            sizes_dict[ur] = ranks[(nodes_down[i], nodes_down[i + 1])]

            sizes_dict[ll] = ranks[(nodes_up[i], nodes_up[i - 1])]
            sizes_dict[lr] = ranks[(nodes_up[i], nodes_up[i + 1])]

        # Add outer edges from the left size to include them in the cost
        if i == dim_min and dim_min > 0:
            output.append(ul)
            output.append(ll)

        # Add outer edges from the right size to include them in the cost
        if i == dim_max - 1 and dim_max < n:
            output.append(ur)
            output.append(lr)



    # Right side of tensor train
    # --O
    #   |
    # --O
    if False and n >= 2:
        i = n - 1
        j = 3 * i
        ul, m, ll, =  (ctg.get_symbol(i) for i in (j - 1, j, j - 2))
        #print(ul, m, ll)
            
        inputs.append([m, ul])
        inputs.append([m, ll])

        sizes_dict[m] = ranks[(nodes_up[i], nodes_down[i])]
        if n == 2:
            sizes_dict[ul] = ranks[(nodes_down[i], nodes_down[i - 1])]
            sizes_dict[ll] = ranks[(nodes_up[i], nodes_up[i - 1])]
            sizes_dict[m] = ranks[(nodes_up[i], nodes_down[i])]

    return inputs, output, sizes_dict


def cotengra_wrapper_solve(input_file, dim_min, dim_max):
    inputs, output, sizes_dict = import_tensor_train(input_file, dim_min, dim_max)

    #print(inputs, output, sizes_dict)

    #fig, ax = ctg.HyperGraph(inputs, output, sizes_dict).plot()
    #fig.savefig("graph.png")
    tree = ctg.array_contract_tree(inputs, output, sizes_dict, optimize='optimal')

    #fig, ax = tree.plot_rubberband()
    #fig.savefig("tree.png")

    print(sizes_dict, output)
    #print("[Cotengra wrapper PY]", input_file, dim_min, "...", dim_max, tree.contraction_cost() * outer_edges_cost, tree.contraction_cost(), outer_edges_cost)

    return tree.contraction_cost()

#print(cotengra_wrapper_solve("/home/pdominik/Tensor_experiments/OptiTenseurs_dev/small_test_2.txt", 0, 2))