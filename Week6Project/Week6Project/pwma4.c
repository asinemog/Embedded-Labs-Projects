/*
 * pwma4.c
 *
 * Created: 28/03/2023 5:25:04 AM
 *  Author: asine
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "bitFunctions.h"

#define F_CPU 16000000UL

void a4Init(float duty){
	
	//set fast PWM mode
	bitSet(TCCR2A, WGM20);
	bitSet(TCCR2A, WGM21);
	bitSet(TCCR2B, WGM22);
	
	// set clear OC2B on compare match, set at BOT
	bitSet(TCCR2A, COM2B1);
	
	// TOP = FCPU/(FPWM*P) - 1
	// TOP = 16mHz/(440*P) - 1
	// valid P for TOP - 1 <= 256(max) are P = 256, P = 1024
	//                                 TOP = 141.00, TOP =  34.51   
	// choose lowest P for accuracy, P = 256, TOP = 141 = OCR2A
	
	OCR2A = 141;
	
	//duty cycle loaded in OCR2B is a percentage of OCR2A
	OCR2B = 141*duty;
	
	// select prescaler = 256
	bitSet(TCCR2B, CS21);
	bitSet(TCCR2B, CS22);
	
}

void a4Start(float duty){
	
	OCR2B = 141*duty;
	bitSet(DDRD, PIND3);
	

}

void a4Stop(void){
	
	bitClear(DDRD, PIND3);
	
}
