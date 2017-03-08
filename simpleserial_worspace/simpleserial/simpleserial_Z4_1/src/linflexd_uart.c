/*****************************************************************************/
/* FILE NAME: linflexd_uart.c         COPYRIGHT (c) NXP Semiconductors 2016  */
/*                                                      All Rights Reserved  */
/* PLATFFORM: DEVKIT-MPC5748G												 */
/* DESCRIPTION: Routines to transmit & receive UART messages with LINflexD.  */
/*              Data is sent one byte at a time without using the FIFO.      */
/*                                                                           */
/*****************************************************************************/
/* REV      AUTHOR        DATE        DESCRIPTION OF CHANGE                  */
/* ---   -----------    ----------    ---------------------                  */
/* 1.0	 Scott Obrien   Mar 31 2014   Initial Version                        */
/*****************************************************************************/
/*
** LINFLEX UART Driver
** Notes:
** 1. UARTCR bits can NOT be written until UARTCR.UART is SET
** 2. There is no way  to determine if the transmit buffer is
** empty before the first data transmission.  The DTF bit (Transmit Complete)
** flag is the only indication that it's OK to write to the data buffer.
** There is no way to set this bit in software.  To set this bit, a transmit
** must occur.
*/
/*****************************************************************************/

#include "linflexd_uart.h"

unsigned char UARTFirstTransmitFlag;
/*****************************************************************************/
/*
** Baud Rate = LINCLK / (16 x LFDIV)
** LINCLK = BR x (16 x LFDIV)
** LINCLK / (BR x 16) = LFDIV
**
** LFDIV = Mantissa.Fraction.
** Mantissa = LINIBRR
** Fraction = LINFBRR / 16
**
** Baud Rate = LINCLK / (16 x LINIBRR.(LINFBRR / 16))
** LINIBRR.(LINFBRR / 16) = LINCLK / (BR x 16)
** LINIBRR = Mantissa[LINCLK / (BR x 16)]
** Remainder =  LINFBRR / 16
** LINFBRR = Remainder * 16
** The Remainder is < 1, So how does the Remainder work during a divide?
** May be best to use a table?
**
** For Reference & Quick Tests
** LINFLEX_x.LINIBRR.R = 416;                   // 9600 at 64MHz
** LINFLEX_x.LINFBRR.R = 11;
**
** LINFLEX_x.LINIBRR.R = 781;                   // 9600 at 120MHz
** LINFLEX_x.LINFBRR.R = 4;
*/
/*****************************************************************************/

void initLINFlexD_2 ( unsigned int MegaHertz, unsigned int BaudRate ) {
  unsigned int Fraction;
  unsigned int Integer;


  LINFlexD_2.LINCR1.B.INIT = 1;     /* Enter Initialization Mode */

	SIUL2.MSCR[PA0].B.OBE = 1;
	SIUL2.GPDO[PA0].B.PDO_4n = 0;

  LINFlexD_2.LINCR1.B.SLEEP = 0;    /* Exit Sleep Mode */
  LINFlexD_2.UARTCR.B.UART = 1;     /* UART Enable, Req'd before UART configuration */
  LINFlexD_2.UARTCR.R = 0x0033;     /* UART Enable, 1 byte tx, no parity, 8 data*/
  LINFlexD_2.UARTSR.B.SZF = 1;      /* CHANGE THIS LINE   Clear the Zero status bit */
  LINFlexD_2.UARTSR.B.DRFRFE = 1;   /* CHANGE THIS LINE  Clear DRFRFE flag - W1C */

  BaudRate  = (MegaHertz * 1000000) / BaudRate;
  Integer   = BaudRate / 16;
  Fraction  = BaudRate - (Integer * 16);

  LINFlexD_2.LINIBRR.R = Integer;
  LINFlexD_2.LINFBRR.R = Fraction;

  LINFlexD_2.LINCR1.B.INIT = 0;     /* Exit Initialization Mode */

  UARTFirstTransmitFlag = 1;        /* Indicate no Tx has taken place yet */

  SIUL2.MSCR[PC8].B.SSS = 1;        /* Pad PC8: Source signal is LIN2_TX  */
  SIUL2.MSCR[PC8].B.OBE = 1;        /* Pad PC8: OBE=1. */
  SIUL2.MSCR[PC8].B.SRC = 3;        /* Pad PC8: Full strength slew rate */
  SIUL2.MSCR[PC9].B.IBE = 1;        /* Pad PC9: Enable pad for input */
  SIUL2.IMCR[202].B.SSS = 2;        /* LIN2_RX : connected to pad PC9 */
}
/*****************************************************************************/

char message[] = "Welcome to MPC5748G! ";

