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


def addLinesToCanvas(canvas, image_file, xstep = 32, ystep = 32, xoffset=0, yoffset=0):
    canvas_width = image_file.width()
    canvas_height = image_file.height()
    if xstep > 0:
        for i in range(xoffset, canvas_width, xstep):
            canvas.create_line(i, 0, i, canvas_height, fill="red", width=1)
    if ystep > 0:
        for i in range(yoffset, canvas_height, ystep):
            canvas.create_line(0, i, canvas_width, i, fill="red", width=1)


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



def callback_select(event):
    if not treeview.selection():
        return
    image = tk.PhotoImage(file=BASEDIR+'/data/textures/'+treeview.selection()[0])
    imageFrame.set_image(image)
    addLinesToCanvas(imageFrame.canvas, imageFrame.image_file)
    status_file['text'] = "File: " + treeview.selection()[0]
    status_image['text'] = "Size: " + str(image.width())+"*"+str(image.height())


def callback_filter(sv):
    print( sv.get() )
    for i in treeview.get_children():
        treeview.delete(i)
    textures = populateTree(sv.get())
    for t in textures:
        treeview.insert('','end',t, text = t)


def callback_canvas_motion(event):
    x, y = event.x, event.y
    if not imageFrame or not imageFrame.image_file:
        return
    status_mouse_over['text'] = "Top: "+str(x-(x%32))+","+str(y-(y%32))+", tile: "+ str( (y//32)*imageFrame.image_file.width()//32+x//32+1 )
    #print('{}, {}'.format(x, y))


if __name__ == "__main__":
    textures = populateTree()
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
    imageFrame.canvas.bind('<Motion>', callback_canvas_motion)

    treeFrame.filterString.trace("w", lambda name, index, mode, sv=treeFrame.filterString: callback_filter(treeFrame.filterString))

    # status bar
    status_frame = tk.Frame(root)
    status_file = tk.Label(status_frame, text="File: NO FILE SELECTED")
    status_file.pack(fill="both", side=tk.LEFT, expand=True)
    status_image = tk.Label(status_frame, text="Image size: X*Y")
    status_image.pack(fill="both", side=tk.LEFT, expand=True)
    status_mouse_over = tk.Label(status_frame, text="Top: x,Y")
    status_mouse_over.pack(fill="both", side=tk.LEFT, expand=True)
    status_frame.grid(row=2, column=0, columnspan=2, sticky="ew")

    root.mainloop()