#! /usr/bin/python3
import os
from nicegui import ui
from nicegui import app

#This is the raltive directory to the base of the SagoSprite dir
#I am launching from extra/editor so I need to go two levels up
BASEDIR = '../..'

app.add_static_files('/textures', BASEDIR+'/data/textures')

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


@ui.page('/texture_select')
async def texture_select():
    img = ui.image('/textures/fallback.png')
    with ui.header(elevated=True).style('background-color: #3874c8').classes('items-center justify-between'):
        ui.label('HEADER')
        ui.button(on_click=lambda: right_drawer.toggle()).props('flat color=white icon=menu')
    with ui.left_drawer(top_corner=True, bottom_corner=True).style('background-color: #d7e3f4'):
        ui.label('Texture list')
        for texture in populateTree(""):
            ui.label(texture)
    with ui.right_drawer(fixed=False).style('background-color: #ebf1fa').props('bordered') as right_drawer:
        ui.label('RIGHT DRAWER')
    with ui.footer().style('background-color: #3874c8'):
        ui.label('FOOTER')


ui.link('texture_select', texture_select)
ui.run()

