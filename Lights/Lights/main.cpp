// USER SETS:
#define BAUD 115200
#define DATA_BITS 8
#define STOP_BITS 1

#define FOSC 16000000
#define MYUBRR FOSC/16/BAUD

#include <avr/io.h>
extern "C" void USART_init(unsigned int ubrr, unsigned int data_bits, unsigned int stop_bits);
extern "C" void USART_ReadConfig(void);
extern "C" uint8_t EEPROM_ReadByte(uint8_t address);
extern "C" void USART_Transmit(uint8_t data);


void USART_Print(const char* str) {
	while (*str) {
		USART_Transmit(*str++);
	}
}

void USART_PrintHex(uint8_t value) {
	const char hex_chars[] = "0123456789ABCDEF";
	USART_Transmit(hex_chars[(value >> 4) & 0x0F]);  // High nibble
	USART_Transmit(hex_chars[value & 0x0F]);         // Low nibble
}

int main(void) {
	USART_init(MYUBRR, DATA_BITS, STOP_BITS);
	
	while(1){
		USART_ReadConfig();
		
		uint8_t firstByte = EEPROM_ReadByte(0);
		
		uint8_t secondByte = EEPROM_ReadByte(1);
		
	}
}
