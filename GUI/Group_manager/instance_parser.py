"""Contains functions necessary to parse the files in order to grab some informations"""
import re

def grab_infos(filename : str):
    """Grabs the size (in nodes) and average weight of an instance given by its name"""
    weight = 0
    n_edge = 1
    with open(filename) as file:
        for line in file:
            if(not re.match("^ *(.)", line)):
                continue

            start_char = re.findall("^ *(.)", line)[0]
            
            match start_char:
                case 'd':
                    dimension = int(re.findall("[1-9][0-9]*", line)[0])
                    n_edge = 3*dimension - 2
                case 'e':
                    weight += int(re.findall("0|[1-9][0-9]*", line)[2])

    avgw = round(weight/n_edge, 2)
    return dimension, avgw