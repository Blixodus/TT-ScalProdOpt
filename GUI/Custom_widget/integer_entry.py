import customtkinter as ctk

class Integer_entry(ctk.CTkEntry):
    """Entry class that can only take an integer"""
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        self.configure(width=35, validate="key", validatecommand=(self.register(validate_entry), '%P'))
        self.pack(side="left")

def validate_entry(input):
    """Checks that the input of an entry is an integer"""
    if(input.isdigit()):
        return True
    elif input == "":
        return True
    else:
        return False

if __name__=="__main__":
    root = ctk.CTk()
    root.geometry("500x500")
    
    Integer_entry(placeholder_text="Dmin", master=root)
    root.mainloop()