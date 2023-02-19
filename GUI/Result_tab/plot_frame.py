#a list of plot frames on the right
#each plot is a lineplot (for now), with the possibility to toggle each algorithm and the time or cost
#possibility to add, remove, copy plots

import customtkinter as ctk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import pandas as pd
from pathlib import Path
import sys
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

from Custom_widget.checkbox_array import Checkbox_array
from Custom_widget.scrollable_frame import Scrollable_frame

class Plot_frame(ctk.CTkFrame) :
    #list storing the plot_frames
    sm_plot_frame_list = []

    #list storing the name of the files
    sm_datafile_list = ["None"]

    #list storing the dataframes
    sm_dataframe_list = []

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.plot = FigureCanvasTkAgg(master=self) #= self.display_plot(data1)
        self.current_dataframe=()
        self.current_goal="cost"

        #a list of checkboxes, one for each algorithm present in the dataset
        self.algo_checkboxes = Checkbox_array(master=self)
        self.algo_checkboxes.pack()

        #a choice between time and cost
        self.selected_data_menu = ctk.CTkOptionMenu(master=self, values=["cost", "time"], command=self.goal_update)
        self.selected_data_menu.pack()

        #a button to save as image
        self.save_entry_name = ctk.CTkEntry(master=self, placeholder_text="Name")
        self.save_button = ctk.CTkButton(master=self, text="Save", command=self.save_plot)
        self.save_entry_name.pack()
        self.save_button.pack()

        #a menu to select a dataset
        self.dataset_selection_menu = ctk.CTkOptionMenu(master=self, values=Plot_frame.sm_datafile_list, command=self.update_dataset_selection)
        self.dataset_selection_menu.pack()

        #button to remove the entry
        remove_button = ctk.CTkButton(master=self, text="Remove", command=self.remove_plot_frame)
        remove_button.pack()

        Plot_frame.sm_plot_frame_list += [self]
    
    #TODO: make this function
    def display_plot(self):
        self.plot.get_tk_widget().destroy()
        # dataset = pd.DataFrame(dataset)
        figure = plt.Figure(figsize=(6,5), dpi=100)
        ax = figure.add_subplot(111)
        self.plot = FigureCanvasTkAgg(figure, self)
        self.plot.get_tk_widget().pack()
        df = self.dataframe[['Dimension', 'Cost']]
        # print(df)
        
        df.plot(kind='line', legend=True, ax=ax)

    def goal_update(self, label):
        self.current_goal=label

    def update_dataset_selection(self, file):
        if file=="None":
            self.plot.get_tk_widget().destroy()
            self.algo_checkboxes.update_checkbox_list([])
            return
        self.dataframe = pd.read_csv(file, sep=";", header=0)
        algorithm_list = list(set(self.dataframe["Algorithm"]))
        self.algo_checkboxes.update_checkbox_list(algorithm_list)
        
        self.display_plot()

    #TODO: proper filepath + prevent overwrite
    def save_plot(self):
        if self.save_entry_name.get() != "":
            self.plot.__getattribute__("figure").savefig(fname="plot/" + self.save_entry_name.get())
        return

    def remove_plot_frame(self):
        Plot_frame.sm_plot_frame_list.remove(self)
        self.destroy()

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

    filenames = list(set(ctk.filedialog.askopenfilenames(initialdir=abs_path_instances, title="Select a file or directory")) - set(Plot_frame.sm_datafile_list))

    #each selected file adds an entry to the list of pandas dataset
    Plot_frame.sm_datafile_list += [file for file in filenames]
    for plot_frame in Plot_frame.sm_plot_frame_list:
        plot_frame.dataset_selection_menu.configure(values=Plot_frame.sm_datafile_list)

def add_plot_frame(parent):
    pf = Plot_frame(master=parent)
    pf.pack()

if __name__ == "__main__":

    root = ctk.CTk()
    root.geometry("800x800")
    
    scroll_frame = Scrollable_frame(master=root, height=600)
    scroll_frame.pack(expand=True, fill="both")

    # button to add a plot
    add_plot_button = ctk.CTkButton(master=root, text="Add plot", command=lambda:add_plot_frame(scroll_frame.scrollable_frame))
    add_plot_button.pack()

    #button to import a dataset
    explore = ctk.CTkButton(master=root, text="Import", command=lambda:import_files())
    explore.pack()

    

    root.mainloop()