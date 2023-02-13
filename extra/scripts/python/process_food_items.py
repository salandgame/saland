import pandas as pd

FILENAME="../food_items.csv"

def main():
    reader = pd.read_csv(FILENAME)
    for row in reader.itertuples():
        print(row)


main()