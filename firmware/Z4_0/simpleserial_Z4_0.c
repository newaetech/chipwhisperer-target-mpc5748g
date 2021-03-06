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
#include "aes.h"

#include "cw308t_mpc5748g.h"
#include "sharedmem.h"
#include "simpleserial.h"

#include "tinyaes128.h"

#define SUPPORT_HSM 0

#ifndef SUPPORT_HSM
#define SUPPORT_HSM 0
#endif

#if SUPPORT_HSM
#include "hsm_interface.h"
#endif

#define KEY_VALUE1 0x5AF0ul
#define KEY_VALUE2 0xA50Ful

typedef void(*ram_func)(void);
typedef void(*aes_func)(uint8_t *);

typedef union {
     uint8_t u8[4];
     uint32_t u32;
     uint8_t *p8;
     uint32_t *p32;
     ram_func f;
} recv_sram ;

aes_func CUSTOM_KEY_FUNC = (aes_func)0x40030000;
aes_func CUSTOM_ENC_FUNC = (aes_func)0x40030100;


//Start in .text section now
__attribute__ ((section(".text")))

/**************************** Serial I/O Functions ************************/

/* Specify UART to use here - need to change init code too if modified */
#define checkchar checkLINFlexD_0

void delay_short(void)
{
     volatile uint32_t i;
     for (i = 0; i < 1000; i++);
}


void flush(void)
{
	flushLINFlexD_0();
}

/** Enable printf() - these are lazy routines to clip into EWL rather than use the
 *  correct callbacks. They work for now though. */

/************************* Simpleserial Commands / Functions *****************/

#define MODULE_INVALID   0x00
#define MODULE_SWAES     0x80
#define MODULE_SWAESHSM  0x40
#define MODULE_HWAES     0x20
#define MODULE_XOR       0x10
#define MODULE_MBED		 0x01
#define MODULE_CUSTOM	 0x02
uint8_t enc_module = MODULE_SWAES;

uint8_t pwgroup;


uint8_t ascii_to_hex(char ch)
{
     if (ch >= '0' && ch <= '9') {
          return ch - 48;
     } else if (ch >= 'A' && ch <= 'F') {
          return ch - 55;
     } else if (ch >= 'a' && ch <= 'f') {
          return ch - 87;
     }
     return 0;
}

uint32_t ascii_to_u32(char text[8])
{
     uint32_t val = 0;
     for (int i = 0; i < 8; i++) {
          val |= ascii_to_hex(text[i]) << (4 * (7 - i));
     }
     return val;
}

uint8_t ascii_to_u8(char text[2])
{
     uint8_t val = 0;
     for (int i = 0; i < 2;i ++) {
          val |= ascii_to_hex(text[i]) << (4 * (1 - i));
     }
     return val;
}

uint32_t uart_get_u32(void)
{
     char buf[8] = {0,0,0,0,0,0,0,0};
     char c;
     char started = 0;
     unsigned int i = 0;

     while(1){
    	 c = getch();

    	 //Skip first \n
    	 if(started == 0){
    		 if (c == '\n'){
    			 started = 1;
    			 continue;
    		 }
    	 }
    	 started = 1;

    	 if(c == ' ') continue;

    	 //Check for done
    	 if ((c == '\n') || (c == '\r')) break;

    	 //Store character
    	 buf[i++] = c;

    	 //Check for done
    	 if (i >= 8) break;
     }

     //Pad left
     if (i < 8){
    	 int diff = 8 - i;
    	 for (int j = 7; j >= diff; j--) {
    		 buf[j] = buf[j-diff];
    	 }

    	 for (int j = 0; j < diff; j++) {
    		 buf[j] = '0';
    	 }
     }

     return ascii_to_u32(buf);
}

