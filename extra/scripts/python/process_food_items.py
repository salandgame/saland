import pandas as pd
import json

FILENAME="../food_items.csv"
FILENAME_SPRITE="../../../data/sprites/food.sprite"

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

def main():
    produce_sprite_file()
    reader = pd.read_csv(FILENAME)
    for row in reader.itertuples():
        print(row)


main()