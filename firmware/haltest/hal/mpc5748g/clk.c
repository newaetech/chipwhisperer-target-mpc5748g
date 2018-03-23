/*****************************************************************************/
/* FILE NAME: clk.c	                   COPYRIGHT (c) NXP Semiconductors 2013 */
/*                                                      All Rights Reserved  */
/* PLATFORM: DEVKIT-MPC5748G												 */
/* DESCRIPTION: Clock out functions					                         */
/*                                                                           */
/*****************************************************************************/
/* REV      AUTHOR        DATE        DESCRIPTION OF CHANGE                  */
/* ---   -----------    ----------    ---------------------                  */
/* 1.0	  SM            29 Jul 2013  Initial Version                         */
/* 1.1	  K Shah		23 Feb 2016  Ported to S32DS						 */
/*****************************************************************************/
#include "clk.h"

void clock_out_FMPLL()
 {
   /* Set Up clock selectors to allow clock out 0 to be viewed */
   MC_CGM.AC6_SC.B.SELCTL = 2;           /* Select PHI_0 (PLL0-sysclk0) */
   MC_CGM.AC6_DC0.B.DE    = 1;           /* Enable AC6 divider 0 (SYSCLK0)*/
   MC_CGM.AC6_DC0.B.DIV   = 1;           /* Divide by 2 (set this to divvalue - 1) */

   /* Configure Pin for Clock out 0 on PG7 */
   SIUL2.MSCR[PG7].R = 0x00000003;       /*  PG7 output, slew-rate control with half-drive*/

   //0x02000003 = full drive
 }

  void clock_out_FIRC()
 {
   /* Set Up clock selectors to allow clock out 0 to be viewed */
   MC_CGM.AC6_SC.B.SELCTL = 1;            /* Select FIRC */
   MC_CGM.AC6_DC0.B.DE    = 1;            /* Enable AC6 divider 0 */
   MC_CGM.AC6_DC0.B.DIV   = 9;            /* Divide by 10 */

   /* Configure Pin for Clock out 0 on PG7 */
   SIUL2.MSCR[PG7].R = 0x00000003;        /* PG7 output, slew-rate control with half-drive */
 }
