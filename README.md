# MPC5748G Firmware
This repo contains the source/project files needed to build firmware for the MPC5748G target board. It is designed to be used with S32 Design Studio (S32DS) (for now).

## S32 Project Info
To import into S32DS, open the Simpleserial_Workspace folder as a "Project from Another Filesystem". It should contain 5 (4 after Z0 is removed?) projects inside:

* Simpleserial_Z4_1: Firmware for Core 0 and the main firmware. This runs all of the AES, glitch, and simpleserial stuff.
* Simpleserial_Z4_2: Firmware for Core 1. This is mainly used to show off communication between cores.
* Simpleserial_Z2_1: Firmware for Core 2 (the Z2 core). This simply turns on the Core 2 LED.
* Simpleserial_Ram: Firmware that can be sent over UART, loaded into RAM, and executed. See CUSTOM_AES for more details

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
* q: 32 bytes: Sets the password
  * arg: password to set
* h: 1 byte: Sets AES library used for encryption
  * arg = 
    * 0x01: Use MBEDTLS (very difficult)
    * 0x02: Use custom AES loaded into RAM
    * 0x80: Use tinyaes128 (moderate)
* w: 1 byte: set_pwgroup
* l: 0 bytes: set_pwlock
* j: 4 bytes: Send and receive data from Core 1
* g: 1 byte: Call glitch
  * arg: = 
    * 0x01: Use glitch 1
    * 0x02: Use glitch 2
    * 0x03: Use flash glitch
* m: 0 bytes: Enter monitor mode (See monitor mode)
* n: 0 bytes: Enter monitor fast mode (See monitor mode)

### Monitor Mode
This firmware does not contain a bootloader. Instead, arbitrary code can be loaded into RAM and executed using monitor mode (Simpleserial command m). All numbers sent should be ASCII hex. No newlines or additional characters should be sent unless prompted. Monitor mode can be used for the following:

* (u)pload to RAM: Upload a binary file to RAM. The user will be prompted to enter the desired memory location and the length of the binary
* (d)ump RAM: Dump RAM over serial. The user will be prompted for the address and amount of memory to dump
* (e)xecute RAM: Jump to RAM and execute. The user will be prompted for the address to jump to
* (w)rite RAM: Write 1 byte to RAM. The user will be prompted for the address to write to and the value to write
* (r)ead RAM: Read 1 byte from RAM. The user will be prompted for the address to read from.
* (q)uit: Go back to looking for simpleserial commands

ChipWhisperer software currently does not have a way to transfer files over UART. Instead, a terminal emulator program like Tera Term or RealTerm can be used to send files.

#### Fast Monitor Mode
A fast monitor mode is included to make automated use of monitor mode (via a script, for example) easier/more reliable. Instead of giving the long prompts like regular monitor mode, fast monitor mode will write "C\n" when it expects a command, "A\n" when it expects and address, "L\n" when it expects a length, "B\n" when it expects a binary file, and "V\n" when it expects a value. Additionally, it will echo back commands and numbers as ASCII hex (lowercase).

### Custom AES
To make using custom AES programs that the user has uploaded to SRAM (via monitor mode) easier, a custom AES library can be selected by simpleserial command 'h02'. If this is used, address 0x40030000 will be executed as a key function and address 0x40030100 will be executed as an encrypt function. The easiest way to make sure your functions work is to use the Simpleserial_Ram project. Replace user_setkey and user_enc with your own function to set the AES key and encrypt. After that, build the project and upload the resulting .bin file using monitor mode.

## Uploading New Firmware
New firmware may be uploaded either by a PowerPC compatable JTAG programmer (Flash or SRAM), or by using monitor mode (SRAM only). A batch file and a config file are included in the simpleserial_workspace/ directory for use with the PEMicro Universal FX and CPROGPPCNEXUS.
