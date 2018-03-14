/****************************************************************************

  Example Application for MPC5748G Demo board for CW308
  See https://wiki.newae.com/CW308T-MPC5748G for details.

    This file is part of the ChipWhisperer Example Targets
    Copyright (C) 2012-2017 NewAE Technology Inc.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/


#include <stdio.h>

#include "derivative.h"
#include "clk.h"
#include "project.h"
#include "mode.h"
#include "linflexd_uart.h"
#include "can.h"

#include "cw308t_mpc5748g.h"
#include "sharedmem.h"
#include "simpleserial.h"

#include "tinyaes128.h"

#define SUPPORT_HSM 1

#ifndef SUPPORT_HSM
#define SUPPORT_HSM 0
#endif

#if SUPPORT_HSM
#include "hsm_interface.h"
#endif

#define KEY_VALUE1 0x5AF0ul
#define KEY_VALUE2 0xA50Ful


extern void xcptn_xmpl(void);

#define TRIG_INIT() SIUL2.MSCR[PI6].B.OBE = 1
#define TRIG_HIGH() SIUL2.GPDO[PI6].B.PDO_4n = 1
#define TRIG_LOW()  SIUL2.GPDO[PI6].B.PDO_4n = 0

//Start in .text section now
__attribute__ ((section(".text")))

/**************************** Serial I/O Functions ************************/

/* Specify UART to use here - need to change init code too if modified */
#define checkchar checkLINFlexD_0


/* putchar needed by simpleserial, pass-through to our function */
void txchar(char c)
{
	txLINFlexD_0(c);
}

/* getchar needed by simpleserial, pass-through to our function */
char rxchar(void)
{
	return rxLINFlexD_0();
}

/** Enable printf() - these are lazy routines to clip into EWL rather than use the
 *  correct callbacks. They work for now though. */
int __read_console(__file_handle handle, unsigned char * buffer, size_t * count)
{
	return (int) __io_error;
}


int __close_console(__file_handle handle)
{
	return (int)__no_io_error;
}

int __write_console(__file_handle handle, unsigned char * buffer, size_t * count)
{
	int tosend = *count;

	while(tosend){
		txchar(*(buffer++));
		tosend--;
	}

	return (int)__no_io_error;
}

/************************* Simpleserial Commands / Functions *****************/

#define MODULE_INVALID   0x00
#define MODULE_SWAES     0x80
#define MODULE_SWAESHSM  0x40
#define MODULE_HWAES     0x20
#define MODULE_XOR       0x10
uint8_t enc_module = MODULE_INVALID;

uint8_t pwgroup;

uint8_t set_module(uint8_t* mod)
{
	enc_module = mod[0] & 0xF0;
	if (enc_module == MODULE_HWAES){
		hsm_setkeyidx(mod[0] & 0x0F);
	} else if (enc_module == MODULE_SWAES){
		;
	} else if (enc_module == MODULE_SWAESHSM){
		;
	} else if (enc_module == MODULE_XOR){
		;
	} else {
		printf("DEBUG: INVALID MODULE %02x", mod[0]);
		return 0xff;
	}

	return 0;
}

uint8_t get_key(uint8_t* k)
{
	if (enc_module == MODULE_HWAES){
		hsm_setkey(k);
	} else if (enc_module == MODULE_SWAES){
		AES128_ECB_tinyaes_setkey(k);
	} else if (enc_module == MODULE_SWAESHSM){
		//todo
		;
	}
	else if (enc_module == MODULE_XOR){
		//todo
		;
	} else {
		return 0xFF;
	}
	return 0x00;
}

uint8_t get_pt(uint8_t* pt)
{
	if (enc_module == MODULE_HWAES){
#if SUPPORT_HSM
		hsm_sendin(pt);
		hsm_start();
		hsm_wait();
		hsm_readout(pt);
#else
		printf("HSM NOT SUPPORTED\n");
#endif
	} else if (enc_module == MODULE_SWAES){
#if SUPPORT_HSM
		hsm_release_gpio();
#endif
		TRIG_HIGH();
		AES128_ECB_tinyaes_crypto(pt);
		TRIG_LOW();
	} else if (enc_module == MODULE_SWAESHSM){
#if SUPPORT_HSM
		hsm_sendin(pt);
		hsm_start();
		hsm_wait();
		hsm_readout(pt);
#else
		printf("HSM NOT SUPPORTED\n");
#endif
	}
	else if (enc_module == MODULE_XOR){
#if SUPPORT_HSM
		hsm_release_gpio();
#endif
		TRIG_HIGH();
		TRIG_LOW();
	}

	simpleserial_put('r', 16, pt);
	return 0x00;
}

