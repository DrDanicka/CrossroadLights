// USER SETS:
#include <avr/io.h>
#include "Road.h"
#include "CrossRoadConfig.h"

extern "C" void USART_init(unsigned int ubrr, unsigned int data_bits, unsigned int stop_bits);
extern "C" void USART_ReadConfig(void);
extern "C" uint8_t EEPROM_ReadByte(uint8_t address);

#define BAUD 115200
#define DATA_BITS 8
#define STOP_BITS 1

#define FOSC 16000000
#define MYUBRR FOSC/16/BAUD

uint8_t currentEEPROMConfig[10] = {
	0xD9,
	0x38, 0x3D,
	0x78, 0x3D,
	0xB8, 0x3D,
	0xF8, 0x3D, 
	0x2D
	};

bool loadCrossRoadConfigFromEEPROM(CrossRoadConfig &config) {
	uint8_t sum = 0;

	// Read the start byte from EEPROM
	uint8_t startByte = EEPROM_ReadByte(0);
	sum += startByte;

	// Validate start byte (equal to 0xD9)
	if (startByte != 0xD9) {
		// TODO Write error to USART
		// Incorrect start byte
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
		// TODO Write error to USART
		// Checksum not correct
		return false;
	}

	// Create CrossRoadConfig object with the 8 bytes
	config = CrossRoadConfig(
		north_byte1, north_byte2,
		west_byte1, west_byte2,
		south_byte1, south_byte2,
		east_byte1, east_byte2
	);

	// TODO Write config loaded successfully
	return true;
}


bool isEEPROMConfigDifferent() {
	for (uint8_t i = 1; i < 10; i++) { // Don't include start byte
		if (currentEEPROMConfig[i] != EEPROM_ReadByte(i)) {
			return true; // EEPROM data is different
		}
	}

	return false;  // EEPROM data is the same
}


int main(void) {
	USART_init(MYUBRR, DATA_BITS, STOP_BITS);
	
	CrossRoadConfig config;
	
	bool success = loadCrossRoadConfigFromEEPROM(config);
	
	if (!success){
		// We set hardwired config
		config = CrossRoadConfig(
			0x38, 0x3D,
			0x78, 0x3D,
			0xB8, 0x3D,
			0xF8, 0x3D
		)
		for (uint8_t i = 0; i < 10; i++) {
			// TODO EEPROM_writeByte(i) = currentEEPROMConfig[i]
		} 
	}else{
		for (uint8_t i = 0; i < 10; i++) {
			currentEEPROMConfig[i] = EEPROM_ReadByte(i);
		}
	}
	
	while(true){
		CrossRoadConfig newConfig;
		
		if (isEEPROMConfigDifferent()){
			loadCrossRoadConfigFromEEPROM(newConfig);
			
			newConfig = config;
		}
	}
	
	return 0;
}
