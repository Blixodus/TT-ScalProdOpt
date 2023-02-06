import customtkinter as ctk
import sys
import os

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

        #frame holding the algorithm entries
        self.algorithm_frame = algorithm_entries.Algo_list_frame(master=self, width=800)

        #button for adding entries
        add_entry_button = ctk.CTkButton(master=self, text="+", command=lambda:algorithm_entries.Algo_entry(self.algorithm_frame))
        add_entry_button.pack(padx = 10, pady=10, side="left")

        #frame holding the instance entries
        self.instance_frame = instance_list.Instance_list_frame(master=self)
        #button for adding an instance file
        explore = ctk.CTkButton(master=self, text="Explore", command=lambda:instance_list.explore_files(self.instance_frame))
        explore.pack(padx=10, pady=10, side="left")

        #TODO: add an export functionality
        #It would be great to have a "select or create" dialog
        self.export_file_name = ctk.CTkEntry(master=self, placeholder_text="File path")
        self.export_file_name.pack(side="left")

        launch=ctk.CTkButton(master=self, text="GO!", command=self.launch_execution)
        launch.pack(side="bottom")

        self.pack(fill="both", expand=True)

    def grab_arguments(self)->tuple[list[dict[str:str]], list[str], str]:
        """
        Grabs all arguments relevant to the execution,
        from both the list of algorithm, and the list of instances
        """
        # print(self.algorithm_frame.grab_all_values())
        # print(self.instance_frame.grab_all_values())
        # print(self.export_file_name.get())
        return self.algorithm_frame.grab_all_values(), self.instance_frame.grab_all_values(), self.export_file_name.get()
    
    def launch_execution(self):
        algo_params, instance_list, export_file = self.grab_arguments()
        status, status_message=algorithm_entries.verify_params(algo_params)
        if not status:
            #TODO: print the message with a proper widget
            print(status_message)
        else:
            print("yeee")
            #TODO: execute the code


if __name__=="__main__":
    root = ctk.CTk()
    root.geometry("1000x1000")
    group_manager = Group_manager_page(master=root)
    
    root.mainloop()