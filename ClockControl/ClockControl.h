/*
 * clockcontrol.h
 *
 * Based on mbed PowerControl Library - Copyright (c) 2010 Michael Wei
 *
 *  Created on: 15 feb. 2022
 *      Author: fep
 */

#ifndef CLOCKCONTROL_CLOCKCONTROL_H_
#define CLOCKCONTROL_CLOCKCONTROL_H_

//shouldn't have to include, but fixes weird problems with defines
//#include "LPC1768/LPC17xx.h"
#include "LPC17xx.h"

#ifndef MBED_CLOCKCONTROL_H
#define MBED_CLOCKCONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

unsigned int setSystemFrequency(unsigned char clkDivider, unsigned char clkSrc, unsigned short M, unsigned char N);
void setPLL0Frequency(unsigned char clkSrc, unsigned short M, unsigned char N);
void setPLL1Frequency(unsigned char clkSrc, unsigned short M, unsigned char N);
#endif

#ifdef __cplusplus
}
#endif


#endif /* CLOCKCONTROL_CLOCKCONTROL_H_ */