uint8_t monitor_fast(uint8_t *k)
{
	  flush();
	  LED_ON(LED_BLOAD);

     char ch = 0;
     while (ch != 'q') {
          printf("C\n");
          ch = getch();
          printf("%c\n", ch);

          if (ch == 'u') {
               recv_sram addr;
               recv_sram len;

               flush();
               printf("A\n");
               addr.u32 = uart_get_u32();
               printf("%08x\n", (unsigned int)addr.u32);
               flush();

               printf("L\n");
               len.u32 = uart_get_u32();
               printf("%08x\n", (unsigned int)len.u32);

               printf("B\n");
               for (int i = 0; i < len.u32; i++) {
                    addr.p8[i] = getch();
               }
          } else if (ch == 'd') {
               recv_sram addr;
               recv_sram len;

               flush();
               printf("A\n");
               addr.u32 = uart_get_u32();
               printf("%08x\n", (unsigned int)addr.u32);

               flush();
               printf("L\n");
               len.u32 = uart_get_u32();
               printf("%08x\n", (unsigned int)len.u32);

               printf("r\n");
               while (getch() != 'r');
               for (int i = 0; i < len.u32; i++) {
                    putch(addr.p8[i]);
               }
          } else if (ch == 'e') {
               recv_sram addr;
               flush();
               printf("A\n");
               addr.u32 = uart_get_u32();
               printf("%08x\n", (unsigned int)addr.u32);
               addr.f();
          } else if (ch == 'w') {
               recv_sram addr;
               recv_sram val;

               addr.u32 = uart_get_u32();
               val.u32 = uart_get_u32();
               printf("%08x, %08x\n", (unsigned int)val.u32);

               *addr.p32 = val.u32;
          } else if (ch == 'r') {
               recv_sram addr;
               addr.u32 = uart_get_u32();
               printf("%08x=", (unsigned int)addr.u32);

               uint32_t val = *addr.p32;
               printf("%08x\n", (unsigned int)val);
          }
     }

     LED_OFF(LED_BLOAD);
     return 0;
}

uint8_t monitor_mode(uint8_t *k)
{
     printf("You are now in monitor mode\n");
     flush();
     LED_ON(LED_BLOAD);
     char ch = '?';
     while (ch != 'q') {
    	 if (ch == '?'){
          printf("Commands:\n"
                 "    u: Upload bin to SRAM\n"
                 "    d: Dump SRAM\n"
                 "    e: Execute SRAM\n"
                 "    w 40020000 00001000: Write 0x00001000 to 0x40020000 in SRAM\n"
                 "    r 40020000: Read 32-bit from SRAM 0x40020000\n"
                 "    q: Quit\n");
    	 }

          ch = getch();

          if (ch == 'u') {
               recv_sram addr;
               recv_sram len;

               flush();
               printf("32bit memory location to load to (ascii hex): \n");
               addr.u32 = uart_get_u32();
               printf("Uploading to %08x\n", (unsigned int)addr.u32);
               flush();
               printf("32bit length of bin file (ascii hex): \n");
               len.u32 = uart_get_u32();
               printf("Got %08x\n", (unsigned int)len.u32);

               printf("Send bin: \n");
               for (int i = 0; i < len.u32; i++) {
                    addr.p8[i] = getch();
               }
          } else if (ch == 'd') {
               recv_sram addr;
               recv_sram len;

               flush();
               printf("32bit memory location to load (ascii hex): \n");
               addr.u32 = uart_get_u32();
               printf("Got %08x\n", (unsigned int)addr.u32);
               flush();
               printf("32bit length of memory (ascii hex): \n");
               len.u32 = uart_get_u32();
               printf("Got %08x\n", (unsigned int)len.u32);

               printf("Send r when ready\n");
               while (getch() != 'r');
               for (int i = 0; i < len.u32; i++) {
                    putch(addr.p8[i]);
               }
          } else if (ch == 'e') {
               recv_sram addr;

               flush();
               printf("Specify 32bit memory location to execute (ascii hex): \n");
               addr.u32 = uart_get_u32();

               printf("Executing memory location %08x\n", (unsigned int)addr.u32);
               addr.f();
          } else if (ch == 'w') {
               recv_sram addr;
               recv_sram val;
               addr.u32 = uart_get_u32();
               val.u32 = uart_get_u32();

               printf("Writing %08x to %08x\n", (unsigned int)val.u32, (unsigned int)addr.u32);
               *addr.p32 = val.u32;
          } else if (ch == 'r') {
               recv_sram addr;

               addr.u32 = uart_get_u32();
               printf("Memory at %08x =", (unsigned int)addr.u32);

               uint32_t val = *addr.p32;
               printf(" %08x\n", (unsigned int) val);
          } else if ((ch == '\n') || (ch == '\r') || (ch == 'q')){
        	  ;
          } else {
        	  printf("Unknown command '%c'\n", ch);
        	  ch = '?';
          }
     }

     LED_OFF(LED_BLOAD);

     return 0;
}
mbedtls_aes_context mbed;
int mbed_init = 0;

