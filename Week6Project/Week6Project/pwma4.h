/*
 * pwma4.h
 *
 * Created: 28/03/2023 5:27:36 AM
 *  Author: asine
 */ 


#ifndef PWMA4_H_
#define PWMA4_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU 16000000UL

void a4Init(float duty);

void a4Start(float duty);

void a4stop(void);



#endif /* PWMA4_H_ */