TARGET=simpleserial_Z2_3
SRC += simpleserial_Z2_3.c

SECONDARY_CORE = YES
CPUTYPE=e200z2
EXTRA_LDSCRIPTS = mem.ld sections.ld

CRYPTO_TARGET=NONE
CRYPTO_OPTIONS=AES128C
PLATFORM=CW308_MPC5748G

# include ../simpleserial/Makefile.simpleserial
FIRMWAREPATH = ../
include $(FIRMWAREPATH)/Makefile.inc
