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
#include "sharedmem.h"
#include <stdio.h>

extern void xcptn_xmpl(void);

__attribute__ ((section(".text")))
int main(void)
{
	uint8_t status;

	AXBS_0.PORT[3].CRS.B.ARB = 1;  /* Round-robin (rotating) priority */

	xcptn_xmpl ();              /* Configure and Enable Interrupts */

	SIUL2.MSCR[PA4].B.OBE = 1;
	SIUL2.GPDO[PA4].B.PDO_4n = 1;



	while(1) {

		/* Example of multi-core signaling. See http://www.nxp.com/assets/documents/data/en/application-notes/AN4805.pdf
		 * for details of how to use this in various situations.
		 */

		status = GATE_UNLOCK;

		//Wait for GATE0 to be locked by CORE0, used as signal to
		//indicate we are expected to do something now
		while(status != CORE0_LOCK){
			status = Get_Gate_status(GATE_0);
		}

		//Do something - in this case use shared ram to pass variable around
		sharedram_uint32[0] = sharedram_uint32[0]+1;

		//Unlock the gate - tells other core we are done
		while(status != GATE_UNLOCK){
			Reset_Gate(GATE_0);
			status = Get_Gate_status(GATE_0);
		}

	}

	return 0;
}