void testLINFlexD_2( void )  {     /* Display message to terminal */
  int i, size;

  size = sizeof(message);
  for (i = 0; i < size; i++) {
    txLINFlexD_2(message[i]);
  }
  txLINFlexD_2(13);                /* Carriage return */
  txLINFlexD_2(10);                /* Line feed */
}
/*****************************************************************************/

unsigned char rxLINFlexD_2() {
  while (LINFlexD_2.UARTSR.B.DRFRFE == 0); /* Wait for data reception complete*/
  LINFlexD_2.UARTSR.R &= UART_DRFRFE;      /* Clear data reception flag W1C */
  return( LINFlexD_2.BDRM.B.DATA4 );       /* Read byte of Data */
}
/*****************************************************************************/

void txLINFlexD_2( unsigned char Data ) {
  if( UARTFirstTransmitFlag )   {         /* 1st byte transmit after init: */
    UARTFirstTransmitFlag = 0;            /* Clear variable */
  }
  else {                                  /* Normal transmit (not 1st time): */
    while (LINFlexD_2.UARTSR.B.DTFTFF == 0); /* Wait for data transmission complete*/
    LINFlexD_2.UARTSR.R &= UART_DTFTFF;      /* Clear DTFTFF flag - W1C */
  }
  LINFlexD_2.BDRL.B.DATA0 = Data;            /* Transmit 8 bits Data */
}
/*****************************************************************************/

unsigned char checkLINFlexD_2()  {      /* Optional utility for status check */
  return( LINFlexD_2.UARTSR.B.DRFRFE ); /* Return Receive Buffer Status */
}
/*****************************************************************************/

void echoLINFlexD_2() {                 /* Optional utility to echo char. */
  txLINFlexD_2( rxLINFlexD_2() );
}
/*****************************************************************************/


/* LINFlex 1 for UART functioning */

void initLINFlexD_1 ( unsigned int MegaHertz, unsigned int BaudRate ) {
  unsigned int Fraction;
  unsigned int Integer;

  LINFlexD_1.LINCR1.B.INIT = 1;     /* Enter Initialization Mode */
  LINFlexD_1.LINCR1.B.SLEEP = 0;    /* Exit Sleep Mode */
  LINFlexD_1.UARTCR.B.UART = 1;     /* UART Enable, Req'd before UART configuration */
  LINFlexD_1.UARTCR.R = 0x0033;     /* UART Enable, 1 byte tx, no parity, 8 data*/
  LINFlexD_1.UARTSR.B.SZF = 1;      /* CHANGE THIS LINE   Clear the Zero status bit */
  LINFlexD_1.UARTSR.B.DRFRFE = 1;   /* CHANGE THIS LINE  Clear DRFRFE flag - W1C */

  BaudRate  = (MegaHertz * 1000000) / BaudRate;
  Integer   = BaudRate / 16;
  Fraction  = BaudRate - (Integer * 16);

  LINFlexD_1.LINIBRR.R = Integer;
  LINFlexD_1.LINFBRR.R = Fraction;

  LINFlexD_1.LINCR1.B.INIT = 0;     /* Exit Initialization Mode */

  UARTFirstTransmitFlag = 1;        /* Indicate no Tx has taken place yet */

  SIUL2.MSCR[PC6].B.SSS = 1;        /* Pad PC6: Source signal is LIN1_TX  */
  SIUL2.MSCR[PC6].B.OBE = 1;        /* Pad PC6: OBE=1. */
  SIUL2.MSCR[PC6].B.SRC = 3;        /* Pad PC6: Full strength slew rate */
  SIUL2.MSCR[PC7].B.IBE = 1;        /* Pad PC7: Enable pad for input */
  SIUL2.IMCR[201].B.SSS = 1;        /* LIN1_RX : connected to pad PC9 */
}
/*****************************************************************************/

void testLINFlexD_1( void )  {     /* Display message to terminal */
  int i, size;

  size = sizeof(message);
  for (i = 0; i < size; i++) {
    txLINFlexD_1(message[i]);
  }
  txLINFlexD_1(13);                /* Carriage return */
  txLINFlexD_1(10);                /* Line feed */
}
/*****************************************************************************/

unsigned char rxLINFlexD_1() {
  while (LINFlexD_1.UARTSR.B.DRFRFE == 0); /* Wait for data reception complete*/
  LINFlexD_1.UARTSR.R &= UART_DRFRFE;      /* Clear data reception flag W1C */
  return( LINFlexD_1.BDRM.B.DATA4 );       /* Read byte of Data */
}
/*****************************************************************************/

