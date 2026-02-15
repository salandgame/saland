#!/usr/bin/env python3

import sys
import json

ANIMATIONS_TO_GENERATE = ["slash", "spellcast", "standing", "walkcycle"]
ANIMATIONS_TO_GENERATE_ONLY_S = ["hurt"]

# Maps race name (used in sprite names) to folder name (used in texture paths)
RACE_FOLDER_MAP = {"male": "male", "female": "thin"}

# Call like: python3 generate_sprite.py pants2 black > ../../sprites/pants2_black.sprite

# Animation parameters: number of frames
ANIMATION_FRAMES = {
    "standing": 1,
    "walkcycle": 8,
    "spellcast": 7,
    "slash": 6,
    "hurt": 6
}

# Animation to folder name mapping
ANIMATION_FOLDER = {
    "standing": "idle",
    "walkcycle": "walk",
    "spellcast": "spellcast",
    "slash": "slash",
    "hurt": "hurt"
}

# Directions and their Y offsets
DIRECTIONS = {
    "N": 0,
    "W": 64,
    "S": 128,
    "E": 192
}

def generate_sprite_entry(race, animation, item_name, color, direction, topx=0):
    """Generate a single sprite entry"""
    race_folder = RACE_FOLDER_MAP.get(race, race)
    animation_folder = ANIMATION_FOLDER.get(animation, animation)
    
    # Texture path: spritesheet/{item_name}/{race_folder}/{animation_folder}/{color}
    texture_name = f"spritesheet/{item_name}/{race_folder}/{animation_folder}/{color}"
    # Sprite name: {race}_{animation}_{item_name}_{color}_{direction}
    sprite_name = f"{race}_{animation}_{item_name}_{color}_{direction}"
    
    topy = DIRECTIONS[direction]
    num_frames = ANIMATION_FRAMES.get(animation, 1)
    
    entry = {
        "texture": texture_name,
        "topx": topx,
        "topy": topy,
        "height": 64,
        "width": 64,
        "number_of_frames": num_frames,
        "frame_time": 100,
        "originx": 32,
        "originy": 50
    }
    
    return sprite_name, entry

def generate_sprites(item_name, color):
    """Generate all sprite entries for the given item and color"""
    sprites = {}
    
    for race in ["male", "female"]:
        # Generate animations with all 4 directions
        for animation in ANIMATIONS_TO_GENERATE:
            topx = 0
            
            # Special case: standing and walkcycle share the same texture file (idle.png and walk.png in same item)
            # but they use different animation folders
            if animation == "walkcycle":
                topx = 64  # walkcycle starts after standing frame in walk spritesheet
            
            for direction in ["N", "W", "S", "E"]:
                sprite_name, entry = generate_sprite_entry(race, animation, item_name, color, direction, topx)
                sprites[sprite_name] = entry
        
        # Generate animations with only S direction
        for animation in ANIMATIONS_TO_GENERATE_ONLY_S:
            sprite_name, entry = generate_sprite_entry(race, animation, item_name, color, "S", topx=0)
            # For hurt animation, it uses its own texture with topy=0
            entry["topy"] = 0
            sprites[sprite_name] = entry
    
    return sprites

def main():
    if len(sys.argv) < 3:
        print("Usage: python3 generate_sprite.py <item_name> <color>", file=sys.stderr)
        print("Example: python3 generate_sprite.py pants2 black > ../../sprites/pants2_black.sprite", file=sys.stderr)
        sys.exit(1)
    
    item_name = sys.argv[1]
    color = sys.argv[2]
    sprites = generate_sprites(item_name, color)
    
    # Output JSON with nice formatting
    print(json.dumps(sprites, indent=4))

if __name__ == "__main__":
    main()
