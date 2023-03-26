/*
 * CFile1.c
 *
 * Created: 26/03/2023 12:52:53 PM
 *  Author: asine
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include "bitFunctions.h"

#define F_CPU 16000000UL

extern unsigned long numOV0;

void delayUS(float t){
	
	cli();
	// save old register vals
	char TCCR0A_old = TCCR0A;
	char TCCR0B_old = TCCR0B
	char TIMSK0_old = TIMSK0;
	
	//enable timer overflow interrupt
	bitSet(TIMSK0, TOIE0);
	
	// e.g. t is 11, desired delay of 11 us
	// 1 cycle is 256*(1/16MHz) = 16 microsecond = tOv
	// num desired overflow count = tOV/MAX * tdesired
	
	unsigned long tmp = 16.0/256.0 * t;
	
	// load 
	numOV0 = tmp;
	TCNT0 = 0;
	
	sei();
	
	// set prescaler to 1 & start counting, wait for interrupt count to count down below threshold
	// then stop counting
	if(tmp > 0){
		bitSet(TCCR0B, CS00);
		while(numOV0);
		bitClear(TCCR0B, CS00);
	}
	
	
	// remaining cycles = t/16*MAX - num.overflows*MAX
	// e.g. if t = 18us, remaining cycles = 288 - 256 = 32
	tmp = t/16*256 - tmp*256;
	
	
	
	
}