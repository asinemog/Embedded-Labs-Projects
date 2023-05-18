/*
 * Lab 11 DHT11.c
 *
 * Created: 12/05/2023 11:52:25 AM
 * Author : asine
 */ 

#define F_CPU 16000000UL
#define BAUD 9600
#define MY_UBBR (F_CPU/16/BAUD - 1)

#include <avr/io.h>
#include <stdlib.h>
#include "util/delay.h"
#include "bitFunctions.h"

#define DATAPIN PIND4
#define DATAPORT PORTD
#define DATAINPUT PIND


void transmitStringUSART(char* x);
void transmitByteUSART(char x);
void initUSART(int ubbr);
unsigned char recieveByte(void);


int main(void)
{
    uint8_t intRH, decRH, intTemp, decTemp, checkSum;
	initUSART(MY_UBBR);
	char data[9];
	
	
    while (1) 
    {
		// signal transmission start, wait >18ms
		DDRD = 0xFF;
		bitClear(DATAPORT, DATAPIN);
		_delay_ms(19);
		// pull back up, wait for 40us
		bitSet(DATAPORT, DATAPIN);
		_delay_us(40);
		
		// after the 40us, a response will be made so change data direction to 
		// listen to the response.
		DDRD = 0x00;
		// response will be low for 80us, then high for 80us
		while(bitCheck(DATAINPUT, DATAPIN));
		while(!bitCheck(DATAINPUT, DATAPIN));
		while(bitCheck(DATAINPUT, DATAPIN));

		// begin reading data
		intRH = recieveByte();
		decRH = recieveByte();
		intTemp = recieveByte();
		decTemp = recieveByte();
		checkSum = recieveByte();
		// check if data is correct
		uint16_t sum = intRH + decRH + intTemp + decTemp;
		//if((char)(sum & 0x0F) == checkSum){
			// send RH over USART
			transmitStringUSART("RH is: ");
			itoa(intRH, data, 10);
			data[8] = '\0';
			transmitStringUSART(data);
			
			transmitStringUSART(".");
			
			itoa(decRH, data, 10);
			data[8] = '\0';
			transmitStringUSART(data);
			transmitStringUSART("%.");
			transmitStringUSART("\r\n");
			
			// send Temperature over USART
			transmitStringUSART("Temp is: ");
			itoa(intTemp, data, 10);
			data[5] = '\0';
			transmitStringUSART(data);
			
			transmitStringUSART(".");
			
			itoa(decTemp, data, 10);
			data[5] = '\0';
			transmitStringUSART(data);
			transmitStringUSART(" C.");
			transmitStringUSART("\r\n");
			
			
		
			
			_delay_ms(2000);
			
			
			
			
			
		//}
    }
	
}


uint8_t recieveByte(void){
	
	// 8 bits in a byte
	unsigned char x = 0;
	for(int i = 0; i < 8; i++){
		
		// wait while data pin is low (should be for 50us)
		while(!bitCheck(DATAINPUT, DATAPIN));
		// wait for more than 28us. if the pin is still high, its a 1 else its 0
		// according to the DHT11 data sheet.
		_delay_us(29);
		if(bitCheck(DATAINPUT, DATAPIN)){
			// if its a 1, put a 1 in the current bit
			x = (x << 1) | 1;
			
		}else{
			// if its a 0, just shift to the next bit
			x = (x << 1);
		}
		// wait for the rest of the high pulse in the case that a 1 is recieved
		while(bitCheck(DATAINPUT, DATAPIN));
		
		
	}
	return x;
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
