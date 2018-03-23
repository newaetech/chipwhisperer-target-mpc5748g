#ifndef MPC5748G_HAL_H
#define MPC5748G_HAL_H
void platform_init(void);
void init_uart(void);
void putch(char c);
char getch(void);

void trigger_setup(void);
void trigger_low(void);
void trigger_high(void);
#endif
