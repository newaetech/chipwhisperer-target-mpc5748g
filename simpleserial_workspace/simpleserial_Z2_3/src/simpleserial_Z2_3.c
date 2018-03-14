/*****************************************************************************/
/* FILE NAME: hello+pll_z2_3.c        COPYRIGHT (c) NXP Semiconductors 2016  */
/*                                                      All Rights Reserved  */
/* PLATFORM: DEVKIT-MPC5748G												 */
/* DESCRIPTION: Main C program for core 2 (e200z2)                          */
/*				Configures the system clock to 160 MHz and also configures	 */
/*				different clock groups.										 */
/*				Turns on the the DS4, DS6 and DS8 LEDs. Also outputs PLL  	 */
/* 				clock scaled to PLL/10 on port pin PG7 (J3_16)				 */
/*				Core 2 : Turns on DS8										 */
/*                                                                           */
/*****************************************************************************/
/* REV      AUTHOR        DATE        DESCRIPTION OF CHANGE                  */
/* ---   -----------    ----------    ---------------------                  */
/* 1.0	  K shah		25 Feb 2016	  Ported to S32DS						 */
/*****************************************************************************/
#include "derivative.h" /* include peripheral declarations */
#include "project.h"


extern void xcptn_xmpl(void);

__attribute__ ((section(".text")))
int main(void)
{
	volatile uint32_t i;

	xcptn_xmpl ();              /* Configure and Enable Interrupts */

	//SIUL2.MSCR[PH5].B.OBE = 1;  /* Pad PH5 (117) OBE=1. On EVB active low DS8 LED */
	SIUL2.MSCR[PK0].B.OBE = 1;
	SIUL2.GPDO[PK0].B.PDO_4n = 1;




	/*
	while(1) {
		for(i = 0; i < 10000; i++);
		//SIUL2.GPDO[PK0].B.PDO_4n ^= 1;
	}
	*/
	
	while(1);

	return 0;
}
