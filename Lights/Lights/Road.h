#ifndef ROAD_H_
#define ROAD_H_

#include <stdint.h>

class Road{
public:
	bool straight;			// Has road going straight
	bool left;				// Has road going left
	bool right;				// Has road going right
	uint16_t green_light;	// Time in seconds
	bool pedestrians;		// Has zebra for pedestrians
	bool exists;			// Flag that based of the directions finds out if road exists
	
	Road(uint8_t byte1, uint8_t byte2);
};


#endif