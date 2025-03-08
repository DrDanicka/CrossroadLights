// USER SETS:
#include <avr/io.h>
#include <util/delay.h>
#include "Road.h"
#include "CrossRoadConfig.h"

extern "C" void USART_init(unsigned int ubrr, unsigned int data_bits, unsigned int stop_bits);
extern "C" void USART_ReadConfig(void);
extern "C" uint8_t EEPROM_ReadByte(uint8_t address);
extern "C" void EEPROM_WriteByte(uint8_t byte, uint8_t address);
extern "C" void puts_own_implementation(const char *str);

#define BAUD 115200
#define DATA_BITS 8
#define STOP_BITS 1
#define LED_PIN PB5
#define BUTTON_PIN PB4
#define DEBOUNCE_TIME 50
#define FOSC 16000000
#define MYUBRR FOSC/16/BAUD

bool crossroad_on = true;

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

	puts_own_implementation("Config loaded successfully!");
}


bool isEEPROMConfigDifferent() {
	for (uint8_t i = 1; i < 10; i++) { // Don't include start byte
		uint8_t currentByte = EEPROM_ReadByte(i);
		if (currentEEPROMConfig[i] != currentByte) {
			return true; // EEPROM data is different
		}
	}
	_delay_ms(10);

	return false;  // EEPROM data is the same
}

void writeEEPROMToCurrent(){
	for (uint8_t i = 0; i < 10; i++) {
		uint8_t currentByte = EEPROM_ReadByte(i);
		currentEEPROMConfig[i] = currentByte;
	}
	_delay_ms(10);
}

void writeCurrentToEEPROM(){
	for (uint8_t i = 0; i < 10; i++){
		EEPROM_WriteByte(currentEEPROMConfig[i], i);
	}
	_delay_ms(10);
}

bool isValidConfigInEEPROM(){
	uint8_t sum = 0;

	// Read the start byte from EEPROM
	uint8_t startByte = EEPROM_ReadByte(0);
	sum += startByte;

	// Validate start byte (equal to 0xD9)
	if (startByte != 0xD9) {
		puts_own_implementation("Incorrect start byte");
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
		puts_own_implementation("Incorrect checksum");
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
	if (!(PINB & (1 << BUTTON_PIN))) { // Button is active low
		_delay_ms(DEBOUNCE_TIME);
		if (!(PINB & (1 << BUTTON_PIN))) {
			return true;
		}
	}
	return false;
}

void toggleCrossroadState() {
	crossroad_on = !crossroad_on; // Toggle state
	if (crossroad_on) {
		PORTB |= (1 << LED_PIN);  // Turn LED ON
		} else {
		PORTB &= ~(1 << LED_PIN); // Turn LED OFF
	}
}

int main(void) {	
	USART_init(MYUBRR, DATA_BITS, STOP_BITS);
	initButtonAndLED(); 
	
	CrossRoadConfig config;
	bool init = true;
	
	while (true){
		if (isButtonPressed()) {
			toggleCrossroadState();
		}
		
		if (crossroad_on){
			// ON state
			// TODO CrossroadLights functionality
			
		}else{
			// OFF state
			if (!isValidConfigInEEPROM()){
				writeCurrentToEEPROM();
			}
			
			loadCrossRoadConfigFromEEPROM(config);
			writeEEPROMToCurrent();
			
			USART_ReadConfig();
			
			
// 			USART_ReadConfig();
// 			
// 			if (init || isEEPROMConfigDifferent()){
// 				if (!isValidConfigInEEPROM()){
// 					writeCurrentToEEPROM();
// 				}
// 				
// 				loadCrossRoadConfigFromEEPROM(config);
// 				writeEEPROMToCurrent();
// 				init = false;
// 			}
		}
	}
	
	return 0;
}
