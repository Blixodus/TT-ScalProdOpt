import sys
import os
import configparser
import numpy as np
import math

from Scripts.naming import get_dir, get_test_filename

# --------------------------- Naive cost computation ---------------------------
# Computation of the naive cost for 2 tensor train case
def compute_naive_cost_t2(size, dims, rank1, rank2, reverse):
    if reverse:
        dims = dims[::-1]
        rank1 = rank1[::-1]
        rank2 = rank2[::-1]

    cost_naive_1 = dims[0] * rank1[1] * rank2[1]
    cost_naive_2 = dims[0] * rank1[1] * rank2[1]

    for i in range(1, size):
        cost_naive_1 = cost_naive_1 + rank2[i] * rank1[i] * dims[i] * rank1[i+1] + dims[i] * rank2[i] * rank1[i+1] * rank2[i+1]
        cost_naive_2 = cost_naive_2 + rank1[i] * rank2[i] * dims[i] * rank2[i+1] + dims[i] * rank1[i] * rank2[i+1] * rank1[i+1]

    return min(cost_naive_1, cost_naive_2)

# Computation of the naive cost for 3 tensor train case
def compute_naive_cost_t3(size, dims_X, dims_Xt, rank_X, rank_Xt, rank_A, reverse):
    cost_naive = 0

    if reverse:
        dims_X = dims_X[::-1]
        dims_Xt = dims_Xt[::-1]
        rank_X = rank_X[::-1]
        rank_Xt = rank_Xt[::-1]
        rank_A = rank_A[::-1]

    for i in range(size - 1):
        cost_naive += dims_X[i] * dims_Xt[i] * rank_X[i + 1] * rank_A[i + 1]
        cost_naive += dims_Xt[i] * rank_X[i + 1] * rank_A[i + 1] * rank_Xt[i + 1]
        cost_naive += rank_X[i + 1] * rank_A[i + 1] * rank_Xt[i + 1] * dims_X[i+1] * dims_Xt[i+1]
        dims_X[i + 1] *= rank_X[i + 1]
        dims_Xt[i + 1] *= rank_Xt[i + 1]

    cost_naive += dims_X[size - 1] * dims_Xt[size - 1] + dims_Xt[size - 1]

    return cost_naive

# ----------------------------- Utility functions ------------------------------
def cumulative_product_bounded(values, start, bound):
    result = 1
    for i in range(start, len(values)):
        result *= values[i]
        if result >= bound:
            return bound
    return result

