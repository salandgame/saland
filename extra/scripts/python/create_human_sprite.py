

import common.directional_sprite
import sys

def print_male(race="male", file=sys.stdout):
    common.directional_sprite.print_sprite(race=race, animation="walkcycle", name=None, texture=race+"_walkcycle", topX=64, topY=0, number_of_frames=8, file=file)
    print(",", file=file)
    common.directional_sprite.print_sprite(race=race, animation="standing", name=None, texture=race+"_walkcycle", topY=0, number_of_frames=1, file=file)
    print(",", file=file)
    common.directional_sprite.print_sprite(race=race, animation="spellcast", name=None, texture=race+"_spellcast", topY=0, number_of_frames=7, file=file)
    print(",", file=file)
    common.directional_sprite.print_sprite(race=race, animation="slash", name=None, texture=race+"_slash", topY=0, number_of_frames=6, file=file)
    print(",", file=file)
    common.directional_sprite.print_sprite_direction(race=race, animation="hurt", name=None, texture=race+"_slash", topY=0, number_of_frames=6, file=file)


def main():
    with open('../../../data/sprites/humans.sprite', 'w') as file:
        print("{", file=file)
        print_male(file=file)
        print(",", file=file)
        print_male("female", file=file)
        print("}", file=file)

main()