import tkinter as tk
from tkinter import ttk

class ImageFrame:
    """Create a frame with a cancas and scrollbars for displaying an image
    """
    def __init__(self, master, image):
        """Keyword arguments:
        master - The root element that the frame must be attached to
        image - The initial image"""
        self.master = master
        self.frame = ttk.Frame(master)
        self.canvas = tk.Canvas(self.frame)
        self.xscroll = ttk.Scrollbar(self.frame, orient=tk.HORIZONTAL, command=self.canvas.xview)
        self.yscroll = ttk.Scrollbar(self.frame, orient=tk.VERTICAL, command=self.canvas.yview)
        self.canvas.grid(row=1, column=0, sticky='nsew')
        self.frame.columnconfigure(0, weight=1)
        self.frame.rowconfigure(1, weight=1)
        self.xscroll.grid(row=2, column=0, sticky='ew')
        self.yscroll.grid(row=1, column=2, sticky='ns')
        self.canvas.config(xscrollcommand=self.xscroll.set, yscrollcommand=self.yscroll.set)
        self.set_image(image)

    def set_image(self, image):
        """Set/Updates the image to display
        ImageFrame will store a reference to the image to prevent it from going out of scope
        This will also clear the canvas
        """
        self.image_file = image
        self.canvas.delete("all")
        if self.image_file:
            self.canvas.create_image(0, 0, image=self.image_file, anchor=tk.NW)
            self.canvas.config(scrollregion=(0, 0, self.image_file.width(), self.image_file.height()))
        else:
            canvas_width=20
            canvas_height=20
            self.canvas.delete("all")
            self.canvas.config(scrollregion=(0, 0, canvas_width, canvas_height))

    def get_frame(self):
        """Retruns a reference to the frame"""
        return self.frame