# ------------------------ Generation of test instance -------------------------
def generate_instance(file, problem_type, tt_dim, dimension, y_eq_xT, ranks_gen, dims_gen, max_val, rounded):
    # Generate dimensions and ranks
    dims  = [[int(dims_gen(i * dimension + j)) for j in range(dimension)] for i in range(tt_dim - 1)]
    
    ranks = []
    if problem_type != "increasing":
        ranks = [[int(ranks_gen(i * (dimension - 1) + j)) for j in range(dimension - 1)] for i in range(tt_dim)]
    else:
        ranks = [[int(ranks_gen(j, i)) for j in range(dimension - 1)] for i in range(tt_dim)]

    # For increasing case sort the values to have increasing ranks towards middle
    #if type == "increasing":
    #    for i in range(len(ranks)):
    #        ranks[i].sort()
    #
    #        rank_left = ranks[i][::2]
    #        rank_right = ranks[i][1::2]
    #        rank_right.reverse()
    #
    #        ranks[i] = rank_left + rank_right

    # Add dummy 1 ranks at the beginning and end to simplify output handling
    for i in range(len(ranks)):
        ranks[i] = [1] + ranks[i] + [1]

    # Cumulative product of dimensions
    cumulative_dims = [[cumulative_product_bounded(dims[t], i, max_val) for i in range(1, dimension + 1)] for t in range(tt_dim - 1)]

    # Rounding of TT to reflect stucture generated during SVD decomposition
    if rounded:
        for t in range(tt_dim):
            for i in range(1, dimension + 1):
                if t == 0:
                    ranks[t][i] = min(ranks[t][i], ranks[t][i - 1] * dims[t][i - 1])
                elif t == tt_dim - 1:
                    ranks[t][i] = min(ranks[t][i], ranks[t][i - 1] * dims[t - 1][i - 1])
                else:
                    ranks[t][i] = min(ranks[t][i], ranks[t][i - 1] * dims[t][i - 1] * dims[t - 1][i - 1])
        
        for t in range(tt_dim):
            for i in range(dimension - 1, 0, -1):
                if t == 0:
                    ranks[t][i] = min(ranks[t][i], ranks[t][i + 1] * dims[t][i])
                elif t == tt_dim - 1:
                    ranks[t][i] = min(ranks[t][i], ranks[t][i + 1] * dims[t - 1][i])
                else:
                    ranks[t][i] = min(ranks[t][i], ranks[t][i + 1] * dims[t][i] * dims[t - 1][i])


    # For y=xT case, set the last tensor train to transpose of the first
    if y_eq_xT:
        ranks[-1] = ranks[0][::-1]
        if tt_dim > 2:
            dims[-1] = dims[0][::-1]

    # Compute naive approach cost
    cost_naive_LR = 0
    cost_naive_RL = 0
    if tt_dim == 2:
        cost_naive_LR = compute_naive_cost_t2(dimension, dims[0][:], ranks[0][:], ranks[1][:], reverse=False)
        cost_naive_RL = compute_naive_cost_t2(dimension, dims[0][:], ranks[0][:], ranks[1][:], reverse=True)
    elif tt_dim == 3:
        cost_naive_LR = compute_naive_cost_t3(dimension, dims[0][:], dims[1][:], ranks[0][:], ranks[2][:], ranks[1], reverse=False)
        cost_naive_RL = compute_naive_cost_t3(dimension, dims[0][:], dims[1][:], ranks[0][:], ranks[2][:], ranks[1], reverse=True)


    # Write the naive cost (for 2 and 3 TT case)
    file.write(f"c Naive cost : {cost_naive_LR}\n")
    file.write(f"c Naive cost : {cost_naive_RL}\n")

    # Write visual representation of the TT
    for t in range(tt_dim):
        # Display t-th row of ranks
        file.write("v")
        for i in range(dimension - 1):
            file.write(f"\t{t * dimension + i}\t*{ranks[t][i+1]}")
        file.write(f"\t{(t + 1) * dimension - 1}\n")

        # Diplay t-th row of dims
        if t < tt_dim - 1:
            file.write("v")
            for i in range(dimension):
                file.write(f"\t*{dims[t][i]}\t")
            file.write("\n")

    # Write TT-dim and dimension
    file.write("t {}\n".format(tt_dim))
    file.write("d {}\n".format(dimension))

    # Write ranks and dimensions (in form of edge weights)
    for t in range(tt_dim):
        # Print edges within the row
        for i in range(dimension - 1):
            file.write(f"e {t * dimension + i} {t * dimension + (i + 1)} {ranks[t][i + 1]}\n")

        # Print edges connected to next row
        if t < tt_dim - 1:
            for i in range(dimension):
                file.write(f"e {t * dimension + i} {(t + 1) * dimension + i} {dims[t][i]}\n")

def cos_like_increasing_gen(length, rand, noise_level=0.1):
    centre = int(length/2)
    return lambda x : 5+(1+np.cos(x*2*np.pi/length+np.pi))*(95/2) + rand.integers(low=-(centre-abs(x-centre))-1, high=centre-abs(x-centre)+1)

