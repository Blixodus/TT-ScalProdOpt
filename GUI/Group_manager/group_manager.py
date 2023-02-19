import customtkinter as ctk
import pyperclip
import sys
import os
import subprocess

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

from Group_manager import algorithm_entries, instance_list

class Group_manager_page(ctk.CTkFrame):
    """
    One of the main tabs, currently only handles 1 group
    Holds two frames
    algorithm_frame : the list of algorithm entries
    instance_frame : the list of instances entries
    """
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        #holds the list of entries
        top_side = ctk.CTkFrame(master=self)

        #holds the list of algorithms
        left_hand_side = ctk.CTkFrame(master=top_side)

        #frame holding the algorithm entries
        self.algorithm_frame = algorithm_entries.Algo_list_frame(master=left_hand_side)#(master=self)
        self.algorithm_frame.pack(side="top", fill="both", expand=True)

        #button for adding entries
        #add_entry_button = ctk.CTkButton(master=self, text="Add algorithm", command=lambda:algorithm_entries.Algo_entry(self.algorithm_frame))
        add_entry_button = ctk.CTkButton(master=left_hand_side, text="Add algorithm", command=lambda:algorithm_entries.Algo_entry(self.algorithm_frame))        
        add_entry_button.pack(padx = 10, pady=10, side="bottom")

        left_hand_side.pack(side="left", fill="both", expand=True)

        #holds the list of networks
        right_hand_side = ctk.CTkFrame(master=top_side)

        #frame holding the instance entries
        self.instance_frame = instance_list.Instance_list_frame(master=right_hand_side)#(master=self)
        self.instance_frame.pack(side="top", fill="both", expand=True)

        #button for adding an instance file
        explore = ctk.CTkButton(master=right_hand_side, text="Explore", command=lambda:instance_list.explore_files(self.instance_frame))#(master=self, text="Explore", command=lambda:instance_list.explore_files(self.instance_frame))
        explore.pack(padx=10, pady=10, side="bottom")

        right_hand_side.pack(side="right", fill="both")

        top_side.pack(side="top", fill="both", expand=True)

        #bottom row with the buttons
        bottom_side = ctk.CTkFrame(master=self)

        #execution
        launch_button=ctk.CTkButton(master=bottom_side, text="Launch execution", command=self.launch_execution)
        launch_button.pack(side="right")

        #command to clipboard
        copy_command_button = ctk.CTkButton(master=bottom_side, text="Copy command", command=self.copy_command)
        copy_command_button.pack(side="right")

        #entry for output_file
        self.export_file_name = ctk.CTkEntry(master=bottom_side, placeholder_text="Output file")
        self.export_file_name.pack(side="right")

        bottom_side.pack(side="bottom", fill="x")

        self.pack(fill="both", expand=True)

    def grab_arguments(self)->tuple[list[dict[str:str]], str, str]:
        """
        Grabs all arguments relevant to the execution,
        from both the list of algorithm, and the list of instances
        """
        # print(self.algorithm_frame.grab_all_values())
        # print(self.instance_frame.grab_all_values())
        # print(self.export_file_name.get())
        return self.algorithm_frame.grab_all_values(), self.instance_frame.grab_all_file_path(), self.export_file_name.get()

    def get_command(self) -> str:
        """Generates and returns the command tied to the selected parameters"""

        #the arguments are caught
        list_algo_dict, files_param, export_file = self.grab_arguments()

        #add edge guards to the parameters
        algorithm_entries.verify_params(list_algo_dict)

        #formatting the parameters
        algo_params = list_dict_to_proper_string(list_algo_dict)
        args = "xmake run -w . OptiTenseurs -a {0} -f {1} -o {2}".format(algo_params, files_param, export_file)
        
        return args

    def copy_command(self):
        """Copies the execution command to clipboard"""
        pyperclip.copy(self.get_command())
        
    def launch_execution(self):
        """Launch the execution of OptiTenseurs on the selected parameters"""
        #the arguments are caught
        #list_algo_dict, files_param, export_file = self.grab_arguments()

        #status, status_message=algorithm_entries.verify_params(list_algo_dict)
        
        #if not status:
            #TODO: print the message with a proper widget
        #    print(status_message)
        #else:
        subprocess.call(self.get_command(), shell=True)


#formatting the parameters, im too lazy to find proper names
def dict_to_proper_string(algo_dict : dict) -> str : 
    algos_args=""
    keys_list = list(algo_dict.keys())
    val_list = list(algo_dict.values())
    for i in range(len(algo_dict)):
        algos_args += str(keys_list[i]) + " " + str(val_list[i]) + " "
    return algos_args[:-1]

def list_dict_to_proper_string(list_algo_dict : list[dict]) -> str :
    string_algo_args =""
    for i, _ in enumerate(list_algo_dict) :
        string_algo_args += "\"" + dict_to_proper_string(list_algo_dict[i]) + "\" "
    return string_algo_args


if __name__=="__main__":
    root = ctk.CTk()
    root.geometry("1000x1000")
    group_manager = Group_manager_page(master=root)
    
    root.mainloop()