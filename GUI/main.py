import customtkinter
from Custom_widget.tab_list import Tab_list
from Group_manager.group_manager import Group_manager_page
from Result_tab.result_page import Result_page

#appearance
# customtkinter.set_appearance_mode("dark")
customtkinter.set_default_color_theme("dark-blue")

if __name__=="__main__":

    root = customtkinter.CTk()
    #dimensions of the root
    root.geometry("1500x1000")
    tab_list = Tab_list(master=root)
    #TODO: Extra tabs are to be added here
    tab_list.set_tabs([("Group manager", Group_manager_page), ("Result tab", Result_page)])
    tab_list.pack(fill="both", expand=True)

    root.mainloop()

    #note to self : how do we align stuff ?