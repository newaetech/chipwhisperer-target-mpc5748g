#include <stdio.h>
#include "mpc5748g_hal.h"
#include "linflexd_uart.h"
#include "can.h"
#include "mode.h"
#include "project.h"
#include "clk.h"

#define TRIG_INIT() SIUL2.MSCR[PI6].B.OBE = 1
#define TRIG_HIGH() SIUL2.GPDO[PI6].B.PDO_4n = 1
#define TRIG_LOW()  SIUL2.GPDO[PI6].B.PDO_4n = 0

extern void xcptn_xmpl(void);

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
          putch(*(buffer++));
          tosend--;
     }

     return (int)__no_io_error;
}

void platform_init(void)
{
     /* Before any other stuff online - setup user I/O */
     LED_SETUP(LED_CORE0);
     LED_SETUP(LED_CLKOK);
     LED_SETUP(LED_HSM);
     LED_SETUP(LED_BLOAD);

     LED_SETUP(LED_USER1);
     LED_SETUP(LED_USER2);
     LED_SETUP(LED_USER3);

     //Show CORE0 is alive (even if clock switch will fail)
     LED_ON(LED_CORE0);
     sharedmem_init();


     xcptn_xmpl (); /* Configure and Enable Interrupts */


     //External clock in = 16 MHz, we will run CPU from that to make SCA easier
     //You can turn on or off the S40 clock domain - it's needed for the PASS module, but not for
     //anything else we use for demos. You can turn off to possibly reduce noise.
     //IF you turn off - be sure to remove talking to PASS module!!!
     system16mhz(1);

     // If we get here - core is probably OK!
     LED_ON(LED_CLKOK);
}

void init_uart(void)
{
     initLINFlexD_0( 4, 38400 );/* Initialize LINFlex0: UART Mode 4 MHz peripheral clock, 38400 Baud */
}
void putch(char c)
{
     txLINFlexD_0(c);
}

char rxchar(void)
{
     return rxLINFlexD_0();
}
char getch(void)
{
     return rxLINFlexD_0();
}

void trigger_setup(void)
{
     TRIG_INIT();
}
void trigger_low(void)
{
     TRIG_LOW();
}
void trigger_high(void)
{
     TRIG_HIGH();
}
