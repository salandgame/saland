#! /usr/bin/pyhton3

import sys
import os

clothes = sys.argv[1:]
RACE = "female"
DIRECTIONS = ['N','W','S','E']

# Call like: python3 generate_sprite.py blouse  corset  long-sleeve_blouse  long-sleeve_shirt  pirate_shirt scoop_neck  sleeveless_shirt > female_shirts.sprite
# Remove last comma in the file

def printArmor(name):
    colors = os.listdir(name)
    for c in colors:
        if not c.startswith("_"):
            topY=0
            for direction in DIRECTIONS:
                print("\""+RACE+"_walkcycle_"+name+"_"+c+"_"+direction+"\" : {")
                print("\"texture\" : \"lpc_womens_shirts/"+name+"/"+c+"/walk\",")
                print("\"topx\" : 0,")
                print("\"topy\" : "+str(topY)+",")
                print("\"height\" : 64,")
                print("\"width\" : 64,")
                print("\"number_of_frames\" : 8,")
                print("\"frame_time\" : 100,")
                print("\"originx\" : 32,")
                print("\"originy\" : 50")
                print("},")
                topY += 64
            topY=0
            for direction in DIRECTIONS:
                print("\""+RACE+"_standing_"+name+"_"+c+"_"+direction+"\" : {")
                print("\"texture\" : \"lpc_womens_shirts/"+name+"/"+c+"/idle\",")
                print("\"topx\" : 0,")
                print("\"topy\" : "+str(topY)+",")
                print("\"height\" : 64,")
                print("\"width\" : 64,")
                print("\"number_of_frames\" : 1,")
                print("\"frame_time\" : 100,")
                print("\"originx\" : 32,")
                print("\"originy\" : 50")
                print("},")
                topY += 64
            topY=0
            for direction in DIRECTIONS:
                print("\""+RACE+"_spellcast_"+name+"_"+c+"_"+direction+"\" : {")
                print("\"texture\" : \"lpc_womens_shirts/"+name+"/"+c+"/magic\",")
                print("\"topx\" : 0,")
                print("\"topy\" : "+str(topY)+",")
                print("\"height\" : 64,")
                print("\"width\" : 64,")
                print("\"number_of_frames\" : 7,")
                print("\"frame_time\" : 100,")
                print("\"originx\" : 32,")
                print("\"originy\" : 50")
                print("},")
                topY += 64
            topY=0
            for direction in DIRECTIONS:
                print("\""+RACE+"_slash_"+name+"_"+c+"_"+direction+"\" : {")
                print("\"texture\" : \"lpc_womens_shirts/"+name+"/"+c+"/swing\",")
                print("\"topx\" : 0,")
                print("\"topy\" : "+str(topY)+",")
                print("\"height\" : 64,")
                print("\"width\" : 64,")
                print("\"number_of_frames\" : 6,")
                print("\"frame_time\" : 100,")
                print("\"originx\" : 32,")
                print("\"originy\" : 50")
                print("},")
                topY += 64
            # hurt is only south facing
            topY=0
            if True:
                direction = 'S'
                print("\""+RACE+"_hurt_"+name+"_"+c+"_"+direction+"\" : {")
                print("\"texture\" : \"lpc_womens_shirts/"+name+"/"+c+"/hurt\",")
                print("\"topx\" : 0,")
                print("\"topy\" : "+str(topY)+",")
                print("\"height\" : 64,")
                print("\"width\" : 64,")
                print("\"number_of_frames\" : 6,")
                print("\"frame_time\" : 100,")
                print("\"originx\" : 32,")
                print("\"originy\" : 50")
                print("},")



print("{")
for a in clothes:
    printArmor(a)
print("}")