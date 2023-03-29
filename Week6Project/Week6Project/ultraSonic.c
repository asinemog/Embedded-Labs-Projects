#include <avr/io.h>
#include <avr/interrupt.h>
#include "delayTimer0.h"
#include "bitFunctions.h"

#define F_CPU 16000000UL

extern unsigned long numOv1;

float ultraSonic(int pinTrigger, int pinEcho){
	// save previous values of registers we are using
	char timsk1 = TIMSK1;
	char tccr1b = TCCR1B;
	char sreg = SREG;
	
	// reset registers
	TCCR1B = 0;
	TIMSK1 = 0;
	SREG = 0;
	
	// enabled timer overflow interrupt
	bitSet(TIMSK1, TOIE1);
	sei();
	
	char numOv1max = 6;
	numOv1 = numOv1max;
	TCNT1 = 0;
	
	bitClear(PORTD, pinTrigger);
	
	// to trigger pulses, set high for > 10us then set low
	bitSet(PORTD, pinTrigger);
	
	delayUS(11);
	//_delay_us(11);
	
	bitClear(PORTD, pinTrigger);
	
	//wait for pinEcho to be high before counting
	while(!bitCheck(PIND, pinEcho));
	//when pinEcho goes high, start timer to measure how many ticks it is high
	sei();
	bitSet(TCCR1B, CS10);
	
	//wait while pinEcho is high
	while(numOv1 && bitCheck(PIND, pinEcho));
	//stop timer when pinEcho goes low
	bitClear(TCCR1B, CS10);
	
	//store num current clock ticks within the cycle
	unsigned int tcnt1 = TCNT1;
	
	float x; //range
	
	//if (numOv1 == 0){
	//	x = 999;
//	}
//	else{
		// speed = distance/time so distance = speed * time
		// speed = speed of sound, time is time take by timer 1 to count how long echo pin was high
		// time = number of ticks taken / no ticks per second (F_CPU/P) in seconds.
		// no of ticks taken = no. overflows + current ticks
		x = ((numOv1max-numOv1)*65536.0 + tcnt1) / 16.0e6 * 343.0/2.0 * 100.0; // range in centimetres
//	}
	
	//return registers back to whatever they were
	TIMSK1 = timsk1;
	SREG = sreg;
	TCCR1B = tccr1b;
	
	return(x);
}
