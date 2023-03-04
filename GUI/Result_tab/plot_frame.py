#a list of plot frames on the right
#each plot is a lineplot (for now), with the possibility to toggle each algorithm and the time or cost
#possibility to add, remove, copy plots

import customtkinter as ctk
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
import pandas as pd
from pathlib import Path
import numpy as np
import sys
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

from Custom_widget.checkbox_array import Checkbox_array
from Custom_widget.scrollable_frame import Scrollable_frame

#TODO: define a dictionary of Algorithm - Color pairs
#See https://matplotlib.org/stable/gallery/color/named_colors.html

#TODO: display the 2 plots by putting 2 subplots instead of one

#TODO: Upscale the plots

#TODO: put the controls on the side

class Plot_frame(ctk.CTkFrame) :
    sm_plot_width = 18
    sm_plot_height = 8
    #list storing the plot_frames
    sm_plot_frame_list = []

    #list storing the name of the files
    sm_datafile_list = ["None"]

    #list storing the dataframes
    sm_dataframe_list = []

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.plot = FigureCanvasTkAgg(master=self) #= self.display_plot(data1)
        self.current_dataframe=pd.DataFrame()
        self.current_goal="Cost"

        #a menu to select a dataset
        self.dataset_selection_menu = ctk.CTkOptionMenu(master=self, values=Plot_frame.sm_datafile_list, command=self.update_dataset_selection)
        self.dataset_selection_menu.pack(side="top")

        #a choice between time and cost
        self.selected_data_menu = ctk.CTkOptionMenu(master=self, values=["Cost", "Time"], command=self.goal_update)
        self.selected_data_menu.pack(side="top")

        #a list of checkboxes, one for each algorithm present in the dataset
        self.algo_checkboxes = Checkbox_array(master=self)
        self.algo_checkboxes.pack(side="left")

        #a button to save as image
        self.save_entry_name = ctk.CTkEntry(master=self, placeholder_text="Name")
        self.save_button = ctk.CTkButton(master=self, text="Save", command=self.save_plot)
        self.save_entry_name.pack(side="bottom")
        self.save_button.pack(side="bottom")

        #button to remove the entry
        remove_button = ctk.CTkButton(master=self, text="x", width=50, command=self.remove_plot_frame)
        remove_button.pack(side="right", anchor="ne")

        Plot_frame.sm_plot_frame_list += [self]
    
    
    def display_plot(self):
        """
        Switch on the current goal to display the proper graph
        """
        self.plot.get_tk_widget().destroy()
        figure = plt.Figure(figsize=(Plot_frame.sm_plot_width, Plot_frame.sm_plot_height), dpi=60)
        self.plot = FigureCanvasTkAgg(figure, self)
        self.plot.get_tk_widget().pack()

        match self.current_goal:
            case "Cost":
                self.display_cost_graph()
                return
            case "Time":
                self.display_time_graph()
    
    def display_time_graph(self):
        """
        Display the time graph
        For each dimension, we plot each algorithms' average execution time
        """
        ax = self.plot.figure.add_subplot(111)

        df = self.current_dataframe
        max_dim = max(df["Dimension"])
        
        ax.set_xlabel("Dimension", size=14)
        # ax.set_xticks(np.arange(0, max_dim+1, 1), fontsize=12)

        ax.set_ylabel("Time", size=14)
        # ax.set_yticks(fontsize=12)
        ax.set_yscale("log")
        
        ax.set_title("Average execution time per amount of dimension", size = 16)

        algo_list = self.algo_checkboxes.grab_positive_checks()
        ax.legend(algo_list)

        #for each algorithm in algo_list
        for algorithm in algo_list:
            algo_df = df[df["Algorithm"]==algorithm]
            #create 2 tabs of size max(df["Dimension"])
            amount = [0]*(max_dim+1)
            total_time = [0]*(max_dim+1)

            #for each entry for that algorithm
            for dim, time in (algo_df[["Dimension", "Time"]]).to_numpy():
                #sum the time inside one tab for each network dimension
                total_time[int(dim)] += time
                #count the amount of network of this size inside the other
                amount[int(dim)] += 1
            
            #clean the 0's
            total_time = list(filter(lambda a: a!=0, total_time))
            amount = list(filter(lambda a: a!=0, amount))

            #compute the average execution time using the 2 tabs
            avg_time = [entry[0]/entry[1] for entry in zip(total_time, amount)]

            ax.plot(np.unique(algo_df["Dimension"]), avg_time)

    def display_cost_graph(self):
        """
        Display the cost graph
        For each network, we plot each algorithm's cost
        """
        ax = self.plot.figure.add_subplot(111)
        df = self.current_dataframe

        #We get a dictionary containing all files included in the dataframe, and their associated number of dimension
        file_carac = dict([(entry[0], entry[1]) for entry in df[["File", "Dimension"]].to_numpy()])
        file_keys = list(file_carac.keys())
        file_values = list(file_carac.values())

        #To avoid repeating labels, while still acknowledging them
        filtered_values = np.array([([val] + [None]*(file_values.count(val)-1)) for val in np.unique(file_values)]).flatten()

        # ax.set_xticks(range(len(file_carac)), file_values)
        ax.set_xticks(range(len(file_carac)), filtered_values)

        # print(list(file_carac.values()))
        ax.set_xlabel("Dimension", size=14)
        # ax.set_xticks(np.arange(0, max_dim+1, 1), fontsize=12)

        ax.set_ylabel("Cost", size=14)
        # ax.set_yticks(fontsize=12)
        ax.set_yscale("log")
        
        ax.set_title("Cost per network given as number of dimension", size = 16)

        algo_list = self.algo_checkboxes.grab_positive_checks()
        ax.legend(algo_list)

        for algorithm in algo_list:
            algo_df = df[df["Algorithm"]==algorithm]

            # each file is mapped to its index in file_carac,
            # and because file_carac is used to build the x axis,
            # it is properly mapped onto the x axis as well 
            file_idx = [file_keys.index(file) for file in algo_df["File"].to_numpy()]
            ax.plot(file_idx, algo_df[["Cost"]].to_numpy())

    def goal_update(self, label):
        """
        Updates the goal (Y axis) to plot
        """
        self.current_goal=label
        if not self.current_dataframe.empty:    
            self.display_plot()

    def update_dataset_selection(self, file):
        """
        Updates the display based on the selected dataset
        """
        if file=="None":
            self.plot.get_tk_widget().destroy()
            self.algo_checkboxes.update_checkbox_list([])
            self.current_dataframe = pd.DataFrame()
            self.save_entry_name.lower()
            self.save_button.lower()
            return
        
        self.save_entry_name.lift()
        self.save_button.lift()
        self.current_dataframe = pd.read_csv(file, sep=";", header=0)
        #Among a single dimension number, this sort guarantees an order in the file for all algorithms
        self.current_dataframe.sort_values(by=["Dimension", "File"], inplace=True)

        algorithm_list = list(set(self.current_dataframe["Algorithm"]))
        self.algo_checkboxes.update_checkbox_list(algorithm_list, selected=True, command=self.display_plot)
        
        self.display_plot()

    #TODO: proper filepath + prevent overwrite
    def save_plot(self):
        """
        Saves the plot
        """
        if self.save_entry_name.get() != "":
            self.plot.__getattribute__("figure").savefig(fname="plot/" + self.save_entry_name.get())
        return

    def remove_plot_frame(self):
        """
        Destroys the plot, removes the display
        """
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

    filenames = [file for file in ctk.filedialog.askopenfilenames(initialdir=abs_path_instances, title="Select a file or directory") if file not in Plot_frame.sm_datafile_list]

    #each selected file adds an entry to the list of pandas dataset
    Plot_frame.sm_datafile_list += [file for file in filenames]
    for plot_frame in Plot_frame.sm_plot_frame_list:
        plot_frame.dataset_selection_menu.configure(values=Plot_frame.sm_datafile_list)

def add_plot_frame(parent):
    pf = Plot_frame(master=parent)
    pf.pack()

if __name__ == "__main__":

    root = ctk.CTk()
    root.geometry("1500x1000")
    
    scroll_frame = Scrollable_frame(master=root, height=600)
    scroll_frame.pack(expand=True, fill="both")

    # button to add a plot
    add_plot_button = ctk.CTkButton(master=root, text="Add plot", command=lambda:add_plot_frame(scroll_frame.scrollable_frame))
    add_plot_button.pack()

    #button to import a dataset
    explore = ctk.CTkButton(master=root, text="Import", command=lambda:import_files())
    explore.pack()

    root.mainloop()