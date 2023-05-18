/*
 * Lab 10 Master.c
 *
 * Created: 7/05/2023 7:09:26 PM
 * Author : asine
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "util/delay.h"

#define SLAVE_ADDR 11


void initTWI(void);
unsigned char getTWSR(void);
unsigned char startTWI(void);
void stopTWI(void);
void initUSART9600(void);
void txByteUSART(unsigned char x);
void txStringUSART(char *x);
char rxByteUSART(void);
void rxStringUSART(char* buffer, char len);
unsigned char txTWI_SLA_W(void);
void txTWI_data_string(char *x);
void txTWI_data_byte(unsigned char x);
void masterTransmit(char *data);
void masterRecieve(char *data);
unsigned char txTWI_SLA_R(void);
char rxTWI_data_byte(void);
void rxTWI_data_string(char *buffer, char len);


#define bitCheck(reg,ind) (reg >> ind & 1)
#define bitClear(reg, ind) (reg &= ~(1 << ind))
#define bitSet(reg, ind) (reg |= (1 << ind))
#define bitToggle(reg, ind) (reg ^= (1 << ind))




int main(void)
{
    initTWI();
	initUSART9600();
	sei();
    while (1) 
    {
		txStringUSART("Tx: ");
		char tx_array[50] = {};
		rxStringUSART(tx_array, 50);
		txStringUSART(tx_array);
		//masterTransmit(tx_array);
		
		txStringUSART("Rx: ");
		char rx_array[50] = {};
		//masterRecieve(rx_array);
		txStringUSART(rx_array);
    }
}

void initTWI(void){
	// prescaler = 1, SCL freq = 500kHz
	TWBR = 8;
	// TWEN = 1
	TWCR |= 4;
	
}


unsigned char getTWSR(void){
	// mask TSWR to get 5 status bits
	return TWSR & 0xF8;
	
}

void masterTransmit(char *data){
	
	// to check for expected status codes
	char status_flag = 0;
	
	// wait for start condition to be sent
	while(!status_flag){
		status_flag = startTWI();
	}
	
	status_flag = 0;
	// wait for address packet to be sent
	while(!status_flag){
		status_flag = txTWI_SLA_W();
	}
	
	txTWI_data_string(data);
	stopTWI();
	
}

void masterRecieve(char *data){
	
	char status_flag = 0;
	
	while(!status_flag){
		status_flag = startTWI();
	}
	
	status_flag = 0;
	while(!status_flag){
		status_flag = txTWI_SLA_R();
	}
	
	rxTWI_data_string(data, 50);
	
}

unsigned char startTWI(void){
	
	// clear int flag and send start signal to begin sending packets
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
	// poll interrupt flag
	while(!bitCheck(TWCR, TWINT));
	// start condition sent = 0x08
	return getTWSR() == 0x08;
}

void stopTWI(void){
	
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	
}

unsigned char txTWI_SLA_W(void){
	
	TWDR = SLAVE_ADDR << 1 | 0;	//SDA + W
	TWCR = (1 << TWINT) | (1 << TWEN); // clear TWINT to send SDA 
	
	// wait for TWINT to go back high indicating
	while(!bitCheck(TWCR, TWINT));
	// SLA+W sent and ACK recieved = 0x18
	return getTWSR() == 0x18;
}

unsigned char txTWI_SLA_R(void){
	
	TWDR = SLAVE_ADDR << 1 | 1; // SDA + R
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	while(!bitCheck(TWCR, TWINT));
	
	return getTWSR() == 0x40; // SDA+R send + ACK recieved = 0x40
	
}

void txTWI_data_byte(unsigned char x){
	
	TWDR = x; //load data
	TWCR = (1 << TWINT) | (1 << TWEN); //
	// wait for int to go high
	while(!bitCheck(TWCR, TWINT));
	
	
}

void txTWI_data_string(char *x){
	
	while (*x != '\0'){
		txTWI_data_byte(*x);
		if(getTWSR() != 0x28){ // 0x28 data sent and ack recieved
			break;
		}
		x++;
		
		
	}
	
}

void rxTWI_data_string(char *buffer, char len){
	
	uint8_t i = 0;
	char data;
	
	data = rxTWI_data_byte();
	
	while(data != '\n' && i < len - 1){
		
		buffer[i] = data;
		i++;
		data = rxTWI_data_byte();
		
	}
	
	buffer[i] = '\0';
}

char rxTWI_data_byte(void){
	
	while((getTWSR() == 0x50) && !bitCheck(TWCR, TWINT));
	unsigned char byte = TWDR;
	if(byte == '\0'){
		TWCR = (1 << TWINT) | (0 << TWEA); // if end of string send NACK
		return byte;
	}else if(getTWSR() == 0x58){
		
		return 0;
		
	}else{
		TWCR = (1 << TWINT) | (1 << TWEA);// if not end of string send ACK and prepare for more bytes
		return byte;
	}
	
	
}

void initUSART9600(void){
	UBRR0 = 103; // 9600 baud rate
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void txByteUSART(unsigned char x){
	
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1 << UDRE0)));
	// Put data into buffer, sends the data
	UDR0 = x;
}

void txStringUSART(char *x){
	while (*x != '\0')
	{
		txByteUSART(*x);
		x++;
	}
	
}

char rxByteUSART(void){
	while(!(UCSR0A & (1 << RXC0)));
	
	return UDR0;
	
}

void rxStringUSART(char *buffer, char len){
	
	uint8_t i = 0;
	char data;
	
	data = rxByteUSART();
	
	while(data != '\n' && i < len - 1){
		
		buffer[i] = data;
		i++;
		data = rxByteUSART();
		
	}
	
	buffer[i] = '\0';
	
	
}