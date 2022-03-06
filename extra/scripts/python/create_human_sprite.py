

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


def print_weapon(texture, weapon_name, file=sys.stdout):
    race = "human"
    common.directional_sprite.print_sprite(race=race, animation="spellcast", name=weapon_name, texture=texture, topY=0, number_of_frames=8, file=file)
    print(",", file=file)
    common.directional_sprite.print_sprite(race=race, animation="standing", name=weapon_name, texture=texture, topY=256*2, number_of_frames=1, file=file)
    print(",", file=file)
    common.directional_sprite.print_sprite(race=race, animation="walkcycle", name=weapon_name, texture=texture, topX=64, topY=256*2, number_of_frames=8, file=file)
    print(",", file=file)
    common.directional_sprite.print_sprite(race=race, animation="slash", name=weapon_name, texture=texture, topY=256*3, number_of_frames=6, file=file)
    print(",", file=file)
    common.directional_sprite.print_sprite_direction(race=race, animation="hurt", name=weapon_name, texture=texture, topY=256*4, number_of_frames=6, file=file)


def main():
    with open('../../../data/sprites/humans.sprite', 'w') as file:
        print("{", file=file)
        print("\"_comment\" : \"This file is generated with extra/scripts/build_sprites.sh. Do not edit.\"", file=file)
        print(",", file=file)
        print_male(file=file)
        print(",", file=file)
        print_male("female", file=file)
        print("}", file=file)
    with open('../../../data/sprites/weapons.sprite', 'w') as file:
        print("{", file=file)
        print("\"_comment\" : \"This file is generated with extra/scripts/build_sprites.sh. Do not edit.\"", file=file)
        print(",", file=file)
        print_weapon("weapons/long_knife", "long_knife", file=file)
        print("}", file=file)

main()