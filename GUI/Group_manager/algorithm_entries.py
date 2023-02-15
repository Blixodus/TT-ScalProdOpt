"""Contains functions and elements necessary to manage the list of algorithm"""
import customtkinter as ctk
import sys
import os
import re

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

from Custom_widget.scrollable_frame import Scrollable_frame
from Custom_widget.integer_entry import Integer_entry
from algorithms import *

class Algo_list_frame(Scrollable_frame):
    """
    Frame holding all the algorithm entries
    What COULD be possible, is to have another list parallel to the list of entry, that only indicates the number of the entry
    That would imply storing the entries in a proper list as well
    """
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.pack(padx=10, pady=10, fill="x", side="left", expand=True) #TODO: hardcode the parameters

    def grab_all_values(self)->list[dict[str:str]]:
        """Grabs all the parameters stored in all the algorithm entries"""
        algo_list = list(filter(lambda a: "Algo_entry" in str(type(a)), list(self.scrollable_frame.children.values())))
        return [(entry.grab_values()) for entry in algo_list]
#note : this method is a bit meh.

class Algo_entry(ctk.CTkFrame):
    """
    Entry class
    Contains the function to add and update entries
    Removal is directly handled by a sub-element
    param_list : dict[str:ctkwidget], a dictionary linking to the relevant fields
    """

    #TODO: harcoder les paramètres
    def __init__(self, master : Scrollable_frame):
        """
        Algo_entry constructor
        Adds an empty entry (main algorithm = "None")
        """
        super().__init__(master=master.scrollable_frame, height=20, width = 800)

        #algo option menu
        main_alg_menu = ctk.CTkOptionMenu(master=self, values=list(ALGORITHMS.keys()), command=self.update_entry)
        main_alg_menu.pack(side="left")

        #Dmin
        dmin_entry=Integer_entry(master=self, placeholder_text="Dmin")
        # dmin = ctk.CTkEntry(master=self, placeholder_text="Dmin", validate="key", width=35)
        # dmin.pack(side="left")

        #Dmax
        dmax_entry = Integer_entry(master=self, placeholder_text="Dmax")
        
        #sub_alg_menu
        sub_alg_menu = ctk.CTkOptionMenu(master=self, values=["None"]+EXACT_ALGO+SEMI_EXACT_ALGO)
        sub_alg_menu.pack(side="left")

        #alg starting solution
        #TODO: this should be a list of entry instead
        # it also implies managing the entries so that any entry with dependencies is executed after its depedency
        start_sol_menu = ctk.CTkOptionMenu(master=self, values=["None"]+HEURISTICS)
        start_sol_menu.pack(side="left")

        #alloted time
        time_entry = Integer_entry(master=self, placeholder_text="Time")
        # time.pack(side="left")

        #test?
        test_box = ctk.CTkCheckBox(master=self, text=None)
        test_box.pack(side="left")

        #close button
        close_button = ctk.CTkButton(master=self, text="x", command=lambda:self.destroy(), width=30, height=30)
        close_button.pack(side="left")

        self.pack(padx=5, pady=5, fill="both", expand=True) #TODO: hardcode the parameters

        #A dict storing the elements here, so they are easily accessible
        self.param_list=dict({"main_alg":main_alg_menu, "dmin":dmin_entry, "dmax":dmax_entry, "sub_alg":sub_alg_menu, "start_sol":start_sol_menu, "time":time_entry, "test":test_box})
        #we choose no algorithm by default
        self.update_entry("None")

        #TODO: handle the creation of child process, for when sub_alg is defined
        #And its destruction when sub_alg_menu.get()="None"
        self.start_sol=None
        #master.grab_all_values()

    def update_entry(self, algo_name):
        """
        Updates an entry's components' visibility based on the lists defined in algorithms.py, when an algortithm is selected
        """
        show_list=ALGORITHMS[algo_name]
        for key in list(show_list.keys()):
            if show_list[key]==NULL:
                #hides the object
                hide(self.param_list[key])
            else:
                #shows the object
                show(self.param_list[key])
        
    def toggle_dmin(self, sub_algo_name):
        """
        Manages the visibility of dmin
        If no sub-algorithm is selected, then the main algorithm should perform the entire search
        """
        if sub_algo_name=="None":
            hide(dict(self.param_list)["dmin"])
        else:
            show(dict(self.param_list)["dmin"])

    def grab_values(self)->dict[str:str]:
        """Grabs all the relevant values stored in the entry"""
        #we grab the values from the objects
        main_alg=self.param_list["main_alg"].get()
        show_list=ALGORITHMS[main_alg]
        #we store the relevant keys
        params=dict()
        for key in list(show_list.keys()):
            #we only keep the keys that are important to this algorithm
            if show_list[key] > 0:
                #we grab the value
                params[key]=self.param_list[key].get()
        return params

def hide(self : ctk.CTkBaseClass):
    """Hides an element"""
    self.lower()

def show(self : ctk.CTkBaseClass):
    """Shows an element"""
    self.lift()


def verify_params(dict_list : list[dict[str:str]]):
    """
    Verifies and completes the parameters
    The parameters are given as a list of algorithm entries
    Each entry corresponds to an algorithm in the execution queue,
    and is represented as a dictionary {"param" : "value"},
    where only the relevent parameters are present (SHOW_LIST > 0)
    """
    status=True
    status_msg=""

    for index, param_dict in enumerate(dict_list):
        algo_name=param_dict["main_alg"]

        for key in list(param_dict.keys()):
            if ALGORITHMS[algo_name][key]==2 and param_dict[key]==('None' or ''):
                status=False
                status_msg+="Missing mandatory argument \"{0}\" in entry {1}".format(key, index) 

        #we check each value in detail to edgeguard them... or do we do that in the c++ code???
        #because dmax needs to be guarded on a per-instance basis, we can't really handle it here, all we can do is make sure 0 < dmin < dmax
        
        #We check that non-mandatory parameters are set within valid boundaries
        #for this we define default values :
            #dmin : 0
            #dmax : 0 (unlimited)
            #time : 5 (minutes)

        #if sub_alg is None, then the main algorithm has to solve the problem entirely, hence dmin=0
        if param_dict.get("dmin") == '' or (param_dict.get("sub_alg")=="None" and not param_dict.get("main_alg")=="Shuffle"):
            param_dict["dmin"]=1

        if param_dict.get("dmax")=='':
            param_dict["dmax"]=-1

        if param_dict.get("time")=='':
            param_dict["time"]=10
    
        #We cast str of numerical values to int
        for param in ["dmin","dmax","time"]:
            if param_dict.get(param)!=None:
                param_dict[param]=int(param_dict[param])

        #We currently authorize dmin and dmax to have "problematic" values
        # entry_dmin = param_dict.get("dmin")
        # entry_dmax= param_dict.get("dmax")
        #We check that dmin<=dmax unless dmax=0
        # if entry_dmin!=None and entry_dmax!=None and entry_dmin>entry_dmax!=0:
        #     status=False
        #     status_msg+="Dmin={0} > Dmax={1} in entry {3}".format(entry_dmin, entry_dmax,index)

    return status, status_msg

if __name__ == "__main__":
    root = ctk.CTk()
    root.geometry("500x350")
    
    algo_frame = Algo_list_frame(master=root)

    #Button to add en entry, this should be outside the frame
    add_entry_button = ctk.CTkButton(master=root, text="+", command=lambda:Algo_entry(algo_frame))
    add_entry_button.pack(padx = 10, pady=10)
    

    root.mainloop()