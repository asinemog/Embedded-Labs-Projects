/*
 * pwmtest.c
 *
 * Created: 19/05/2023 11:09:25 AM
 *  Author: asine
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include "bitFunctions.h"
#include "util/delay.h"

int main(void){

	
	

	DDRB = 0xFF;
	
	// 	set fast PWM mode

	bitSet(TCCR1A, WGM10);
	bitSet(TCCR1A, WGM11);
	bitSet(TCCR1B, WGM12);
	bitSet(TCCR1B, WGM13);
	
	
	// clear on compare match, set on bot
	bitSet(TCCR1A, COM1B1);
	
	// TOP = FCPU/(FPWM*P) - 1
	// TOP = 16mHz/(100*P) - 1
	// P = 8 gives TOP = 19999 < MAX (65535)
	OCR1A = 19999;
	
	// default duty 60%
	OCR1B = 17000;
	
	bitSet(TCCR1B, CS11);
	
	
	while(1){
		
		
		
	}
}