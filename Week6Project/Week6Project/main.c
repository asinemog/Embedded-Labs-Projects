/*
 * Week6Project.c
 *
 * Created: 21/03/2023 10:24:20 PM
 * Author : asine
 */ 
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



ISR(TIMER0_OVF_vect){
	numOv--;
}

int main(void){
	// init DDR etc
	float range;
	while(1){
		
		range = ultraSonic(pinTrigger, pinEcho);
	
	}
	
}

