/*
 * Lab 8 Slave.c
 *
 * Created: 21/04/2023 12:50:59 PM
 * Author : asine
 */ 

#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "util/delay.h"
#include "bitFunctions.h"

#define DDR_SPI DDRB
#define pinMISO PINB4

void transmitStringUSART(char* pdata);
void recieveCharUSART(void);
void transmitByteUSART(char c);
void initUSART(int ubbr);
void initSPIslave(void);

volatile unsigned char spi_data_rx = 0;


ISR(SPI_STC_vect){
	
	spi_data_rx = SPDR;
	
}

int main(void)
{
    float tempC = 0;
    while (1) 
    {
		tempC = (float) spi_data_rx / 10.0 + 15.0;
		char tempC0[15];
		dtostrf(tempC, 10, 3, tempC);
		tempc0[10] = "\0";
		transmitStringUSART(tempC0);
		transmitStringUSART("\r\n");
		 
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

void initSPIslave(void){
	// set MISO as output
	bitSet(DDR_SPI, pinMISO);
	
	// set this board as slave and enable SPI
	bitSet(SPCR, SPE);
	bitSet(SPCR, SPIE);
	bitClear(SPCR, MSTR);
	sei();
}