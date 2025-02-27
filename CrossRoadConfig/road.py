class Road:
    def __init__(self, canvas, directions, green_time, pedestrian):
        self.canvas = canvas
        self.directions = directions
        self.green_time = green_time
        self.pedestrian = pedestrian
        self.zebra_height = 20


    def draw(self, x, y):
        pass

    def draw_straight_arrow(self, coords):
        pass

    def draw_left_arrow(self, coords):
        pass

    def draw_right_arrow(self, coords):
        pass

    def delete(self, background):
        pass

    def draw_zebra(self):
        pass


class RoadNorth(Road):
    def __init__(self, canvas, directions, green_time, pedestrian):
        super().__init__(canvas, directions, green_time, pedestrian)

    def draw(self, x, y):
        width, height = 100, 250
        line_x = x + (width * 3 // 5)

        self.canvas.create_rectangle(x, y, x + width, y + height, fill="gray", outline="gray")

        # Draw black line at 3/5 of the width
        self.canvas.create_line(line_x, y, line_x, y + height - 104 - self.zebra_height, fill="black", width=2)

        # Draw white stripes
        for j in [20, 40, 80]:
            for i in range(y, y + height - 110 - self.zebra_height, 15):
                self.canvas.create_line(x + j, i, x + j, i + 10, fill="white", width=3)

        # Draw black line in front of the lights
        self.canvas.create_line(x, y + height - 104 - self.zebra_height, x + width, y + height - 104 - self.zebra_height, fill="black", width=2)

        right_lane_coords = (200, 160-self.zebra_height)
        middle_lane_coords = (220, 160-self.zebra_height)
        left_lane_coords = (240, 160-self.zebra_height)

        if self.directions['Right']:
            self.draw_right_arrow(right_lane_coords)
        else:
            self.draw_straight_arrow(right_lane_coords)

        if self.directions['Left']:
            self.draw_left_arrow(left_lane_coords)
        else:
            self.draw_straight_arrow(left_lane_coords)

        self.draw_straight_arrow(middle_lane_coords)

        if self.pedestrian:
            self.draw_zebra()


    def draw_zebra(self):
        x = 205
        y = 178

        for i in range(0, 10):
            self.canvas.create_line(x + i * 10, y, x + i * 10, y + self.zebra_height, fill="white", width=4)

    def draw_straight_arrow(self, coords):
        x, y = coords

        length_of_arrow = 30
        middle_of_line = 10
        offset = 5
        width = 2

        self.canvas.create_line(x + middle_of_line, y, x + middle_of_line, y + length_of_arrow, fill="white", width=width)
        self.canvas.create_line(x + offset, y + length_of_arrow - offset, x + middle_of_line, y + length_of_arrow,
                           fill="white", width=width)
        self.canvas.create_line(x + middle_of_line + offset, y + length_of_arrow - offset, x + middle_of_line,
                           y + length_of_arrow, fill="white", width=width)


    def draw_left_arrow(self, coords):
        x, y = coords

        length_of_arrow = 25
        middle_of_line = 10
        offset = 5
        width = 2

        # Main line
        self.canvas.create_line(x + middle_of_line, y, x + middle_of_line, y + length_of_arrow, fill="white",
                                width=width)
        # Left line
        self.canvas.create_line(x + middle_of_line, y + length_of_arrow, x + middle_of_line + offset,
                                y + length_of_arrow + offset, fill="white", width=width)

        # Arrow head
        self.canvas.create_line(x + middle_of_line, y + length_of_arrow + offset, x + middle_of_line + offset,
                                y + length_of_arrow + offset, fill="white", width=width)
        self.canvas.create_line(x + middle_of_line + offset, y + length_of_arrow, x + middle_of_line + offset,
                                y + length_of_arrow + offset, fill="white",
                                width=width)


    def draw_right_arrow(self, coords):
        x, y = coords

        length_of_arrow = 25
        middle_of_line = 10
        offset = 5
        width = 2

        # Main line
        self.canvas.create_line(x + middle_of_line, y, x + middle_of_line, y + length_of_arrow, fill="white",
                                width=width)
        # Right line
        self.canvas.create_line(x + middle_of_line, y + length_of_arrow, x + middle_of_line - offset,
                                y + length_of_arrow + offset, fill="white", width=width)

        # Arrow head
        self.canvas.create_line(x + middle_of_line, y + length_of_arrow + offset, x + middle_of_line - offset,
                                y + length_of_arrow + offset, fill="white", width=width)
        self.canvas.create_line(x + middle_of_line - offset, y + length_of_arrow, x + middle_of_line - offset,
                                y + length_of_arrow + offset, fill="white",
                                width=width)

    def delete(self, background):
        self.canvas.create_rectangle(200, 50, 300, 200, fill=background, outline=background)


class RoadSouth(Road):
    def __init__(self, canvas, directions, green_time, pedestrian):
        super().__init__(canvas, directions, green_time, pedestrian)

    def draw(self, x, y):
        width, height = 100, 250
        line_x = x + (width * 2 // 5)

        self.canvas.create_rectangle(x, y, x + width, y + height, fill="gray", outline="gray")

        # Draw black line at 2/5 of the width
        self.canvas.create_line(line_x, y + 102 + self.zebra_height, line_x, y + height, fill="black", width=2)

        # Draw white stripes
        for j in [20, 60, 80]:
            for i in range(y + 110 + self.zebra_height, y + height - 10, 15):
                self.canvas.create_line(x + j, i, x + j, i + 10, fill="white", width=3)

        # Draw black line in front of the lights
        self.canvas.create_line(x, y + 102 + self.zebra_height, x + width, y + 102 + self.zebra_height, fill="black", width=2)

        right_lane_coords = (280, 310 + self.zebra_height)
        middle_lane_coords = (260, 310 + self.zebra_height)
        left_lane_coords = (240, 310 + self.zebra_height)

        if self.directions['Right']:
            self.draw_right_arrow(right_lane_coords)
        else:
            self.draw_straight_arrow(right_lane_coords)

        if self.directions['Left']:
            self.draw_left_arrow(left_lane_coords)
        else:
            self.draw_straight_arrow(left_lane_coords)

        self.draw_straight_arrow(middle_lane_coords)

        if self.pedestrian:
            self.draw_zebra()

    def draw_straight_arrow(self, coords):
        x, y = coords

        length_of_arrow = 30
        middle_of_line = 10
        offset = 5
        width = 2

        self.canvas.create_line(x + middle_of_line, y, x + middle_of_line, y + length_of_arrow, fill="white", width=width)
        self.canvas.create_line(x + offset, y + offset, x + middle_of_line, y, fill="white", width=width)
        self.canvas.create_line(x + middle_of_line + offset, y + offset, x + middle_of_line, y, fill="white", width=width)

    def draw_left_arrow(self, coords):
        x, y = coords

        length_of_arrow = 25
        middle_of_line = 10
        offset = 5
        width = 2

        # Main line
        self.canvas.create_line(x + middle_of_line, y + offset, x + middle_of_line, y + length_of_arrow + offset, fill="white", width=width)

        # Left line
        self.canvas.create_line(x + middle_of_line, y + offset, x + middle_of_line - offset, y, fill="white", width=width)

        # Arrow head
        self.canvas.create_line(x + middle_of_line - offset, y, x + middle_of_line - offset, y + offset, fill="white", width=width)
        self.canvas.create_line(x + middle_of_line, y, x + middle_of_line - offset, y, fill="white", width=width)

    def draw_right_arrow(self, coords):
        x, y = coords

        length_of_arrow = 25
        middle_of_line = 10
        offset = 5
        width = 2

        # Main line
        self.canvas.create_line(x + middle_of_line, y + offset, x + middle_of_line, y + length_of_arrow + offset,
                                fill="white", width=width)

        # Right line
        self.canvas.create_line(x + middle_of_line, y + offset, x + middle_of_line + offset, y, fill="white",
                                width=width)

        # Arrow head
        self.canvas.create_line(x + middle_of_line + offset, y, x + middle_of_line + offset, y + offset, fill="white",
                                width=width)
        self.canvas.create_line(x + middle_of_line, y, x + middle_of_line + offset, y, fill="white", width=width)

    def draw_zebra(self):
        x = 205
        y = 300  # Adjusted for the southern direction

        for i in range(0, 10):
            self.canvas.create_line(x + i * 10, y, x + i * 10, y + self.zebra_height, fill="white", width=4)

    def delete(self, background):
        self.canvas.create_rectangle(200, 300, 300, 450, fill=background, outline=background)


class RoadEast(Road):
    def __init__(self, canvas, directions, green_time, pedestrian):
        super().__init__(canvas, directions, green_time, pedestrian)

    def draw(self, x, y):
        width, height = 250, 100
        line_y = y + (height * 2 // 5)

        self.canvas.create_rectangle(x, y, x + width, y + height, fill="gray", outline="gray")

        # Draw black line at 2/5 of the height
        self.canvas.create_line(x, line_y, x + width - 102 - self.zebra_height, line_y, fill="black", width=2)

        # Draw white stripes
        for j in [20, 60, 80]:
            for i in range(x, x + width - 110 - self.zebra_height, 15):
                self.canvas.create_line(i, y + j, i + 10, y + j, fill="white", width=3)

        # Draw black line in front of the lights
        self.canvas.create_line(x + width - 104 - self.zebra_height, y, x + width - 104 - self.zebra_height, y + height, fill="black", width=2)

        right_lane_coords = (160 - self.zebra_height, 280)
        middle_lane_coords = (160 - self.zebra_height, 260)
        left_lane_coords = (160 - self.zebra_height, 240)

        if self.directions['Right']:
            self.draw_right_arrow(right_lane_coords)
        else:
            self.draw_straight_arrow(right_lane_coords)

        if self.directions['Left']:
            self.draw_left_arrow(left_lane_coords)
        else:
            self.draw_straight_arrow(left_lane_coords)

        self.draw_straight_arrow(middle_lane_coords)

        if self.pedestrian:
            self.draw_zebra()

    def draw_straight_arrow(self, coords):
        x, y = coords

        length_of_arrow = 30
        middle_of_line = 10
        offset = 5
        width = 2

        # Main line
        self.canvas.create_line(x, y + middle_of_line, x + length_of_arrow, y + middle_of_line, fill="white",
                                width=width)
        # Arrow head
        self.canvas.create_line(x + length_of_arrow - offset, y + offset, x + length_of_arrow, y + middle_of_line,
                                fill="white", width=width)
        self.canvas.create_line(x + length_of_arrow - offset, y + middle_of_line + offset, x + length_of_arrow,
                                y + middle_of_line, fill="white", width=width)

    def draw_left_arrow(self, coords):
        x, y = coords

        length_of_arrow = 25
        middle_of_line = 10
        offset = 5
        width = 2

        # Main line
        self.canvas.create_line(x, y + middle_of_line, x + length_of_arrow, y + middle_of_line, fill="white",
                                width=width)

        # Left line
        self.canvas.create_line(x + length_of_arrow, y + middle_of_line, x + length_of_arrow + offset, y + middle_of_line - offset, fill="white", width=width)

        # Arrow head
        self.canvas.create_line(x + length_of_arrow, y + middle_of_line - offset, x + length_of_arrow + offset, y + middle_of_line - offset, fill="white", width=width)
        self.canvas.create_line(x + length_of_arrow + offset, y + middle_of_line, x + length_of_arrow + offset, y + middle_of_line - offset, fill="white", width=width)


    def draw_right_arrow(self, coords):
        x, y = coords

        length_of_arrow = 25
        middle_of_line = 10
        offset = 5
        width = 2

        # Main line
        self.canvas.create_line(x, y + middle_of_line, x + length_of_arrow, y + middle_of_line, fill="white",
                                width=width)

        # Right line
        self.canvas.create_line(x + length_of_arrow, y + middle_of_line, x + length_of_arrow + offset,
                                y + middle_of_line + offset, fill="white", width=width)

        # Arrow head
        self.canvas.create_line(x + length_of_arrow, y + middle_of_line + offset, x + length_of_arrow + offset,
                                y + middle_of_line + offset, fill="white", width=width)
        self.canvas.create_line(x + length_of_arrow + offset, y + middle_of_line, x + length_of_arrow + offset,
                                y + middle_of_line + offset, fill="white", width=width)

    def draw_zebra(self):
        x = 178  # Adjusted for East direction
        y = 205

        for i in range(0, 10):
            self.canvas.create_line(x, y + i * 10, x + self.zebra_height, y + i * 10, fill="white", width=4)

    def delete(self, background):
        self.canvas.create_rectangle(50, 200, 200, 300, fill=background, outline=background)

class RoadWest(Road):
    def __init__(self, canvas, directions, green_time, pedestrian):
        super().__init__(canvas, directions, green_time, pedestrian)

    def draw(self, x, y):
        width, height = 250, 100
        line_y = y + (height * 3 // 5)

        self.canvas.create_rectangle(x, y, x + width, y + height, fill="gray", outline="gray")

        # Draw black line at 3/5 of the height
        self.canvas.create_line(x + 102 + self.zebra_height, line_y, x + width, line_y, fill="black", width=2)

        # Draw white stripes
        for j in [20, 40, 80]:
            for i in range(x + 110 + self.zebra_height, x + width - 10, 15):
                self.canvas.create_line(i, y + j, i + 10, y + j, fill="white", width=3)

        # Draw black line in front of the lights
        self.canvas.create_line(x + 102 + self.zebra_height, y, x + 102 + self.zebra_height, y + height, fill="black", width=2)

        right_lane_coords = (310 + self.zebra_height, 200)
        middle_lane_coords = (310 + self.zebra_height, 220)
        left_lane_coords = (310 + self.zebra_height, 240)

        if self.directions['Right']:
            self.draw_right_arrow(right_lane_coords)
        else:
            self.draw_straight_arrow(right_lane_coords)

        if self.directions['Left']:
            self.draw_left_arrow(left_lane_coords)
        else:
            self.draw_straight_arrow(left_lane_coords)

        self.draw_straight_arrow(middle_lane_coords)

        if self.pedestrian:
            self.draw_zebra()


    def draw_straight_arrow(self, coords):
        x, y = coords

        length_of_arrow = 30
        middle_of_line = 10
        offset = 5
        width = 2

        self.canvas.create_line(x, y + middle_of_line, x + length_of_arrow, y + middle_of_line, fill="white",
                                width=width)
        self.canvas.create_line(x + offset, y + offset, x, y + middle_of_line,
                                fill="white", width=width)
        self.canvas.create_line(x + offset, y + middle_of_line + offset, x,
                                y + middle_of_line, fill="white", width=width)

    def draw_left_arrow(self, coords):
        x, y = coords

        length_of_arrow = 25
        middle_of_line = 10
        offset = 5
        width = 2

        # Main line
        self.canvas.create_line(x + offset, y + middle_of_line, x + length_of_arrow + offset, y + middle_of_line, fill="white",
                                width=width)

        # Left line
        self.canvas.create_line(x + offset, y + middle_of_line, x, y + middle_of_line + offset, fill="white",
                                width=width)

        # Arrow head
        self.canvas.create_line(x, y + middle_of_line + offset, x, y + middle_of_line, fill="white",
                                width=width)
        self.canvas.create_line(x, y + middle_of_line + offset, x + offset, y + middle_of_line + offset, fill="white", width=width)

    def draw_right_arrow(self, coords):
        x, y = coords

        length_of_arrow = 25
        middle_of_line = 10
        offset = 5
        width = 2

        # Main line
        self.canvas.create_line(x + offset, y + middle_of_line, x + length_of_arrow + offset, y + middle_of_line,
                                fill="white",
                                width=width)

        # Right line
        self.canvas.create_line(x + offset, y + middle_of_line, x, y + middle_of_line - offset, fill="white",
                                width=width)

        self.canvas.create_line(x, y + middle_of_line - offset, x, y + middle_of_line, fill="white",
                                width=width)
        self.canvas.create_line(x, y + middle_of_line - offset, x + offset, y + middle_of_line - offset, fill="white",
                                width=width)

    def draw_zebra(self):
        x = 300  # Adjusted for West direction
        y = 205

        for i in range(0, 10):
            self.canvas.create_line(x, y + i * 10, x + self.zebra_height, y + i * 10, fill="white", width=4)

    def delete(self, background):
        self.canvas.create_rectangle(300, 200, 450, 300, fill=background, outline=background)