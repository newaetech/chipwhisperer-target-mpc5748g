# MPC5748G Firmware
This repo contains the source/project files needed to build firmware for the MPC5748G target board. It is designed to be used with S32 Design Studio (S32DS).

## S32 Project Info
To import into S32DS, open the Simpleserial_Workspace folder as a "Project from Another Filesystem". It should contain 5 (4 after Z0 is removed?) projects inside:

* Simpleserial_Z4_1: Firmware for Core 0 and the main firmware. This runs all of the AES, glitch, and simpleserial stuff.
* Simpleserial_Z4_2: Firmware for Core 1. This is mainly used to show off communication between cores.
* Simpleserial_Z2_1: Firmware for Core 2 (the Z2 core**. This simply turns on the Core 2 LED.
* Simpleserial_Ram: Firmware that can be sent over UART, loaded into RAM, and executed. See CUSTOM_AES for more details

## Startup


## Simpleserial Commands
* k: 16 bytes: Load a new AES key
  * arg: key to be loaded
* p: 16 bytes: Encrypts plaintext and returns encrypted data
  * arg: plaintext to be encrypted
* q: 32 bytes: Sets the password
  * arg: password to set
* h: 1 byte: Sets AES library used for encryption
  * arg = 
    * 0x01: Use MBEDTLS
    * 0x02: Use custom AES loaded into RAM
    * 0x80: Use tinyaes128
* w: 1 byte: set_pwgroup
* l: 0 bytes: set_pwlock
* j: 4 bytes: Send and receive data from Core 1
* g: 1 byte: Call glitch
  * arg: = 
    * 0x01: Use glitch 1
    * 0x02: Use glitch 2
    * 0x03: Use flash glitch
* m: 0 bytes: Enter monitor mode (See monitor mode)

## Monitor Mode
This firmware does not contain a bootloader. Instead, arbitrary code can be loaded into RAM and executed using monitor mode (Simpleserial command m). All numbers sent should be ASCII hex. No newlines or otherMonitor mode can be used for the following:

* (u)pload to RAM: Upload a binary file to RAM. The user will be prompted to enter the desired memory location and the length of the binary
* (d)ump RAM: Dump RAM over serial. The user will be prompted for the address and amount of memory to dump
* (e)xecute RAM: Jump to RAM and execute. The user will be prompted for the address to jump to
* (w)rite RAM: Write 1 byte to RAM. The user will be prompted for the address to write to and the value to write
* (r)ead RAM: Read 1 byte from RAM. The user will be prompted for the address to read from.
* (q)uit: Go back to looking for simpleserial commands

ChipWhisperer software currently does not have a way to transfer files over UART. Instead, a terminal emulator program like Tera Term or RealTerm can be used to send files.

## Custom AES

