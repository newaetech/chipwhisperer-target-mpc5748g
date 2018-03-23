# MPC5748G Firmware
This repo contains the source/project files needed to build firmware for the MPC5748G target board. It can be used with the standard ChipWhisperer build system.

## S32 Project Info
To import into S32DS, open the Simpleserial_Workspace folder as a "Project from Another Filesystem". It should contain 5 (4 after Z0 is removed?) projects inside:

* Simpleserial_Z4_1: Firmware for Core 0 and the main firmware. This runs all of the AES, glitch, and simpleserial stuff.
* Simpleserial_Z4_2: Firmware for Core 1. This is mainly used to show off communication between cores.
* Simpleserial_Z2_1: Firmware for Core 2 (the Z2 core). This simply turns on the Core 2 LED.
* Simpleserial_Ram: Firmware that can be sent over UART, loaded into RAM, and executed. See CUSTOM_AES for more details

## ChipWhisperer Build System
### Prerequisites
The ChipWhisperer build system must first be setup with the location of the build tools. Since the MPC5748G uses vle instructions, which aren't supported on the main powerpc-gcc branch, the build tools shipped with S32DS must be used and the build system must be told where these build tools are located. To do this, edit haltest/hal/mpc5748g/Makefile.mpc5748g and change the variable S32PATH to the install location of S32DS. The default install location of S32DS is c:/nxp/S32DS_Power_v2017.R1 and is the default value of S32PATH.

Once this makefile is updated, you should be able to build as normal with `make PLATFORM=CW308_MPC5748G CRYPTO_TARGET=NONE`.

### Building for Multiple Cores
By default (as in what would normally be used for building ChipWhisperer firmware), the build system will build for Core 1, which has different requirements than building for cores 1 or 2. To build for cores 1 and 2, additional linker scripts should be provided to prevent the cores from using the same flash and RAM. These should be specified by the variable EXTRA_LDSCRIPTS. In addition, the HAL should be told that these are secondary cores by setting the variable SECONDARY_CORE to YES.

Because core 2 is a e200z2 core (cores 0 and 1 are e200z4), to build for core2, you should set CPUTYPE=e200z2 in your makefile.

For examples on the above, see the folders Z4_1 (for core 1), and Z2_2 (for core 2).

### Building for SRAM
Like when building for multiple cores, additional steps must be taken to build for SRAM (for loading via monitor mode, for example). Like with cores 1 and 2, extra linker scripts need to be provided to put the code into SRAM. The variables SECONDARY_CORE and NOSTARTUP should both be set to YES. 

To get a binary file for use with monitor mode, the makefile must include a target to build the binary file. An example, which also shows the correct options, linker scripts, and example custom AES program that can be extended, is shown in the Z4_RAM folder. This makefile will generate a binary file along with disassembly of the binary file when `make binary` is run. If you want to build for SRAM, using this build folder is the easiest way.

### Simpleserial Note
Note that this repo uses a slightly modified simpleserial. In the simpleserial provided with the rest of the ChipWhisperer firmware, the max number of simpleserial commands is 9 (plus the 'v' command). As the default firmware uses 10 commands, simpleserial.c was modified to allow for 19 commands.

## Firmware Description
### Communication
UART communication is done at 38400 baud and CAN communication is done with Standard IDs at 500kbps.

### Startup
On startup, the user will be prompted about activating cores 1 and 2. Activating them allows simpleserial command 'j' to be used, while leaving them off makes CPA attacks easier.

### Simpleserial Commands
* k: 16 bytes: Load a new AES key
  * arg: key to be loaded
* p: 16 bytes: Encrypts plaintext and returns encrypted data
  * arg: plaintext to be encrypted
* q: 32 bytes: Unlock password group
  * arg: password to use to unlock the group
* h: 1 byte: Sets AES library used for encryption
  * arg = 
    * 0x01: Use MBEDTLS (very difficult)
    * 0x02: Use custom AES loaded into RAM
    * 0x10: Use XOR with key
    * 0x80: Use tinyaes128 (moderate)
