/*
 * ultraSonic.h
 *
 * Created: 27/03/2023 1:07:01 PM
 *  Author: asine
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "bitFunctions.h"
#include "delayTimer0.h"

#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

float ultraSonic(int pinTrigger, int pinEcho);

#endif /* ULTRASONIC_H_ */

