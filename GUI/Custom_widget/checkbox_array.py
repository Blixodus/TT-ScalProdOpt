import customtkinter as ctk

class Checkbox_array(ctk.CTkFrame):
    

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.m_checkbox_list = []


    def update_checkbox_list(self, label_list, selected=False, command=lambda:()):
        """
        Updates the checkbox list with the items passed as label_list
        (one checkbox per item)

        selected (default = False) : whether or not the checkboxes should be selected by default
        """
        for checkbox in self.m_checkbox_list:
            checkbox.destroy()

        self.m_checkbox_list.clear()

        self.m_checkbox_list = [ctk.CTkCheckBox(master=self, text=name, command=command) for name in label_list]
        if selected:
            for cb in self.m_checkbox_list:
                cb.select()

        self.configure(require_redraw=True)
        for checkbox in self.m_checkbox_list:
            checkbox.pack(side="bottom", anchor="w")

        return 0

    def grab_positive_checks(self):
        """Return all selected checkboxes as a list of label"""
        return [cb._text for cb in self.m_checkbox_list if cb.get()]
