/*
 * can.h
 *
 *  Created on: Mar 1, 2016
 *      Author: B55457
 */

#ifndef CAN_H_
#define CAN_H_

#include <MPC5748G.h>
#include "project.h"

void can0_set_id(uint16_t id);
void can0_init_rx(void);
void can0_init_tx(void);
void can0_tx(uint8_t * TxData, unsigned int TxLen);
void can0_rx(void);
unsigned int can0_rx_ready(void);
void delay(void);

extern uint32_t  RxLENGTH;            	/* Recieved message number of data bytes */
extern uint8_t   RxDATA[8];           	/* Received message data string*/

#endif /* CAN_H_ */
