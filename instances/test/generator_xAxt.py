import numpy as np

def compute_naive_cost(size, dims_X, dims_Xt, rank_X, rank_Xt, rank_A, reverse=False):
    cost_naive = 0
    if reverse:
        dims_X = dims_X[::-1]
        dims_Xt = dims_Xt[::-1]
        rank_X = rank_X[::-1]
        rank_Xt = rank_Xt[::-1]
        rank_A = rank_A[::-1]
    for i in range(size - 1):
        cost_naive += dims_X[i] * dims_Xt[i] * rank_X[i] * rank_A[i]
        cost_naive += dims_Xt[i] * rank_X[i] * rank_A[i] * rank_Xt[i]
        cost_naive += rank_X[i] * rank_A[i] * rank_Xt[i] * dims_X[i+1] * dims_Xt[i+1]
        dims_X[i+1] *= rank_X[i]
        dims_Xt[i+1] *= rank_Xt[i]
    return cost_naive

def generate_instance(file, size, instance, f_rank1, f_rank2, f_dims, rounded):
    # Generate dimensions and ranks
    dims_X = [int(f_dims(i)) for i in range(size)]
    dims_Xt = dims_X[::-1]
    rank_X = [1] + [int(f_rank1(i)) for i in range(size-1)] + [1]
    rank_Xt = rank_X[::-1]
    rank_A = [1] + [int(f_rank2(i)) for i in range(size-1)] + [1]

    # "Rounding" of TT
    if(rounded):
        for i in range(size-1):
            rank_X[i+1] = min(rank_X[i+1], rank_X[i]*dims_X[i])
            rank_Xt[i+1] = min(rank_Xt[i+1], rank_Xt[i]*dims_Xt[i])
            rank_A[i+1] = min(rank_A[i+1], rank_A[i]*dims_X[i]*dims_Xt[i])
        for i in range(size-1):
            rank_X[-i-2] = min(rank_X[-i-2], rank_X[-i-1]*dims_X[i])
            rank_Xt[-i-2] = min(rank_Xt[-i-2], rank_Xt[-i-1]*dims_Xt[i])
            rank_A[-i-2] = min(rank_A[-i-2], rank_A[-i-1]*dims_X[i]*dims_Xt[i])

    # Compute naive approach cost
    cost_naive_1 = compute_naive_cost(size, dims_X[:], dims_Xt[:], rank_X[:], rank_Xt[:], rank_A[:])
    cost_naive_2 = compute_naive_cost(size, dims_X[:], dims_Xt[:], rank_X[:], rank_Xt[:], rank_A[:], reverse=True)

    # Print TT product
    file.write("c Naive cost : {}\n".format(cost_naive_1))
    file.write("c Naive cost : {}\n".format(cost_naive_2))
    file.write("v")
    for i in range(size-1):
        file.write("\t{}\t*{}".format(i, rank_X[i+1]))
    file.write("\t{}\nv".format(size-1))
    for i in range(size):
        file.write("\t*{}\t".format(dims_X[i]))
    file.write("\nv")
    for i in range(size-1):
        file.write("\t{}\t*{}".format(size+i, rank_A[i+1]))
    file.write("\t{}\nv".format(2*size-1))
    for i in range(size):
        file.write("\t*{}\t".format(dims_Xt[i]))
    file.write("\nv")
    for i in range(size-1):
        file.write("\t{}\t*{}".format(2*size+i, rank_Xt[i+1]))
    file.write("\t{}\n".format(3*size-1))

    file.write("d {}\n".format(size))
    for i in range(size-1):
        file.write("e {} {} {}\n".format(i, i+1, rank_X[i+1]))
    for i in range(size):
        file.write("e {} {} {}\n".format(i, size+i, dims_X[i]))
    for i in range(size-1):
        file.write("e {} {} {}\n".format(size+i, size+i+1, rank_A[i+1]))
    for i in range(size):
        file.write("e {} {} {}\n".format(size+i, 2*size+i, dims_Xt[i]))
    for i in range(size-1):
        file.write("e {} {} {}\n".format(2*size + i, 2*size + (i+1), rank_Xt[i+1]))


if __name__ == "__main__":
    rng = np.random.default_rng(seed=1000)
    random_1_10 = lambda x : rng.integers(low=1, high=11)
    random_2_10 = lambda x : rng.integers(low=2, high=11)
    random_3_10 = lambda x : rng.integers(low=2, high=11)

    for amount in range(50):
        for inst in range(3, 101):
            # file = open("instance_"+str(inst)+"_random.txt", "w")
            # generate_instance(file, inst, random_1_20, random_1_20, random_2_20, True)
            # file.close()
            inst_str = str(inst)
            if len(inst_str) == 1:
                inst_str = "00" + inst_str
            elif len(inst_str) == 2:
                inst_str = "0" + inst_str

            amount_str = str(amount)
            if len(amount_str) == 1:
                amount_str = "00" + amount_str
            elif len(amount_str) == 2:
                amount_str = "0" + amount_str

            file = open("xAxt/instance_{}_{}.txt".format(inst_str, amount_str), "w")
            generate_instance(file, inst, amount, random_1_10, random_2_10, random_3_10, True)
            file.close()