* w: 1 byte: Select the password group to be used with 'q' and 'l'
* l: 0 bytes: Check the lock status of the password group
* j: 4 bytes: Send and receive data from Core 1
* g: 1 byte: Call glitch
  * arg: = 
    * 0x01: Use glitch 1
    * 0x02: Use glitch 2
    * 0x03: Use flash glitch
* m: 0 bytes: Enter monitor mode (See monitor mode)
* n: 0 bytes: Enter monitor fast mode (See monitor mode)

### Monitor Mode
This firmware does not contain a bootloader. Instead, arbitrary code can be loaded into RAM and executed using monitor mode (Simpleserial command m). All numbers sent should be ASCII hex and will be padded with 0s if a full 32bit number is not given (10 will become 00000010). Commands are terminated in the same way as regular simpleserial commands (with an '\r' or a '\n'). Monitor mode can be used for the following:

* (u)pload to RAM: Upload a binary file to RAM. The user will be prompted to enter the desired memory location and the length of the binary
* (d)ump RAM: Dump RAM over serial. The user will be prompted for the address and amount of memory to dump. The user will be asked to send an 'r' before the memory will be sent.
* (e)xecute RAM: Jump to RAM and execute. The user will be prompted for the address to jump to
* (w)rite RAM: Write 4 bytes to RAM. Give the address and value with the command (so w 40030000 11200000)
* (r)ead RAM: Read 4 bytes from RAM. Give the address with the command (so r 40030000)
* (q)uit: Go back to looking for simpleserial commands

ChipWhisperer software currently does not have a way to transfer files over UART. Instead, a terminal emulator program like Tera Term or RealTerm can be used to send files.

#### Fast Monitor Mode
A fast monitor mode is included to make automated use of monitor mode (via a script, for example) easier/more reliable. Instead of giving the long prompts like regular monitor mode, fast monitor mode will write "C\n" when it expects a command, "A\n" when it expects an address, "L\n" when it expects a length, "B\n" when it expects a binary file, and "V\n" when it expects a value. Additionally, it will echo back commands and numbers as ASCII hex (lowercase).

### Custom AES
To make using custom AES programs that the user has uploaded to SRAM (via monitor mode) easier, a custom AES library can be selected by simpleserial command 'h02'. If this is used, address 0x40030000 will be executed as a key function and address 0x40030100 will be executed as an encrypt function. The easiest way to make sure your functions work is to use the Simpleserial_Ram project. Replace user_setkey and user_enc with your own function to set the AES key and encrypt. After that, build the project and upload the resulting .bin file using monitor mode.

### Password Commands
To show off the password group features of the MPC5748G (and for CPA attacks on the module), 3 simpleserial commands are included: 'q' for transmitting passwords for locking and unlocking purposes, 'w' to select which password group should be used, and 'l' to check the lock status of the group. For more information, including which passwords are used for the different groups, see docs/password.md.

### CAN
This firmware also has a simple CAN demo loaded. It expects a message with ID 0x555 and a 4 byte password. It will respond with DE00 if the password is wrong and DE01 if the password is correct. The default password is DEADBEEF.


## Uploading New Firmware
New firmware may be uploaded either by a PowerPC compatable JTAG programmer (Flash or SRAM), or by using monitor mode (SRAM only). A batch file and a config file are included in the simpleserial_workspace/ directory for use with the PEMicro Universal FX and CPROGPPCNEXUS.

## Device Setup
The MPC5748G expects a 16MHz input clock signal. Without this, simpleserial and CAN won't work. This can be done via a crystal on the X1 pins of the CW308, or by feeding in a 16MHz clock signal (say from a CWLite).

You can check if the MPC5748G is getting the correct clock signal by checking the CLK LED on the board. If it's on, everything should be functioning correctly.

The MPC5748G board requires 5V and 3.3V to function. This (may?) requires use of the external 5V power jack on the CW308.
