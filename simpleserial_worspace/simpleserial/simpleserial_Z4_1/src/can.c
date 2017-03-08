/*****************************************************************************/
/* FILE NAME: can.c                   COPYRIGHT (c) NXP Semiconductors 2016  */
/*                                                      All Rights Reserved  */
/* DESCRIPTION: CAN Driver Functions							             */
/*                                                                           */
/*****************************************************************************/
/* REV      AUTHOR               DATE        DESCRIPTION OF CHANGE           */
/* ---   -----------          ----------    -----------------------          */
/* 1.0	 S Mihalik/H Osornio  07 Mar 2014   Initial Version                  */
/* 1.1	 S Mihalik            09 Mar 2015   Change from MPC5748G: 96 buffers.*/
/* 1.2	 K Shah				  01 Mar 2016	Ported to S32DS					 */
/*****************************************************************************/

#include "can.h"

uint32_t  RxCODE;              	/* Received message buffer code */
uint32_t  RxID;                	/* Received message ID */
uint32_t  RxLENGTH;            	/* Recieved message number of data bytes */
uint8_t   RxDATA[8];           	/* Received message data string*/
uint32_t  RxTIMESTAMP;         	/* Received message time */

void initCAN_0_rx(void) {         	/* General init.  MB IDs: MB4 ID_STD=0x555 */
  uint8_t   i;

  CAN_0.MCR.B.MDIS = 1;       	/* Disable module before selecting clock source*/
  CAN_0.CTRL1.B.CLKSRC=0;     	/* Clock Source = oscillator clock (40 MHz) */
  CAN_0.MCR.B.MDIS = 0;       	/* Enable module for config. (Sets FRZ, HALT)*/
  while (!CAN_0.MCR.B.FRZACK) {}/* Wait for freeze acknowledge to set */
                 	 	 	 	/* Good practice: wait for FRZACK on freeze mode entry/exit */
  CAN_0.CTRL1.R = 0x01DB0086; 	/* CAN bus: 16 MHz clksrc, 500 kbps with 16 tq */
                              	/* PRESDIV+1 = Fclksrc/Ftq = 16 MHz/8MHz = 2 */
                              	/*    so PRESDIV = 1 */
                              	/* PSEG2 = Phase_Seg2 - 1 = 4 - 1 = 3 */
                              	/* PSEG1 = PSEG2 = 3 */
                              	/* PROPSEG= Prop_Seg - 1 = 7 - 1 = 6 */
                              	/* RJW = Resync Jump Width - 1 = 4 = 1 */
  	  	  	  	  	  	  	  	/* SMP = 1: use 3 bits per CAN sample */
                              	/* CLKSRC=0 (unchanged): Fcanclk= Fxtal= 40 MHz*/
  for (i=0; i<96; i++) {      	/* MPC574xG has 96 buffers after MPC5748G rev 0*/
    CAN_0.MB[i].CS.B.CODE = 0;  /* Inactivate all message buffers */
  }
  CAN_0.MB[4].CS.B.IDE = 0;     /* MB 4 will look for a standard ID */
  CAN_0.MB[4].ID.B.ID_STD = 0x555; /* MB 4 will look for ID = 0x555 */
  CAN_0.MB[4].CS.B.CODE = 4;    /* MB 4 set to RX EMPTY */
  CAN_0.RXMGMASK.R = 0x1FFFFFFF;/* Global acceptance mask */

  SIUL2.MSCR[16].B.SSS = 1;    	/* Pad PB0: Source signal is CAN0_TX  */
  SIUL2.MSCR[16].B.OBE = 1;    	/* Pad PB0: Output Buffer Enable */
  SIUL2.MSCR[16].B.SRC = 3;    	/* Pad PB0: Maximum slew rate */
  SIUL2.MSCR[17].B.IBE = 1;    	/* Pad PB1: Enable pad for input - CAN0_RX */
  SIUL2.IMCR[188].B.SSS = 2;   	/* CAN0_RX: connected to pad PB1 */

  CAN_0.MCR.R = 0x0000003F;     /* Negate FlexCAN_0 halt state for 64 MBs */
  while (CAN_0.MCR.B.FRZACK & CAN_0.MCR.B.NOTRDY) {} /* Wait to clear */
                 	 	 	 	/* Good practice: wait for FRZACK on freeze mode entry/exit */


}

