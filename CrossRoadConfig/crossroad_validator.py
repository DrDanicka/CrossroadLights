from orientation import Orientation

def validate_crossroad(roads):
    road_orientations = list(roads.keys())
    straight_mapping = {
        Orientation.NORTH: Orientation.SOUTH,
        Orientation.EAST: Orientation.WEST,
        Orientation.SOUTH: Orientation.NORTH,
        Orientation.WEST: Orientation.EAST
    }

    left_mapping = {
        Orientation.NORTH: Orientation.EAST,
        Orientation.EAST: Orientation.SOUTH,
        Orientation.SOUTH: Orientation.WEST,
        Orientation.WEST: Orientation.NORTH
    }

    right_mapping = {
        Orientation.NORTH: Orientation.WEST,
        Orientation.EAST: Orientation.NORTH,
        Orientation.SOUTH: Orientation.EAST,
        Orientation.WEST: Orientation.SOUTH
    }

    for orientation, road in roads.items():
        if road.directions["Straight"]:
            if straight_mapping[orientation] not in road_orientations:
                return (False, f"Road from {orientation.name[0]} does not have a road going straight.")
        if road.directions["Left"]:
            if left_mapping[orientation] not in road_orientations:
                return (False, f"Road from {orientation.name[0]} does not have a road going left.")
        if road.directions["Right"]:
            if right_mapping[orientation] not in road_orientations:
                return (False, f"Road from {orientation.name[0]} does not have a road going right.")

    return (True, '')