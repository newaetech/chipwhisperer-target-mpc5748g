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

struct H2H_tag {
  union {
    vuint32_t R;
    struct {
      vuint32_t  :28;
      vuint32_t DATA:1;                  /* Data Read */
      vuint32_t reserved1:1;              /* reserved */
      vuint32_t reserved2:1;              /* reserved */
      vuint32_t BUSY:1;                 /* Busy */
    } B;
  } HSM2HTF;

  union {
    vuint32_t R;
  } HSM2HTIE;

  union {
    vuint32_t R;
    struct {
      vuint32_t  :23;
      vuint32_t SET_KEY_IFX:1;			 /* Set key index */
      vuint32_t KEY_IDX:3;				 /* Key index */
      vuint32_t READ:1;                  /* Read Output */
      vuint32_t DATA:1;                  /* Data Load */
      vuint32_t KEY:1;                   /* Key Load */
      vuint32_t RESET:1;                 /* Reset */
      vuint32_t START:1;                 /* Start Conversion */
    } B;
  } HT2HSMF;

  union {
    vuint32_t R;
  } HT2HSMIE;

  union {
    vuint32_t R;
  } HSM2HTS;

  union {
    vuint32_t R;
  } HT2HSMS;
};

#define H2H (*(volatile struct H2H_tag *) 0xFFF30000UL)


//void peri_clock_gating (void); /* Configures gating/enabling peripheral(LIN) clocks */
void puts (char * str);
void key2hsm(uint8_t * key);
void text2hsm(uint8_t * text);
void hsm2text(uint8_t * text);

#define txchar txLINFlexD_0
#define rxchar rxLINFlexD_0
#define checkchar checkLINFlexD_0

#define STATE_IDLE 0
#define STATE_KEY 1
#define STATE_PLAIN 2
#define STATE_KEYINDEX 3
#define KEY_LENGTH 16
#define BUFLEN (KEY_LENGTH*4)

#define LED_CORE0 PJ12
#define LED_CORE1 PA4
#define LED_CORE2 PK0
#define LED_BLOAD PF1
#define LED_CLKOK PB0
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

#define LED_SETUP(led) SIUL2.MSCR[led].B.OBE = 1;
#define LED_ON(led)    SIUL2.GPDO[led].B.PDO_4n = 1;
#define LED_OFF(led)   SIUL2.GPDO[led].B.PDO_4n = 0;


__attribute__ ((section(".text")))

/************************************ Main ***********************************/
int main(void)
{
	uint32_t i;
	//uint8_t test[16];
	char message[70];
	//uint32_t loopcnt = 0;

	uint8_t key[16];
	uint8_t pt[16];
	uint8_t ct[16];
	char asciibuf[BUFLEN];
	uint8_t tmp[KEY_LENGTH]; // = {DEFAULT_KEY};

	char c;
	char state = 0;
	int ptr;

	//Init this before anything else
	LED_SETUP(LED_CORE0);
	LED_SETUP(LED_CLKOK);
	LED_SETUP(LED_HSM);

	//Show CORE0 is alive (even if clock switch will fail)
	LED_ON(LED_CORE0);

	xcptn_xmpl (); /* Configure and Enable Interrupts */

	hw_init();

	system16mhz();

	//clock_out_FMPLL();          /* Pad PG7 = CLOCKOUT = PLL0/10 */

	//SSS = 0x03 selects HSM for PD12
	//SIUL2.MSCR[PD12].B.SSS = 0x03;

	initLINFlexD_0( 8, 38400 );/* Initialize LINFlex1: UART Mode 8MHz, 38400 Baud */
	testLINFlexD_0();			/* Display a message on the terminal */

	//We can use some of the spare I/O if we want
	SIUL2.MSCR[PH5].B.OBE = 1;
	SIUL2.GPDO[PH5].B.PDO_4n = 0;


#if 0
	MC_ME.PCTL[70].B.RUN_CFG = 0x1;   /* FlexCAN 0: select peripheral configuration RUN_PC[1] */
	//initCAN_0_tx();
	initCAN_0_rx();

	uint8_t tx_buffer[5] = {'H', 'e', 'l', 'l', 'o'};

	TransmitMsg(tx_buffer, 5);

	int pwok;
	uint8_t correct_pw[] = {0xDE, 0xAD, 0xBE, 0xEF};

	SIUL2.MSCR[PD12].B.OBE = 1;
	SIUL2.GPDO[PD12].B.PDO_4n = 0;

	while(1){
		ReceiveMsg();
		SIUL2.GPDO[PJ4].B.PDO_4n ^= 1;


		if (RxLENGTH == 4){
			SIUL2.GPDO[PD12].B.PDO_4n = 1;
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
			TransmitMsg(tx_buffer, 2);
			SIUL2.GPDO[PD12].B.PDO_4n = 0;
		}
	}

#endif


#if 0
	/* Check if the HSM is running */
	for(i = 0; i < 16; i++){
		pt[i] = 0;
		key[i] = 0;

	}

	hex_print(pt, 16, message);
	puts(message);

	puts("\nSending to HSM\n");

	text2hsm(pt);
	key2hsm(key);

	puts("Starting Enc\n");
	H2H.HT2HSMF.B.START = 1;
	while(H2H.HT2HSMF.B.START);

	puts("Enc Done, reading\n");
	hsm2text(ct);

	hex_print(ct, 16, message);
	puts(message);

	puts("\nOMG!\n");
#endif

	puts("Checking HSM\n");
	H2H.HT2HSMF.B.START = 1;
	while(H2H.HT2HSMF.B.START);
	LED_ON(LED_HSM);

	puts("Hello\n");

	while(1){

		//Wait for character
		while(checkchar() == 0);

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
				key2hsm(tmp);
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
				text2hsm(pt);
				H2H.HT2HSMF.B.START = 1;
				while(H2H.HT2HSMF.B.START);

				hsm2text(ct);

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
				H2H.HT2HSMF.B.KEY_IDX = c - '0';
				H2H.HT2HSMF.B.SET_KEY_IFX = 1;
			} else {
				puts("Invalid key index\n");
			}
			state = STATE_IDLE;
		}
	}

	/*
	while(1){
		for (i = 0; i < 20000; i++) {
			if (checkLINFlexD_2()){
				echoLINFlexD_2();
			}

		}


		SIUL2.GPDO[PA12].B.PDO_4n ^= 1;
	}
	*/
	
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

