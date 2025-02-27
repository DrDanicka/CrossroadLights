import tkinter as tk
from tkinter import messagebox, filedialog
import json
from orientation import Orientation
from road import RoadNorth, RoadEast, RoadSouth, RoadWest
from crossroad_validator import validate_crossroad

class TrafficLightConfigurator:
    def __init__(self, root):
        self.root = root
        self.root.title("Traffic Light Configurator")

        self.background_color = 'green'
        self.canvas = tk.Canvas(root, width=500, height=500, bg=self.background_color)
        self.canvas.pack()

        self.canvas.create_text(250, 20, text="N", font=("Arial", 24, "bold"))
        self.canvas.create_text(480, 250, text="W", font=("Arial", 24, "bold"))
        self.canvas.create_text(250, 480, text="S", font=("Arial", 24, "bold"))
        self.canvas.create_text(20, 250, text="E", font=("Arial", 24, "bold"))

        self.roads = {}
        self.buttons = {}

        self.btn_frame = tk.Frame(root)
        self.btn_frame.pack()

        for orientation in Orientation:
            self.buttons[orientation] = tk.Button(self.btn_frame, text=f"Add Road from {orientation.name[0]}", command=lambda o=orientation: self.add_road(o))
            self.buttons[orientation].pack(side="left", padx=5)

        self.btn_save = tk.Button(root, text="Save Config", command=self.save_config)
        self.btn_save.pack()

    def add_road(self, orientation: Orientation):
        if len(self.roads) >= 4:
            messagebox.showwarning("Limit Reached", "A crossroad can have a maximum of 4 roads.")
            return

        road_window = tk.Toplevel(self.root)
        road_window.title("Add Road")

        tk.Label(road_window, text="Allowed Directions").pack()
        directions = {"Straight": tk.BooleanVar(), "Left": tk.BooleanVar(), "Right": tk.BooleanVar()}

        for direction, var in directions.items():
            tk.Checkbutton(road_window, text=direction, variable=var).pack()

        tk.Label(road_window, text="Green Light Duration (s)").pack()
        green_time = tk.Entry(road_window)
        green_time.pack()

        pedestrian_var = tk.BooleanVar()
        tk.Checkbutton(road_window, text="Has Pedestrian Light", variable=pedestrian_var).pack()

        def save_road():
            road_classes = {Orientation.NORTH: RoadNorth, Orientation.EAST: RoadEast, Orientation.SOUTH: RoadSouth, Orientation.WEST: RoadWest}
            road = road_classes[orientation](self.canvas, {k: v.get() for k, v in directions.items()}, int(green_time.get()), pedestrian_var.get())
            self.roads[orientation] = road
            road_window.destroy()
            self.draw_road(road, orientation)
            self.buttons[orientation].pack_forget()
            self.add_delete_button(orientation)

        tk.Button(road_window, text="Save", command=save_road).pack()

    def draw_road(self, road, orientation: Orientation):
        positions = {
            Orientation.NORTH: (200, 50),
            Orientation.WEST: (200, 200),
            Orientation.SOUTH: (200, 200),
            Orientation.EAST: (50, 200)
        }
        road.draw(*positions[orientation])

    def add_delete_button(self, orientation: Orientation):
        delete_btn = tk.Button(self.btn_frame, text=f"Delete Road from {orientation.name[0]}", command=lambda: self.delete_road(orientation))
        delete_btn.pack(side="left", padx=5)
        self.buttons[orientation] = delete_btn

    def delete_road(self, orientation: Orientation):
        if orientation in self.roads:
            self.roads[orientation].delete(self.background_color)
            del self.roads[orientation]
            self.buttons[orientation].destroy()
            self.buttons[orientation] = tk.Button(self.btn_frame, text=f"Add Road from {orientation.name[0]}", command=lambda o=orientation: self.add_road(o))
            self.buttons[orientation].pack(side="left", padx=5)

    def save_config(self):
        if len(self.roads) < 3:
            messagebox.showwarning("Minimum Roads", "A crossroad must have at least 3 roads.")
            return

        validation_result, message = validate_crossroad(self.roads)
        if validation_result:
            pass
        else:
            messagebox.showerror("Invalid Crossroad", message)
            return

        file_path = filedialog.asksaveasfilename(defaultextension=".json", filetypes=[("JSON files", "*.json")])
        if file_path:
            with open(file_path, "w") as file:
                json.dump([road.__dict__ for road in self.roads.values()], file, indent=4)
            messagebox.showinfo("Saved", "Configuration saved successfully!")

if __name__ == "__main__":
    root = tk.Tk()
    app = TrafficLightConfigurator(root)
    root.mainloop()
