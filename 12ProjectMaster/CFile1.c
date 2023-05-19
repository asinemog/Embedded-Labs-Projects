
#define F_CPU 16000000Ul

#include <avr/io.h>
#include "bitFunctions.h"

int main(void){
	
	DDRD = 0xFF;
	
	//set fast PWM mode
	bitSet(TCCR0A, WGM00);
	bitSet(TCCR0A, WGM01);
	bitSet(TCCR0B, WGM02);
	
	// set clear OC2B on compare match, set at BOT
	bitSet(TCCR0A, COM0B1);
	
	// TOP = FCPU/(FPWM*P) - 1
	// TOP = 16mHz/(100*P) - 1
	// only valid P is 1024 for max = 255 (timer 2)
	// where TOP = 16e6/(100*1024) - 1 = 155.25
	OCR0A = 156;
	
	//OCR2B = 78;
	// start counting with prescaler = 1024
	bitSet(TCCR0B, CS00);
	bitSet(TCCR0B, CS01);
	bitSet(TCCR0B, CS02);
	
	OCR0B = 100;
	
	while(1){
		
		
		
	}
}