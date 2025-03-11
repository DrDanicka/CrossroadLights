#include "CrossRoadLogic.h"


// Default constructor
CrossRoadLogic::CrossRoadLogic() : numberOfRoads(0), currentStateThree(ThreeRoadsCrossRoadStates::MAIN_STRAIGHT), nextStateThree(ThreeRoadsCrossRoadStates::MAIN_STRAIGHT),
currentInterval(0), previousTime(0), currentTime(0), lightsState(TrafficLightsStates::RED) {}

// Constructor with initialization
CrossRoadLogic::CrossRoadLogic(CrossRoadConfig &config){
	// Save config
	this->config = config;
	
	// Initialize timer
	timer0Init();
	
	// Get how many roads exist
	allRoadsArr[0] = config.north;
	allRoadsArr[1] = config.east;
	allRoadsArr[2] = config.south;
	allRoadsArr[3] = config.west;;
	
	uint8_t index = 0;
	uint8_t nonexistentRoad = 5;
	
	for (uint8_t i = 0; i < 4; i++){
		if (allRoadsArr[i].exists){
			existingRoads[index] = allRoadsArr[i];
			index++;
		}else{
			nonexistentRoad = i;
		}
	}
	numberOfRoads = index;
	
	// Order the existing roads into array
	if (numberOfRoads == 3){
		for(uint8_t i = 0; i < numberOfRoads; i++){
			orderedRoads[i] = allRoadsArr[(nonexistentRoad + 1) % 4];
			nonexistentRoad++;
			
		}
	}else{
		for(uint8_t i = 0; i < numberOfRoads; i++){
			orderedRoads[i] = allRoadsArr[i];
		}
	}
	
	// Initialize crossroad based of the number of roads
	// Start with last states and change that change in the begining because of firstTime=True
	if (numberOfRoads == 3){
		// Set starting states
		currentStateThree = ThreeRoadsCrossRoadStates::SIDE_RIGHT;
		nextStateThree = ThreeRoadsCrossRoadStates::MAIN_STRAIGHT;
		
		// Calculate intervals for all states
		intervals[0] = ((orderedRoads[0].green_light > orderedRoads[2].green_light) ? orderedRoads[0].green_light : orderedRoads[2].green_light ) * 1000;
		intervals[1] = ((orderedRoads[1].green_light > orderedRoads[2].green_light) ? orderedRoads[1].green_light : orderedRoads[2].green_light ) * 1000;
		intervals[2] = ((orderedRoads[0].green_light > orderedRoads[1].green_light) ? orderedRoads[0].green_light : orderedRoads[1].green_light ) * 1000;
		
		currentInterval = intervals[2];
		firstTime = true;
		lightsState = TrafficLightsStates::RED;
		
		
	}else{
		// Set starting states
		currentStateFour = FourRoadsCrossRoadStates::TWO_LEFT;
		nextStateFour = FourRoadsCrossRoadStates::ONE_STRAIGHT;
		
		// Calculate intervals for all states
		intervals[0] = ((orderedRoads[0].green_light > orderedRoads[2].green_light) ? orderedRoads[0].green_light : orderedRoads[2].green_light ) * 1000;
		intervals[1] = ((orderedRoads[1].green_light > orderedRoads[3].green_light) ? orderedRoads[1].green_light : orderedRoads[3].green_light ) * 1000;
		intervals[2] = (intervals[0] > intervals[1]) ? intervals[0] : intervals[1];
		intervals[3] = (intervals[0] > intervals[1]) ? intervals[0] : intervals[1];
		
		currentInterval = intervals[3];
		firstTime = true;
		lightsState = TrafficLightsStates::RED;
	}
}

void CrossRoadLogic::startTrafficLights(){
	if (firstTime){
		// Set red lights on all existing traffic lights
		for (uint8_t i = 0; i < 4; i++){
			allRoadsArr[i].allRed();
		}
	}
	
	currentTime = millis();
	
	if (numberOfRoads == 3){
		controlThreeCrossRoads();	
	}else{
		controlFourCrossRoads();
	}
}

