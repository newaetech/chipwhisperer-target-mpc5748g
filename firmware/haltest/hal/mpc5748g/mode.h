/*
 * mode.h
 *
 *  Created on: Feb 25, 2016
 *      Author: B55457
 */

#ifndef MODE_H_
#define MODE_H_

#include "project.h"

void PLL_160MHz (void);
void system160mhz (void);
void PLL_40MHz (void);
void system40mhz (void);
void system16mhz(unsigned int turn_on_s40);
void enter_STOP_mode (void);

#endif /* MODE_H_ */
