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
    if dim_min is not None and dim_max is not None:
        nodes_up = nodes_up[dim_min:dim_max]
        nodes_down = nodes_down[dim_min:dim_max]
        n = dim_max - dim_min

    # Left side of tensor train
    # O--
    # |
    # O--
    einsum_str = "ab,ac,"

    inputs = [['a', 'b'], ['a', 'c']]
    output = []
    sizes_dict = {}

    if n == 1:
        einsum_str = "a,a,"
        inputs = [['a'], ['a']]

    #print(nodes_up, nodes_down)
    sizes_dict['a'] = ranks[(nodes_up[0], nodes_down[0])]

    # Middle part of tensor train
    for i in range(1, n - 1):
        # set the upper left/right, middle and lower left/right indices
        # --O--
        #   |
        # --O--
        j = 3 * i
        ul, ur, m, ll, lr = (ctg.get_symbol(i)
                             for i in (j - 1, j + 2, j, j - 2, j + 1))
        einsum_str += "{}{}{},{}{}{},".format(m, ul, ur, m, ll, lr)
        inputs.append([m, ul, ur])
        inputs.append([m, ll, lr])

        sizes_dict[m] = ranks[(nodes_up[i], nodes_down[i])]

        sizes_dict[ul] = ranks[(nodes_down[i], nodes_down[i - 1])]
        sizes_dict[ur] = ranks[(nodes_down[i], nodes_down[i + 1])]

        sizes_dict[ll] = ranks[(nodes_up[i], nodes_up[i - 1])]
        sizes_dict[lr] = ranks[(nodes_up[i], nodes_up[i + 1])]



    # Right side of tensor train
    # --O
    #   |
    # --O
    if n >= 2:
        i = n - 1
        j = 3 * i
        ul, m, ll, =  (ctg.get_symbol(i) for i in (j - 1, j, j - 2))
        einsum_str += "{}{},{}{}".format(m, ul, m, ll)
        inputs.append([m, ul])
        inputs.append([m, ll])

        sizes_dict[m] = ranks[(nodes_up[i], nodes_down[i])]
        if n == 2:
            sizes_dict[ul] = ranks[(nodes_down[i], nodes_down[i - 1])]
            sizes_dict[ll] = ranks[(nodes_up[i], nodes_up[i - 1])]
            sizes_dict[m] = ranks[(nodes_up[i], nodes_down[i])]

    return inputs, output, sizes_dict, einsum_str


def cotengra_wrapper_solve(input_file, dim_min, dim_max):
    #print("[Cotengra wrapper PY]", input_file, dim_min, "...", dim_max)

    inputs, output, sizes_dict, _ = import_tensor_train(input_file, dim_min, dim_max)
    tree = ctg.array_contract_tree(inputs, output, sizes_dict, optimize='optimal')

    return tree.contraction_cost()

#print(cotengra_wrapper_solve("/home/pdominik/Tensor_experiments/OptiTenseurs/instances/test/uniform/instance_03_00.txt", 1))