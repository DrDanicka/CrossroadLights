

#ifndef CROSSROAD_H_
#define CROSSROAD_H_

#include <avr/io.h>
#include "CrossRoadConfig.h"
#include "twi.h"
#include "Road.h"
#include "MyTimer.h"

// States of crossroad with 3 roads
enum ThreeRoadsCrossRoadStates{
	MAIN_STRAIGHT,
	SIDE_LEFT,
	SIDE_RIGHT
};

// States of crossroad with 3 roads
enum FourRoadsCrossRoadStates{
	ONE_STRAIGHT,
	TWO_STRAIGHT,
	ONE_LEFT,
	TWO_LEFT	
};

// States of traffic lights
enum TrafficLightsStates{
	RED,
	RED_ORANGE,
	GREEN,
	ORANGE,
};

class CrossRoadLogic{
public:
	// Default constructor
	CrossRoadLogic();

	// Constructor with initialization
	CrossRoadLogic( CrossRoadConfig &config);
	
	void startTrafficLights();
	void blinkOrange();
private:
	void controlThreeCrossRoads();
	// Function for three roads that controls two roads and one pedestrian light with directions specified
	void controlThreeCrossRoadsLights(
		uint8_t road1_index,
		uint8_t road2_index,
		uint8_t pedestrians_index,
		Directions road1_direction, 
		Directions road2_directions
	);
	
	void controlFourCrossRoads();
	// Function for four roads that controls straight direction and 2 pedestrian lights
	void controlFourCrossRoadsLights1(
		uint8_t road1_index, 
		uint8_t road2_index, 
		uint8_t pedestrians_index1, 
		uint8_t pedestrians_index2
	);
	// Function for four roads that controls left and right turns
	void controlFourCrossRoadsLights2(
		uint8_t road1_left_index, 
		uint8_t road2_left_index, 
		uint8_t road1_right_index,
		uint8_t road2_right_index
	);
	
	
	CrossRoadConfig config;
	uint8_t numberOfRoads;
	Road allRoadsArr[4];	// Array of all roads
	Road existingRoads[4];	// Array of only existing roads
	Road orderedRoads[4];	// Array of existing roads ordered from 0
	uint32_t intervals[4];	// Array of time intervals for ordered roads
	uint32_t currentInterval;
	
	ThreeRoadsCrossRoadStates currentStateThree;
	ThreeRoadsCrossRoadStates nextStateThree;
	FourRoadsCrossRoadStates currentStateFour;
	FourRoadsCrossRoadStates nextStateFour;
	TrafficLightsStates lightsState;
	
	// Time variables 
	uint32_t previousTime;
	uint32_t currentTime;
	uint16_t orangeInterval = 1000;
	uint32_t previousBlinkTime = 0;
	
	// Bool variables
	bool firstTime = false; 
	bool orangeLightsOn = false;
	
};



#endif /* CROSSROAD_H_ */