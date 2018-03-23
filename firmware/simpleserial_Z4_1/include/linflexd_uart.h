/*
 * linflexd_uart.h
 *
 *  Created on: Mar 2, 2016
 *      Author: B55457
 */

#ifndef LINFLEXD_UART_H_
#define LINFLEXD_UART_H_

#include "derivative.h"
#include "project.h"

#define UART_DTFTFF             0x0002 /* Data transmit complete */
#define UART_DRFRFE             0x0004 /* Data reception complete */

void initLINFlexD_2 ( unsigned int MegaHertz, unsigned int BaudRate );
void txLINFlexD_2( unsigned char Data );
unsigned char rxLINFlexD_2( void );
unsigned char checkLINFlexD_2( void );
void echoLINFlexD_2( void );
void testLINFlexD_2( void );

void initLINFlexD_1 ( unsigned int MegaHertz, unsigned int BaudRate );
void txLINFlexD_1( unsigned char Data );
unsigned char rxLINFlexD_1( void );
unsigned char checkLINFlexD_1( void );
void echoLINFlexD_1( void );
void testLINFlexD_1( void );

void initLINFlexD_0 ( unsigned int MegaHertz, unsigned int BaudRate );
void txLINFlexD_0( unsigned char Data );
unsigned char rxLINFlexD_0( void );
unsigned char checkLINFlexD_0( void );
void echoLINFlexD_0( void );
void testLINFlexD_0( void );
void flushLINFlexD_0( void );


#endif /* LINFLEXD_UART_H_ */