void initCAN_0_tx(void) {          /* General init. No MB IDs iniialized */
  uint8_t	i;

  CAN_0.MCR.B.MDIS = 1;       	/* Disable module before selecting clock source*/
  CAN_0.CTRL1.B.CLKSRC=0;     	/* Clock Source = oscillator clock (40 MHz) */
  CAN_0.MCR.B.MDIS = 0;       	/* Enable module for config. (Sets FRZ, HALT)*/
  while (!CAN_0.MCR.B.FRZACK) {}/* Wait for freeze acknowledge to set */
  CAN_0.CTRL1.R = 0x01DB0086;  	/* CAN bus: same as for CAN_0 */
  /* CAN bus: 40 MHz clksrc, 500 kbps with 16 tq */
                                	/* PRESDIV+1 = Fclksrc/Ftq = 16 MHz/8MHz = 2 */
                                	/*    so PRESDIV = 1 */
                                	/* PSEG2 = Phase_Seg2 - 1 = 4 - 1 = 3 */
                                	/* PSEG1 = PSEG2 = 3 */
                                	/* PROPSEG= Prop_Seg - 1 = 7 - 1 = 6 */
                                	/* RJW = Resync Jump Width - 1 = 4 = 1 */
    	  	  	  	  	  	  	  	/* SMP = 1: use 3 bits per CAN sample */
                                	/* CLKSRC=0 (unchanged): Fcanclk= Fxtal= 40 MHz*/

  for (i=0; i<96; i++) {      	/* MPC574xG has 96 buffers after MPC5748G rev 0*/
    CAN_0.MB[i].CS.B.CODE = 0;  /* Inactivate all message buffers */
  }
  CAN_0.MB[0].CS.B.CODE = 8;    /* Message Buffer 0 set to TX INACTIVE */

  SIUL2.MSCR[16].B.SSS = 1;    	/* Pad PB0: Source signal is CAN0_TX  */
  SIUL2.MSCR[16].B.OBE = 1;    	/* Pad PB0: Output Buffer Enable */
  SIUL2.MSCR[16].B.SRC = 3;    	/* Pad PB0: Maximum slew rate */
  SIUL2.MSCR[17].B.IBE = 1;    	/* Pad PB1: Enable pad for input - CAN0_RX */
  SIUL2.IMCR[188].B.SSS = 2;   	/* CAN0_RX: connected to pad PB1 */

  CAN_0.MCR.R = 0x0000003F;     /* Negate FlexCAN_0 halt state for 64 MB */
  while (CAN_0.MCR.B.FRZACK & CAN_0.MCR.B.NOTRDY) {} /* Wait to clear */
                 	 	 	 	/* Good practice: wait for FRZACK on freeze mode entry/exit */
}

void TransmitMsg(uint8_t * TxData, unsigned int TxLen) {   		/* Assumption:  Message buffer CODE is INACTIVE */
  uint8_t	i;

  CAN_0.MB[0].CS.B.IDE = 0;       /* Use standard ID length */
  CAN_0.MB[0].ID.B.ID_STD = 0x555;/* Transmit ID is 0x555 */
  CAN_0.MB[0].CS.B.RTR = 0;       /* Data frame, not remote Tx request frame */
  CAN_0.MB[0].CS.B.DLC = TxLen;/*#bytes to transmit w/o null*/
  for (i=0; i < TxLen; i++) {
    CAN_0.MB[0].DATA.B[i] = TxData[i];      /* Data to be transmitted */
  }
  CAN_0.MB[0].CS.B.SRR = 1;     /* Tx frame (not req'd for standard frame)*/
  CAN_0.MB[0].CS.B.CODE =0xC;   /* Activate msg. buf. to transmit data frame */

}

void ReceiveMsg(void) {
  uint8_t j;
  uint32_t __attribute__ ((unused)) dummy;

  while (CAN_0.IFLAG1.B.BUF4TO1I != 8) {};  /* Wait for CAN_0 MB 4 flag */
  RxCODE   = CAN_0.MB[4].CS.B.CODE; 		/* Read CODE, ID, LENGTH, DATA, TIMESTAMP*/
  RxID     = CAN_0.MB[4].ID.B.ID_STD;
  RxLENGTH = CAN_0.MB[4].CS.B.DLC;
  for (j=0; j<RxLENGTH; j++) {
    RxDATA[j] = CAN_0.MB[4].DATA.B[j];
  }
  RxTIMESTAMP = CAN_0.MB[4].CS.B.TIMESTAMP;
  dummy = CAN_0.TIMER.R;             /* Read TIMER to unlock message buffers */
  CAN_0.IFLAG1.R = 0x00000010;       /* Clear CAN_0 MB 4 flag */
}