void CrossRoadLogic::controlThreeCrossRoads(){
	// If current time interval was reached
	if (currentTime - previousTime >= currentInterval || firstTime){
		// Change state to current state
		currentStateThree = nextStateThree;
		// Set next state
		nextStateThree = static_cast<ThreeRoadsCrossRoadStates>((static_cast<int>(nextStateThree) + 1) % numberOfRoads);
		// Set new interval
		currentInterval = intervals[currentStateThree];
		
		// Set red lights on all existing traffic lights
		for (uint8_t i = 0; i < numberOfRoads; i++){
			orderedRoads[i].allRed();
		}
		previousTime = currentTime;
		firstTime = false;
	}
	
	// Main loop -> set traffic lights based of the state
	if (currentStateThree == ThreeRoadsCrossRoadStates::MAIN_STRAIGHT){
		controlThreeCrossRoadsLights(0, 2, 1, Directions::STRAIGHT, Directions::STRAIGHT);
	}else if (currentStateThree == ThreeRoadsCrossRoadStates::SIDE_LEFT){
		controlThreeCrossRoadsLights(1, 2, 0, Directions::LEFT, Directions::RIGHT);
	}else{
		controlThreeCrossRoadsLights(1, 0, 2, Directions::RIGHT, Directions::LEFT);
	}
	
	// Check if pedestrian button was set
	if (orderedRoads[0].pedestrians && orderedRoads[0].buttonClicked()){
		nextStateThree = (currentStateThree != ThreeRoadsCrossRoadStates::SIDE_LEFT) ? ThreeRoadsCrossRoadStates::SIDE_LEFT : nextStateThree;
	}else if(orderedRoads[1].pedestrians && orderedRoads[1].buttonClicked()){
		nextStateThree = (currentStateThree != ThreeRoadsCrossRoadStates::MAIN_STRAIGHT) ? ThreeRoadsCrossRoadStates::MAIN_STRAIGHT : nextStateThree;
	}else if(orderedRoads[2].pedestrians && orderedRoads[2].buttonClicked()){
		nextStateThree = (currentStateThree != ThreeRoadsCrossRoadStates::SIDE_RIGHT) ? ThreeRoadsCrossRoadStates::SIDE_RIGHT : nextStateThree;
	}
}

void CrossRoadLogic::controlThreeCrossRoadsLights(
	uint8_t road1_index, uint8_t road2_index, uint8_t pedestrians_index,
	Directions road1_direction, Directions road2_directions){
	if (currentTime - previousTime < orangeInterval){
		// Lights for the cars
		orderedRoads[road1_index].redOrange(road1_direction);
		orderedRoads[road2_index].redOrange(road2_directions);
		
		// Lights for the pedestrians
		if (orderedRoads[pedestrians_index].pedestrians){
			// Only if pedestrians were selected for this road
			orderedRoads[pedestrians_index].greenPedestrians();	
		}
	}else if(currentTime - previousTime >= orangeInterval && (currentTime - previousTime) < (currentInterval - 2 * orangeInterval)){
		// Lights for the cars
		orderedRoads[road1_index].green(road1_direction);
		orderedRoads[road2_index].green(road2_directions);
	}else if ((currentTime - previousTime) >= (currentInterval - 2 * orangeInterval) && (currentTime - previousTime) < (currentInterval - orangeInterval)){
		// Lights for the cars
		orderedRoads[road1_index].orange(road1_direction);
		orderedRoads[road2_index].orange(road2_directions);
	}else{
		// Lights for the cars
		orderedRoads[road1_index].red(road1_direction);
		orderedRoads[road2_index].red(road2_directions);
		
		// Lights for the pedestrians
		if (orderedRoads[pedestrians_index].pedestrians){
			// Only if pedestrians were selected for this road
			orderedRoads[pedestrians_index].redPedestrians();	
		}
	}
}

void CrossRoadLogic::controlFourCrossRoads(){
	// If current time interval was reached
	if (currentTime - previousTime >= currentInterval || firstTime){
		// Change state to current state
		currentStateFour = nextStateFour;
		// Set next state
		nextStateFour = static_cast<FourRoadsCrossRoadStates>((static_cast<int>(nextStateFour) + 1) % numberOfRoads);
		// Set new interval
		currentInterval = intervals[currentStateFour];
		
		// Set red lights on all existing traffic lights
		for (uint8_t i = 0; i < numberOfRoads; i++){
			orderedRoads[i].allRed();
		}
		previousTime = currentTime;
		firstTime = false;
	}
	
	// Main loop -> set traffic lights based of the state
	if (currentStateFour == FourRoadsCrossRoadStates::ONE_STRAIGHT){
		controlFourCrossRoadsLights1(0, 2, 1, 3);
	}else if (currentStateFour == FourRoadsCrossRoadStates::TWO_STRAIGHT){
		controlFourCrossRoadsLights1(1, 3, 0, 2);
	}else if (currentStateFour == FourRoadsCrossRoadStates::ONE_LEFT){
		controlFourCrossRoadsLights2(0, 2, 1, 3);
	}else if(currentStateFour == FourRoadsCrossRoadStates::TWO_LEFT){
		controlFourCrossRoadsLights2(1, 3, 0, 2);
	}
	
	// Check if pedestrian button was set
	if ((orderedRoads[0].pedestrians && orderedRoads[0].buttonClicked()) || (orderedRoads[2].pedestrians && orderedRoads[2].buttonClicked())){
		nextStateFour = (currentStateFour != FourRoadsCrossRoadStates::TWO_STRAIGHT) ? FourRoadsCrossRoadStates::TWO_STRAIGHT : nextStateFour;
	}else if((orderedRoads[1].pedestrians && orderedRoads[1].buttonClicked()) || (orderedRoads[3].pedestrians && orderedRoads[3].buttonClicked())){
		nextStateFour = (currentStateFour != FourRoadsCrossRoadStates::ONE_STRAIGHT) ? FourRoadsCrossRoadStates::ONE_STRAIGHT : nextStateFour;
	}
}

