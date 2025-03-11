#include "Config.h"


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
		puts("Incorrect start byte. ");
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
		puts("Incorrect checksum. ");
		return false;
	}

	return true;
}

void flushUSART() {
	while (UCSR0A & (1 << RXC0)) {
		// Read and discard the data
		uint8_t dummy = UDR0;
	}
}