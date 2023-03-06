import customtkinter as ctk
import os
import sys
from pathlib import Path


SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

from Group_manager import instance_parser
from Custom_widget.scrollable_frame import Scrollable_frame

"""Contains functions and elements necessary for the list of instance files"""

class Instance_list_frame(Scrollable_frame):
    """Frame holding all instance entries"""
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.pack(padx=10, pady=10, side="top")
        
    def grab_all_file_path(self) -> str :
        """Iterates over the Instance_entry objects and returns their stored values as a single string"""
        entry_list = list(filter(lambda a: "Instance_entry" in str(type(a)), list(self.scrollable_frame.children.values())))
        files_string = ""
        for entry in entry_list:
            files_string += entry.file_path + " "
        return files_string #[(entry.file_path) for entry in entry_list]

class Instance_entry(ctk.CTkFrame):
    """
    Single instance entry
    Contains a constructor that sets up all the component
    Removal is directly handled by a sub-element
    file_path : the path to the instance file
    file_size : the number of dimensions of the network
    average_weight : the average weight of the edges
    """
    def __init__(self, filename : str, **kwargs):
        """
        Constructor for Instance_entry
        adds all necessary elements
        """
        super().__init__(**kwargs)

        #name of the file
        #we dont want to print its entire path
        splitted = filename.split('/')
        pathname = ['/'+i for i in splitted[-3:]] #TODO: that's disgusting... put an edge guard somewhere
        name_label = ctk.CTkLabel(master=self, text=pathname)
        name_label.pack(side="left")

        #getting infos from the file
        size, avgw = instance_parser.grab_infos(filename)

        #size in node/edges
        size_label = ctk.CTkLabel(master=self, text=size, width=30)
        size_label.pack(side="left")
        
        #average weight
        avgweight_label = ctk.CTkLabel(master=self, text=avgw, width=30)
        avgweight_label.pack(side="left")

        remove = ctk.CTkButton(master=self, text="x", command=lambda:self.destroy(), width=30, height=30)
        remove.pack(side="left")

        self.pack()

        self.file_path=filename
        self.file_size=size
        self.average_weight=avgw

        #kwargs["master"].master.master.grab_all_values()

    def grab_values(self):
        """
        Grabs the file path from an entry
        """
        return self.file_path #, self.file_size, self.average_weight

def explore_files(frame : Scrollable_frame):
    """Opens a file explorer, multiple files can be selected at once"""
    #TODO: we need to be careful with the initial directory, it's 100% gonna break something
    #print(os.getcwd()) #gives the current directory
    #TODO: put a proper filetypes parameter
    abs_list_curr_dir = Path(os.getcwd()).parts
    abs_path_instances = ""
    for dir in abs_list_curr_dir:
        if(dir in {"GUI", "results", "instances", "plot"}):
            break
        abs_path_instances += dir + "/"

    abs_path_instances += "instances/"

    filenames = ctk.filedialog.askopenfilenames(initialdir=abs_path_instances, title="Select a file or directory", filetypes=[("Networks", ".txt")]) 

    #each selected file adds an entry to the list of instances
    for file in filenames:
        Instance_entry(filename=file, master=frame.scrollable_frame, width=400, height=100)


if __name__ == "__main__":
    root = ctk.CTk()
    root.geometry("500x350")

    instance_frame = Instance_list_frame(master=root)
    
    #adding a file
    explore = ctk.CTkButton(master=root, text="Explore", command=lambda:explore_files(instance_frame))
    explore.pack()

    #we need to be careful with what the default directory is
    #print(instance_parser.grab_infos("../instances/small/instance_1_4.txt"))

    root.mainloop()