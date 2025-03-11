#ifndef MYTIMER_H_
#define MYTIMER_H_

#include <avr/io.h>
#include <avr/interrupt.h>

extern volatile uint32_t timer0_millis; // Declare as extern

void timer0Init();
uint32_t millis();

#endif /* MYTIMER_H_ */
