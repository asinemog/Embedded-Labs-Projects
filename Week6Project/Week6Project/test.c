
#define F_CPU 16000000UL


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "ultraSonic.h"

#define bitSet(reg, ind) (reg |= 1 << ind)

#define bitClear(reg, ind) (reg &= ~(1 << ind) )


//e.g. PIND = 0b00010100,
// reg = PIND = 0b00010100
// ind = PIND4 = 4
// PIND >> PIND4 & 1 = 0b00010100 >> 4 & 1 = 0b00000001 & 0b00000001 = 1/true
//
#define bitCheck(reg, ind) (reg >> ind & 1)


#define bitToggle(reg, ind) (reg ^= 1 << ind)





unsigned long numOv1;
unsigned long numOv;



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

#define pinTrigger PORTD4
#define pinEcho PORTD5

#define BAUD 9600
#define MY_UBBR (F_CPU/16/BAUD - 1)


int main(void){
	
	initUSART(MY_UBBR);
	
	bitSet(DDRD, pinTrigger);
	bitClear(DDRD, pinEcho);
	bitSet(PORTD, pinEcho);
	
	float range;
	char rangeStr[15];
	
	char alarmIsOn = 0;
	
	while(1){
		
		
		
		if(alarmIsOn){
			range = ultraSonic(pinTrigger, pinEcho);
			dtostrf(range, 10, 3, rangeStr);
			rangeStr[10] = '\0';
			
			transmitStringUSART(rangeStr);
			transmitStringUSART("\r\n");
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