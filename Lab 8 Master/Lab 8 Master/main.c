/*
 * Lab 8 Master.c
 *
 * Created: 21/04/2023 12:33:00 AM
 * Author : asine
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "util/delay.h"
#include "bitFunctions.h"

#define DDR_SPI DDRB
#define pinSCK PINB5
#define pinMOSI PINB4
#define pinMISO PINB3
#define pinSS PINB2


void transmitStringUSART(char* pdata);
void recieveCharUSART(void);
void transmitByteUSART(char c);
void initUSART(int ubbr);
void initADC(void);
void initSPImaster(void);



int main(void)
{
    
	
    while (1) 
    {
    }
}


void initSPImaster(void){
	// set SS, clock and master out pins as outputs
	bitSet(DDR_SPI, pinSS);
	bitSet(DDR_SPI, pinSCK);
	bitSet(DDR_SPI, pinMOSI);
	
	// enable SPI and set this board as master
	bitSet(SPCR, SPE);
	bitSet(SPCR, MSTR);
	
	// set up MISO pin as input
	bitClear(DDR_SPI, pinMISO);
	
	
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

void initADC(void){
	// Vref = AREF
	bitSet(ADMUX, REFS0);

	// ADC prescaler = 128
	bitSet(ADCSRA, ADPS0);
	bitSet(ADCSRA, ADPS1);
	bitSet(ADCSRA, ADPS2);
	
	// enable ADC
	bitSet(ADCSRA, ADEN);
}
