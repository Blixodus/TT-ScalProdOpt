import re
import os

def check_file(filename):
    # Import text file with tensor train description
    text_file = open(filename, "r")
    lines = text_file.readlines()
    text_file.close()

    # Locate the dimension of the tensor train
    N = 0
    tt_dim = 2
    for line in lines:
        if line.startswith('d'):
            N = int(re.split(r'\W+', line)[1])
            break

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

    weight[(N - 1, N)] = 1
    weight[(N, N - 1)] = 1
    weight[(2 * N - 1, 2 * N)] = 1
    weight[(2 * N, 2 * N - 1)] = 1
    weight[(0, -1)] = 1
    weight[(-1, 0)] = 1

    # Check the condition of the tensor train raised in the review
    for i in range(0, N - 1):
        if weight[(i, i + 1)] > weight[(i + 1, i + 2)] * weight[(i + 1, i + 1 + N)]:
            return False

        if weight[(i + N, i + 1 + N)] > weight[(i + 1 + N, i + 2 + N)] * weight[(i + 1, i + 1 + N)]:
            return False

        if weight[(i, i + 1)] > weight[(i - 1, i)] * weight[(i, i + N)]:
            return False

        if weight[(i + N, i + 1 + N)] > weight[(i - 1 + N, i + N)] * weight[(i, i + N)]:
            return False    

    return True

ROOT_DIR = "/gpfs/workdir/torria/pdominik/Tests/Tests_dim2"        

for dirpath, dirnames, filenames in os.walk(ROOT_DIR):
    if "xAy" in dirpath or "xAxT" in dirpath:
        continue

    if "Tests_dim2" in dirpath and "increasing" in dirpath:
        continue

    # Check if there are no subdirectories
    if not dirnames:
        # Generate full paths for the files in the directory
        file_paths = [os.path.join(dirpath, filename) for filename in filenames]
        
        if file_paths:
            satisfied = 0
            for file_path in file_paths:
                if check_file(file_path):
                    satisfied += 1

            print(f"Test suite: {dirpath}, Satisfied: {satisfied}/{len(file_paths)} ({satisfied / len(file_paths) * 100:.2f}%)")