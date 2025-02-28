import tkinter as tk
from tkinter import messagebox, filedialog
import struct
from orientation import Orientation
from road import RoadNorth, RoadEast, RoadSouth, RoadWest
from crossroad_validator import validate_crossroad
import serial

class TrafficLightConfigurator:
    def __init__(self, root):
        self.root = root
        self.root.title("Traffic Light Configurator")

        self.background_color = '#29bb17'
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
            self.buttons[orientation].pack(side="left", padx=5, pady=5)

        self.btn_verify = tk.Button(root, text="Verify Config", command=self.verify_config)
        self.btn_verify.pack(pady=5)

        self.btn_save = tk.Button(root, text="Save Config", command=self.save_config)
        self.btn_save.pack(pady=5)

        self.btn_send = tk.Button(root, text="Send via USART", command=self.send_config_to_usart)
        self.btn_send.pack(pady=5)


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
            # Create object of a road based of the orientation
            road = road_classes[orientation](self.canvas, {k: v.get() for k, v in directions.items()}, int(green_time.get()), pedestrian_var.get())
            # Save the object in the dictionary
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
            # Delete the road from the canvas
            self.roads[orientation].delete(self.background_color)
            # Delete the road from the dictionary
            del self.roads[orientation]
            self.buttons[orientation].destroy()
            self.buttons[orientation] = tk.Button(self.btn_frame, text=f"Add Road from {orientation.name[0]}", command=lambda o=orientation: self.add_road(o))
            self.buttons[orientation].pack(side="left", padx=5)

    def encode_roads(self):
        crossroad_encoded = bytearray()
        crossroad_encoded.append(0xD9)  # Start byte

        orientation_mapping = {
            Orientation.NORTH: 0b00,
            Orientation.WEST: 0b01,
            Orientation.SOUTH: 0b10,
            Orientation.EAST: 0b11
        }

        for i, orientation in enumerate(orientation_mapping.keys()):
            if orientation not in self.roads:
                encoded = orientation_mapping[orientation] << 14  # Encode orientation and set the rest to 0
            else:
                road = self.roads[orientation]

                directions = (road.directions["Straight"] << 2) | \
                             (road.directions["Left"] << 1) | \
                             (road.directions["Right"])

                # Set green time to 10 bits to get 2 byte representation for a road
                green_time_bits = min(road.green_time, 1023)
                pedestrian_bit = int(road.pedestrian) & 0b1

                encoded = (orientation_mapping[orientation] << 14) | \
                          (directions << 11) | \
                          (green_time_bits << 1) | \
                          pedestrian_bit

            # Store as 2 bytes (big-endian)
            crossroad_encoded.extend(struct.pack('>H', encoded))

        # Sum 1 Start byte and 8 bytes of data and get the last 8 bits
        checksum = sum(crossroad_encoded) & 0xFF
        crossroad_encoded.append(checksum)

        return crossroad_encoded


    def verify_config(self):
        validation_result, message = validate_crossroad(self.roads)
        if validation_result:
            messagebox.showinfo("Verification Success", "The crossroad configuration is valid!")
        else:
            messagebox.showerror("Verification Failed", message)


    def send_config_to_usart(self, port="/dev/ttyUSB0", baudrate=115200):
        # Reevaluate the crossroad configuration when sending via USART
        validation_result, message = validate_crossroad(self.roads)
        if not validation_result:
            messagebox.showerror("Invalid Crossroad", message)
            return

        encoded_data = self.encode_roads()

        try:
            with serial.Serial(port, baudrate, timeout=1) as ser:
                ser.write(encoded_data)
                print("Configuration sent successfully.")

                response = ser.read(1)
                if response:
                    print(f"Microcontroller Response: {response.hex()}")
        except serial.SerialException as e:
            print(f"Error: {e}")
            messagebox.showerror("Serial Error", "Failed to send data to microcontroller.")


    def save_config(self):
        file_path = filedialog.asksaveasfilename(defaultextension=".txt", filetypes=[("Text files", "*.txt")])
        if file_path:
            with open(file_path, "w") as file:
                file.write(' '.join(f'0x{b:02X}' for b in self.encode_roads()))
            messagebox.showinfo("Saved", "Configuration saved successfully!")

if __name__ == "__main__":
    root = tk.Tk()
    app = TrafficLightConfigurator(root)
    root.mainloop()
