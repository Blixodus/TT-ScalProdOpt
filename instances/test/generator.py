import numpy as np

def generate_instance(file, size, f_rank1, f_rank2, f_dims, rounded):
    # Generate dimensions and ranks
    dims = [int(f_dims(i)) for i in range(size)]
    rank1 = [1] + [int(f_rank1(i)) for i in range(size-1)] + [1]
    rank2 = [1] + [int(f_rank2(i)) for i in range(size-1)] + [1]
    # "Rounding" of TT
    if(rounded):
        for i in range(size-1):
            rank1[i+1] = min(rank1[i+1], rank1[i]*dims[i])
            rank2[i+1] = min(rank2[i+1], rank2[i]*dims[i])
        for i in range(size-1):
            rank1[-i-2] = min(rank1[-i-2], rank1[-i-1]*dims[i])
            rank2[-i-2] = min(rank2[-i-2], rank2[-i-1]*dims[i])
    # Print TT product
    file.write("v")
    for i in range(size-1):
        file.write(" {} *{}".format(i, rank1[i+1]))
    file.write(" {}\nv".format(size-1))
    for i in range(size):
        file.write(" *{}  ".format(dims[i]))
    file.write("\nv")
    for i in range(size-1):
        file.write(" {} *{}".format(size+i, rank2[i+1]))
    file.write(" {}\n".format(2*size-1))
    file.write("d {}\n".format(size*2))
    for i in range(size-1):
        file.write("e {} {} {}\n".format(i, i+1, rank1[i+1]))
    for i in range(size):
        file.write("e {} {} {}\n".format(i, size+i, dims[i]))
    for i in range(size-1):
        file.write("e {} {} {}\n".format(size+i, size+i+1, rank2[i+1]))


rng = np.random.default_rng(seed=1000)
random_1_20 = lambda x : rng.integers(low=1, high=21)
random_2_20 = lambda x : rng.integers(low=2, high=21)
file = open("instance_10_random.txt", "w")
generate_instance(file, 10, random_1_20, random_1_20, random_2_20, True)
file.close()
file = open("instance_20_random.txt", "w")
generate_instance(file, 20, random_1_20, random_1_20, random_2_20, True)
file.close()
file = open("instance_30_random.txt", "w")
generate_instance(file, 30, random_1_20, random_1_20, random_2_20, True)
file.close()
file = open("instance_40_random.txt", "w")
generate_instance(file, 40, random_1_20, random_1_20, random_2_20, True)
file.close()
