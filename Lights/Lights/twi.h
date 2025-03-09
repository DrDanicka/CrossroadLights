#ifndef TWI_H
#define TWI_H

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

// Define I2C addresses for each expander
#define NORTH_ADDRESS 0x58
#define EAST_ADDRESS 0x5A
#define SOUTH_ADDRESS 0x59
#define WEST_ADDRESS 0x5B

// Register addresses for AW9523B
#define REG_CONFIG_PORTA 0x04
#define REG_CONFIG_PORTB 0x05
#define REG_OUTPUT_PORTA 0x02
#define REG_OUTPUT_PORTB 0x03
#define REG_LED_MODE_A   0x13  // P1 LED Mode Register
#define REG_LED_MODE_B   0x12  // P0 LED Mode Register
#define REG_GLOBAL_CTRL  0x11  // Global Control Register

// TWI (I2C) settings
#define F_SCL 100000 // I2C clock speed (100 kHz)
#define TWI_BIT_RATE_REGISTER ((F_CPU / F_SCL) - 16) / 2

// Function prototypes
void TWI_Init();
void TWI_Start();
void TWI_Stop();
void TWI_Write(uint8_t data);
uint8_t TWI_Read(uint8_t ack);
void writeRegister(uint8_t address, uint8_t reg, uint8_t value);
uint8_t readRegister(uint8_t address, uint8_t reg);
void configureExpander(uint8_t address);
void setPinState(uint8_t address, uint8_t reg, uint8_t pin, bool state);

#endif