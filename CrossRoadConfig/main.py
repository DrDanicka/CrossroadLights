import tkinter as tk
from tkinter import messagebox, filedialog
import json
from enum import Enum
from road import RoadNorth, RoadEast, RoadSouth, RoadWest

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

        self.roads = []
        self.buttons = {}

        self.btn_frame = tk.Frame(root)
        self.btn_frame.pack()

        self.buttons[Orientation.NORTH] = tk.Button(self.btn_frame, text="Add Road N", command=lambda: self.add_road(Orientation.NORTH))
        self.buttons[Orientation.NORTH].pack(side="left", padx=5)

        self.buttons[Orientation.SOUTH] = tk.Button(self.btn_frame, text="Add Road S", command=lambda: self.add_road(Orientation.SOUTH))
        self.buttons[Orientation.SOUTH].pack(side="left", padx=5)

        self.buttons[Orientation.EAST] = tk.Button(self.btn_frame, text="Add Road E", command=lambda: self.add_road(Orientation.EAST))
        self.buttons[Orientation.EAST].pack(side="left", padx=5)

        self.buttons[Orientation.WEST] = tk.Button(self.btn_frame, text="Add Road W", command=lambda: self.add_road(Orientation.WEST))
        self.buttons[Orientation.WEST].pack(side="left", padx=5)

        self.btn_save = tk.Button(root, text="Save Config", command=self.save_config)
        self.btn_save.pack()

    def add_road(self, orientation: Orientation):
        if len(self.roads) >= 4:
            messagebox.showwarning("Limit Reached", "A crossroad can have a maximum of 4 roads.")
            return

        road_window = tk.Toplevel(self.root)
        road_window.title("Add Road")

        tk.Label(road_window, text="Allowed Directions").pack()
        directions = {"S": tk.BooleanVar(), "L": tk.BooleanVar(), "R": tk.BooleanVar()}

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
            if orientation == Orientation.NORTH:
                road = RoadNorth(self.canvas, {k: v.get() for k, v in directions.items()}, int(red_time.get()), int(green_time.get()), pedestrian_var.get())
            elif orientation == Orientation.EAST:
                road = RoadEast(self.canvas, {k: v.get() for k, v in directions.items()}, int(red_time.get()), int(green_time.get()), pedestrian_var.get())
            elif orientation == Orientation.SOUTH:
                road = RoadSouth(self.canvas, {k: v.get() for k, v in directions.items()}, int(red_time.get()), int(green_time.get()), pedestrian_var.get())
            elif orientation == Orientation.WEST:
                road = RoadWest(self.canvas, {k: v.get() for k, v in directions.items()}, int(red_time.get()), int(green_time.get()), pedestrian_var.get())
            self.roads.append(road)
            road_window.destroy()
            self.draw_road(road, orientation)
            self.buttons[orientation].destroy()
            del self.buttons[orientation]

        tk.Button(road_window, text="Save", command=save_road).pack()

    def draw_road(self, road, orientation: Orientation):
        positions = {
            Orientation.NORTH: (200, 50),
            Orientation.WEST: (200, 200),
            Orientation.SOUTH: (200, 200),
            Orientation.EAST: (50, 200)
        }
        road.draw(*positions[orientation])

    def save_config(self):
        if len(self.roads) < 3:
            messagebox.showwarning("Minimum Roads", "A crossroad must have at least 3 roads.")
            return

        file_path = filedialog.asksaveasfilename(defaultextension=".json", filetypes=[("JSON files", "*.json")])
        if file_path:
            with open(file_path, "w") as file:
                json.dump([road.__dict__ for road in self.roads], file, indent=4)
            messagebox.showinfo("Saved", "Configuration saved successfully!")

if __name__ == "__main__":
    root = tk.Tk()
    app = TrafficLightConfigurator(root)
    root.mainloop()
