import customtkinter as ctk
from scrollable_frame import Scrollable_frame

class Message_frame(Scrollable_frame):
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        #the text
        self.text_frame=ctk.CTkTextbox(master=self.scrollable_frame, takefocus=False, state="disabled")

        self.text_frame.pack(fill="both", expand=True)

    def write(self):
        self.text_frame.insert("0.0", "loool")
        self.configure(require_redraw=True)
        print(self.text_frame.get("0.0", "0.4"))

if __name__=="__main__":
    root=ctk.CTk()
    root.geometry("500x500")
    m_f=Message_frame(master=root)
    m_f.pack(fill="both", expand=True)

    button=ctk.CTkButton(master=root, command=lambda:m_f.write())
    button.pack()
    root.mainloop()