void CrossRoadLogic::controlFourCrossRoadsLights1(uint8_t road1_index, uint8_t road2_index, uint8_t pedestrians_index1,uint8_t pedestrians_index2){
	if (currentTime - previousTime < orangeInterval){
		// Lights for the cars
		orderedRoads[road1_index].redOrange(Directions::STRAIGHT);
		orderedRoads[road2_index].redOrange(Directions::STRAIGHT);
		
		// Lights for the pedestrians
		if (orderedRoads[pedestrians_index1].pedestrians){
			orderedRoads[pedestrians_index1].greenPedestrians();
		}
		if (orderedRoads[pedestrians_index2].pedestrians){
			orderedRoads[pedestrians_index2].greenPedestrians();
		}
	}else if(currentTime - previousTime >= orangeInterval && (currentTime - previousTime) < (currentInterval - 2 * orangeInterval)){
		// Lights for the cars
		orderedRoads[road1_index].green(Directions::STRAIGHT);
		orderedRoads[road2_index].green(Directions::STRAIGHT);
	}else if ((currentTime - previousTime) >= (currentInterval - 2 * orangeInterval) && (currentTime - previousTime) < (currentInterval - orangeInterval)){
		// Lights for the cars
		orderedRoads[road1_index].orange(Directions::STRAIGHT);
		orderedRoads[road2_index].orange(Directions::STRAIGHT);
	}else{
		// Lights for the cars
		orderedRoads[road1_index].red(Directions::STRAIGHT);
		orderedRoads[road2_index].red(Directions::STRAIGHT);
		
		// Lights for the pedestrians
		if (orderedRoads[pedestrians_index1].pedestrians){
			orderedRoads[pedestrians_index1].redPedestrians();
		}
		if (orderedRoads[pedestrians_index2].pedestrians){
			orderedRoads[pedestrians_index2].redPedestrians();
		}
	}
}

void CrossRoadLogic::controlFourCrossRoadsLights2(uint8_t road1_left_index, uint8_t road2_left_index, uint8_t road1_right_index, uint8_t road2_right_index){
	if (currentTime - previousTime < orangeInterval){
		// Lights for the cars
		orderedRoads[road1_left_index].redOrange(Directions::LEFT);
		orderedRoads[road2_left_index].redOrange(Directions::LEFT);
		orderedRoads[road1_right_index].redOrange(Directions::RIGHT);
		orderedRoads[road2_right_index].redOrange(Directions::RIGHT);
		
	}else if(currentTime - previousTime >= orangeInterval && (currentTime - previousTime) < (currentInterval - 2 * orangeInterval)){
		// Lights for the cars
		orderedRoads[road1_left_index].green(Directions::LEFT);
		orderedRoads[road2_left_index].green(Directions::LEFT);
		orderedRoads[road1_right_index].green(Directions::RIGHT);
		orderedRoads[road2_right_index].green(Directions::RIGHT);
	}else if ((currentTime - previousTime) >= (currentInterval - 2 * orangeInterval) && (currentTime - previousTime) < (currentInterval - orangeInterval)){
		// Lights for the cars
		orderedRoads[road1_left_index].orange(Directions::LEFT);
		orderedRoads[road2_left_index].orange(Directions::LEFT);
		orderedRoads[road1_right_index].orange(Directions::RIGHT);
		orderedRoads[road2_right_index].orange(Directions::RIGHT);
	}else{
		// Lights for the cars
		orderedRoads[road1_left_index].red(Directions::LEFT);
		orderedRoads[road2_left_index].red(Directions::LEFT);
		orderedRoads[road1_right_index].red(Directions::RIGHT);
		orderedRoads[road2_right_index].red(Directions::RIGHT);
	}
}

// Blink all orange lights
void CrossRoadLogic::blinkOrange(){
	// One second blinking interval
	if (millis() - previousBlinkTime >= orangeInterval){
		for (uint8_t i = 0; i < 4; i++){
			if (orangeLightsOn){
				allRoadsArr[i].turnOff(Directions::STRAIGHT);
				allRoadsArr[i].turnOff(Directions::LEFT);
				allRoadsArr[i].turnOff(Directions::RIGHT);
				allRoadsArr[i].turnOffPedestrians();
			}else{
				allRoadsArr[i].allOrange();
				allRoadsArr[i].turnOffPedestrians();
			}
		}
		orangeLightsOn = !orangeLightsOn;
		
		previousBlinkTime = millis();	
	}
	
	// Makes sure that orange lights turn off when traffic lights start running
	firstTime = true;
}