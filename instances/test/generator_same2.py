import numpy as np

def generate_instance(file, size, instance, f_rank1, f_rank2, f_dims, rounded):
    # Generate dimensions and ranks
    dims = [2 for i in range(size)]

    rank1 = [1] + [2 for i in range(size-1)] + [1]
    rank2 = [1] + [2 for i in range(size-1)] + [1]
    # "Rounding" of TT
    if(rounded):
        for i in range(size-1):
            rank1[i+1] = min(rank1[i+1], rank1[i]*dims[i])
            rank2[i+1] = min(rank2[i+1], rank2[i]*dims[i])
        for i in range(size-1):
            rank1[-i-2] = min(rank1[-i-2], rank1[-i-1]*dims[i])
            rank2[-i-2] = min(rank2[-i-2], rank2[-i-1]*dims[i])
    # Compute naive approach cost
    cost_naive_1 = dims[0] * rank1[1] * rank2[1]
    cost_naive_2 = dims[0] * rank1[1] * rank2[1]
    for i in range(1, size):
        cost_naive_1 = cost_naive_1 + rank2[i] * rank1[i] * dims[i] * rank1[i+1] + dims[i] * rank2[i] * rank1[i+1] * rank2[i+1]
        cost_naive_2 = cost_naive_2 + rank1[i] * rank2[i] * dims[i] * rank2[i+1] + dims[i] * rank1[i] * rank2[i+1] * rank1[i+1]
    # Print TT product
    file.write("c Naive cost : {}\n".format(cost_naive_1))
    file.write("c Naive cost : {}\n".format(cost_naive_2))
    file.write("v")
    for i in range(size-1):
        file.write("\t{}\t*{}".format(i, rank1[i+1]))
    file.write("\t{}\nv".format(size-1))
    for i in range(size):
        file.write("\t*{}\t".format(dims[i]))
    file.write("\nv")
    for i in range(size-1):
        file.write("\t{}\t*{}".format(size+i, rank2[i+1]))
    file.write("\t{}\n".format(2*size-1))
    file.write("d {}\n".format(size))
    for i in range(size-1):
        file.write("e {} {} {}\n".format(i, i+1, rank1[i+1]))
    for i in range(size):
        file.write("e {} {} {}\n".format(i, size+i, dims[i]))
    for i in range(size-1):
        file.write("e {} {} {}\n".format(size+i, size+i+1, rank2[i+1]))

def cos_like(length):
    centre = int(length/2)
    return lambda x : 5+(1+np.cos(x*2*np.pi/length+np.pi))*(95/2) + rng.integers(low=-(centre-abs(x-centre))-1, high=centre-abs(x-centre)+1)



# file = open("instance_5_random.txt", "w")
# generate_instance(file, 5, random_1_20, random_1_20, random_2_20, True)
# file.close()

if __name__ == "__main__":
    rng = np.random.default_rng(seed=1000)
    random_1_20 = lambda x : rng.integers(low=1, high=11)
    random_2_20 = lambda x : rng.integers(low=2, high=11)

    for amount in range(1):
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

            file = open("same2/instance_{}_{}.txt".format(inst_str, amount_str), "w")
            generate_instance(file, inst, amount, random_1_20, random_2_20, None, True)
            file.close()