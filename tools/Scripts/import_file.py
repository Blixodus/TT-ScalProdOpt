import re
import cotengra as ctg

def import_tensor_train(filename, dim_min=None, dim_max=None, reversed=False):
    # Import text file with tensor train description
    text_file = open(filename, "r")
    lines = text_file.readlines()
    text_file.close()

    # Locate the dimension of the tensor train
    n = 0
    tt_dim = 2 # for backward compatibility with older test cases
    for line in lines:
        if line.startswith('d'):
            n = int(re.split('\W+', line)[1])
            break
    
    for line in lines:
        if line.startswith('t'):
            tt_dim = int(re.split('\W+', line)[1])
            break

    if dim_min == None:
        dim_min = 0

    if dim_max == None:
        dim_max = n - 1

    # Parse the weight of each dimension and rank
    weight = {}
    for line in lines:
        if line.startswith('e'):
            elements = re.split(' |\t', line)
            a = int(elements[1])
            b = int(elements[2])
            w = int(elements[3])
            weight[(a, b)] = w
            weight[(b, a)] = w

    # Generate node ids for each row and column
    nodes = [[n * r + c for c in range(n)] for r in range(tt_dim)]

    # Generate symbols for each edge
    move_x = [0, 1, 0, -1]
    move_y = [1, 0, -1, 0]

    symbols = {}
    symbol_count = 0
    sizes_dict = {}

    for i in range(tt_dim):
        for j in range(n):
            for k in range(4):
                x = i + move_x[k]
                y = j + move_y[k]
                if x >= 0 and x < tt_dim and y >= 0 and y < n:
                    if ((i, j), (x, y)) not in symbols:
                        symbols[((i, j), (x, y))] = ctg.get_symbol(symbol_count)
                        symbols[((x, y), (i, j))] = symbols[((i, j), (x, y))]
                        sizes_dict[symbols[((i, j), (x, y))]] = weight[(nodes[i][j], nodes[x][y])]
                        symbol_count += 1

    # Generate tensor train 
    inputs = []
    output = []
    input_node_included = False

    # Generate the input node (representing the part of TT which is already
    # contracted prior to this subproblem) and the output node (representing 
    # the part of TT which is result of this computation)

    # Add outer edges from the left size to include them in the cost
    if dim_min > 0:
        edges = []
        for i in range(tt_dim):
            edges.append(symbols[((i, dim_min - 1), (i, dim_min))])

        if reversed == False:
            inputs.append(edges)
            input_node_included = True
        else:
            output += edges

    # Add outer edges from the right size to include them in the cost
    if dim_max + 1 < n:
        edges = []
        for i in range(tt_dim):
            edges.append(symbols[((i, dim_max), (i, dim_max + 1))])
        if reversed == False:
            output += edges
        else:
            inputs.append(edges)
            input_node_included = True

    # Set the upper left/right, middle and lower left/right indices
    #   |
    # --O--
    #   |
    for i in range(tt_dim):
        for j in range(dim_min, dim_max + 1):
            edges = []
            for k in range(4):
                x = i + move_x[k]
                y = j + move_y[k]
                if x >= 0 and x < tt_dim and y >= max(0, dim_min - 1) and y <= min(n - 1, dim_max + 1):
                    edges.append(symbols[((i, j), (x, y))])
            
            inputs.append(edges)

    #print(inputs, output, sizes_dict, input_node_included)
    return inputs, output, sizes_dict, input_node_included