def cos_like_increasing_gen2(length, max_val, rand, problem_size):
    # Generate random peak location
    center_offset_1 = 0.25 * length + rand.uniform(-length / 10, length / 10)
    center_offset_2 = 0.50 * length + rand.uniform(-length / 10, length / 10)
    center_offset_3 = 0.75 * length + rand.uniform(-length / 10, length / 10)

    # Prepare scaling function
    def scale_value(y, new_min=2, new_max=max_val):
        y = max(-1, min(1, y))
        return (new_max - new_min) * (y + 1) / 2 + new_min

    # Generate cosine-like increasing wave
    def gen_value(x, row):
        if row < 1:
            return scale_value(math.cos((x - center_offset_1) * (2 * math.pi / length))  + rand.uniform(-0.4, 0.4))
        elif row < 2 and problem_size > 2:
            return scale_value(math.cos((x - center_offset_2) * (2 * math.pi / length))  + rand.uniform(-0.4, 0.4))
        else:
            return scale_value(math.cos((x - center_offset_3) * (2 * math.pi / length))  + rand.uniform(-0.4, 0.4))

    return gen_value


# ------------------------------- Main function --------------------------------
if __name__ == "__main__":
    # Load configuration file
    if len(sys.argv) > 1:
        config_file = sys.argv[1]
        if not os.path.exists(config_file):
            exit("Error! Provided configuration file does not exist.")
    else:
        exit("Error! No configuration file provided.")

    # Read configuration file
    config = configparser.ConfigParser()
    config.read(config_file)

    # Prepare test directory path
    test_dir = config['Tests']['test_dir']
    if not os.path.exists(test_dir):
        os.makedirs(test_dir)

    # Retrieve test cases parameters
    tt_dims = [int(tt_dim) for tt_dim in config['General']['tt_dims'].split(',')]
    max_size = int(config['Tests']['max_size'])
    nb_instances = int(config['Tests']['nb_instances'])

    types = config['General']['types'].split(',')
    rank_types = config['General']['rank_types'].split(',')
    const_dim = int(config['Tests']['const_dim'])

    # Retrieve seed
    seed = int(config['Tests']['seed'])

    # Generation of the test cases
    case_no = 1
    for tt_dim in tt_dims:
        # Consider case y=xT for smaller TT dimensions
        y_cases = [False]
        if tt_dim <= 3:
            y_cases = [True, False]

        for y_eq_xT in y_cases:
            for type in types:
                for rank_type in rank_types:
                    # Determine max rank based on rank type
                    max_rank = 0
                    if rank_type == 'low':
                        max_rank = 50
                    elif rank_type == 'medium':
                        max_rank = 200
                    elif rank_type == 'high':
                        max_rank = 1000
                    else:
                        exit(f"Error! Invalid rank type {rank_type}.")

                    # Prepare random generators
                    rand = np.random.default_rng(seed=seed)
                    ranks_gen = lambda x : rand.integers(low=2, high=(max_rank + 1))

                    if type in ["increasing", "quantized"]:
                        dims_gen  = lambda x : const_dim
                    elif type in ["random"]:
                        dims_gen  = lambda x : rand.integers(low=2, high=51)
                    else:
                        exit(f"Error! Invalid type {type}.")

                    # Prepare directory for test files
                    dir = get_dir(test_dir, tt_dim, y_eq_xT, type, rank_type)

                    # Generate test files for given parameters
                    print(f"[Info {case_no}] Generating test cases: TT-dim={tt_dim}, (y=xT)={y_eq_xT}, type={type}, rank_type={rank_type}")
                    case_no += 1
                    for dimension in range(3, max_size + 1):
                        for instance in range(1, nb_instances + 1):
                            if type == "increasing":
                                ranks_gen = cos_like_increasing_gen2(dimension, max_rank, rand, tt_dim)

                            filename = get_test_filename(dir, dimension, instance)
                            test_file = open(filename, "w")
                            generate_instance(test_file, type, tt_dim, dimension, y_eq_xT, ranks_gen, dims_gen, max_rank, True)
                            test_file.close()