uint8_t set_module(uint8_t* mod)
{
     enc_module = mod[0]; //& 0xF0;
     //needed?
     if (mbed_init) {
          mbedtls_aes_free(&mbed);
     }

     if (enc_module == MODULE_HWAES){
          hsm_setkeyidx(mod[0] & 0x0F);
     } else if (enc_module == MODULE_SWAES){
          ;
     } else if (enc_module == MODULE_SWAESHSM){
          ;
     } else if (enc_module == MODULE_XOR){
          ;
     } else if (enc_module == MODULE_MBED) {
          mbedtls_aes_init(&mbed);
     } else if (enc_module == MODULE_CUSTOM) {
          ;
     } else {
          printf("DEBUG: INVALID MODULE %02x\n", mod[0]);
          return 0xff;
     }

     return 0;
}

uint8_t xor_key[16];

uint8_t get_key(uint8_t* k)
{
     if (enc_module == MODULE_HWAES){
#if SUPPORT_HSM
          hsm_setkey(k);
#else
          printf("HSM NOT SUPPORTED\n");
#endif
     } else if (enc_module == MODULE_SWAES){
          AES128_ECB_tinyaes_setkey(k);
     }
     else if (enc_module == MODULE_XOR){
          memcpy((void *)xor_key, (void *)k, 16);
     } else if (enc_module == MODULE_MBED){
          mbedtls_aes_setkey_enc(&mbed, k, 128);

     } else if (enc_module == MODULE_CUSTOM) {
          if (*(uint32_t *)CUSTOM_KEY_FUNC == 0x00000000) {
               printf("Illegal instruction where start of key function expected\n");
               return 0xFF;
          }
          CUSTOM_KEY_FUNC(k);
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
          trigger_high();
          AES128_ECB_tinyaes_crypto(pt);
          trigger_low();
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
          trigger_high();
          for(unsigned int i = 0; i < 16; i++){
        	  pt[i] ^= xor_key[i];
          }
          trigger_low();
     } else if (enc_module == MODULE_MBED) {
#if SUPPORT_HSM
          hsm_release_gpio();
#endif
          trigger_high();
          mbedtls_aes_crypt_ecb(&mbed, MBEDTLS_AES_ENCRYPT, pt, pt);
          trigger_low();
     } else if (enc_module == MODULE_CUSTOM) {
          if (*(uint32_t *)CUSTOM_ENC_FUNC == 0x00000000) {
               printf("Illegal instruction where start of encryption function expected\n");
               return 0xFF;
          }
#if SUPPORT_HSM
          hsm_release_gpio();
#endif
          trigger_high();
          CUSTOM_ENC_FUNC(pt);
          trigger_low();
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

     trigger_high();

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

     trigger_low();
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


uint8_t glitch_example2()
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

          trigger_high();
          for(i = 0; i < 500; i++){
               for(j = 0; j < 500; j++){
                    totalcnt++;
                    test++;
               }
               trigger_low();

               if (j != 500){
                    printf("GLITCH 1\n");
                    trigger_low();
                    return 0x00;
               }

               if ((i % 50) == 0){
                    trigger_high();
               }

          }
          if ((i != 500) || (totalcnt != 250000) || (test != 250000)) {
               printf("GLITCH 2\n");
               printf("%u %u %u %u %u\n", i, j, totalcnt, test, lcnt++);
               trigger_low();
               return 0x00;
          }
          trigger_low();
          //printf("%u %u %u %u %u\n", i, j, totalcnt, test, lcnt++);
     }

     printf("You broke out of the loop!\n");

     return 0x00;
}