void key2hsm(uint8_t * key)
{
	H2H.HT2HSMS.R = *((uint32_t *)(key + 0));
	H2H.HT2HSMF.B.KEY = 1;
	while(H2H.HT2HSMF.B.KEY);

	H2H.HT2HSMS.R = *((uint32_t *)(key + 4));
	H2H.HT2HSMF.B.KEY = 1;
	while(H2H.HT2HSMF.B.KEY);

	H2H.HT2HSMS.R = *((uint32_t *)(key + 8));
	H2H.HT2HSMF.B.KEY = 1;
	while(H2H.HT2HSMF.B.KEY);

	H2H.HT2HSMS.R = *((uint32_t *)(key + 12));
	H2H.HT2HSMF.B.KEY = 1;
	while(H2H.HT2HSMF.B.KEY);
}

void text2hsm(uint8_t * text)
{
	H2H.HT2HSMS.R = *((uint32_t *)(text + 0));
	H2H.HT2HSMF.B.DATA = 1;
	while(H2H.HT2HSMF.B.DATA);

	H2H.HT2HSMS.R = *((uint32_t *)(text + 4));
	H2H.HT2HSMF.B.DATA = 1;
	while(H2H.HT2HSMF.B.DATA);

	H2H.HT2HSMS.R = *((uint32_t *)(text + 8));
	H2H.HT2HSMF.B.DATA = 1;
	while(H2H.HT2HSMF.B.DATA);

	H2H.HT2HSMS.R = *((uint32_t *)(text + 12));
	H2H.HT2HSMF.B.DATA = 1;
	while(H2H.HT2HSMF.B.DATA);
}

void hsm2text(uint8_t * text)
{
	H2H.HSM2HTF.B.DATA = 1; /* Reset new data flag */
	H2H.HT2HSMF.B.READ = 1;
	while(H2H.HT2HSMF.B.READ);

	while(H2H.HSM2HTF.B.DATA == 0);
	*((uint32_t *)(text + 0))  = H2H.HSM2HTS.R;
	H2H.HSM2HTF.B.DATA = 1; /* Reset new data flag */

	while(H2H.HSM2HTF.B.DATA == 0);
	*((uint32_t *)(text + 4)) = H2H.HSM2HTS.R;
	H2H.HSM2HTF.B.DATA = 1; /* Reset new data flag */

	while(H2H.HSM2HTF.B.DATA == 0);
	*((uint32_t *)(text + 8)) = H2H.HSM2HTS.R;
	H2H.HSM2HTF.B.DATA = 1; /* Reset new data flag */

	while(H2H.HSM2HTF.B.DATA == 0);
	*((uint32_t *)(text + 12)) = H2H.HSM2HTS.R;
	H2H.HSM2HTF.B.DATA = 1; /* Reset new data flag */
}
/********************  End of Main ***************************************/


