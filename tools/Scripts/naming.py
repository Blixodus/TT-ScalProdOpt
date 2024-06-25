import os

# Get directory path for given test group
def get_dir(root_dir, tt_dim, y_eq_xT, type, rank_type):
    # Prepare test case name (general for higher TT-dim, specific for 2 and 3)
    test_case_id = f"T{tt_dim:02}"
    if tt_dim == 2:
        if y_eq_xT:
            test_case_id = "xxT"
        else:
            test_case_id = "xy"
    elif tt_dim == 3:
        if y_eq_xT:
            test_case_id = "xAxT"
        else:
            test_case_id = "xAy"

    # Prepare directory
    dir = f"{root_dir}/{test_case_id}/{type}/{rank_type}"
    if not os.path.exists(dir):
        os.makedirs(dir)

    return dir

# Get filename for given test case
def get_test_filename(dir, dimension, instance):
    return f"{dir}/d_{dimension:03}_v_{instance:03}.txt"

# Get algorithm name (as part of file name)
def get_algorithm_name(algorithm, delta=None):
    if delta:
        return f"{algorithm}_{delta}"
    return f"{algorithm}"

# Get filename for given test case
def get_result_filename(dir, algorithm, delta=None):
    return f"{dir}/{get_algorithm_name(algorithm, delta)}.txt"

