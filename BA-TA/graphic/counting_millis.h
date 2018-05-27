/*
 * counting_millis.h
 *
 * Created: 27-05-2018 12:46:14
 *  Author: Soren
 */ 


// 16MHz Clock
#define F_CPU 16000000UL

// Calculate the value needed for
// the CTC match value in OCR1A.
#define CTC_MATCH_OVERFLOW ((F_CPU / 1000) / 8)

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>