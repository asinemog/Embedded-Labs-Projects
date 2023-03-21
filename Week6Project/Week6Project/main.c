/*
 * Week6Project.c
 *
 * Created: 21/03/2023 10:24:20 PM
 * Author : asine
 */ 

#include <avr/io.h>

#define F_CPU 16000000UL
#define __DELAY_BACKWARD_COMPATIBLE__
#include "util/delay.h"
#include "bitFunctions.h"

#define LEDPIN PINB5

void blinkLed(int period){
	
		bitSet(PORTB, LEDPIN);
		_delay_ms(period);
		bitClear(PORTB, LEDPIN);
		_delay_ms(period);
	
	
}

int main(void)
{
    //testing header file
	bitSet(DDRB, LEDPIN);
	
	
	
    while (1) 
    {
		blinkLed(2000);
    }
}

