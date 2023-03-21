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
	
	
	
	
	//enable fast pwm for timer 2
	TCCR2A |= (1<<WGM20) | (1<<WGM21);
	TCCR2B |= (1<<WGM22);
	
	TCCR2A |= (1<<COM2B1);
	//prescaler to 32
	TCCR2B |= (1<<CS20) | (1<<CS21);
	
	OCR2A = 135;
	OCR2B = 81;
	
	//TIMER 1
	//wgm
	TCCR1A |= (1<<WGM10) | (1<<WGM11);
	TCCR1B |= (1<<WGM12) | (1<<WGM13);
	
	TCCR1A |= (1<<COM1B1);
	
	TCCR1B|= (1<<CS10);
	
	OCR1A = 4323;
	OCR1B = 2593;
	
	
	
	while(1){
		
	}
	
}