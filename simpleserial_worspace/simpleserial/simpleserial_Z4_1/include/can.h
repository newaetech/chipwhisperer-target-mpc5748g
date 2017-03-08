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

void initCAN_0_rx(void);
void initCAN_0_tx (void);
void TransmitMsg(uint8_t * TxData, unsigned int TxLen);
void ReceiveMsg (void);
void delay(void);

extern uint32_t  RxLENGTH;            	/* Recieved message number of data bytes */
extern uint8_t   RxDATA[8];           	/* Received message data string*/

#endif /* CAN_H_ */
