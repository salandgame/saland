#!/usr/bin/env python3
"""
Sprite Definition Generator for Saland Adventures

This script generates sprite definition JSON files for character clothing/equipment items.
It creates sprite entries for all 5 character animations (slash, spellcast, standing, 
walkcycle, and hurt) across both male and female character models.

Usage:
    python3 generate_sprite.py <item_name> <color1> [color2 ...] > ../../sprites/<item_name>.sprite

Arguments:
    item_name: Name of the clothing/equipment item (e.g., "pants2", "shirt1")
    colors: One or more color variants (e.g., "black", "blue", "red")

Output:
    Generates JSON with sprite definitions in the format:
    {
        "race_animation_item_color_direction": {
            "texture": "spritesheet/item/race_folder/animation_folder/color",
            "topx": <x_offset>,
            "topy": <y_offset>,
            "height": 64,
            "width": 64,
            "number_of_frames": <frame_count>,
            "frame_time": 100,
            "originx": 32,
            "originy": 50
        },
        ...
    }

Animations Generated:
    - slash: Attack animation, 4 directions (N/W/S/E), 6 frames
    - spellcast: Magic casting animation, 4 directions (N/W/S/E), 7 frames
    - standing: Idle pose, 4 directions (N/W/S/E), 1 frame
    - walkcycle: Walking animation, 4 directions (N/W/S/E), 8 frames
    - hurt: Damage reaction, 1 direction (S only), 6 frames

Notes:
    - Sprite names use "male" and "female" for character race
    - Texture paths use "male" and "thin" folders (thin = female body type)
    - Standing and walkcycle animations use separate texture folders (idle/walk)
    - Total output: 34 sprite entries (17 per race)

Examples:
    # Single color:
    python3 generate_sprite.py pants2 black > ../../sprites/pants2_black.sprite
    
    # Multiple colors:
    python3 generate_sprite.py pants2 black blue red > ../../sprites/pants2.sprite
    
    This generates sprites like:
    - "male_walkcycle_pants2_black_N" -> texture: "spritesheet/pants2/male/walk/black"
    - "male_walkcycle_pants2_blue_N" -> texture: "spritesheet/pants2/male/walk/blue"
    - "female_hurt_pants2_black_S" -> texture: "spritesheet/pants2/thin/hurt/black"
"""

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
        print("Usage: python3 generate_sprite.py <item_name> <color1> [color2 ...]", file=sys.stderr)
        print("Example: python3 generate_sprite.py pants2 black > ../../sprites/pants2_black.sprite", file=sys.stderr)
        print("         python3 generate_sprite.py pants2 black blue red > ../../sprites/pants2.sprite", file=sys.stderr)
        sys.exit(1)
    
    item_name = sys.argv[1]
    colors = sys.argv[2:]  # All remaining arguments are colors
    
    # Generate sprites for all colors and merge into one dictionary
    all_sprites = {}
    for color in colors:
        sprites = generate_sprites(item_name, color)
        all_sprites.update(sprites)
    
    # Output JSON with nice formatting
    print(json.dumps(all_sprites, indent=4))

if __name__ == "__main__":
    main()
