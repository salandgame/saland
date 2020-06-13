import tkinter as tk
from tkinter import ttk

class TreeFrame:
    """Create a frame with a tree view
    """
    def __init__(self, master):
        self.master = master
        self.frame = ttk.Frame(master)
        self.treeview = ttk.Treeview(self.frame)
        self.xscroll = ttk.Scrollbar(self.frame, orient=tk.HORIZONTAL, command=self.treeview.xview)
        self.yscroll = ttk.Scrollbar(self.frame, orient=tk.VERTICAL, command=self.treeview.yview)
        self.treeview.grid(row=1, column=0, sticky='nsew')
        self.frame.columnconfigure(0, weight=1)
        self.frame.rowconfigure(1, weight=1)
        self.xscroll.grid(row=2, column=0, sticky='ew')
        self.yscroll.grid(row=1, column=2, sticky='ns')
        self.treeview.config(xscrollcommand=self.xscroll.set, yscrollcommand=self.yscroll.set)
        #Add filter box
        self.filterString = tk.StringVar()
        self.filterEntryBox = tk.Entry (self.frame, textvariable=self.filterString)
        self.filterEntryBox.grid(row=3, column=0, sticky='nsew')
        

    def get_treeview(self):
        return self.treeview

    def get_frame(self):
        """Retruns a reference to the frame"""
        return self.frame