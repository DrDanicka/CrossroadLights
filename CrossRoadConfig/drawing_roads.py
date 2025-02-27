def draw_road_north(canvas, x, y):
    width, height = 100, 250
    line_x = x + (width * 3 // 5)

    canvas.create_rectangle(x, y, x + width, y + height, fill="gray", outline="gray")

    # Draw black line at 3/5 of the width
    canvas.create_line(line_x, y, line_x, y + height - 102, fill="black", width=2)

    # Draw white stripes
    for j in [20, 40, 80]:
        for i in range(y, y + height - 110, 15):
            canvas.create_line(x + j, i, x + j, i + 10, fill="white", width=3)

    # Draw black line in front of the lights
    canvas.create_line(x, y + height - 102, x + width, y + height - 102, fill="black", width=2)


def draw_road_west(canvas, x, y):
    width, height = 250, 100
    line_y = y + (height * 3 // 5)

    canvas.create_rectangle(x, y, x + width, y + height, fill="gray", outline="gray")

    # Draw black line at 3/5 of the height
    canvas.create_line(x + 102, line_y, x + width, line_y, fill="black", width=2)

    # Draw white stripes
    for j in [20, 40, 80]:
        for i in range(x + 110, x + width - 10, 15):
            canvas.create_line(i, y + j, i + 10, y + j, fill="white", width=3)

    # Draw black line in front of the lights
    canvas.create_line(x + 102, y, x + 102, y + height, fill="black", width=2)


def draw_road_south(canvas, x, y):
    width, height = 100, 250
    line_x = x + (width * 2 // 5)

    canvas.create_rectangle(x, y, x + width, y + height, fill="gray", outline="gray")

    # Draw black line at 2/5 of the width
    canvas.create_line(line_x, y + 102, line_x, y + height, fill="black", width=2)

    # Draw white stripes
    for j in [20, 60, 80]:
        for i in range(y + 110, y + height - 10, 15):
            canvas.create_line(x + j, i, x + j, i + 10, fill="white", width=3)

    # Draw black line in front of the lights
    canvas.create_line(x, y + 102, x + width, y + 102, fill="black", width=2)


def draw_road_east(canvas, x, y):
    width, height = 250, 100
    line_y = y + (height * 2 // 5)

    canvas.create_rectangle(x, y, x + width, y + height, fill="gray", outline="gray")

    # Draw black line at 2/5 of the height
    canvas.create_line(x, line_y, x + width - 102, line_y, fill="black", width=2)

    # Draw white stripes
    for j in [20, 60, 80]:
        for i in range(x, x + width - 110, 15):
            canvas.create_line(i, y + j, i + 10, y + j, fill="white", width=3)

    # Draw black line in front of the lights
    canvas.create_line(x + width - 102, y, x + width - 102, y + height, fill="black", width=2)
