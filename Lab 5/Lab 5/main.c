/*
 * Lab 5.c
 *
 * Created: 23/03/2023 3:45:54 PM
 * Author : asine
 */ 

#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define BAUD 9600
#define MY_UBBR F_CPU/BAUD/16 - 1


int main(void)
{
    UBRR0 = MY_UBBR;
	
	UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
	
	UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);
    
	unsigned char tmp = 0;
	while (1) 
    { 
		while(!(UCSR0A & (1<<RXC0)));
		tmp = UDR0;
		while(!(UCSR0A & (1<<UDRE0)));
		UDR0 = tmp + 1;
	}
}


