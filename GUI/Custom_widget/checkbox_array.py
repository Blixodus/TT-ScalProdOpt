import customtkinter as ctk

class Checkbox_array(ctk.CTkFrame):
    

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.m_checkbox_list = []


    def update_checkbox_list(self, label_list):
        for checkbox in self.m_checkbox_list:
            checkbox.destroy()

        self.m_checkbox_list.clear()

        self.m_checkbox_list = [ctk.CTkCheckBox(master=self, text=name) for name in label_list]
        self.configure(require_redraw=True)
        for checkbox in self.m_checkbox_list:
            checkbox.pack()

        return 0

    #TODO:
    def grab_positive_checks():
        return []
