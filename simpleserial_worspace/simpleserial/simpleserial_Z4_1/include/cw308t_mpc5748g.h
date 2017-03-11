/*
 * cw308_mpc5748g.h
 *
 *  Created on: Mar 10, 2017
 *      Author: colin
 */

#ifndef CW308T_MPC5748G_H_
#define CW308T_MPC5748G_H_

#define LED_CORE0 PJ12
#define LED_CORE1 PA4
#define LED_CORE2 PK0
#define LED_BLOAD PF1
#define LED_CLKOK PB9
#define LED_HSM   PB12

#define CAN0TX    PA13
#define CAN0RX    PA15

#define LED_USER1 PC13
#define LED_USER2 PC14
#define LED_USER3 PC15

#define UARTTX    PB0
#define UARTRX    PB1
#define GPIO3     PI7
#define GPIO4     PI6

#define CAN0TX    PA13
#define CAN0RX    PA15

#define CAN4TX    PJ14
#define CAN4RX    PJ10

#define LED_SETUP(led)  SIUL2.MSCR[led].B.OBE = 1
#define LED_ON(led)     SIUL2.GPDO[led].B.PDO_4n = 1
#define LED_OFF(led)    SIUL2.GPDO[led].B.PDO_4n = 0
#define LED_TOGGLE(led) SIUL2.GPDO[led].B.PDO_4n ^= 1

#endif /* CW308T_MPC5748G_H_ */
