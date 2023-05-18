/*
 * Lab 9 AC + IC.c
 *
 * Created: 28/04/2023 11:09:17 AM
 * Author : asine
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "bitFunctions.h"
#include "util/delay.h"

#define MAX 256
#define BAUD 9600
#define MY_UBBR (F_CPU/16/BAUD - 1)


void transmitStringUSART(char* x);
void recieveCharUSART(void);
void transmitByteUSART(char x);
void initUSART(int ubbr);



unsigned int numOv = 0;
unsigned char flag_rising = 1;
unsigned char flag_ready = 0;
volatile unsigned int tcnt1_new = 0;
volatile unsigned int tcnt1_old = 0;


float t1 = 0;
float t2 = 0;

char ici_cnt = 0;
char testflag = 0;
volatile float tLow = 0.0;
volatile float tHigh = 0.0;

ISR(TIMER1_OVF_vect){
	cli();
	numOv++;
	sei();
}

ISR(TIMER1_CAPT_vect){
	//testflag++;
	if(flag_rising){
		
		tcnt1_new = ICR1;
		bitClear(TCCR1B, ICES1);
		
		t1 =  (((float) tcnt1_new + 1.0) + ((float)(numOv))*65536.0) / (float) 16e6 * 1000.0;
		
		tLow = t1-t2;
		
		flag_rising = 0;
		
		
	} else{
		
		tcnt1_new = ICR1;
		bitSet(TCCR1B, ICES1);
		
		t2 = (((float) tcnt1_new + 1.0) + ((float)(numOv))*65536.0) / (float) 16e6 * 1000.0;
		tHigh = - t1 + t2;
		
		flag_rising = 1;
		
		ici_cnt++;
		
		if(ici_cnt == 3){
			
			flag_ready = 1;
			ici_cnt = 0;
			bitClear(TIMSK1, ICIE1);
			
		}
		
		
	}
	
	
}

void initADC_SC(void){
	// REF = AVCC
	bitSet(ADMUX, REFS0);
	//Prescaler to 128 (50-200KHz range required)
	bitSet(ADCSRA, ADPS0);
	bitSet(ADCSRA, ADPS1);
	bitSet(ADCSRA, ADPS2);
	// enable by default
	bitSet(ADCSRA, ADEN);
	//bitSet(ADCSRA, ADSC);
	//bitSet(ADMUX, MUX0);
	
}

void initTC2_PWM(void){
	// Fast PWM TOP Mode
	bitSet(TCCR2A, WGM20);
	bitSet(TCCR2A, WGM21);
	bitSet(TCCR2B, WGM22);
	
	// clear OC2B on compare match, set at BOT
	bitClear(TCCR2A, COM2B0);
	bitSet(TCCR2A, COM2B1);
	// TOP = FCPU/(FPWM*P) - 1
	// TOP + 1 = FCPU/(FPWM*P)
	// FPWM*P = FCPU/(TOP+1)
	// FPWM = FCPU/(P*(TOP+1))
	// TOP will vary from 0 to 255 based on ADC, 
	// so FPWM will vary from FCPU/(P*1) to CPU/(P*256)
	// based on the potentiometer
	// Setting Prescaler P = 1024 gives us a range of
	// 61Hz - 15.625 KHz
	
	
	
	// default TOP = MAX
	OCR2A = 100 - 1;
	// default duty cycle
	OCR2B = 100*0.9 - 1;
	
	// Prescaler to 1024
	bitSet(TCCR2B, CS20);
	bitSet(TCCR2B, CS21);
	bitSet(TCCR2B, CS22);
	
}

void initTC1_IC(void){
	
	// set normal mode
	bitClear(TCCR1A, WGM10);
	bitClear(TCCR1A, WGM11);
	bitClear(TCCR1B, WGM12);
	bitClear(TCCR1B, WGM13);
	
	
	// enable overflow interrupts
	bitSet(TIMSK1, TOIE1);
	
	// enable input capture based on AC 
	bitSet(ACSR, ACIC);
	
	// enable input capture interrupt
	bitSet(TIMSK1, ICIE1);
	
	//start with falling edge detect
	bitClear(TCCR1B, ICES1);
	
	
	// prescaler = 1
	bitSet(TCCR1B, CS10);
	

	
}

float myMap(float x, float x1, float x2, float y1, float y2){
	
	return (y2-y1)/(x2-x1)*(x-x1) + y1;
	
}

int read_ADC(char channel){
	
	// check valid channel value

	
		// set channel
		ADMUX &= 0xF0;
		ADMUX |= channel;
		bitSet(ADMUX, REFS0);
		// start a conversion
		bitSet(ADCSRA, ADSC);
		// poll ADSC bit for conversion completion
		while(ADCSRA & (1<<ADSC));
		int result = ADC;
		
		return result;
	
}






int main(void)
{
   // DDRD = 0x00;
	
	//DDRC = 0x00;
	initADC_SC();
	initTC2_PWM();
	initUSART(MY_UBBR);
	initTC1_IC();
	
	sei();
	bitSet(DDRD, PIND3);
	bitClear(DDRD, PIND6);
	bitClear(DDRD, PIND7);
	
	float top_reading;
	float duty_reading;
	float ocr2a;
	float ocr2b;
	
	float true_freq;
	float true_duty;
	float ic_freq;
	float ic_duty;
	
    while (1) 
    {
		duty_reading = read_ADC(0);
		top_reading = read_ADC(1);
		
	
		//ocr2a = (char) (myMap(top_reading, 0, 1023, 1, 256));
		ocr2a = myMap(top_reading, 0, 1023, 15 , 155);
		float temp = myMap(duty_reading, 0, 1023, 0, 1);
		ocr2b = ocr2a * temp;
		
		
		char dutytemp[15];
		dtostrf(temp, 10, 3, dutytemp);
		dutytemp[10] = "\0";
		//transmitStringUSART(dutytemp);
		//transmitStringUSART("\r\n");
		
		// FPWM = FCPU/(P*(TOP+1))
	OCR2A = ocr2a;
	OCR2B = ocr2b;
		
	if(flag_ready){
		
		ic_freq = 1.0/(tHigh + tLow);
		ic_duty	= tHigh/(tHigh + tLow);
		true_freq = (float) 16e6 /((ocr2a+1.0)*1024.0);
		true_duty = temp;
		
		char icfreq[15];
		dtostrf(ic_freq, 10, 3, icfreq);
		icfreq[10] = '\0';
		transmitStringUSART(icfreq);
		transmitStringUSART("\r\n");
		
		
		char icduty[15];
		dtostrf(ic_duty, 10, 3, icduty);
		icduty[10] = '\0';
		transmitStringUSART(icduty);
		transmitStringUSART("\r\n");
		
		char truefreq[15];
		dtostrf(true_freq, 10, 3, truefreq);
		truefreq[10] = '\0';
		transmitStringUSART(truefreq);
		transmitStringUSART("\r\n");
		
		char trueduty[15];
		dtostrf(true_duty, 10, 3, trueduty);
		trueduty[10] = '\0';
		transmitStringUSART(trueduty);
		transmitStringUSART("\r\n");
		
	}
	float temp3 = (float) testflag;
	char testflag1[15];
	dtostrf(temp3, 10, 3, testflag1);
	testflag1[10] = '\0';
	//transmitStringUSART(testflag1);
	//transmitStringUSART("\r\n");
	_delay_ms(1000);
		
	
		
		
		
		
		
		
		
		
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

