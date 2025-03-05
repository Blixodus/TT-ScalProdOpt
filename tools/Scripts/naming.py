import os

# Get test case name
def get_test_case_name(tt_dim, y_eq_xT):
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
    return test_case_id

def get_test_case_name2(tt_dim, y_eq_xT):
    test_case_id = f"T{tt_dim:02}"
    if tt_dim == 2:
        if y_eq_xT:
            test_case_id = "$x^Tx$"
        else:
            test_case_id = "$x^Ty$"
    elif tt_dim == 3:
        if y_eq_xT:
            test_case_id = "$x^TAx$"
        else:
            test_case_id = "$x^TAy$"
    elif tt_dim == 4:
        if y_eq_xT:
            test_case_id = "$x^TABx$"
        else:
            test_case_id = "$x^TABy$"
    return test_case_id

def get_test_case_name3(tt_dim, y_eq_xT):
    test_case_id = f"T{tt_dim:02}"
    if tt_dim == 2:
        if y_eq_xT:
            test_case_id = "xTx"
        else:
            test_case_id = "xy"
    elif tt_dim == 3:
        if y_eq_xT:
            test_case_id = "xTAx"
        else:
            test_case_id = "xTAy"
    return test_case_id

# Get directory path for given test group
def get_dir(root_dir, tt_dim, y_eq_xT, type, rank_type, rank_const = None, dim_const = None):
    # Prepare test case name (general for higher TT-dim, specific for 2 and 3)
    test_case_id = get_test_case_name(tt_dim, y_eq_xT)

    # Prepare directory
    dir = f"{root_dir}/{test_case_id}/{type}/{rank_type}"

    if rank_const and dim_const:
        dir = f"{root_dir}/{test_case_id}/{type}/{rank_type}/{rank_const:03}_{dim_const:03}"

    if not os.path.exists(dir):
        os.makedirs(dir)

    return dir

def get_dir_ratio(dir, rank_const = None, dim_const = None):
    if rank_const and dim_const:
        dir = f"{dir}/{rank_const:03}_{dim_const:03}"

    if not os.path.exists(dir):
        os.makedirs(dir)
    
    return dir

# Get filename for given test case
def get_test_filename(dir, dimension, instance):
    return f"{dir}/d_{dimension:03}_v_{instance:03}.txt"
    

# Get algorithm name (as part of file name)
def get_algorithm_name(algorithm, delta=None):
    if delta and algorithm != "optimal":
        return f"{algorithm}_{delta}"
    return f"{algorithm}"

# Get filename for given test case
def get_result_filename(dir, algorithm, delta=None):
    return f"{dir}/{get_algorithm_name(algorithm, delta)}.txt"

