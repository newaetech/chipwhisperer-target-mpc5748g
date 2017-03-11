/*****************************************************************************/
/* FILE NAME: hello+pll_z4_1.c        COPYRIGHT (c) NXP Semiconductors 2016  */
/*                                                      All Rights Reserved  */
/* PLATFORM: DEVKIT-MPC5748G												 */
/* DESCRIPTION: Main C program for core 0 (e200z4a)                          */
/*				Configures the system clock to 160 MHz and also configures	 */
/*				different clock groups.										 */
/*				Turns on the the DS4, DS6 and DS8 LEDs. Also outputs PLL  	 */
/* 				clock scaled to PLL/10 on port pin PG7 (J3_16)				 */
/*				Core 0 : Configures System clock and Turns on DS4			 */
/*                                                                           */
/*****************************************************************************/
/* REV      AUTHOR        DATE        DESCRIPTION OF CHANGE                  */
/* ---   -----------    ----------    ---------------------                  */
/* 1.0	  SM            29 Jul 2013   Initial Version                        */
/* 1.2    SM            12 Feb 2015   Removed unrequired SIUL ME_PCTL code 	 */
/* 1.3	  K Shah		25 Feb 2016	  Ported to S32DS						 */
/*****************************************************************************/

#include "derivative.h" /* include peripheral declarations */
#include "clk.h"
#include "project.h"
#include "mode.h"
#include "linflexd_uart.h"
#include "can.h"

#define SUPPORT_HSM 1

#if SUPPORT_HSM
#include "hsm_interface.h"
#endif

#define KEY_VALUE1 0x5AF0ul
#define KEY_VALUE2 0xA50Ful

uint8_t* hex_decode(const char *in, int len,uint8_t *out);
void hex_print(const uint8_t * in, int len, char *out);

extern void xcptn_xmpl(void);

void hw_init(void)
{
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
}

#define txchar txLINFlexD_0
#define rxchar rxLINFlexD_0
#define checkchar checkLINFlexD_0

#define STATE_IDLE 0
#define STATE_KEY 1
#define STATE_PLAIN 2
#define STATE_KEYINDEX 3
#define KEY_LENGTH 16
#define BUFLEN (KEY_LENGTH*4)

//void peri_clock_gating (void); /* Configures gating/enabling peripheral(LIN) clocks */
void puts (char * str);

__attribute__ ((section(".text")))