uint8_t set_pwlock(uint8_t * status)
{
	PASS.PG[pwgroup].LOCK3.B.PGL = 1;
	printf("r%02x %08x\n", pwgroup, (unsigned int)PASS.PG[pwgroup].LOCK3.R);
	return 0x00;
}

uint8_t set_pwgroup(uint8_t * group)
{
	if (group[0] < 4){
		pwgroup = group[0];
		return 0;
	} else {
		return 1;
	}
}

/***
 * This example tries to unlock a password group. The device must be in "OEM Production" lifecycle for this to
 * work, otherwise the password locks don't do anything.
 */
uint8_t set_pw(uint8_t * pw)
{
	int i;
#if SUPPORT_HSM
	hsm_release_gpio();
#endif

	PASS.CHSEL.B.GRP = pwgroup;

	TRIG_HIGH();

	PASS.CIN[0].R = *((uint32_t *)pw + 0);
	PASS.CIN[1].R = *((uint32_t *)pw + 1);
	PASS.CIN[2].R = *((uint32_t *)pw + 2);
	PASS.CIN[3].R = *((uint32_t *)pw + 3);
	PASS.CIN[4].R = *((uint32_t *)pw + 4);
	PASS.CIN[5].R = *((uint32_t *)pw + 5);
	PASS.CIN[6].R = *((uint32_t *)pw + 6);
	PASS.CIN[7].R = *((uint32_t *)pw + 7);

	//Some low-noise time for comparison before setting trigger low
	for(i = 0; i < 16; i++);

	TRIG_LOW();
	printf("r%02x %08x\n", pwgroup, (unsigned int)PASS.PG[pwgroup].LOCK3.R);

	return pwgroup;
}

/***
 * This example sends data to another core. You must have enabled the other core for this to work, otherwise
 * the process will hang forever (oops). It uses both the shared ram along with the hardware gate's to manage
 * this. See NXP App-Note AN4805 for more examples of using these gates, this is just one small example.
 */
uint8_t core_example(uint8_t * data)
{
	uint8_t status;

	/* Wait for previous processing to be done */
	status = CORE0_LOCK;
	while(status != GATE_UNLOCK){
		status = Get_Gate_status(GATE_0);
	}

	/* Write shared resource */
	sharedram_uint32[0] = 0xBAADBEEF;

	printf("Waiting on CORE1, sent %08x\n", (unsigned int)sharedram_uint32[0]);

	/* Lock gate, this tells other core we are done & they can read data */
	while(status != CORE0_LOCK){
		status = Lock_Gate(GATE_0);
	}

	/* Wait for other core to reset this gate, which means they are done */
	while(status == CORE0_LOCK){
		status = Get_Gate_status(GATE_0);
	}

	printf("Doneski, CORE1 sends %08x\n", (unsigned int)sharedram_uint32[0]);

	return 0;
}

uint8_t glitch_example2(uint8_t * data)
{
#if SUPPORT_HSM
			hsm_release_gpio();
#endif
	printf("Entering infinite glitch loop...\n");
	unsigned int lcnt = 0;
	volatile unsigned int i,j,totalcnt;
	while(1){
	totalcnt = 0;
	unsigned int test = 0;

	TRIG_HIGH();
	for(i = 0; i < 500; i++){
		for(j = 0; j < 500; j++){
			totalcnt++;
			test++;
		}
		TRIG_LOW();

		if (j != 500){
			printf("GLITCH 1\n");
			TRIG_LOW();
			return 0x00;
		}

		if ((i % 50) == 0){
			TRIG_HIGH();
		}

	}
	if ((i != 500) || (totalcnt != 250000) || (test != 250000)) {
		printf("GLITCH 2\n");
		printf("%u %u %u %u %u\n", i, j, totalcnt, test, lcnt++);
		TRIG_LOW();
		return 0x00;
	}
	TRIG_LOW();
	//printf("%u %u %u %u %u\n", i, j, totalcnt, test, lcnt++);
	}

	printf("You broke out of the loop!\n");

	return 0x00;
}