uint8_t glitch_example()
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

          trigger_high();
          for(i = 0; i < 1000; i++){
               for(j = 0; j < 1000; j++){
                    totalcnt++;
                    test++;
               }
               trigger_low();

               if ((i % 50) == 0){
                    trigger_high();
               }

          }
          printf("%u %u %u %u %u\n", i, j, totalcnt, test, lcnt++);
          trigger_low();
     }

     printf("You broke out of the loop!\n");

     return 0x00;
}


void read_flash(void)
{
#if SUPPORT_HSM
     hsm_release_gpio();
#endif

     printf("Entering flash glitch loop...\n");
     uint32_t i = 0;

     while(1){
          i++;
          trigger_high();
          uint32_t * rv = (uint32_t *)(0x00400200);
          volatile uint32_t val = *rv;
          if (val != 0x55AA50AF) {
               printf("GLITCH: %x", (unsigned int)val);
          }
          trigger_low();
          if ((i % 100000) == 0) {
               printf("%x\n", (unsigned int)val);
          }
     }
}

uint8_t glitch_call(uint8_t *data)
{
     printf("Call glitch %hhu\n", *data);
     switch (*data) {
     case 1:
          glitch_example();
          break;
     case 2:
          glitch_example2();
          break;
     case 3:
          read_flash();
          break;
     default:
    	  printf("Invalid glitch call\n");

     }
     return 0;
}

void turn_on_cores()
{
     MC_ME.CCTL[2].R = 0x00FE;
     /* Set Start address for core 1: Will reset and start */
     MC_ME.CADDR[2].R = 0x11d0000 | 0x1;
     /* enable core 2 in all modes */
     MC_ME.CCTL[3].R = 0x00FE;
     /* Set Start address for core 2: Will reset and start */
     MC_ME.CADDR[3].R = 0x13a0000 | 0x1;

     //turn cores on
     MC_ME.MCTL.R = 0x50005AF0;
     MC_ME.MCTL.R = 0x5000A50F;

     while(MC_ME.GS.B.S_MTRANS == 1);      /* Wait for mode transition complete */
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

     platform_init();
     //Optional - enable clock out (will increase noise so not on by default, not needed to our clkin anyway)
     //clock_out_FMPLL();          /* Pad PG7 = CLOCKOUT = PLL0/10 */

     delay_short();
     init_uart();
     printf("CW308T-MPC5748G Online. Firmware compile date: %s : %s", __DATE__, __TIME__);
     printf(" See http://cwdocs.com/mp57 for documentation\n");
     printf("Activate cores 1 and 2? [y/N]\n");
     if (getch() == 'y') {
          turn_on_cores();
     }
     delay_short();
     printf(" Device information:\n");
     printf("   Lifecycle from LCSTAT: %u (%s)\n", PASS.LCSTAT.B.LIFE, lcstatstring[PASS.LCSTAT.B.LIFE]);
     printf("   Censorship status from LCSTAT: %d\n", PASS.LCSTAT.B.CNS);
     printf("   Firmware HSM support: %s\n", SUPPORT_HSM ? "enabled":"disabled");
     printf("See command list at http://cwdocs.com/mp57\n");

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
     simpleserial_addcmd('p', 16,  get_pt);
     simpleserial_addcmd('q',  32,  set_pw);
     simpleserial_addcmd('h', 1, set_module);
     simpleserial_addcmd('w', 1, set_pwgroup);
     simpleserial_addcmd('l', 0, set_pwlock);
     simpleserial_addcmd('j', 4, core_example);
     simpleserial_addcmd('g', 1, glitch_call);
     simpleserial_addcmd('m', 0, monitor_mode);
     simpleserial_addcmd('n', 0, monitor_fast);

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

                    trigger_high();
                    pwok = 0x00;
                    for(i =0; i < 4; i++){
                         pwok |= (correct_pw[i] ^ RxDATA[i]);
                    }
                    trigger_low();

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


     return 0;
}

/********************  End of Main ***************************************/


/******************** Machine Check Exception ***************************/
