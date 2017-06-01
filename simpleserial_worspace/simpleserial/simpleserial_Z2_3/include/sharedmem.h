#ifndef _SHAREDMEM_H_
#define _SHAREDMEM_H_


#define GATE_UNLOCK 0
#define CORE0_LOCK  1
#define CORE1_LOCK 	2
#define CORE2_LOCK 	3
#define GATE_0  	0
#define GATE_1  	1
#define GATE_2  	2

#define stringify(s) tostring(s)
#define tostring(s) #s
#define mfspr(rn) ({unsigned int rval; __asm__ volatile("mfspr %0," stringify(rn) : "=r" (rval)); rval;})
#define mtspr(rn, v)   __asm__ volatile("mtspr " stringify(rn) ",%0" : : "r" (v))

/* Access to shared ram as 32-bit array */
extern volatile uint32_t * sharedram_uint32;
extern volatile uint8_t  * sharedram_uint8;

uint8_t Get_Gate_status(uint8_t gate_no);
uint8_t Lock_Gate(uint8_t gate_no);
uint8_t Reset_Gate(uint8_t gate_no);
uint8_t Unlock_Gate(uint8_t gate_no);


#endif
