
#ifndef CONFIG_H_
#define CONFIG_H_

#include <avr/io.h>
#include "CrossRoadConfig.h"


extern "C" uint8_t EEPROM_ReadByte(uint8_t address);
extern "C" void EEPROM_WriteByte(uint8_t byte, uint8_t address);
extern "C" void puts(const char *str);

extern uint8_t currentEEPROMConfig[10];

// Function prototypes
void loadCrossRoadConfigFromEEPROM(CrossRoadConfig &config);
bool isEEPROMConfigDifferent();
void writeEEPROMToCurrent();
void writeCurrentToEEPROM();
bool isValidConfigInEEPROM();
void flushUSART();


#endif /* CONFIG_H_ */