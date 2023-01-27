"""Contains functions necessary to parse the files in order to grab some informations"""

def grab_infos(filename : str):
    """Grabs the size (in nodes) and average weight of an instance given by its name"""
    weight = 0
    with open(filename) as file:
        for line in file:
            tokens = list(filter(lambda a: a != ' ', line))
            #could also use match
            if tokens[0]=='p':
                size = int(tokens[1])
            elif tokens[0]=='e':
                weight+=int(tokens[3]) 
                #TODO: this is actually stupid, we should do the average relative to the number of edges

    avgw = round(weight/size, 2)
    return size, avgw