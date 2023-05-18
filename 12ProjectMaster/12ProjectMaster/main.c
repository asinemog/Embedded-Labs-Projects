/*
 * 12ProjectMaster.c
 *
 * Created: 18/05/2023 3:08:05 PM
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

float myMap(float x, float x1, float x2, float y1, float y2);
void transmitStringUSART(char* x);
void recieveCharUSART(void);
void transmitByteUSART(char x);
void initUSART(int ubbr);
void initADC(void);
void initSPImaster(void);
float processADC(void);
void initTimer0(void);


volatile uint16_t tempReading;

volatile uint8_t readFlag = 0;



ISR(ADC_vect){
	// read adc and set flag for reading
	tempReading = ADC;
	readFlag = 1;
	
	
}


int main(void)
{
    
	float tempC = 0.0;
	float duty = 0.8;
	uint16_t testReading = 600;
	uint8_t tempr1 = 0;
	
 	initADC();
 	initUSART(MY_UBBR);
	initTimer0();
	sei();

	OCR2B = 156*duty;
	
    while (1) 
    {
		if(readFlag){
			
			tempC = processADC();
			
			
		}
		if(tempReading < 400){
			tempr1 = 0;
		}else if(tempReading > 600){
			tempr1 = 200;
		}else{
			tempr1 = tempReading - 400;
		}
		
		duty = myMap(tempr1, 0, 200, 0, 1);
		uint8_t ocr2b = duty * 156;
		OCR2B = ocr2b;
		
    }
}


float myMap(float x, float x1, float x2, float y1, float y2){
	return (y2-y1)/(x2-x1)*(x-x1) + y1;
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
	bitSet(ADCSRA, ADIE);
	//sei();
	
	// enable ADC
	bitSet(ADCSRA, ADEN);
	bitSet(ADCSRA, ADSC);
	
	
}

float processADC(void){
	// do processing
	cli();
	readFlag = 0;
	
	char tempADC[15];
	itoa(tempReading, tempADC, 10);
	tempADC[10] = '\0';
	transmitStringUSART("ADC reading is: ");
	transmitStringUSART(tempADC);
	
	double tempK = log(10000.0 * ((1023.0 / (float)tempReading - 1.0)));
	tempK = 1.0 / (0.001129148 + (0.000234125 + (0.0000000876741 * tempK * tempK )) * tempK ); // Kelvin
	float tempC = (tempK - 273.15); //
	float tempF = (tempC * 9.0)/ 5.0 + 32.0;
	
	char tempC0[15];
	dtostrf(tempC, 10, 3, tempC0);
	tempC0[10] = '\0';
	transmitStringUSART(". Temperature calc is: ");
	transmitStringUSART(tempC0);
	transmitStringUSART("\r\n");
	
	// change duty cycle based on 
	//duty = myMap(tempReading, 0, 1023, 0.4, 1);
	//OCR2B = duty * 156;
	
	sei();
	
	return tempC;
	
}

void initTimer0(void){
	
	DDRD = 0xFF;
	
	//set fast PWM mode
	bitSet(TCCR2A, WGM20);
	bitSet(TCCR2A, WGM21);
	bitSet(TCCR2B, WGM22);
	
	// set clear OC2B on compare match, set at BOT
	bitSet(TCCR2A, COM2B1);
	
	// TOP = FCPU/(FPWM*P) - 1
	// TOP = 16mHz/(100*P) - 1
	// only valid P is 1024 for max = 255 (timer 2)
	// where TOP = 16e6/(100*1024) - 1 = 155.25 
	OCR2A = 156;
	
	//OCR2B = 78;
	// start counting with prescaler = 1024
	bitSet(TCCR2B, CS20);
	bitSet(TCCR2B, CS21);
	bitSet(TCCR2B, CS22);
	
	
	
	
}