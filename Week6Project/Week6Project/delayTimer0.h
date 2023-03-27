/*
 * delayTimer0.h
 *
 * Created: 26/03/2023 1:47:40 PM
 *  Author: asine
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "bitFunctions.h"

#ifndef DELAYTIMER0_H_
#define DELAYTIMER0_H_

void delayUS(float t);

void delay50ms(void);


#endif /* DELAYTIMER0_H_ */

