import tkinter as tk
from tkinter import messagebox, filedialog
import json
from enum import Enum
from drawing_roads import draw_road_north, draw_road_west, draw_road_south, draw_road_east


class Orientation(Enum):
    NORTH = 1
    EAST = 2
    SOUTH = 3
    WEST = 4

class TrafficLightConfigurator:
    def __init__(self, root):
        self.root = root
        self.root.title("Traffic Light Configurator")

        self.canvas = tk.Canvas(root, width=500, height=500, bg="green")
        self.canvas.pack()

        self.canvas.create_text(250, 20, text="N", font=("Arial", 24, "bold"))
        self.canvas.create_text(480, 250, text="W", font=("Arial", 24, "bold"))
        self.canvas.create_text(250, 480, text="S", font=("Arial", 24, "bold"))
        self.canvas.create_text(20, 250, text="E", font=("Arial", 24, "bold"))

        self.config = {"roads": []}

        self.btn_frame = tk.Frame(root)
        self.btn_frame.pack()

        self.btn_add_road_n = tk.Button(self.btn_frame, text="Add Road N", command=lambda: self.add_road(Orientation.NORTH))
        self.btn_add_road_n.pack(side="left", padx=5)

        self.btn_add_road_s = tk.Button(self.btn_frame, text="Add Road S", command=lambda: self.add_road(Orientation.SOUTH))
        self.btn_add_road_s.pack(side="left", padx=5)

        self.btn_add_road_e = tk.Button(self.btn_frame, text="Add Road E", command=lambda: self.add_road(Orientation.EAST))
        self.btn_add_road_e.pack(side="left", padx=5)

        self.btn_add_road_w = tk.Button(self.btn_frame, text="Add Road W", command=lambda: self.add_road(Orientation.WEST))
        self.btn_add_road_w.pack(side="left", padx=5)

        self.btn_save = tk.Button(root, text="Save Config", command=self.save_config)
        self.btn_save.pack()

    def add_road(self, orientation: Orientation):
        if len(self.config["roads"]) >= 4:
            messagebox.showwarning("Limit Reached", "A crossroad can have a maximum of 4 roads.")
            return

        road_window = tk.Toplevel(self.root)
        road_window.title("Add Road")

        tk.Label(road_window, text="Allowed Directions").pack()
        directions = {"Straight": tk.BooleanVar(), "Left": tk.BooleanVar(), "Right": tk.BooleanVar()}

        for direction, var in directions.items():
            tk.Checkbutton(road_window, text=direction, variable=var).pack()

        tk.Label(road_window, text="Red Light Duration (s)").pack()
        red_time = tk.Entry(road_window)
        red_time.pack()

        tk.Label(road_window, text="Green Light Duration (s)").pack()
        green_time = tk.Entry(road_window)
        green_time.pack()

        pedestrian_var = tk.BooleanVar()
        tk.Checkbutton(road_window, text="Has Pedestrian Light", variable=pedestrian_var).pack()

        def save_road():
            road_data = {
                "directions": {k: v.get() for k, v in directions.items()},
                "red_time": int(red_time.get()),
                "green_time": int(green_time.get()),
                "pedestrian": pedestrian_var.get()
            }
            self.config["roads"].append(road_data)
            road_window.destroy()
            self.draw_road(orientation=orientation, direction=road_data["directions"], pedestrian=road_data["pedestrian"])

        tk.Button(road_window, text="Save", command=save_road).pack()


    def draw_road(self, orientation: Orientation, direction: dict, pedestrian: bool):
        if orientation == Orientation.NORTH:
            draw_road_north(self.canvas, 200, 50)
        elif orientation == Orientation.WEST:
            draw_road_west(self.canvas, 200, 200)
        elif orientation == Orientation.SOUTH:
            draw_road_south(self.canvas, 200, 200)
        elif orientation == Orientation.EAST:
            draw_road_east(self.canvas, 50, 200)


    def save_config(self):
        if len(self.config["roads"]) < 3:
            messagebox.showwarning("Minimum Roads", "A crossroad must have at least 3 roads.")
            return

        file_path = filedialog.asksaveasfilename(defaultextension=".json", filetypes=[("JSON files", "*.json")])
        if file_path:
            with open(file_path, "w") as file:
                json.dump(self.config, file, indent=4)
            messagebox.showinfo("Saved", "Configuration saved successfully!")


if __name__ == "__main__":
    root = tk.Tk()
    app = TrafficLightConfigurator(root)
    root.mainloop()
