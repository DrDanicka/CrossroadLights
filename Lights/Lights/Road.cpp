#include "Road.h"

uint8_t right_pins[3] = {0, 1, 2};
uint8_t left_pins[3] = {3, 4, 5};
uint8_t straight_pins[3] = {3, 4, 5};
uint8_t expanderAddresses[4] = {NORTH_ADDRESS, EAST_ADDRESS, SOUTH_ADDRESS, WEST_ADDRESS};

Road::Road() : orientation(0), straight(false), left(false), right(false),
green_light(0), pedestrians(false), exists(false) {}

Road::Road(uint8_t byte1, uint8_t byte2){
	/*
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
	*/
	orientation = (byte1 >> 6) & 0x03;
	straight = (byte1 & (1 << 5));
	left = (byte1 & (1 << 4));
	right = (byte1 & (1 << 3));
	green_light = ((byte1 & 0x07) << 7) | ((byte2 & 0xFE) >> 1);
	green_light += 3; // Because of orange/red switching
	pedestrians = (byte2 & (1 << 0));
	
	expanderAddress = expanderAddresses[orientation];
	
	exists = !(!straight && !left && !right);
}

// Helper function that sets state of one light in direction at light index
void Road::turnOnOneLight(Directions direction, uint8_t light_index, bool state){
	if (direction == Directions::STRAIGHT && straight){
		for(uint8_t i = 0; i < 3; i++){
			if (i == light_index){
				setPinState(expanderAddress, REG_OUTPUT_PORTA, straight_pins[i], state);
			}else{
				setPinState(expanderAddress, REG_OUTPUT_PORTA, straight_pins[i], !state);
			}
		}
	}else if (direction == Directions::LEFT && left){
		for(uint8_t i = 0; i < 3; i++){
			if (i == light_index){
				setPinState(expanderAddress, REG_OUTPUT_PORTB, left_pins[i], state);
			}else{
				setPinState(expanderAddress, REG_OUTPUT_PORTB, left_pins[i], !state);
			}
		}
	}else if (direction == Directions::RIGHT && right){
		for(uint8_t i = 0; i < 3; i++){
			if (i == light_index){
				setPinState(expanderAddress, REG_OUTPUT_PORTA, right_pins[i], state);
			}else{
				setPinState(expanderAddress, REG_OUTPUT_PORTA, right_pins[i], !state);
			}
		}
	}
}

// Turn on green in direction
void Road::green(Directions direction){
	uint8_t green_index = 2;
	
	turnOnOneLight(direction, green_index, true);
}

// Turn on orange in direction
void Road::orange(Directions direction){
	uint8_t orange_index = 1;
	
	turnOnOneLight(direction, orange_index, true);
}

// Turn on red in direction
void Road::red(Directions direction){
	uint8_t red_index = 0;
	
	turnOnOneLight(direction, red_index, true);
}

// Turn on red and orange at the same time in direction
void Road::redOrange(Directions direction){
	uint8_t green_index = 2;
	
	// Turns off only green light and tuns on red and orange
	turnOnOneLight(direction, green_index, false);
}

void Road::greenPedestrians(){
	// Green is connected to port 1
	setPinState(expanderAddress, REG_OUTPUT_PORTB, 1, true);
	setPinState(expanderAddress, REG_OUTPUT_PORTB, 0, false);
}

void Road::redPedestrians(){
	// Green is connected to port 2
	setPinState(expanderAddress, REG_OUTPUT_PORTB, 0, true);
	setPinState(expanderAddress, REG_OUTPUT_PORTB, 1, false);
}

// Turns off car traffic light in the direction
void Road::turnOff(Directions direction){
	if (direction == Directions::STRAIGHT){
		for(uint8_t i = 0; i < 3; i++){
			setPinState(expanderAddress, REG_OUTPUT_PORTA, straight_pins[i], 0);
		}
	}else if (direction == Directions::LEFT){
		for(uint8_t i = 0; i < 3; i++){
			setPinState(expanderAddress, REG_OUTPUT_PORTB, left_pins[i], 0);
		}
	}else{
		for(uint8_t i = 0; i < 3; i++){
			setPinState(expanderAddress, REG_OUTPUT_PORTA, right_pins[i], 0);
		}
	}
}

// Turns off pedestrian lights
void Road::turnOffPedestrians(){
	setPinState(expanderAddress, REG_OUTPUT_PORTB, 1, false);
	setPinState(expanderAddress, REG_OUTPUT_PORTB, 0, false);
}

// Sets all existing lights to red
void Road::allRed(){
	if (straight){
		red(Directions::STRAIGHT);	
	}else{
		turnOff(Directions::STRAIGHT);
	}
	
	if (left){
		red(Directions::LEFT);
	}else{
		turnOff(Directions::LEFT);
	}
	
	if (right){
		red(Directions::RIGHT);	
	} else{
		turnOff(Directions::RIGHT);
	}
	
	if (pedestrians){
		redPedestrians();	
	}else{
		turnOffPedestrians();
	}
}

// Sets all lights to orange
void Road::allOrange(){
	uint8_t orange_pin = 1;
	
	setPinState(expanderAddress, REG_OUTPUT_PORTA, straight_pins[orange_pin], 1);
	setPinState(expanderAddress, REG_OUTPUT_PORTB, left_pins[orange_pin], 1);
	setPinState(expanderAddress, REG_OUTPUT_PORTA, right_pins[orange_pin], 1);
}

bool Road::buttonClicked(){
	// Read the current state of PB2 (pin of button) at address 0x01
	uint8_t address = 0x01;
	bool currentButtonState = readPinState(expanderAddress, address, 2);

	
	// Check if the button state has changed
	if (currentButtonState != lastButtonState) {
        lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > DEBOUNCE_TIME) {
        if (currentButtonState != buttonPressed) {
            buttonPressed = currentButtonState;

            if (!buttonPressed) {
                return true; // Button was clicked
            }
        }
    }

    // Update the last button state
    lastButtonState = currentButtonState;
	
	return false;
}
