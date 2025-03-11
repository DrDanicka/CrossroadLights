// USER SETS:
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "Road.h"
#include "CrossRoadConfig.h"
#include "twi.h"
#include "Config.h"
#include "CrossRoadLogic.h"

extern "C" void USART_init();
extern "C" void USART_ReadConfig(void);
extern "C" void puts(const char *str);

#define LED_PIN PB5
#define BUTTON_PIN PB4

// Declare global current EEPROM config
extern uint8_t currentEEPROMConfig[10] = {
	0xD9,
	0x38, 0x3D,
	0x78, 0x3D,
	0xB8, 0x3D,
	0xF8, 0x3D,
	0x2D
};

// Define states of the crossroad
enum CrossRoadState{
	ON,
	OFF
};

CrossRoadState crossRoadState = CrossRoadState::OFF;


// Initialize Button and Led that are connected directly to microcontroller
void initButtonAndLED() {
	DDRB |= (1 << LED_PIN);			// Set LED pin as OUTPUT
	DDRB &= ~(1 << BUTTON_PIN);		// Set button pin as INPUT
	PORTB |= (1 << BUTTON_PIN);		// Enable pull-up resistor
}

// Simple button pressed function with delay
bool isButtonPressed() {
    if (!(PINB & (1 << BUTTON_PIN))) {
        _delay_ms(DEBOUNCE_TIME);

        if (!(PINB & (1 << BUTTON_PIN))) {
            while (!(PINB & (1 << BUTTON_PIN))) {}
            return true;
        }
    }
    return false;
}

// Change state of the crossroad
void toggleCrossroadState() {
	if (crossRoadState == CrossRoadState::ON) {
		crossRoadState = CrossRoadState::OFF;
		PORTB &= ~(1 << LED_PIN); // Turn LED OFF
	} else {
		crossRoadState = CrossRoadState::ON;
		PORTB |= (1 << LED_PIN);  // Turn LED ON
	}
}

void initializeEverything(){
	USART_init();
	initButtonAndLED();
	TWI_Init();
	configureAllExpanders();
}

int main(void) {	
	initializeEverything();
	
	CrossRoadConfig config;
	CrossRoadLogic crossRoadLogic;
	bool newUsartConfig = false;
	
	// Main loop
	while (true){
		// If main button pressed -> change state
		if (isButtonPressed()) {
			toggleCrossroadState();
		}
		
		if (crossRoadState == CrossRoadState::ON){
			// ON state
			crossRoadLogic.startTrafficLights();
			// Flush USART in case there was something sent while ON
			flushUSART();
		}else{
			// OFF state

			// If there is not valid state in EEPROM -> read what is in current array
			// This can be default configuration or last configuration
			if (!isValidConfigInEEPROM()){
 				writeCurrentToEEPROM();
 			}

			// If EEPROM config changed (something was sent) -> create new config object and crossRoadLogic object
 			if (isEEPROMConfigDifferent()){
 				loadCrossRoadConfigFromEEPROM(config);
				writeEEPROMToCurrent();		// Write EEPROM content to current (we checked that it's valid)
				crossRoadLogic = CrossRoadLogic(config);
 			}else{
				if (newUsartConfig){
					// Print that same config was accepted
					puts("Same config accepted!");
				}
			}
			
			// Blink orange when OFF
			crossRoadLogic.blinkOrange();
			newUsartConfig = false;
			
			// Check if there was something sent to USART
			if (UCSR0A & (1 << RXC0)) {
				// Reads config to buffer and then stores to EEPROM if valid start byte and checksum
				USART_ReadConfig();
				newUsartConfig = true;
			}
		}
	}
	
	return 0;
}
