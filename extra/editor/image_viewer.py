#! /bin/python3

import tkinter as tk
from tkinter import ttk
from image_frame import *

#This is the raltive directory to the base of the SagoSprite dir
#I am launching from extra/editor so I need to go two levels up
BASEDIR = '../..'

root = tk.Tk()
image = tk.PhotoImage(file=BASEDIR+'/data/textures/terrain.png')
imageFrame = ImageFrame(root, image)
imageFrame.get_frame().pack(expand=tk.YES, fill=tk.BOTH)

import os
textures = os.listdir(BASEDIR+"/data/textures")
textures = list(filter(lambda x: x.endswith('.png'), textures))
sprites = os.listdir(BASEDIR+"/data/sprites")
sprites = list(filter(lambda x: x.endswith('.sprite'), sprites))
print(textures, sprites)

root.mainloop()
