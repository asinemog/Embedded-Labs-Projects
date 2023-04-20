/*
 * projnew.c
 *
 * Created: 30/03/2023 1:53:17 AM
 * Author : asine
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdbool.h>

#include "bitFunctions.h"
#include "util/delay.h"


#define pinTrigger PIND6
#define pinEcho PIND7
#define pinAlarmButton PIND4
#define pinVolumeButton PIND5
#define maxPwmTimeUS 1000000

unsigned long numOv;
unsigned long numOv1;

// bool state for controlling alarm, default off
int alarmIsOn = 0;
// bool volume mode state
int volume = 0;

ISR(TIMER0_OVF_vect){
	numOv--;
}

ISR(TIMER1_OVF_vect){
	numOv--;
}

ISR(TIMER2_COMPA_vect){
	
}


void delayUS(float t){
	
	cli();
	char TCCR0A_old = TCCR0A;
	char TCCR0B_old = TCCR0B;
	char TIMSK0_old = TIMSK0;
	char SREG_old = SREG;
	
	// num overflows = Time desired/(P/F_CPU)/MAX
	// numOV is long, typecast to long rounds down (floor)
	numOv = t*16.0/256.0;
	
	//calc remaining fraction of overflows
	float tmp = t*16.0/256.0 - numOv;
	
	unsigned long remain = (1 - tmp)*256;
	bitSet(TIMSK0, TOIE0);
	TCCR0A = 0;
	TCCR0B = 0;
	
	if(numOv){
		TCNT0 = 0;
		sei();
		TCCR0B |= (1<<CS00);
		while(numOv);
		TCCR0B &= ~(1<<CS00);
		cli();
	}
	
	if(tmp > 0.0){
		//set count to remain to make the rest of the fractional time up
		TCNT0 = remain;
		numOv = 1;
		sei();
		TCCR0B |= (1<<CS00);
		while(numOv > 0);
		TCCR0B &= ~(1<<CS00);
		cli();
	}
	cli();
	TCCR0A = TCCR0A_old;
	TCCR0B = TCCR0B_old;
	TIMSK0 = TIMSK0_old;
	SREG = SREG_old;
}

int alarmButton(int alarmState){
	
	int alarmButtonStatus = bitCheck(PIND, pinAlarmButton);
	//debounce the bouncing signal
	//delayUS(20e3);
	if(!alarmButtonStatus){
		delayUS(20e3);
		//check if still pressed
		alarmButtonStatus = bitCheck(PIND, pinAlarmButton);
		if(!alarmButtonStatus){
		
			//toggle the alarm bool state
			if(alarmState == 1){
				alarmState = 0;
				}else{
				alarmState = 1;
			}
			//wait for unpress
			while(!alarmButtonStatus){
				alarmButtonStatus = bitCheck(PIND, pinAlarmButton);
			}
			// debounce release
			//delayUS(20e3);
			delayUS(20e3);
	}
}
	
	return(alarmState);
}

float ultraSonic(void){
	// save previous values of registers we are using
	
	
	char timsk1 = TIMSK1;
	char tccr1b = TCCR1B;
	char sreg = SREG;
	
	// reset registers
	TCCR1B = 0;
	TIMSK1 = 0;
	SREG = 0;
	
	// enabled timer overflow interrupt
	bitSet(TIMSK1, TOIE1);
	sei();
	
	char numOv1max = 6;
	numOv1 = numOv1max;
	TCNT1 = 0;
	
	bitClear(PORTD, pinTrigger);
	
	// to trigger pulses, set high for > 10us then set low
	bitSet(PORTD, pinTrigger);
	
	delayUS(11);
	//_delay_us(11);
	
	bitClear(PORTD, pinTrigger);
	
	//wait for pinEcho to be high before counting
	while(!bitCheck(PIND, pinEcho));
	//when pinEcho goes high, start timer to measure how many ticks it is high
	sei();
	bitSet(TCCR1B, CS10);
	
	//wait while pinEcho is high
	while(numOv1 && bitCheck(PIND, pinEcho));
	//stop timer when pinEcho goes low
	bitClear(TCCR1B, CS10);
	
	//store num current clock ticks within the cycle
	unsigned int tcnt1 = TCNT1;
	
	float x; //range
	
	//if (numOv1 == 0){
	//	x = 999;
	//	}
	//	else{
	// speed = distance/time so distance = speed * time
	// speed = speed of sound, time is time take by timer 1 to count how long echo pin was high
	// time = number of ticks taken / no ticks per second (F_CPU/P) in seconds.
	// no of ticks taken = no. overflows + current ticks
	x = ((numOv1max-numOv1)*65536.0 + tcnt1) / 16.0e6 * 343.0/2.0 * 100.0; // range in centimetres
	//	}
	
	//return registers back to whatever they were
	TIMSK1 = timsk1;
	SREG = sreg;
	TCCR1B = tccr1b;
	
	return(x);
}


// using int as boolean value
int volumeButton(int volume){
	
	int volumeButtonStatus = bitCheck(PIND, pinVolumeButton);
	
	
	delayUS(20e3);
	
	// check if still pressed
	if(!volumeButtonStatus){
		// cycle through volume modes on each press
		switch(volume){
			case 0:
				volume = 1;
				break;
			
			case 1:
				volume = 2;
				break;
			
			case 2:
				volume = 0;
				break;
			
			default:
				volume = 0;
				break;
		}
		
		while(!volumeButtonStatus){
			
			volumeButtonStatus = bitCheck(PIND, pinVolumeButton);
			
		}
		delayUS(20e3);
	}
	
	return(volume);
	
}

void a4Init(void){
	
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
	
	//duty cycle loaded in OCR2B is a percentage of OCR2
	
	// select prescaler = 256
	bitSet(TCCR2B, CS21);
	bitSet(TCCR2B, CS22);
	
}

int main(void){
    
	//set up alarm button as input with pullup resistor
	bitClear(DDRD, pinAlarmButton);
	bitSet(PORTD, pinAlarmButton);
	
	//set up volume button as input with pullup resistor
	bitClear(DDRD, pinVolumeButton);
	bitSet(PORTD, pinVolumeButton);
	
	//set up trigger pin as output for ultrasonic modules
	bitSet(DDRD, pinTrigger);
	
	//set up echo pin as input from ultrasonic module with pullup resistor
	bitClear(DDRD, pinEcho);
	bitSet(PORTD, pinEcho);
	
	//initialise the Fast PWM settings for timer 2 to output 440hz
	a4Init();
	
	
	//default duty cycle is 20%
	float duty = 0.2;
    
	float scaledDelay;
	
	int i = 0;
	
	while (1) 
    {
		alarmIsOn = alarmButton(alarmIsOn);
		
		while(alarmIsOn){
			alarmIsOn = alarmButton(alarmIsOn);
			volume = volumeButton(volume);
			
			
			/*
			switch(volume){
				
				case 0:
				duty = 0.2;
				break;
				
				case 1;
				duty = 0.4;
				break;
				
				case 2:
				duty = 0.8;
				break;
				
				default:
				0.2;
				break;
			}
			
			
			
			*/
			
			if(volume == 0){
				duty = 0.2;
				}else if(volume == 1){
				duty = 0.4;
				}else if(volume == 2){
				duty = 0.8;
				}else{
				duty = 0.2;
			}
			// set duty cycle
			OCR2B = duty*142 - 1;
			
			//calc buzzerON
			scaledDelay = ultraSonic()*(1000-750)/400 + 75;
			
			//enable PWM 440Hz, P = 256
			bitSet(TCCR2B, CS21);
			bitSet(TCCR2B, CS22);
			bitSet(DDRD, PIND3);
			
			//delay based on scalar, poll alarm button
			i = 0;
			while(i < scaledDelay && alarmIsOn){
				
				delayUS(1e3);
				alarmIsOn = alarmButton(alarmIsOn);
				i++;
			}
			
			//recalculate in between to make change in buzzer speed smoother
			scaledDelay = ultraSonic()*(1000-10)/400 + 10;
			
			//disable PWM
			bitClear(TCCR2B, CS21);
			bitClear(TCCR2B, CS22);
			bitClear(DDRD, PIND3);
			
			//delay based on scalar, poll alarm button 
				i = 0;
				while(i < scaledDelay && alarmIsOn){
					
					delayUS(1e3);
					
					alarmIsOn = alarmButton(alarmIsOn);
					i++;
				}
			
			
			
		}
			//disable PWM
			bitClear(TCCR2B, CS21);
			bitClear(TCCR2B, CS22);
			bitClear(DDRD, PIND3);
		
		
		
		
		
    }
}

