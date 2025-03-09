import time
import threading
import tkinter as tk
from tkinter import messagebox, filedialog
import struct
import serial
from serial.tools import list_ports

from orientation import Orientation
from road import RoadNorth, RoadWest, RoadSouth, RoadEast
from crossroad_validator import validate_crossroad


class TrafficLightConfigurator:
    def __init__(self, root):
        self.root = root
        self.root.title("Traffic Light Configurator")

        # Create a canvas to draw the crossroad
        self.background_color = '#29971c' # Green
        self.canvas = tk.Canvas(root, width=500, height=500, bg=self.background_color)
        self.canvas.pack()

        # Serial communication variables
        self.serial_connection = None
        self.serial_thread = None
        self.reading_serial = False

        # Letters for orientation
        self.canvas.create_text(250, 20, text="N", font=("Arial", 24, "bold"))
        self.canvas.create_text(480, 250, text="E", font=("Arial", 24, "bold"))
        self.canvas.create_text(250, 480, text="S", font=("Arial", 24, "bold"))
        self.canvas.create_text(20, 250, text="W", font=("Arial", 24, "bold"))

        # Dictionary to store the roads and buttons
        self.roads = {}
        self.buttons = {}

        # First row of buttons to add and delete roads
        self.btn_frame_adds_delete = tk.Frame(root)
        self.btn_frame_adds_delete.pack()

        # Second row of buttons to verify and save the configuration
        self.btn_frame_save_verify = tk.Frame(root)
        self.btn_frame_save_verify.pack()

        # Third row for USART communication
        self.usart_frame = tk.Frame(root)
        self.usart_frame.pack()

        # Add buttons for each orientation
        for orientation in Orientation:
            self.buttons[orientation] = tk.Button(self.btn_frame_adds_delete, text=f"Add Road from {orientation.name[0]}", command=lambda o=orientation: self.add_road(o))
            self.buttons[orientation].pack(side="left", padx=5, pady=5)

        # Add verify button
        self.btn_verify = tk.Button(self.btn_frame_save_verify, text="Verify Config", command=self.verify_config)
        self.btn_verify.pack(side="left", padx=5)

        # Add save button
        self.btn_save = tk.Button(self.btn_frame_save_verify, text="Save Config", command=self.save_config)
        self.btn_save.pack(side="left", padx=5)

        # Text box for baud rate
        tk.Label(self.usart_frame, text="Baud Rate:").pack(side='left', padx=5)
        self.baud_rate_entry = tk.Entry(self.usart_frame)
        self.baud_rate_entry.insert(0, "115200")  # Default baud rate
        self.baud_rate_entry.pack(side='left', padx=5)

        # Serial port dropdown menu
        self.serial_ports = tk.StringVar()
        self.serial_ports_dropdown = None
        self.update_serial_ports()
        self.root.after(2000, self.auto_refresh_serial_ports)  # Auto refresh serial ports every 2 seconds

        # Button to send configuration via USART
        self.btn_send = tk.Button(root, text="Send via USART", command=self.send_config_to_usart)
        self.btn_send.pack(pady=5)

        # Start reading USART
        self.start_serial_reading()


    def add_road(self, orientation: Orientation):
        road_window = tk.Toplevel(self.root)
        road_window.title("Add Road")

        # Allowed directions for the road
        tk.Label(road_window, text="Allowed Directions").pack()
        directions = {"Straight": tk.BooleanVar(), "Left": tk.BooleanVar(), "Right": tk.BooleanVar()}

        # Add checkboxes for each direction
        for direction, var in directions.items():
            tk.Checkbutton(road_window, text=direction, variable=var).pack()

        # Green light duration
        tk.Label(road_window, text="Green Light Duration (s)").pack()
        green_time = tk.Entry(road_window)
        green_time.pack()

        # Add checkbox for pedestrian light
        pedestrian_var = tk.BooleanVar()
        tk.Checkbutton(road_window, text="Has Pedestrian Light", variable=pedestrian_var).pack()

        def save_road():
            road_classes = {
                Orientation.NORTH: RoadNorth,
                Orientation.EAST: RoadEast,
                Orientation.SOUTH: RoadSouth,
                Orientation.WEST: RoadWest}
            # Create object of a road, based of the orientation
            road = road_classes[orientation](self.canvas, {k: v.get() for k, v in directions.items()}, int(green_time.get()), pedestrian_var.get())
            # Save the object in the dictionary
            self.roads[orientation] = road

            # Close the window of the road
            road_window.destroy()
            # Draw the road on the canvas
            self.draw_road(road, orientation)

            # Remove the add button and add the delete button
            self.buttons[orientation].pack_forget()
            self.add_delete_button(orientation)

        tk.Button(road_window, text="Save", command=save_road).pack()

    def draw_road(self, road, orientation: Orientation):
        # Positions of the roads based on the orientation
        positions = {
            Orientation.NORTH: (200, 50),
            Orientation.EAST: (200, 200),
            Orientation.SOUTH: (200, 200),
            Orientation.WEST: (50, 200)
        }
        # Draw road on the canvas with tuple parameter unpacking
        road.draw(*positions[orientation])

    def add_delete_button(self, orientation: Orientation):
        # Add a delete button for the road
        delete_btn = tk.Button(self.btn_frame_adds_delete, text=f"Delete Road from {orientation.name[0]}", command=lambda: self.delete_road(orientation))
        delete_btn.pack(side="left", padx=5)
        self.buttons[orientation] = delete_btn

    def delete_road(self, orientation: Orientation):
        if orientation in self.roads:
            # Delete the road from the canvas
            self.roads[orientation].delete(self.background_color)
            # Delete the road from the dictionary
            del self.roads[orientation]

            # Remove the delete button and add the add button
            self.buttons[orientation].destroy()
            self.buttons[orientation] = tk.Button(self.btn_frame_adds_delete, text=f"Add Road from {orientation.name[0]}", command=lambda o=orientation: self.add_road(o))
            self.buttons[orientation].pack(side="left", padx=5)

    def encode_roads(self):
        '''
        Encoding format:
        O = Orientation
        S = Straight
        L = Left
        R = Right
        Gt = Green time
        P = Pedestrians

        byte1:
        | O | O | S | L | R | Gt | Gt | Gt |

        byte2:
        | Gt | Gt | Gt | Gt | Gt | Gt | Gt | P |
        '''
        crossroad_encoded = bytearray()
        crossroad_encoded.append(0xD9)  # Start byte

        orientation_mapping = {
            Orientation.NORTH: 0b00,
            Orientation.EAST: 0b01,
            Orientation.SOUTH: 0b10,
            Orientation.WEST: 0b11
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
        if len(self.roads) < 3:
            messagebox.showerror("Invalid Crossroad", "A crossroad must have at least 3 roads.")
            return

        # Validates if all roads have a road going in selected directions for certain road
        validation_result, message = validate_crossroad(self.roads)
        if validation_result:
            messagebox.showinfo("Verification Success", "The crossroad configuration is valid!")
        else:
            messagebox.showerror("Verification Failed", message)


    # Serial communication methods
    def update_serial_ports(self):
        # Get available serial ports
        available_ports = self.get_serial_ports()

        if self.serial_ports_dropdown:
            self.serial_ports_dropdown.destroy()  # Remove old dropdown

        if available_ports:
            self.serial_ports.set(available_ports[0])  # Select the first port
            self.serial_ports_dropdown = tk.OptionMenu(self.usart_frame, self.serial_ports, *available_ports)
            self.serial_ports_dropdown.config(state="normal")  # Enable dropdown
        else:
            self.serial_ports.set("No Ports Found")
            self.serial_ports_dropdown = tk.OptionMenu(self.usart_frame, self.serial_ports, "No Ports Found")
            self.serial_ports_dropdown.config(state="disabled")  # Disable dropdown

        self.serial_ports_dropdown.pack(side='left', padx=5)

    def auto_refresh_serial_ports(self):
        # Refresh the serial ports every 2 seconds
        self.update_serial_ports()
        self.root.after(2000, self.auto_refresh_serial_ports)


    def get_serial_ports(self):
        # Get available serial ports
        return [port.device for port in list_ports.comports()]


    def send_config_to_usart(self):
        # Reevaluate the crossroad configuration when sending via USART
        if len(self.roads) < 3:
            messagebox.showerror("Invalid Crossroad", "A crossroad must have at least 3 roads.")
            return

        validation_result, message = validate_crossroad(self.roads)
        if not validation_result:
            messagebox.showerror("Invalid Crossroad", message)
            return

        # Get the selected serial port
        selected_port = self.serial_ports.get()
        if not selected_port or selected_port == "No Ports Found":
            messagebox.showerror("Serial Error", "No serial port selected!")
            return

        # Get baud rate
        try:
            baudrate = int(self.baud_rate_entry.get())
        except ValueError:
            messagebox.showerror("Invalid Baud Rate", "Please enter a valid number for baud rate.")
            return

        encoded_data = self.encode_roads()

        try:
            # Stop reading USART first
            self.serial_connection.write(encoded_data)
            messagebox.showinfo("Success!", "The crossroad configuration has been sent to the microcontroller.")

        except serial.SerialException as e:
            print(f"Error: {e}")
            messagebox.showerror("Serial Error", "Failed to send data to microcontroller.")

        # Start reading USART again
        self.start_serial_reading()

    def start_serial_reading(self):
        # Get selected port from GUI
        selected_port = self.serial_ports.get()

        if not selected_port or selected_port == "No Ports Found":
            self.root.after(3000, self.start_serial_reading)
            return

        try:
            baudrate = int(self.baud_rate_entry.get())
        except ValueError:
            self.root.after(3000, self.start_serial_reading)
            return

        if self.serial_connection is None:
            try:
                self.serial_connection = serial.Serial(selected_port, baudrate, timeout=1)
                self.reading_serial = True
                self.serial_thread = threading.Thread(target=self.read_serial_data, daemon=True)
                self.serial_thread.start()
            except serial.SerialException as e:
                print(f"Error: {e}")
                self.root.after(3000, self.start_serial_reading)

    def read_serial_data(self):
        while self.reading_serial and self.serial_connection:
            try:
                if self.serial_connection.in_waiting:
                    data = self.serial_connection.readline().strip()
                    if data:
                        print(f"Microcontroller: {data.decode('utf-8', errors='ignore')}")
            except serial.SerialException:
                print("Serial connection lost.")
                self.reading_serial = False
                self.serial_connection = None
                self.root.after(3000, self.start_serial_reading)
                break

    def save_config(self):
        # Save the configuration to a file in hex format
        file_path = filedialog.asksaveasfilename(defaultextension=".txt", filetypes=[("Text files", "*.txt")])
        if file_path:
            with open(file_path, "w") as file:
                file.write(' '.join(f'0x{b:02X}' for b in self.encode_roads()))

if __name__ == "__main__":
    root = tk.Tk()
    app = TrafficLightConfigurator(root)
    root.mainloop()
