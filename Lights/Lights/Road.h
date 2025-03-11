#ifndef ROAD_H_
#define ROAD_H_

#include <avr/io.h>
#include "twi.h"
#include "MyTimer.h"

#define DEBOUNCE_TIME 50

enum Directions{
	STRAIGHT,
	LEFT,
	RIGHT	
};

class Road{
public:
	bool straight;			// Has road going straight
	bool left;				// Has road going left
	bool right;				// Has road going right
	uint16_t green_light;	// Time in seconds
	bool pedestrians;		// Has zebra for pedestrians
	bool exists;			// Flag that based of the directions finds out if road exists
	
	Road();
	
	Road(uint8_t byte1, uint8_t byte2);
	
	// Light functions
	void green(Directions direction);
	void red(Directions direction);
	void orange(Directions direction);
	void redOrange(Directions direction);
	void greenPedestrians();
	void redPedestrians();
	void allRed();
	void turnOff(Directions direction);
	void turnOffPedestrians();
	void allOrange();
	
	bool buttonClicked();	// Check if pedestrian light was clicked
private:
	uint8_t orientation;
	uint8_t expanderAddress = 0x00;
	bool lastButtonState = true;
	bool buttonPressed = true;
	uint32_t lastDebounceTime = 0;
	
	void turnOnOneLight(Directions direction, uint8_t light_index, bool state);
};


#endif