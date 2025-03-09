// USER SETS:
#include <avr/io.h>
#include <util/delay.h>
#include "Road.h"
#include "CrossRoadConfig.h"
#include "twi.h"

extern "C" void USART_init(unsigned int ubrr);
extern "C" void USART_ReadConfig(void);
extern "C" uint8_t EEPROM_ReadByte(uint8_t address);
extern "C" void EEPROM_WriteByte(uint8_t byte, uint8_t address);
extern "C" void puts(const char *str);

#define LED_PIN PB5
#define BUTTON_PIN PB4
#define DEBOUNCE_TIME 50

#define BAUD 115200
#define FOSC 16000000
#define MYUBRR FOSC/16/BAUD


enum CrossRoadState{
	ON,
	OFF
};

CrossRoadState crossRoadState = CrossRoadState::OFF;

uint8_t currentEEPROMConfig[10] = {
	0xD9,
	0x38, 0x3D,
	0x78, 0x3D,
	0xB8, 0x3D,
	0xF8, 0x3D, 
	0x2D
	};

void loadCrossRoadConfigFromEEPROM(CrossRoadConfig &config) {
	// Read the 8 configuration bytes
	uint8_t north_byte1 = EEPROM_ReadByte(1);
	uint8_t north_byte2 = EEPROM_ReadByte(2);
	uint8_t west_byte1 = EEPROM_ReadByte(3);
	uint8_t west_byte2 = EEPROM_ReadByte(4);
	uint8_t south_byte1 = EEPROM_ReadByte(5);
	uint8_t south_byte2 = EEPROM_ReadByte(6);
	uint8_t east_byte1 = EEPROM_ReadByte(7);
	uint8_t east_byte2 = EEPROM_ReadByte(8);

	// Create CrossRoadConfig object with the 8 bytes
	config = CrossRoadConfig(
		north_byte1, north_byte2,
		west_byte1, west_byte2,
		south_byte1, south_byte2,
		east_byte1, east_byte2
	);

	puts("Config loaded successfully!");
}


bool isEEPROMConfigDifferent() {
	for (uint8_t i = 1; i < 10; i++) { // Don't include start byte
		uint8_t currentByte = EEPROM_ReadByte(i);
		if (currentEEPROMConfig[i] != currentByte) {
			return true; // EEPROM data is different
		}
	}

	return false;  // EEPROM data is the same
}

void writeEEPROMToCurrent(){
	for (uint8_t i = 0; i < 10; i++) {
		uint8_t currentByte = EEPROM_ReadByte(i);
		currentEEPROMConfig[i] = currentByte;
	}
}

void writeCurrentToEEPROM(){
	puts("Invalid Config in EEPROM -> switching to previous/default!");
	
	for (uint8_t i = 0; i < 10; i++){
		EEPROM_WriteByte(i, currentEEPROMConfig[i]);
	}
}

bool isValidConfigInEEPROM(){
	uint8_t sum = 0;

	// Read the start byte from EEPROM
	uint8_t startByte = EEPROM_ReadByte(0);
	sum += startByte;

	// Validate start byte (equal to 0xD9)
	if (startByte != 0xD9) {
		puts("Incorrect start byte");
		return false;
	}

	// Read the 8 configuration bytes
	uint8_t north_byte1 = EEPROM_ReadByte(1);
	uint8_t north_byte2 = EEPROM_ReadByte(2);
	uint8_t west_byte1 = EEPROM_ReadByte(3);
	uint8_t west_byte2 = EEPROM_ReadByte(4);
	uint8_t south_byte1 = EEPROM_ReadByte(5);
	uint8_t south_byte2 = EEPROM_ReadByte(6);
	uint8_t east_byte1 = EEPROM_ReadByte(7);
	uint8_t east_byte2 = EEPROM_ReadByte(8);

	// Sum up for checksum
	sum += north_byte1 + north_byte2;
	sum += west_byte1 + west_byte2;
	sum += south_byte1 + south_byte2;
	sum += east_byte1 + east_byte2;

	// Read the checksum byte from EEPROM
	uint8_t storedChecksum = EEPROM_ReadByte(9);

	// Verify checksum
	if ((sum & 0xFF) != storedChecksum) {
		puts("Incorrect checksum");
		return false;
	}

	return true;
}

