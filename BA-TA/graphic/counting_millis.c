/*
 * counting_millis.c
 *
 * Created: 27-05-2018 12:32:57
 *  Author: Soren
 */ 

#include "counting_millis.h"

volatile unsigned long timer1_millis;


void CountingInit()
{
	// CTC mode, Clock/8
	TCCR1B |= (1 << WGM12) | (1 << CS11);

	// Load the high byte, then the low byte
	// into the output compare
	OCR1AH = (CTC_MATCH_OVERFLOW >> 8);
	OCR1AL = CTC_MATCH_OVERFLOW;

	// Enable the compare match interrupt
	TIMSK1 |= (1 << OCIE1A);

	// PC0/Analog 0 to Output
	DDRC |= (1 << PC0);
    // Now enable global interrupts
    sei();
}

ISR (TIMER1_COMPA_vect)
{
	timer1_millis++;
}

unsigned long millis ()
{
	unsigned long millis_return;

	// Ensure this cannot be disrupted
	ATOMIC_BLOCK(ATOMIC_FORCEON) {
		millis_return = timer1_millis;
	}
	
	return millis_return;
}

