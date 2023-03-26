/*
 * Week6Project.c
 *
 * Created: 21/03/2023 10:24:20 PM
 * Author : asine
 */ 
#define F_CPU 16000000UL


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "bitFunctions.h"
//#include "delayTimer0.h"
#include "util/delay.h"

unsigned long numOv;

//void delayUs(float x);

ISR(TIMER0_OVF_vect){
	numOv--;
}

int main(void){
	
	DDRD = 0xFF;
	PORTD = 0x00;
	
	while(1){
		
		bitClear(PORTD, PIND3);
		_delay_us(10);
		bitSet(PORTD, PIND3);
		_delay_us(10);
	}
	
}

/*void delayUs(float x){ // x is the delay time in us
	char TCCR0A_old = TCCR0A;
	char TCCR0B_old = TCCR0B;
	char OCR0A_old = OCR0A;
	
	TCCR0A = 0;
	TCCR0B = 0;
	bitSet(TCCR0A, WGM01); // select CTC mode
	
	
	float top = 199;
	OCR0A = top; // TOP
	
	unsigned long tmp = x*16.0/top;
	
	if (tmp){
		TCNT0 = 0;
		
		numOv = tmp;
		sei();
		bitSet(TCCR0B, CS00);
		while(numOv); // delay integer multiples of counting cycles

		cli();
		bitClear(TCCR0B, CS00);
	}
	
	tmp = x*16. - top*tmp; // less than one counting cycle
	
	if (tmp > 0){
		TCNT0 = top + 1 - tmp; // so that after (top-TCNT0) clock cycles, another OVF will be set
		
		numOv = 1;
		
		sei();
		bitSet(TCCR0B, CS00);
		while(numOv);
		
		cli();
	}
	
	TCCR0A = TCCR0A_old;
	TCCR0B = TCCR0B_old;
	OCR0A = OCR0A_old;
}*/