void initButtonAndLED() {
	DDRB |= (1 << LED_PIN);  // Set LED pin as OUTPUT
	DDRB &= ~(1 << BUTTON_PIN);  // Set button pin as INPUT
	PORTB |= (1 << BUTTON_PIN);  // Enable pull-up resistor
}

bool isButtonPressed() {
    if (!(PINB & (1 << BUTTON_PIN))) {
        _delay_ms(DEBOUNCE_TIME);

        if (!(PINB & (1 << BUTTON_PIN))) {
            while (!(PINB & (1 << BUTTON_PIN))) {}
            return true;  // Button was pressed and released
        }
    }
    return false;  // Button was not pressed
}

void toggleCrossroadState() {
	if (crossRoadState == CrossRoadState::ON) {
		crossRoadState = CrossRoadState::OFF;
		PORTB &= ~(1 << LED_PIN); // Turn LED OFF
	} else {
		crossRoadState = CrossRoadState::ON;
		PORTB |= (1 << LED_PIN);  // Turn LED ON
	}
}

// Global variables for blinking logic
volatile uint8_t blinkState = 0; // 0 = off, 1 = on
volatile uint16_t blinkTimer = 0; // Timer for blinking

void blinkLightsOFF() {
	// Check if it's time to toggle the blink state
	if (blinkTimer >= 5000) { // 500 ms delay for blinking
		blinkTimer = 0; // Reset the timer
		blinkState = !blinkState; // Toggle the blink state

		// Array of all expander addresses
		uint8_t expanderAddresses[] = {NORTH_ADDRESS, EAST_ADDRESS, SOUTH_ADDRESS, WEST_ADDRESS};

		// Iterate over all expanders and toggle PA1, PA4, and PB4
		for (uint8_t i = 0; i < sizeof(expanderAddresses) / sizeof(expanderAddresses[0]); i++) {
			setPinState(expanderAddresses[i], REG_OUTPUT_PORTA, 1, blinkState); // PA1
			setPinState(expanderAddresses[i], REG_OUTPUT_PORTA, 4, blinkState); // PA4
			setPinState(expanderAddresses[i], REG_OUTPUT_PORTB, 4, blinkState); // PB4
		}
	}
}

void setLOWAllExpanderPins(){
	uint8_t expanderAddresses[] = {NORTH_ADDRESS, EAST_ADDRESS, SOUTH_ADDRESS, WEST_ADDRESS};

	// Iterate over all expanders and turnoff all ports
	for (uint8_t i = 0; i < sizeof(expanderAddresses) / sizeof(expanderAddresses[0]); i++) {
		for (uint8_t j = 0; j < 8; j++){
			setPinState(expanderAddresses[i], REG_OUTPUT_PORTA, j, 0);
			setPinState(expanderAddresses[i], REG_OUTPUT_PORTB, j, 0); // PA1
		}
	}
}

void configureAllExpanders(){
	configureExpander(NORTH_ADDRESS);
	configureExpander(EAST_ADDRESS);
	configureExpander(SOUTH_ADDRESS);
	configureExpander(WEST_ADDRESS);
	
	setLOWAllExpanderPins();
}

int main(void) {	
	USART_init(MYUBRR);
	initButtonAndLED(); 
	TWI_Init();
	configureAllExpanders();
	
	CrossRoadConfig config;
	bool new_usart_config = false;
	
	while (true){
		if (isButtonPressed()) {
			toggleCrossroadState();
		}
		
		if (crossRoadState == CrossRoadState::ON){
			// ON state
			// TODO CrossroadLights functionality
			setLOWAllExpanderPins();
		}else{
			// OFF state
			blinkLightsOFF();
			
			if (!isValidConfigInEEPROM()){
 				writeCurrentToEEPROM();
 			}

 			if (isEEPROMConfigDifferent()){
 				loadCrossRoadConfigFromEEPROM(config);
				writeEEPROMToCurrent();
 			}else{
				if (new_usart_config){
					puts("Same config accepted!");
				}
			}
			
			new_usart_config = false;
			
			if (UCSR0A & (1 << RXC0)) {
				USART_ReadConfig();
				new_usart_config = true;
			}
		}
		
		// Increment the blink timer (non-blocking)
		_delay_ms(1); // 1 ms delay
		blinkTimer++;
	}
	
	return 0;
}
