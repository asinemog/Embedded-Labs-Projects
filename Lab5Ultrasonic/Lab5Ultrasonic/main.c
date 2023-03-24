/*
 * Lab5Ultrasonic.c
 *
 * Created: 24/03/2023 1:11:03 PM
 * Author : asine
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include "util/delay.h"
#include "bitFunctions.h"



unsigned long numOV0;
unsigned long numOV1;
unsigned long numOV = 0;

float ultraSonic(void);
void delayUS(float t);
void transmitStringUSART(char* pdata);
void recieveCharUSART(void);
void transmitByteUSART(char c);
void initUART(int ubbr);



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






int main(void)
{
    initUART(MY_UBBR);
	
	bitSet(DDRB, pinTrigger);
	
	bitClear(DDRD, pinEcho);
	
	
	float range;
	char rangeString[15];
	
	
    while (1) 
    {
		range = ultraSonic();
		dtostrf(range, 10, 3, rangeString);
		rangeString[10] = '\0';
		
		transmitStringUSART(rangeString);
		transmitStringUSART("\r\n");
		
		
    }
}

float ultraSonic(void){
	// save previous values of registers we are using
	char timsk1 = TIMSK2;
	char tccr1b = TCCR1B;
	char sreg = SREG;
	
	// reset registers
	TCCR1B = 0;
	TIMSK1 = 0;
	SREG = 0;
	
	// enabled timer overflow interrupt
	bitSet(TIMSK1, TOIE1);
	sei();
	
	char numOV1max = 80;
	numOV1 = numOV1max;
	TCNT1 = 0;
	
	bitClear(PORTB, pinTrigger);
	
	// to trigger pulses, set high for > 10us then set low
	bitSet(PORTB, pinTrigger);
	
	delayUS(11);
	//_delay_us(11);
	
	bitClear(PORTB, pinTrigger);
	
	//wait for pinEcho to be high before counting
	while(!bitCheck(PIND, pinEcho));
	//when pinEcho goes high, start timer to measure how many ticks it is high
	TCCR1B = 1;
	
	//wait while pinEcho is high
	while(numOV1 && bitCheck(PIND, pinEcho));
	//stop timer when pinEcho goes low
	TCCR1B = 0;
	
	//store num current clock ticks within the cycle
	unsigned int tcnt1 = TCNT1;
	
	float x; //range
	
	if (numOV1 == 0){
		x = 999;
	}
	else{
		x = ((numOV1max-numOV1)*65536.0 + tcnt1) / 16.0e6 * 343.0/2.0 * 100; // range in centimetres
	}
	
	//return registers back to whatever they were
	TIMSK1 = timsk1;
	SREG = sreg;
	TCCR1B = tccr1b;
	
	return(x);
}

void initUART(int ubbr){
	UBRR0 = ubbr;
	
	UCSR0B |= (1<<TXEN0);
	
	UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);
	
	
}

void transmitStringUSART(char* pdata){
	
	
	// loop while current char isnt stop char and end of string hasnt been reached
	while(*pdata != '\0'){
		transmitByteUSART(*pdata);
		pdata++;
	
	}
}

void transmitByteUSART(char c){
	while(!bitCheck(UCSR0A, UDRE0));
	UDR0 = c;
	
}

void delayUS(float t){
	cli();
	char TCCR0A_old = TCCR0A;
	char TCCR0B_old = TCCR0B;
	char TIMSK0C_old = TIMSK0;
	
	TCCR0A = 0;
	TCCR0B = 0;
	TIMSK0 = 0;
	
	bitSet(TIMSK0, TOIE0);
	
	unsigned long tmp = 16.0/256. *t;
	
	numOV = tmp;
	
	TCNT0 = 0;
	
	sei();
	
	bitSet(TCCR0B, CS00);
	
	while(numOV);
	
	bitClear(TCCR0B, CS00);
	
	tmp = 16.0*t - tmp*256;
	
	if(tmp){
		TCNT0 = 256 - tmp;
		numOV = 1;
		
		bitSet(TCCR0B, CS00);
		
		while(numOV);
	}
	cli();
	
	TCCR0A = TCCR0A_old;
	TCCR0B = TCCR0B_old;
	TIMSK0 = TIMSK0C_old;
}