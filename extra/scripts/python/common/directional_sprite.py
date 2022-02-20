
import sys


DIRECTIONS = ['N','W','S','E']


def print_sprite_direction(race, animation, name, texture, topY=0, topX=0, number_of_frames=1, frame_time=100, file=sys.stdout, direction="S"):
    name_part = ""
    if name:
        name_part = name+"_"
    print("\""+race+"_"+animation+"_"+name_part+direction+"\" : {", file=file)
    print("\"texture\" : \""+texture+"\",", file=file)
    print("\"topx\" : "+str(topX)+",", file=file)
    print("\"topy\" : "+str(topY)+",", file=file)
    print("\"height\" : 64,", file=file)
    print("\"width\" : 64,", file=file)
    print("\"number_of_frames\" : "+str(number_of_frames)+",", file=file)
    print("\"frame_time\" : 100,", file=file)
    print("\"originx\" : 32,", file=file)
    print("\"originy\" : 50", file=file)
    print("}", file=file)

def print_sprite(race, animation, name, texture, topY=0, topX=0, number_of_frames=1, frame_time=100, file=sys.stdout):
    first = True
    for direction in DIRECTIONS:
        if first:
            first = False
        else:
            print(",", file=file)
        print_sprite_direction(race, animation, name, texture, topY, topX, number_of_frames, frame_time, file, direction)
        topY += 64
        
        