void txLINFlexD_1( unsigned char Data ) {
  if( UARTFirstTransmitFlag )   {         /* 1st byte transmit after init: */
    UARTFirstTransmitFlag = 0;            /* Clear variable */
  }
  else {                                  /* Normal transmit (not 1st time): */
    while (LINFlexD_1.UARTSR.B.DTFTFF == 0); /* Wait for data transmission complete*/
    LINFlexD_1.UARTSR.R &= UART_DTFTFF;      /* Clear DTFTFF flag - W1C */
  }
  LINFlexD_1.BDRL.B.DATA0 = Data;            /* Transmit 8 bits Data */
}
/*****************************************************************************/

unsigned char checkLINFlexD_1()  {      /* Optional utility for status check */
  return( LINFlexD_1.UARTSR.B.DRFRFE ); /* Return Receive Buffer Status */
}
/*****************************************************************************/

void echoLINFlexD_1() {                 /* Optional utility to echo char. */
  txLINFlexD_1( rxLINFlexD_1() );
}
/*****************************************************************************/




/* LINFlex 1 for UART functioning */

void initLINFlexD_0 ( unsigned int MegaHertz, unsigned int BaudRate ) {
  unsigned int Fraction;
  unsigned int Integer;

  LINFlexD_0.LINCR1.B.INIT = 1;     /* Enter Initialization Mode */
  LINFlexD_0.LINCR1.B.SLEEP = 0;    /* Exit Sleep Mode */
  //LINFlexD_0.LINCR1.B.MME = 1;
  LINFlexD_0.UARTCR.B.UART = 1;     /* UART Enable, Req'd before UART configuration */
  LINFlexD_0.UARTCR.R = 0x0033;     /* UART Enable, 1 byte tx, no parity, 8 data*/
  LINFlexD_0.UARTSR.B.SZF = 1;      /* CHANGE THIS LINE   Clear the Zero status bit */
  LINFlexD_0.UARTSR.B.DRFRFE = 1;   /* CHANGE THIS LINE  Clear DRFRFE flag - W1C */

  BaudRate  = (MegaHertz * 1000000) / BaudRate;
  Integer   = BaudRate / 16;
  Fraction  = BaudRate - (Integer * 16);

  LINFlexD_0.LINIBRR.R = Integer;
  LINFlexD_0.LINFBRR.R = Fraction;

  LINFlexD_0.LINCR1.B.INIT = 0;     /* Exit Initialization Mode */

  UARTFirstTransmitFlag = 1;        /* Indicate no Tx has taken place yet */

  SIUL2.MSCR[PB0].B.SSS = 3;        /* Pad PC6: Source signal is LIN1_TX  */
  SIUL2.MSCR[PB0].B.OBE = 1;        /* Pad PC6: OBE=1. */
  SIUL2.MSCR[PB0].B.PUE = 1;
  SIUL2.MSCR[PB0].B.PUS = 1;
  SIUL2.MSCR[PB0].B.SRC = 3;        /* Pad PC6: Full strength slew rate */
  SIUL2.MSCR[PB1].B.IBE = 1;        /* Pad PC7: Enable pad for input */
  SIUL2.IMCR[200].B.SSS = 1;        /* LIN0_RX : connected to pad PB1 */
}
/*****************************************************************************/

void testLINFlexD_0( void )  {     /* Display message to terminal */
  int i, size;

  size = sizeof(message);
  for (i = 0; i < size; i++) {
    txLINFlexD_0(message[i]);
  }
  txLINFlexD_0(13);                /* Carriage return */
  txLINFlexD_0(10);                /* Line feed */
}
/*****************************************************************************/

unsigned char rxLINFlexD_0() {
  while (LINFlexD_0.UARTSR.B.DRFRFE == 0); /* Wait for data reception complete*/
  LINFlexD_0.UARTSR.R &= UART_DRFRFE;      /* Clear data reception flag W1C */
  return( LINFlexD_0.BDRM.B.DATA4 );       /* Read byte of Data */
}
/*****************************************************************************/

void txLINFlexD_0( unsigned char Data ) {
  if( UARTFirstTransmitFlag )   {         /* 1st byte transmit after init: */
    UARTFirstTransmitFlag = 0;            /* Clear variable */
  }
  else {                                  /* Normal transmit (not 1st time): */
    while (LINFlexD_0.UARTSR.B.DTFTFF == 0); /* Wait for data transmission complete*/
    LINFlexD_0.UARTSR.R &= UART_DTFTFF;      /* Clear DTFTFF flag - W1C */
  }
  LINFlexD_0.BDRL.B.DATA0 = Data;            /* Transmit 8 bits Data */
}
/*****************************************************************************/

unsigned char checkLINFlexD_0()  {      /* Optional utility for status check */
  return( LINFlexD_0.UARTSR.B.DRFRFE ); /* Return Receive Buffer Status */
}
/*****************************************************************************/

void echoLINFlexD_0() {                 /* Optional utility to echo char. */
  txLINFlexD_0( rxLINFlexD_0() );
}
