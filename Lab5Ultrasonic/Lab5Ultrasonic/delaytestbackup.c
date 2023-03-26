void delayUS(float t){
	
	
	char TCCR0A_old = TCCR0A;
	char TCCR0B_old = TCCR0B;
	char TIMSK0_old = TIMSK0;
	char SREG_old = SREG;
	
	
	numOv = t*16.0/256.0;
	float tmp = t*16.0/256.0 - numOv; 
	unsigned long remain = (1 - tmp)*256;
	TIMSK0 = 0x01;
	
	if(numOv){
		TCNT0 = 0;
		sei();
		TCCR0B |= (1<<CS00);
		while(numOv);
		TCCR0B &= ~(1<<CS00);
		cli();
	}
	
	if(tmp > 0.0){
		TCNT0 = remain;
		numOv = 1;
		sei();
		TCCR0B |= (1<<CS00);
		while(numOv > 0);
		TCCR0B &= ~(1<<CS00);
		cli();
	}
	
	TCCR0A = TCCR0A_old;
	TCCR0B = TCCR0B_old;
	TIMSK0 = TIMSK0_old;
	SREG = SREG_old;
}