uint8_t glitch_example(uint8_t * data)
{
#if SUPPORT_HSM
			hsm_release_gpio();
#endif
	printf("Entering infinite glitch loop...\n");
	unsigned int lcnt = 0;
	unsigned int i,j,totalcnt;
	while(1){
	totalcnt = 0;
	unsigned int test = 0;

	TRIG_HIGH();
	for(i = 0; i < 1000; i++){
		for(j = 0; j < 1000; j++){
			totalcnt++;
			test++;
		}
		TRIG_LOW();

		if ((i % 50) == 0){
			TRIG_HIGH();
		}

	}
	printf("%u %u %u %u %u\n", i, j, totalcnt, test, lcnt++);
	TRIG_LOW();
	//printf("%u %u %u %u %u\n", i, j, totalcnt, test, lcnt++);
	}

	printf("You broke out of the loop!\n");

	return 0x00;
}


void read_flash(void)
{
#if SUPPORT_HSM
			hsm_release_gpio();
#endif

	uint32_t i = 0;

	while(1){
		i++;
		TRIG_HIGH();
		uint32_t * rv = (uint32_t *)(0x00400200);
		volatile uint32_t val = *rv;
		if (val != 0x55AA50AF) {
			printf("GLITCH: %x", (unsigned int)val);
		}
		TRIG_LOW();
		if ((i % 100000) == 0) {
			printf("%x\n", (unsigned int)val);
		}
	}

}

const char  * lcstatstring[] = { "Failure Analysis",
		                        "Reserved",
								"OEM Production",
								"Customer Delivery",
								"Reserved",
								"Reserved",
								"Freescale Production",
								"In Field" };

