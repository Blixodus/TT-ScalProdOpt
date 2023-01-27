import customtkinter as ctk

class Tab_list(ctk.CTkTabview):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

    def set_tabs(self, list : list[(str, ctk.CTkBaseClass)]):
        for str, val in list:
            tab=self.add(str)
            val(master=tab)