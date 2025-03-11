#include "MyTimer.h"

volatile uint32_t timer0_millis = 0; // Define the global variable here

void timer0Init() {
	// Set Timer0 to normal mode
	TCCR0A = 0x00;

	// Set prescaler to 64
	TCCR0B = (1 << CS01) | (1 << CS00);

	// Enable Timer0 overflow interrupt
	TIMSK0 = (1 << TOIE0);

	// Enable global interrupts
	sei();
}

ISR(TIMER0_OVF_vect) {
	// Increment the millisecond counter
	timer0_millis++;
}

uint32_t millis() {
	uint32_t m;
	cli(); // Disable interrupts to avoid race conditions
	m = timer0_millis;
	sei(); // Re-enable interrupts
	return m;
}
