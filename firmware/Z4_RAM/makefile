TARGET = simpleserial_Z4_RAM
SRC += simpleserial_Z4_RAM.c

CPUTYPE=e200z4
EXTRA_LDSCRIPTS = mem.ld sections_ram.ld
SECONDARY_CORE = YES
NOSTARTUP = YES

CRYPTO_TARGET=NONE
CRYPTO_OPTIONS=AES128C
PLATFORM=CW308_MPC5748G
# COMMON += -fPIC

#include ../simpleserial/Makefile.simpleserial
FIRMWAREPATH = ../
include $(FIRMWAREPATH)/Makefile.inc


binary: $(TARGET-PLAT).bin

%.bin: %.elf
	$(OBJCOPY) -O binary $< $@
	$(OBJDUMP) -mpowerpc -Me200z4 -D -b binary -EB $@ > disbin.S
	#$(OBJDUMP) -mpowerpc -Me200z4 -D -EB $< > diself.S
