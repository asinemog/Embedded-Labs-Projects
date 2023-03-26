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
	char TCCR0B_old = TCCR0B;
	char TIMSK0_old = TIMSK0;
	char SREG_old = SREG;
	
	//enable timer overflow interrupt
	bitSet(TIMSK0, TOIE0);
	
	// e.g. t is 11, desired delay of 11 us
	// 1 cycle is 256*(1/16MHz) = 16 microsecond = tOv
	// num desired overflow count = tOV/MAX * tdesired
	
	unsigned long tmp = 16.0/256.0 * t;
	
	// load 
	numOV0 = tmp;
	TCNT0 = 0;
	
	
	
	// if at least 1 overflow is needed, set prescaler to 1 & start counting, wait for 
	// interrupt count to count down below thresholdthen stop counting
	if(tmp){
		
		sei();
		bitSet(TCCR0B, CS00);
		while(numOV0);
		bitClear(TCCR0B, CS00);
		cli();
	}
	
	
	// remaining cycles = t/16*MAX - num.overflows*MAX
	// e.g. if t = 18us, remaining cycles = 288 - 256 = 32
	tmp = t/16.0*256.0 - tmp*256.0;
	
	if(tmp > 0){
		
		// count the rest of the way (which will be less than 1 counting cycle)
		sei();
		TCNT0 = 256 - tmp;
		numOV0 = 1;
		bitSet(TCCR0B, CS00);
		while(numOV0);
		bitClear(TCCR0B, CS00);
		cli();
	}
	
	
	
	TCCR0A = TCCR0A_old;
	TCCR0B = TCCR0B_old;
	TIMSK0 = TIMSK0_old;
	SREG = SREG_old;
	

}