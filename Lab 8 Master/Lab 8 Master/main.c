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

#define BAUD 9600
#define MY_UBBR (F_CPU/16/BAUD - 1)

#define DDR_SPI DDRB
#define pinSCK PINB5
#define pinMISO PINB4
#define pinMOSI PINB3
#define pinSS PINB2


void transmitStringUSART(char* x);
void recieveCharUSART(void);
void transmitByteUSART(char x);
void initUSART(int ubbr);
void initADC(void);
void initSPImaster(void);


uint16_t tempReading = 0;
volatile unsigned char spi_tx_data = 0;
volatile unsigned char spi_rx_data = 0;



int main(void)
{
    initUSART(MY_UBBR);
    initADC();
	initSPImaster();
    
	while (1) 
    {
		while(!(ADCSRA & (1<<ADIF)));
		
		bitSet(ADCSRA, ADIF);
		
		{
		tempReading = ADC;
		
		double tempK = log(10000.0 * ((1023.0 / (float)tempReading - 1.0)));
		tempK = 1.0 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK )) * tempK ); // Kelvin
		float tempC = (tempK - 273.15); //
		float tempF = (tempC * 9.0)/ 5.0 + 32.0;

		
		char tempC0[15];
		dtostrf(tempC, 10, 3, tempC0);
		
		tempC0[10] = "\0";
		transmitStringUSART(tempC0);
		transmitStringUSART("\r\n");
		
		// times float by 10 to preserve 1 decimal place when casting to char, slave side will divide by 10
		char temp = (char) ((tempC - 15.0) *10.0);
		spi_tx_data = temp;
	
		
		bitClear(PORTB, pinSS);
		SPDR = spi_tx_data;
		// poll transmission complete flag SPIF
		while(!(SPSR & (1<<SPIF)));
		
		// read bus data to set SPIF low
		spi_rx_data = SPDR;
		
		// signal transmission end
		bitSet(PORTB, pinSS);
		
		}
	}
		
		
			
		
		
		
}


void initSPImaster(void){
	// set SS, clock and master out pins as outputs
	bitSet(DDR_SPI, pinSS);
	bitSet(DDR_SPI, pinSCK);
	bitSet(DDR_SPI, pinMOSI);
	
	// drive SS pin high
	bitSet(PORTB, pinSS);
	
	// enable SPI and set this board as master
	bitSet(SPCR, SPE);
	bitSet(SPCR, MSTR);
	
	//bitSet(SPCR, SPIE);
	//sei();
	// set up MISO pin as input
	//bitClear(DDR_SPI, pinMISO);
	
	
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
	
	
	// set to free running mode
	bitSet(ADCSRA, ADATE);
	bitClear(ADCSRB, ADTS0);
	bitClear(ADCSRB, ADTS1);
	bitClear(ADCSRB, ADTS2);
	
	// ADC interrupt enable
	//bitSet(ADCSRA, ADIE);
	//sei();
	
	// enable ADC
	bitSet(ADCSRA, ADEN);
	bitSet(ADCSRA, ADSC);
	
	
}