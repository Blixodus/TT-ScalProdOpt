#a list of plot frames on the right
#the dataset can be shown and hidden on the left
#each plot is a lineplot (for now), with the possibility to toggle each algorithm and the time or cost
#possibility to add, remove, copy plots
import sys, os
import customtkinter as ctk
from pathlib import Path

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

from Result_tab.plot_frame import Plot_frame
from Custom_widget.scrollable_frame import Scrollable_frame

def import_files():
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

    abs_path_instances += "results/"

    filenames = [file for file in ctk.filedialog.askopenfilenames(initialdir=abs_path_instances, title="Select a file or directory") if file not in Plot_frame.sm_datafile_list]

    #each selected file adds an entry to the list of pandas dataset
    Plot_frame.sm_datafile_list += [file for file in filenames]
    for plot_frame in Plot_frame.sm_plot_frame_list:
        plot_frame.dataset_selection_menu.configure(values=Plot_frame.sm_datafile_list)

def add_plot_frame(parent):
    pf = Plot_frame(master=parent)
    pf.save_entry_name.lower()
    pf.save_button.lower()
    pf.pack()

class Result_page(Scrollable_frame):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        # button to add a plot
        add_plot_button = ctk.CTkButton(master=self, text="Add plot", command=lambda:add_plot_frame(self.scrollable_frame))
        add_plot_button.pack()

        #button to import a dataset
        explore = ctk.CTkButton(master=self, text="Import", command=lambda:import_files())
        explore.pack()

        self.pack(expand=True, fill="both")

if __name__=="__main__":
    root = ctk.CTk()
    root.geometry("800x800")

    res_tab = Result_page(master=root)

    root.mainloop()