/************************************ Main ***********************************/
int main(void)
{
	uint32_t i;
	//uint8_t test[16];
	//char message[70];
	//uint32_t loopcnt = 0;

	//uint8_t key[16];
	uint8_t pt[16];
	uint8_t ct[16];
	char asciibuf[BUFLEN];
	uint8_t tmp[KEY_LENGTH];

	char c;
	char state = 0;
	int ptr;

	//Init this before anything else
	LED_SETUP(LED_CORE0);
	LED_SETUP(LED_CLKOK);
	LED_SETUP(LED_HSM);

	LED_SETUP(LED_USER1);
	LED_SETUP(LED_USER2);
	LED_SETUP(LED_USER3);

	//Show CORE0 is alive (even if clock switch will fail)
	LED_ON(LED_CORE0);

	xcptn_xmpl (); /* Configure and Enable Interrupts */

	hw_init();

	//External clock in = 16 MHz
	system16mhz();

	LED_ON(LED_CLKOK);

	//clock_out_FMPLL();          /* Pad PG7 = CLOCKOUT = PLL0/10 */

	//SSS = 0x03 selects HSM for PD12
	//SIUL2.MSCR[PD12].B.SSS = 0x03;

	initLINFlexD_0( 4, 38400 );/* Initialize LINFlex0: UART Mode 4 MHz peripheral clock, 38400 Baud */
	testLINFlexD_0();			/* Display a message on the terminal */

	//We can use some of the spare I/O if we want, here is HDR5 for example
	SIUL2.MSCR[PH5].B.OBE = 1;
	SIUL2.GPDO[PH5].B.PDO_4n = 0;


	/**** CAN SETUP ****/
	MC_ME.PCTL[70].B.RUN_CFG = 0x1;   /* FlexCAN 0: select peripheral configuration RUN_PC[1] */
	can0_init_rx();

	uint8_t tx_buffer[5] = {'H', 'e', 'l', 'l', 'o'};
	//Don't uncomment the following unless you actually have something
	//listening on CAN.
	//can0_tx(tx_buffer, 5);

	int pwok;
	uint8_t correct_pw[] = {0xDE, 0xAD, 0xBE, 0xEF};


#if SUPPORT_HSM
	puts("Checking HSM\n");
	if (hsm_check()){
		LED_ON(LED_HSM);
	}
#endif

	puts("Hello World\n");

	/*
	 * Basic application has two modes: CAN and UART.
	 *
	 *  CAN MODE:
	 *    Send 4-byte password with ID 0x555, system responds with DE00 if wrong, DE01 if correct.
	 *    Default password is DEADBEEF.
	 *
	 *  UART MODE:
	 *    38,400 baud. Uses ChipWhisperer Simple-Serial Protocol, performs AES encryptions.
	 *
	 */
	while(1){
		if (can0_rx_ready()){
			can0_rx();
			LED_TOGGLE(LED_USER1);

			if (RxLENGTH == 4){
				pwok = 0x00;
				for(i =0; i < 4; i++){
					pwok |= (correct_pw[i] ^ RxDATA[i]);
				}

				if (pwok == 0){
					pwok = 1;
				} else {
					pwok = 0;
				}

				tx_buffer[0] = 0xDE;
				tx_buffer[1] = pwok;

				if (pwok){
					LED_ON(LED_USER2);
				} else {
					LED_ON(LED_USER3);
				}

				can0_tx(tx_buffer, 2);
			}
		}

		//Wait for character
		if(checkchar() == 0){
			continue;
		}

		c = rxchar();

		if (c == 'x') {
			ptr = 0;
			state = STATE_IDLE;
			continue;
		}

		if (c == 'k') {
			ptr = 0;
			state = STATE_KEY;
			continue;
		}

		else if (c == 'p') {
			ptr = 0;
			state = STATE_PLAIN;
			continue;
		}

		else if (c == 'h'){
			state = STATE_KEYINDEX;
			continue;
		}


		else if (state == STATE_KEY) {
			if ((c == '\n') || (c == '\r')) {
				asciibuf[ptr] = 0;
				hex_decode(asciibuf, ptr, tmp);
				hsm_setkey(tmp);
				state = STATE_IDLE;
			} else {
				asciibuf[ptr++] = c;
			}
		}

		else if (state == STATE_PLAIN) {
			if ((c == '\n') || (c == '\r')) {
				asciibuf[ptr] = 0;
				hex_decode(asciibuf, ptr, pt);
				/* Do Encryption */
				hsm_sendin(pt);
				hsm_start();
				hsm_wait();
				hsm_readout(ct);

				/* Print Results */
				hex_print(ct, 16, asciibuf);

				puts("r");
				puts(asciibuf);
				puts("\n");

				state = STATE_IDLE;
			} else {
                if (ptr >= BUFLEN){
                    state = STATE_IDLE;
                } else {
                    asciibuf[ptr++] = c;
                }
			}
		} else if (state == STATE_KEYINDEX) {
			if ((c >= '0') && ( c <= '7')){
				hsm_setkeyidx(c-'0');
			} else {
				puts("Invalid key index\n");
			}
			state = STATE_IDLE;
		}
	}

	return 0;
}

#if 0
void peri_clock_gating (void) {		/* Configures gating/enabling peripheral clocks for modes*/
  MC_ME.RUN_PC[0].R = 0x00000000;  	/* Gate off clock for all RUN modes */
  MC_ME.RUN_PC[1].R = 0x000000FC;  	/* Configures peripheral clock for all RUN modes */
  MC_ME.PCTL[52].B.RUN_CFG = 0x1;  	/* LINFlex 2: select peripheral configuration RUN_PC[1] */
//  MC_ME.PCTL[51].B.RUN_CFG = 0x1;  	/* LINFlex 1: select peripheral configuration RUN_PC[1] */
}
#endif

void puts (char * str){

	while(*str){
		txchar(*str++);
	}

}

char hex_lookup[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

uint8_t* hex_decode(const char *in, int len,uint8_t *out)
{
        unsigned int i, t, hn, ln;

        for (t = 0,i = 0; i < len; i+=2,++t) {

                hn = in[i] > '9' ? (in[i]|32) - 'a' + 10 : in[i] - '0';
                ln = in[i+1] > '9' ? (in[i+1]|32) - 'a' + 10 : in[i+1] - '0';
                out[t] = (hn << 4 ) | ln;
        }

        return out;
}

void hex_print(const uint8_t * in, int len, char *out)
{
		unsigned int i,j;
		j=0;
		for (i=0; i < len; i++) {
			out[j++] = hex_lookup[in[i] >> 4];
			out[j++] = hex_lookup[in[i] & 0x0F];
		}

		out[j] = 0;
}

/********************  End of Main ***************************************/


