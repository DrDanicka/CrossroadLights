#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>

// Define I2C addresses for each expander
#define EXPANDER_1_ADDRESS 0x58
#define EXPANDER_2_ADDRESS 0x59

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

int main(void) {
	// Initialize TWI (I2C)
	TWI_Init();

	// Configure both expanders
	configureExpander(EXPANDER_1_ADDRESS);
	configureExpander(EXPANDER_2_ADDRESS);

	while (1) {
		// Turn on PB0 and PA0 on both expanders
		setPinState(EXPANDER_1_ADDRESS, REG_OUTPUT_PORTA, 0, 0); // Expander 1, PA0 HIGH
		setPinState(EXPANDER_1_ADDRESS, REG_OUTPUT_PORTB, 0, 1); // Expander 1, PB0 HIGH
		setPinState(EXPANDER_2_ADDRESS, REG_OUTPUT_PORTA, 0, 0); // Expander 2, PA0 HIGH
		setPinState(EXPANDER_2_ADDRESS, REG_OUTPUT_PORTB, 0, 1); // Expander 2, PB0 HIGH
	}
}

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
	
	// Set all pins on Port A and Port B as output
	writeRegister(address, REG_CONFIG_PORTA, 0x00); // Port A as output
	writeRegister(address, REG_CONFIG_PORTB, 0x00); // Port B as output

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