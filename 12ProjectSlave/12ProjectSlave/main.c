/*
 * lab9_assessment.cpp
 *
 * Created: 4/27/2023 12:09:16 PM
 * Author : Kai
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL
#include "util/delay.h"
#include "bitFunctions.h"


#define BAUD 9600
#define MY_UBBR (F_CPU/16/BAUD - 1)

char flag_rising = 1;
char flag_ready = 0; // ready for uploading

volatile float overflow_cnt = 0;
volatile unsigned int overflow_cnt_array[4] = {};

char ici_cnt = 0;

volatile float tLow = 0;
volatile float tHigh = 0;
volatile unsigned int tcnt1_new = 0;
volatile unsigned int tcnt1_old = 0;


float t1 = 0, t2 = 0;

void initUSART(int ubbr);
void transmitStringUSART(char* x);
void transmitByteUSART(char x);


ISR( TIMER1_CAPT_vect ){
	
	if ( flag_rising ) {				
		
		tcnt1_new = ICR1;	
		bitClear( TCCR1B, ICES1 );
			
		t1 = (float) ( ( tcnt1_new + 1 ) + overflow_cnt*65536 ) 
				/ (float) 16e6 * 1000.;		// in ms due to *1000
		
		tHigh = t1 - t2;		
		
		flag_rising = 0;
					
	} else {
		
		tcnt1_new = ICR1;
		bitSet( TCCR1B, ICES1 );
		
		t2 = (float) ( ( tcnt1_new + 1 ) + overflow_cnt*65536 ) 
				/ (float) 16e6 * 1000.;
		
		tLow = - t1 + t2;	
		
		flag_rising = 1;		
		
		ici_cnt++;
		
		if ( ici_cnt == 3 ){
			flag_ready = 1;
			ici_cnt = 0;
			bitClear(TIMSK1, ICIE1);
		}
	}		
}

ISR( TIMER1_OVF_vect ){
	overflow_cnt++;
}


// a function mapping a value x between x1 and x2 to a value between y1 and y2
// using a linear transformation
float myMap(float x, float x1, float x2, float y1, float y2){
	return (y2-y1)/(x2-x1)*(x-x1) + y1;
}

int main(void)
{
 
	
	
	// pin mode initialization for AC pins
	bitClear(DDRD, PD7); // connected to PD3, negative of AC
	bitClear(DDRD, PD6); // 3.3V input, positive of AC
	
	bitSet(ACSR, ACIC); // enable comparator-based input capture 
		
	bitSet(TCCR1B, ICES1); // start with rising edge trigger	
	bitSet(TIMSK1, ICIE1); // enable input capture interrupt 	
	bitSet(TIMSK1, TOIE1); // overflow interrupt of TC1
	
	bitSet(TCCR1B, CS10); // start TC1 by effecting clock
	
	
	sei();
	
	float freqTrue;
	float dcTrue;
	
	float freqEst;
	float dcEst;
	initUSART(MY_UBBR);
	
	while (1) 
    {
		
		
		if (flag_ready){	
			
			freqEst = 1000./( tHigh + tLow ); // Hz, as tXXX is in ms
			dcEst = tHigh/( tHigh + tLow ) * 100.; // duty cycle, in %
										
			char tempArray[15];
			dtostrf(freqEst, 10, 3, tempArray);
			tempArray[10] = '\0';
			transmitStringUSART(tempArray);
			transmitStringUSART("\r\n");		
			
			dtostrf(dcEst, 10, 3, tempArray);
			tempArray[10] = '\0';
			transmitStringUSART(tempArray);
			transmitStringUSART("\r\n");
				
			
						
			flag_ready = 0;
			
			bitSet(TIMSK1, ICIE1);					
		}
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


