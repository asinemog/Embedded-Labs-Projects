/*
 * Lab5Ultrasonic.c
 *
 * Created: 24/03/2023 1:11:03 PM
 * Author : asine
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "bitFunctions.h"

#ifndef F_CPU
#define F_CPU 16000000UL
#endif 

unsigned long numOV0;
unsigned long numOV1;

float ultraSonic(void);
void delayUS(float);

ISR(TIMER0_OVF_vect){
	numOV0--;
}

ISR(TIMER1_OVF_vect){
	numOV1--;
}

#define pinTrigger PORTB4
#define pinEcho PORTD4

#define BAUD 9600
#define MY_UBBR (F_CPU/16/BAUD - 1)

void usartINIT(int ubbr){
	UBRR0 = ubbr;
	
	UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);
	
	
}


float ultraSonic(void){
	// save previous values
	char timsk1 = TIMSK2;
	char tccr1b = TCCR1B;
	char sreg = SREG;
	
	TCCR1B = 0;
	TIMSK1 = 0;
	SREG = 0;
	
	bitSet(TIMSK1, TOIE1);
	sei();
	
	char numOV1max = 80;
	numOV1 = numOV1max;
	TCNT1 = 0;
	
	bitClear(PORTB, pinTrigger);
	bitSet(PORTB, pinTrigger);
	
	delayUS(11);
	
	bitClear(PORTB, pinTrigger);
	
	//while pinEcho is high, wait for it to go low
	while(!bitCheck(PIND, pinEcho));
	TCCR1B = 1;
	
	while(numOV1 && bitCheck(PIND, pinEcho));
	TCCR1B = 0;
	
	//store num current clock ticks within the cycle
	unsigned int tcnt1 = TCNT1;
	
	float x; //range
	
	if (numOV1 == 0){
		x = 999;
	}
	else{
		x = ((numOV1max-numOV1)*65536.0 + tcnt1) / 16.0e6 * 343.0/2.0; // range in metres
	}
	
	//return registers back to whatever they were
	TIMSK1 = timsk1;
	SREG = sreg;
	TCCR1B = tccr1b;
	
	return(x);
}

int main(void)
{
    /* Replace with your application code */
    while (1) 
    {
    }
}

