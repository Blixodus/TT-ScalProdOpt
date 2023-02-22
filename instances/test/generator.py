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
        file.write("\t{}\t*{}".format(i, rank1[i+1]))
    file.write("\t{}\nv".format(size-1))
    for i in range(size):
        file.write("\t*{}\t".format(dims[i]))
    file.write("\nv")
    for i in range(size-1):
        file.write("\t{}\t*{}".format(size+i, rank2[i+1]))
    file.write("\t{}\n".format(2*size-1))
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

def cos_like(length):
    centre = int(length/2)
    def lam(x):
        return 5+(1+np.cos(x*2*np.pi/length+np.pi))*(95/2) + rng.integers(low=-(centre-abs(x-centre))-1, high=centre-abs(x-centre)+1)
    return lam
    #return lambda x : 5+(1+np.cos(x*2*np.pi/length+np.pi))*(95/2) + rng.integers(low=-(centre-abs(x-centre)), high=centre-abs(x-centre))

for inst in range(10, 41, 10):
    file = open("instance_"+str(inst)+"_random.txt", "w")
    generate_instance(file, inst, random_1_20, random_1_20, random_2_20, True)
    file.close()
    file = open("instance_"+str(inst)+"_cos_like.txt", "w")
    generate_instance(file, inst, cos_like(inst), cos_like(inst), random_2_20, True)
    file.close()
