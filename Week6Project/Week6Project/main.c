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

#define pinTrigger PIND4
#define pinEcho PIND5
#define pinAlarmButton PIND6
#define pinVolumeButton PIND7
#define maxPwmTimeUS 1000000

unsigned long numOv;
unsigned long numOv1;
unsigned long numCmp;

float calcBuzzerSpeed(void);


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
	
	char alarmButtonStatusOld = 1;
	char alarmButtonStatus;
	char volButtonStatusOld = 1;
	char volButtonStatus;
	
	char alarmIsOn = 0;
	
	char volume = 0;
	float duty = 0.3;
	float buzzerSpeed = 0.0;
	// set buttons as inputs
	bitClear(DDRD, pinAlarmButton);
	bitClear(DDRD, pinVolumeButton);
	
	// set pinTrigger as output and pinEcho as input
	bitSet(DDRD, pinTrigger);
	bitClear(DDRD, pinEcho);
	
	//configure pullup resistors
	bitSet(PORTD, pinAlarmButton);
	bitSet(PORTD, pinVolumeButton);
	
	
	
	
	//initialise timer2 PWM
	a4Init(duty);

	//
	
	while(1){
		
		alarmButtonStatus = bitCheck(PIND, pinAlarmButton);
		
		if(alarmButtonStatus != alarmButtonStatusOld){
			
			delay50ms();
			alarmButtonStatus = bitCheck(PIND, pinAlarmButton);
			
			if(alarmButtonStatus != alarmButtonStatusOld){
				alarmIsOn = !alarmIsOn;
				alarmButtonStatus = alarmButtonStatusOld;
			}
			
		}
		
		if(alarmIsOn){
			
			volButtonStatus = bitCheck(PIND, pinVolumeButton);
			
			if(volButtonStatus != volButtonStatusOld){
				
				delay50ms();
				volButtonStatus = bitCheck(PIND, pinVolumeButton);
				
				if(volButtonStatus != volButtonStatusOld){
					// cycle through volume options with each button press
					if(volume == 2){
						volume = 0;
					}
					else{
						volume++;
					}
					volButtonStatusOld = volButtonStatus;
				}
								
			}
			
			switch(volume){
				
				case 0:
					//
					duty = 0.4;
					break;
				case 1:
					//
					duty = 0.7;
					break;
				case 2:
					duty = 0.9;
					break;
				default:
					volume = 0;
					duty = 0.3;
					break;				
			}
			
			buzzerSpeed = calcBuzzerSpeed();
					
			if(buzzerSpeed > 0 && buzzerSpeed <=1.0){
				
				a4Start(duty);
				delayUS(maxPwmTimeUS*buzzerSpeed);
				
				buzzerSpeed = calcBuzzerSpeed();
				a4Stop();
				
				if(buzzerSpeed > 0 && buzzerSpeed <=1.0){
					delayUS(maxPwmTimeUS*buzzerSpeed);	
				}
				
			}	
						
		}
	
	}
	
}

float calcBuzzerSpeed(void){
	float range = ultraSonic(pinTrigger, pinEcho);
	float maxRange = 300;
	float threshRange = 150;
	float buzzerSpeed = 0.0;
	
	if(range <= threshRange && range > 0){
		buzzerSpeed = range/threshRange;
	}
	else if(range > threshRange && range < maxRange){
		buzzerSpeed = 1.0;
	}
	else{
		buzzerSpeed = 0.0;
	}
	
	return(buzzerSpeed);
	
}