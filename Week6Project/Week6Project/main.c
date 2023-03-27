/*
 * Week6Project.c
 *
 * Created: 21/03/2023 10:24:20 PM
 * Author : asine
 */ 
#define F_CPU 16000000UL


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "bitFunctions.h"
#include "delayTimer0.h"
#include "ultraSonic.h"
#include "pwma4.h"

#define pinTrigger PINB4
#define pinEcho PIND4
#define pinAlarmButton PIND1
#define pinVolumeButton PIND2
#define maxPwmTimeUS 1000000

unsigned long numOv;
unsigned long numOv1;
unsigned long numCmp;

void calcBuzzerSpeed(void);


ISR(TIMER0_OVF_vect){
	numOv--;
}

ISR(TIMER1_OVF_vect){
	numOv1--;
}

ISR(TIMER0_COMPA_vect){
	numCmp--;
}

int main(void){
	
	//char alarmButtonStatusOld = 1;
	char alarmButtonStatus;
	//char volButtonStatusOld = 1;
	char volButtonStatus;
	
	char alarmIsOn = 0;
	
	char volume = 0;
	float duty = 0.3;
	float range;
	float maxRange = 300;
	float threshRange = 150;
	float buzzerSpeed;
	// set buttons as inputs
	bitClear(DDRD, pinAlarmButton);
	bitClear(DDRD, pinVolumeButton);
	
	//configure pullup resistors
	bitSet(PORTD, pinAlarmButton);
	bitSet(PORTD, pinVolumeButton);
	
	//initialise timer2 PWM
	//a4Init(duty);

	//
	
	while(1){
		
		alarmButtonStatus = bitCheck(PIND, pinAlarmButton);
		
		if(alarmButtonStatus != 1){
			
			delay50ms();
			alarmButtonStatus = bitCheck(PIND, pinAlarmButton);
			
			if(alarmButtonStatus != 1){
				alarmIsOn = !alarmIsOn;
			}
			
		}
		
		if(alarmIsOn){
			
			volButtonStatus = bitCheck(PIND, pinVolumeButton);
			
			if(volButtonStatus != 1){
				
				delay50ms();
				volButtonStatus = bitCheck(PIND, pinVolumeButton);
				
				if(volButtonStatus != 1){
					// cycle through volume options with each button press
					if(volume == 2){
						volume = 0;
					}
					else{
						volume++;
					}
					
				}
								
			}
			
			switch(volume){
				
				case 0:
					duty = 0.3;
					break;
				case 1:
					duty = 0.6;
					break;
				case 2:
					duty = 1.0;
					break;
				default:
					volume = 0;
					duty = 0.3;
					break;				
			}
			
			calcBuzzerSpeed();
					
			if(buzzerSpeed > 0 && buzzerSpeed <=1.0){
				
				a4Start(duty);
				delayUS(maxPwmTimeUS*buzzerSpeed);
				
				calcBuzzerSpeed();
				a4Stop();
				
				if(buzzerSpeed > 0 && buzzerSpeed <=1.0){
					delayUS(maxPwmTimeUS*buzzerSpeed);	
				}
				
			}	
						
		}
	
	}
	
}

void calcBuzzerSpeed(void){
	range = ultraSonic(pinTrigger, pinEcho);
	
	if(range <= threshRange && range > 0){
		buzzerSpeed = range/threshRange;
	}
	else if(range > threshRange && range < maxRange){
		buzzerSpeed = 1.0;
	}
	else{
		buzzerSpeed = 0.0;
	}
}