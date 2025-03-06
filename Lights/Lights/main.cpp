// USER SETS:
#define BAUD 115200
#define DATA_BITS 8
#define STOP_BITS 1

#define FOSC 16000000
#define MYUBRR FOSC/16/BAUD

#include <avr/io.h>
extern "C" void USART_init(unsigned int ubrr, unsigned int data_bits, unsigned int stop_bits);
extern "C" void USART_ReadConfig(void);


int main(void) {
	USART_init(MYUBRR, DATA_BITS, STOP_BITS);
	
	while(1){
		USART_ReadConfig();
	}
}
