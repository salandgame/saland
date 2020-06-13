#! /bin/python3

import tkinter as tk
from image_frame import *
from tree_frame import *

#This is the raltive directory to the base of the SagoSprite dir
#I am launching from extra/editor so I need to go two levels up
BASEDIR = '../..'

import os
#textures_filenames = os.listdir(BASEDIR+"/data/textures")
#textures = list(filter(lambda x: x.endswith('.png'), textures_filenames))
#textures.sort()


def addFolderToList(theFolder, theList, filter2 = None):
    textures_filenames = os.listdir(BASEDIR+"/data/textures/"+theFolder)
    for x in textures_filenames:
        if x.endswith('.png'):
            if not filter2 or filter2 in x or filter2 in theFolder:
                theList.append(theFolder+"/"+x)
    folders = list(filter(lambda x: os.path.isdir(BASEDIR+"/data/textures/"+theFolder+"/"+x), textures_filenames))
    print(folders)
    for f in folders:
        addFolderToList(theFolder+"/"+f, theList, filter2)

def populateTree(filter2 = None):
    textures = []
    addFolderToList("", textures, filter2)
    return textures
#folders = list(filter(lambda x: os.path.isdir(BASEDIR+"/data/textures/"+x), textures_filenames))
#addFolderToList()
#print(folders)

textures = populateTree()

def callback_select(event):
    if not treeview.selection():
        return
    image = tk.PhotoImage(file=BASEDIR+'/data/textures/'+treeview.selection()[0])
    imageFrame.set_image(image)


def callback_filter(sv):
    print( sv.get() )
    for i in treeview.get_children():
        treeview.delete(i)
    textures = populateTree(sv.get())
    for t in textures:
        treeview.insert('','end',t, text = t)


root = tk.Tk()
treeFrame = TreeFrame(root)
treeview = treeFrame.get_treeview()
treeFrame.get_frame().grid(row=0, column=0, sticky='ns')
for t in textures:
    treeview.insert('','end',t, text = t)
root.columnconfigure(1, weight=1)
root.rowconfigure(0, weight=1)
treeview.bind('<<TreeviewSelect>>', callback_select)

imageFrame = ImageFrame(root, None)
imageFrame.get_frame().grid(row=0, column=1, sticky='nsew')

treeFrame.filterString.trace("w", lambda name, index, mode, sv=treeFrame.filterString: callback_filter(treeFrame.filterString))
root.mainloop()