import customtkinter as ctk

class Scrollable_frame(ctk.CTkFrame):
    """
    Scrollable frame
    Object must be added in scrollable_frame
    TODO: make it work with the mouse-wheel
    """
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

        canvas = ctk.CTkCanvas(self)
        scrollbar = ctk.CTkScrollbar(self, orientation="vertical", command=canvas.yview)
        self.scrollable_frame = ctk.CTkFrame(canvas)

        self.scrollable_frame.bind(
            "<Configure>",
            lambda e: canvas.configure(
                scrollregion=canvas.bbox("all")
            )
        )

        canvas.create_window((0, 0), window=self.scrollable_frame, anchor="nw")
        canvas.configure(yscrollcommand=scrollbar.set)

        canvas.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")