#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif


#define PWMOUT1 PINB2
#define PWMOUT2 PIND3

int main(void){

	cli();

	DDRB = 0xFF; //Set ports B and D as outputs
	DDRD = 0xFF;

	

	
	//enable phase correct PWM with TOP = OCR1A for Timer 1
	TCCR1A |= (1<<WGM10) | (1<<WGM10);
	TCCR1B |=  (1<<WGM13);

	//
	TCCR1A |= (1<<COM1B1);

	// set prescaler to 1
	TCCR1B |= (1<<CS10);

	// Set TOP to
	OCR1A = 2162;
	OCR1B = 1297;
	
	//TIMER 2
	TCCR2A |= (1<<WGM20);
	TCCR2B |= (1<<WGM22);
	
	//
	TCCR2A |= (1<<COM2B1);
	
	
	//prescaler = 32
	TCCR2B |= (1<<CS21) | (1<<CS20);
	
	OCR2A = 68;
	OCR2B = 41;
	

	while(1){
		
	}


	//









}