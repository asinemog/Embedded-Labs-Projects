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

extern unsigned long numOv;
extern unsigned long numCmp;


void delayUS(float t){
	
	
	char TCCR0A_old = TCCR0A;
	char TCCR0B_old = TCCR0B;
	char TIMSK0_old = TIMSK0;
	char SREG_old = SREG;
	
	// num overflows = Time desired/(P/F_CPU)/MAX
	// numOV is long, typecast to long rounds down (floor)
	numOv = t*16.0/256.0;
	
	//calc remaining fraction of overflows
	float tmp = t*16.0/256.0 - numOv;
	unsigned long remain = (1 - tmp)*256;
	bitSet(TIMSK0, TOIE0);
	TCCR0A = 0;
	TCCR0B = 0;
	
	if(numOv){
		TCNT0 = 0;
		sei();
		TCCR0B |= (1<<CS00);
		while(numOv > 0);
		TCCR0B &= ~(1<<CS00);
		cli();
	}
	
	if(tmp > 0.0){
		TCNT0 = remain;
		numOv = 1;
		sei();
		TCCR0B |= (1<<CS00);
		while(numOv > 0);
		TCCR0B &= ~(1<<CS00);
		cli();
	}
	
	TCCR0A = TCCR0A_old;
	TCCR0B = TCCR0B_old;
	TIMSK0 = TIMSK0_old;
	SREG = SREG_old;
}

void delay50ms(void){
	
	char TCCR0A_old = TCCR0A;
	char TCCR0B_old = TCCR0B;
	char SREG_old = SREG;
	char TIMSK0_old = TIMSK0;
	char TCNT0_old= TCNT0;
	
	//reset reg for stability
	TCCR0A = 0x00;
	TCCR0B = 0x00;
	TIMSK0 = 0x00;
	
	// set up WGM for CTC mode (TOP = OCRA)
	bitSet(TCCR0A, WGM01);
	
	//enable output compare interrupt
	bitSet(TIMSK0, OCIE0A);
	
	//numOv*(top-1)*P/F_CPU = time delay. 
	// for prescaler 1024, top - 1 = 156, 1 overflow is ~10ms
	// so 5 overflows ~50ms
	float top = 156 - 1;
	OCR0A = top;
	numCmp = 6;
	
	TCNT0 = 0;
	
	//prescaler is 1024
	
	sei();
	
	bitSet(TCCR0B, CS00);
	bitSet(TCCR0B, CS02);
	
 	while(numCmp);
	
	bitClear(TCCR0B, CS00);
	bitClear(TCCR0B, CS02);
	
	cli();
	
	
	TCCR0A = TCCR0A_old;
	TCCR0B = TCCR0B_old;
	TIMSK0 = TIMSK0_old;
	SREG = SREG_old;
	TCNT0 = TCNT0_old;
	
	
}