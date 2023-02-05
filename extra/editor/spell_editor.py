import tkinter as tk
import tkinter.ttk as ttk
import json

FILENAME = '../../data/saland/spells/base_spells.json'

def safe_cast(val, to_type, default=None):
    try:
        return to_type(val)
    except (ValueError, TypeError):
        return default

class App:
    def __init__(self, master):
        self.master = master
        self.master.title("JSON File Editor")
        self.master.geometry("800x600")

        self.listbox = tk.Listbox(self.master, width=50)
        self.listbox.pack(side="left", fill="y")
        self.listbox.bind("<<ListboxSelect>>", self.on_listbox_select)

        self.frame = tk.Frame(self.master)
        self.frame.pack(side="right")

        self.name = tk.StringVar()
        self.icon = tk.StringVar()
        self.item_name = tk.StringVar()
        self.tile = tk.StringVar()
        self.type = tk.StringVar()

        tk.Label(self.frame, text="Name").pack()
        tk.Entry(self.frame, textvariable=self.name).pack()
        tk.Label(self.frame, text="Icon").pack()
        tk.Entry(self.frame, textvariable=self.icon).pack()
        tk.Label(self.frame, text="Item Name").pack()
        tk.Entry(self.frame, textvariable=self.item_name).pack()
        tk.Label(self.frame, text="Tile").pack()
        tk.Entry(self.frame, textvariable=self.tile).pack()
        tk.Label(self.frame, text="Type").pack()
        ttk.Combobox(self.frame, textvariable=self.type, values=["dot", "tile", ""]).pack()
        tk.Button(self.frame, text="Update", command=self.update_item).pack()
        tk.Button(self.frame, text="Save", command=self.save).pack()
        tk.Button(self.frame, text="Add", command=self.add).pack()
        tk.Button(self.frame, text="Remove", command=self.remove).pack()

        self.load()

    def visible_name(self, item):
        ret = item["name"]
        if item.get("tile", None):
            ret += ":{}".format(item.get("tile"))
        if item.get("item_name"):
            ret += ":{}".format(item.get("item_name"))
        return ret

    def load(self):
        try:
            with open(FILENAME, "r") as file:
                self.data = json.load(file)
                for item in self.data["items"]:
                    self.listbox.insert("end", self.visible_name(item))
        except Exception as e:
            print("Failed to load: {}. Reason: {}".format(FILENAME, e))
            self.data = {"items": []}

    def save(self):
        with open(FILENAME, "w") as file:
            json.dump(self.data, file, indent=4)

    def update_item(self):
        selection = self.listbox.curselection()
        if selection:
            index = selection[0]
            item = self.data["items"][index]
            item["name"] = self.name.get()
            item["icon"] = self.icon.get()
            item["item_name"] = self.item_name.get()
            item["tile"] = safe_cast(self.tile.get(), int)
            item["type"] = self.type.get()
            self.data["items"][index] = item
            self.save()



    def on_listbox_select(self, event):
        selection = self.listbox.curselection()
        if selection:
            index = selection[0]
            item = self.data["items"][index]
            self.name.set(item.get("name", ""))
            self.icon.set(item.get("icon", ""))
            self.item_name.set(item.get("item_name", ""))
            self.tile.set(safe_cast(item.get("tile", ""), int, ""))
            self.type.set(item.get("type", ""))

    def add(self):
        item = {
            "name": self.name.get(),
            "icon": self.icon.get(),
            "item_name": self.item_name.get(),
            "tile": safe_cast(self.tile.get(), int),
            "type": self.type.get()
        }
        self.data["items"].append(item)
        self.listbox.insert("end", self.visible_name(item))

    def remove(self):
        selection = self.listbox.curselection()
        if selection:
            index = selection[0]
            self.listbox.delete(index)
            self.data["items"].pop(index)


root = tk.Tk()
app = App(root)
root.mainloop()