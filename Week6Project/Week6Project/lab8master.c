/*
 * Lab 7 ADC.c
 *
 * Created: 14/04/2023 4:15:35 PM
 * Author : asine
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#define F_CPU 16000000UL

#define BAUD 9600
#define MY_UBBR (F_CPU/16/BAUD - 1)
#define TOP 124

void transmitStringUSART(char* pdata);
void recieveCharUSART(void);
void transmitByteUSART(char c);
void initUSART(int ubbr);
void initADC(void);


#define bitSet(reg, ind) (reg |= 1 << ind)
#define bitClear(reg, ind) (reg &= ~(1 << ind) )

//e.g. PIND = 0b00010100,
// reg = PIND = 0b00010100
// ind = PIND4 = 4
// PIND >> PIND4 & 1 = 0b00010100 >> 4 & 1 = 0b00000001 & 0b00000001 = 1/true
//
#define bitCheck(reg, ind) (reg >> ind & 1)
#define bitToggle(reg, ind) (reg ^= 1 << ind)

int main(void)
{
	initUSART(MY_UBBR);
	initADC();
	
	DDRD = 0xFF;
	bitSet(TCCR0A, WGM00);
	bitSet(TCCR0A, WGM01);
	bitSet(TCCR0B, WGM02);
	

	bitSet(TCCR0A, COM0B1);
	
	OCR0A = 124;
	//OCR0B = 124;
	
	
	bitSet(TCCR0B, CS02);
    /* Replace with your application code */
    while (1) 
    {
		
		bitSet(ADCSRA, ADSC);

		while(ADCSRA & (1<<ADSC));
		uint16_t result = ADC;
		float voltage = ((float) result / 1024.0) * 5.0;
		float resistance = 1000*(5.0 -voltage)/voltage;
	
		char voltagea0[15];	
		dtostrf(resistance, 10, 3, voltagea0);
		voltagea0[10] = "\0";
		transmitStringUSART(voltagea0);
		transmitStringUSART("\r\n");
		
		float duty =  resistance/13000.0;
		OCR0B = 124.0*duty - 1.0;
		
		
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

