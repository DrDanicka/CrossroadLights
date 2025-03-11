#include "twi.h"


// Initialize TWI (I2C)
void TWI_Init() {
	// Set bit rate register
	TWBR0 = TWI_BIT_RATE_REGISTER;
	
	// Enable TWI
	TWCR0 = (1 << TWEN);
}

// Send a START condition
void TWI_Start() {
	TWCR0 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR0 & (1 << TWINT)));
}

// Send a STOP condition
void TWI_Stop() {
	TWCR0 = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

// Write a byte to the TWI bus
void TWI_Write(uint8_t data) {
	TWDR0 = data;
	TWCR0 = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR0 & (1 << TWINT)));
}

// Read a byte from the TWI bus
uint8_t TWI_Read(uint8_t ack) {
	if (ack) {
		TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
		} else {
		TWCR0 = (1 << TWINT) | (1 << TWEN);
	}
	while (!(TWCR0 & (1 << TWINT)));
	return TWDR0;
}

// Write a register on the AW9523B
void writeRegister(uint8_t address, uint8_t reg, uint8_t value) {
	TWI_Start();
	TWI_Write((address << 1) | 0); // Send address + write bit
	TWI_Write(reg);                // Send register address
	TWI_Write(value);              // Send value
	TWI_Stop();
}

// Read a register from the AW9523B
uint8_t readRegister(uint8_t address, uint8_t reg) {
	uint8_t value;
	TWI_Start();
	TWI_Write((address << 1) | 0); // Send address + write bit
	TWI_Write(reg);                // Send register address
	TWI_Start();
	TWI_Write((address << 1) | 1); // Send address + read bit
	value = TWI_Read(0);           // Read value with NACK
	TWI_Stop();
	return value;
}

// Configure the AW9523B expander
void configureExpander(uint8_t address) {
	
	// Set all pins on port A
	writeRegister(address, REG_CONFIG_PORTA, 0x00);
	
	// Set PB2 as input because of a button there
	writeRegister(address, REG_CONFIG_PORTB, 0x04);

	// Set all pins on Port A and Port B to GPIO mode
	writeRegister(address, REG_LED_MODE_B, 0xFF);
	writeRegister(address, REG_LED_MODE_A, 0xFF);

	// Set P1 (PA0-PA7) to Push-Pull Mode
	writeRegister(address, REG_GLOBAL_CTRL, 0x10);
}

// Set the state of a pin on the AW9523B
void setPinState(uint8_t address, uint8_t reg, uint8_t pin, bool state) {
	// Read the current state of the output register
	uint8_t currentState = readRegister(address, reg);

	// Modify the specific pin
	if (state) {
		currentState |= (1 << pin); // Set the pin HIGH
		} else {
		currentState &= ~(1 << pin); // Set the pin LOW
	}

	// Write the new state back to the output register
	writeRegister(address, reg, currentState);
}

// Reads pin state at address
bool readPinState(uint8_t address, uint8_t reg, uint8_t pin) {
	uint8_t portState = readRegister(address, reg);
	return (portState & (1 << pin)) != 0;
}

// Configure all expanders
void configureAllExpanders(){
	configureExpander(NORTH_ADDRESS);
	configureExpander(EAST_ADDRESS);
	configureExpander(SOUTH_ADDRESS);
	configureExpander(WEST_ADDRESS);
}