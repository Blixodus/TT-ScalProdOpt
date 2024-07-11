def generate_contraction_list(contraction_tree):
    if len(contraction_tree) > 2:
        exit("Error! The contraction tree is not binary.")
    
    contraction_list = []
    ids = []

    for i in range(len(contraction_tree)):
        if isinstance(contraction_tree[i], tuple):
            local_list, min_id = generate_contraction_list(contraction_tree[i])
            ids.append(min_id)
            contraction_list += local_list
        else:
            ids.append(contraction_tree[i])

    for i in range(len(ids) - 1):
        contraction_list.append((ids[i], ids[i + 1]))

    return contraction_list, min(ids)