/*
 * Lab5Ultrasonic.c
 *
 * Created: 24/03/2023 1:11:03 PM
 * Author : asine
 */ 

#define F_CPU 16000000UL


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>




//e.g. PIND = 0b00010100,
// reg = PIND = 0b00010100
// ind = PIND4 = 4
// PIND >> PIND4 & 1 = 0b00010100 >> 4 & 1 = 0b00000001 & 0b00000001 = 1/true
//


#define F_CPU 16000000UL


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "bitFunctions.h"
#include "delayTimer0.h"
#include "ultraSonic.h"

#define pinTrigger PORTB4
#define pinEcho PORTD4

unsigned long numOv;
unsigned long numOv1;
unsigned long numCmp;

void transmitStringUSART(char* pdata);
void recieveCharUSART(void);
void transmitByteUSART(char c);
void initUSART(int ubbr);



ISR(TIMER0_OVF_vect){
	numOv--;
}

ISR(TIMER1_OVF_vect){
	numOv1--;
}

ISR(TIMER0_COMPA_vect){
	numCmp--;
}

#define pinTrigger PORTB4
#define pinEcho PORTD4

#define BAUD 9600
#define MY_UBBR (F_CPU/16/BAUD - 1)



int main(void)
{
    initUSART(MY_UBBR);
	
	bitSet(DDRB, pinTrigger);
	
	bitClear(DDRD, pinEcho);
	bitSet(PORTD, pinEcho);
	
	DDRB = 0xFF;
	
	
	//float range;
	//char rangeStr[15];
	
	
    while (1) 
    {
		/*range = ultraSonic(pinTrigger, pinEcho);
		dtostrf(range, 10, 3, rangeStr);
		rangeStr[10] = '\0';
		
		transmitStringUSART(rangeStr);
		transmitStringUSART("\r\n");*/
		delayUS(20);
		PORTB |= (1<<PINB5);
		delayUS(20);
		PORTB &= ~(1<<PINB5);
		
    }
}



void initUSART(int ubbr){
	UBRR0 = ubbr;
	
	UCSR0B |= (1<<TXEN0) ;
	
	UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);
	
	
}

void transmitStringUSART(char* x){
	
	
	// loop while current char isnt stop char and end of string hasnt been reached
	while(*x != '\0'){
		transmitByteUSART(*x);
		x++;
	
	}
}

void transmitByteUSART(char x){
	while(!bitCheck(UCSR0A, UDRE0));
	UDR0 = x;
	
}

