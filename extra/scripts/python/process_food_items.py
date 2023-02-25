import pandas as pd
import json

FILENAME="../food_items.csv"
FILENAME_SPRITE="../../../data/sprites/food.sprite"
FILENAME_ITEMS="../../../data/saland/items/food_items.json"
FILENAME_SPELLS="../../../data/saland/spells/spell_food_items.json"

def produce_sprite_file():
    df = pd.read_csv(FILENAME)
    items = {}

    for index, row in df.iterrows():
        name = "item_food_" + row["food_name"]
        item = {
            "texture": "food",
            "topx": int(row["x"]),
            "topy": int(row["y"]),
            "height": 32,
            "width": 32,
            "originx": 16,
            "originy": 16
        }
        items[name] = item

    # Save the dictionary as a json file
    with open(FILENAME_SPRITE, "w") as f:
        json.dump(items, f, indent=4)


def produce_item_file():
    df = pd.read_csv(FILENAME)
    items = {}
    items_arrays = []

    for index, row in df.iterrows():
        name = "item_food_" + row["food_name"]
        item = {
            "itemid": "food_"+row["food_name"],
            "sprite": name,
            "description": row["description"],
            "radius": float(row["radius"]),
            "isStatic": False,
            "pickup": True,
            "isDestructible": False
        }
        items_arrays.append(item)
    items["items"] = items_arrays
    # Save the dictionary as a json file
    with open(FILENAME_ITEMS, "w") as f:
        json.dump(items, f, indent=4)


def produce_spell_file():
    df = pd.read_csv(FILENAME)
    items = {}
    items_arrays = []

    for index, row in df.iterrows():
        name = "item_food_" + row["food_name"]
        item = {
            "icon": name,
            "name": "spell_spawn_item",
            "item_name": row["food_name"]
        }
        items_arrays.append(item)
    items["spells"] = items_arrays
    # Save the dictionary as a json file
    with open(FILENAME_SPELLS, "w") as f:
        json.dump(items, f, indent=4)


def main():
    produce_sprite_file()
    produce_item_file()
    produce_spell_file()
    reader = pd.read_csv(FILENAME)
    for row in reader.itertuples():
        print(row)


main()