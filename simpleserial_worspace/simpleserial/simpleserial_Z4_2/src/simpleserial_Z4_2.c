/*****************************************************************************/
/* FILE NAME: hello+pll_z4_1.c        COPYRIGHT (c) NXP SEMICONDUCTORS 2016  */
/*                                                      All Rights Reserved  */
/* PLATFORM: DEVKIT-MPC5748G												 */
/* DESCRIPTION: Main C program for core 1 (e200z4b)                          */
/*				Configures the system clock to 160 MHz and also configures	 */
/*				different clock groups.										 */
/*				Turns on the the DS4, DS6 and DS8 LEDs. Also outputs PLL  	 */
/* 				clock scaled to PLL/10 on port pin PG7 (J3_16)				 */
/*				Core 1 : Turns on DS6										 */
/*																			 */
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
	AXBS_0.PORT[3].CRS.B.ARB = 1;  /* Round-robin (rotating) priority */
	
	xcptn_xmpl ();              /* Configure and Enable Interrupts */

	SIUL2.MSCR[PA4].B.OBE = 1;
	SIUL2.GPDO[PA4].B.PDO_4n = 1;

	while(1) {
		;
	}
	
	return 0;
}