/************************************ Main ***********************************/
int main(void)
{
	uint32_t i;

	/* Before any other stuff online - setup user I/O */
	LED_SETUP(LED_CORE0);
	LED_SETUP(LED_CLKOK);
	LED_SETUP(LED_HSM);

	LED_SETUP(LED_USER1);
	LED_SETUP(LED_USER2);
	LED_SETUP(LED_USER3);

	TRIG_INIT();

	//Show CORE0 is alive (even if clock switch will fail)
	LED_ON(LED_CORE0);

	//Turn on other cores (if in use) now
#if defined(DEBUG_SECONDARY_CORES)
	uint32_t mctl = MC_ME.MCTL.R;
#if defined(TURN_ON_CPU1)
	/* enable core 1 in all modes */
	MC_ME.CCTL[2].R = 0x00FE;
	/* Set Start address for core 1: Will reset and start */
	MC_ME.CADDR[2].R = 0x11d0000 | 0x1;
#endif
#if defined(TURN_ON_CPU2)
	/* enable core 2 in all modes */
	MC_ME.CCTL[3].R = 0x00FE;
	/* Set Start address for core 2: Will reset and start */
	MC_ME.CADDR[3].R = 0x13a0000 | 0x1;
#endif
	MC_ME.MCTL.R = (mctl & 0xffff0000ul) | KEY_VALUE1;
	MC_ME.MCTL.R =  mctl; /* key value 2 always from MCTL */
#endif /* defined(DEBUG_SECONDARY_CORES) */

	sharedmem_init();


	xcptn_xmpl (); /* Configure and Enable Interrupts */


	//External clock in = 16 MHz, we will run CPU from that to make SCA easier
	//You can turn on or off the S40 clock domain - it's needed for the PASS module, but not for
	//anything else we use for demos. You can turn off to possibly reduce noise.
	//IF you turn off - be sure to remove talking to PASS module!!!
	system16mhz(1);

	// If we get here - core is probably OK!
	LED_ON(LED_CLKOK);

	//Optional - enable clock out (will increase noise so not on by default, not needed to our clkin anyway)
	//clock_out_FMPLL();          /* Pad PG7 = CLOCKOUT = PLL0/10 */

	initLINFlexD_0( 4, 38400 );/* Initialize LINFlex0: UART Mode 4 MHz peripheral clock, 38400 Baud */

	printf("CW308T-MPC5748G Online. Firmware compile date: %s : %s", __DATE__, __TIME__);
	printf(" See http://cwdocs.com/mp57 for documentation\n");
	printf(" Device information:\n");
	printf("   Lifecycle from LCSTAT: %u (%s)\n", PASS.LCSTAT.B.LIFE, lcstatstring[PASS.LCSTAT.B.LIFE]);
	printf("   Firmware HSM support: %s\n", SUPPORT_HSM ? "enabled":"disabled");
	//printf("   JTAG Password: ");

	//We can use some of the spare I/O if we want, here is HDR5 for example being set high
	SIUL2.MSCR[PH5].B.OBE = 1;
	SIUL2.GPDO[PH5].B.PDO_4n = 0;

	/**** CAN SETUP ****/
	MC_ME.PCTL[70].B.RUN_CFG = 0x1;   /* FlexCAN 0: select peripheral configuration RUN_PC[1] */
	can0_init_rx();

	uint8_t tx_buffer[5] = {'H', 'e', 'l', 'l', 'o'};
	//Example of sending a CAN message.
	//DO NOT uncomment the following unless you actually have something listening on CAN, since it will hold here
	//until the message transmits OK (we aren't using interrupt-driven CAN).
	//can0_tx(tx_buffer, 5);

	int pwok;
	uint8_t correct_pw[] = {0xDE, 0xAD, 0xBE, 0xEF};

#if SUPPORT_HSM
	printf("Checking for HSM... ");
	if (hsm_check()){
		puts("Found");
		LED_ON(LED_HSM);
	} else {
		LED_OFF(LED_HSM);
		puts("NOT found");
	}
#endif

	simpleserial_init();
    simpleserial_addcmd('k', 16, get_key);
    //simpleserial_addcmd('c', 16, set_ct);
    simpleserial_addcmd('p', 16,  get_pt);
    simpleserial_addcmd('q',  32,  set_pw);
    simpleserial_addcmd('h', 1, set_module);
    simpleserial_addcmd('w', 1, set_pwgroup);
    simpleserial_addcmd('l', 0, set_pwlock);
    simpleserial_addcmd('j', 4, core_example);
    simpleserial_addcmd('g', 0, glitch_example);

	/*
	 * Basic application has two modes: CAN and UART.
	 *
	 *  CAN MODE:
	 *    Send 4-byte password with ID 0x555, system responds with DE00 if wrong, DE01 if correct.
	 *    Default password is DEADBEEF.
	 *
	 *  UART MODE:
	 *    38,400 baud. Uses ChipWhisperer Simple-Serial Protocol, performs AES encryptions & other tasks.
	 *
	 */


	while(1){
		if (can0_rx_ready()){
#if SUPPORT_HSM
			hsm_release_gpio();
#endif
			can0_rx();

			if (RxLENGTH == 4){
				LED_OFF(LED_USER1);
				LED_OFF(LED_USER2);
				LED_ON(LED_USER3);

				TRIG_HIGH();
				pwok = 0x00;
				for(i =0; i < 4; i++){
					pwok |= (correct_pw[i] ^ RxDATA[i]);
				}
				TRIG_LOW();

				if (pwok == 0){
					pwok = 1;
				} else {
					pwok = 0;
				}

				tx_buffer[0] = 0xDE;
				tx_buffer[1] = pwok;

				if (pwok){
					LED_ON(LED_USER1);
				} else {
					LED_ON(LED_USER2);
				}

				LED_OFF(LED_USER3);

				can0_tx(tx_buffer, 2);
			}
		}

		//Wait for character
		if(checkchar()){
			simpleserial_get();
		}
	}

	/*
		else if (c == 'c'){
			ptr = 0;

			MC_ME.CCTL[2].R = 0x0000;
			MC_ME.CCTL[3].R = 0x0000;

			continue;
		}

		else if (c == 'w'){
			ptr = 0;

			MC_ME.CCTL[2].R = 0x00FE;
			MC_ME.CCTL[3].R = 0x00FE;

			continue;
		}
	*/


	return 0;
}

/********************  End of Main ***************************************/


/******************** Machine Check Exception ***************************/
