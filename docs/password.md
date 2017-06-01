# PASS Module (Password Locking) #

Device censorship / FLASH locking is done via 4 password group register sets.

Each group has 4 registers (LOCK0-LOCK3). Each LOCKn register sets locks on various sections of memory. Setting a bit to '0' unlocks a certain section for read or write.

Each LOCKn register across groups are OR'd together. To totally unlock flash you would set all registers in all groups to 0.

A separate password is set for each group. This allows for example you to have a high-security segment that is never unlocked, while still having another password protect the entire flash "in general" (but is loaded automatically at boot).

## Using PASS Module ##

In order to use/test this module, a few things must happen:

* Program UTEST flash to set default PASS settings via DCF records. Note if you don't do this they will default to 1's (LOCKED)
* Program passwords for each group so they can be unlocked.
* Set device lifecycle to "OEM Production", as password check module cannot be used before that.

## Censorship Setup

Note a seperate DCF record needs to be setup if you wish device to be UNCENSORED. Once moving to OEM Production the device is censored by default - this will mean the JTAG interface is locked.

## Example Module Setup ##

The example here configures the PASS module in a method that can be experimented with, but doesn't by default lock anything. Note it's possible to change this by appending DCF records.

* All flash segments unlocked for erase/write & read
* Censorship DISABLED
* Passwords set for JTAG & Group 0-3 (can be used to change settings, but by default not needed).

The following passwords are as words fed into CIN0..CIN7:


JTAG:

	012FFFFFDEAD2FFFCD401823AAAAAAABFFFFFFFFFFFFFFFF00000000FFFFFFFF

GROUP0:

	0000000000000000FFFFFFFFFFFFFFFF00000000FFFFFFFF0000000000000000

GROUP1:

	01234567FFFFFFFFAAAAAAAA0000000070CA75AAFEEDBEEFCAFEFACEDEADBEEF

GROUP2:

	60f1510019089cd4dfa99f3817361025b7fa518728eb6483dadf3d686d52f9a9	

GROUP3:

	5c3c3dc267b1d8f792f633c51389356c104100f0b52f1aa7f85c2c786d376cf8


## UTest Programming ##

### DCF Records ###

The following DCF records will set all lock-bits to ZERO. Note the first 32-bits are the lock-bit setting for any given lock:

    0x0000000000100100 ;LOCK0_PG0														
    0x0000000000100104 ;LOCK1_PG0														
    0x0000000000100108 ;LOCK2_PG0														
    0x000000000010010C ;LOCK3_PG0														
    0x0000000000100110 ;LOCK0_PG1														
    0x0000000000100114 ;LOCK1_PG1														
    0x0000000000100118 ;LOCK2_PG1														
    0x000000000010011C ;LOCK3_PG1
    0x0000000000100120 ;LOCK0_PG2		
    0x0000000000100124 ;LOCK1_PG2														
    0x0000000000100128 ;LOCK2_PG2														
    0x000000000010012C ;LOCK3_PG2														
    0x0000000000100130 ;LOCK0_PG3														
    0x0000000000100134 ;LOCK1_PG3														
    0x0000000000100138 ;LOCK2_PG3														
    0x000000000010013C ;LOCK3_PG3		

The following DCF record disables censorship:

	0x000055AA001000B0


### Password File ###


The following shows an example .s19 file that can be programmed from address 400120 to 4001C0:

    S214400120FFFFFFFF00000000FFFFFFFFFFFFFFFF96
    S214400130AAAAAAABCD401823DEAD2FFF012FFFFFA2
    S2144001400000000000000000FFFFFFFF000000006E
    S214400150FFFFFFFFFFFFFFFF000000000000000062
    S214400160DEADBEEFCAFEFACEFEEDBEEF70CA75AA91
    S21440017000000000AAAAAAAAFFFFFFFF01234567C6
    S2144001806d52f9a9dadf3d6828eb6483b7fa5187E8
    S21440019017361025dfa99f3819089cd460f1510006
    S2144001A06d376cf8f85c2c78b52f1aa7104100f024
    S2144001B01389356c92f633c567b1d8f75c3c3dc2BF

This passwords in the file are as follows (**NOTE: these need to be word-reversed if feeding into program, see note below**):

JTAG:

	FFFFFFFF00000000FFFFFFFFFFFFFFFFAAAAAAABCD401823DEAD2FFF012FFFFF

GROUP0:

	0000000000000000FFFFFFFF00000000FFFFFFFFFFFFFFFF0000000000000000

GROUP1:

	DEADBEEFCAFEFACEFEEDBEEF70CA75AA00000000AAAAAAAAFFFFFFFF01234567

GROUP2:

	6d52f9a9dadf3d6828eb6483b7fa518717361025dfa99f3819089cd460f15100

GROUP3:

	6d376cf8f85c2c78b52f1aa7104100f01389356c92f633c567b1d8f75c3c3dc2

For the passwords the MSB is written to CIN0, so the order is WORD-REVERSED from as written in the file. For example to unlock GROUP0:

CIN0 = 0x00000000
CIN1 = 0000000000
CIN2 = 0xFFFFFFFF
CIN3 = 0xFFFFFFFF
CIN4 = 0000000000
CIN5 = 0x00000000
CIN6 = 0xFFFFFFFF
CIN7 = 0x00000000

The following lists the passwords in word-reversed order that can be used into the simpleserial example:

JTAG:

	012FFFFFDEAD2FFFCD401823AAAAAAABFFFFFFFFFFFFFFFF00000000FFFFFFFF

GROUP0:

	0000000000000000FFFFFFFFFFFFFFFF00000000FFFFFFFF0000000000000000

GROUP1:

	01234567FFFFFFFFAAAAAAAA0000000070CA75AAFEEDBEEFCAFEFACEDEADBEEF

GROUP2:

	60f1510019089cd4dfa99f3817361025b7fa518728eb6483dadf3d686d52f9a9	

GROUP3:

	5c3c3dc267b1d8f792f633c51389356c104100f0b52f1aa7f85c2c786d376cf8

SIDENOTE: The following code generates the reversed-order thing:

	input_text = "FFFFFFFF00000000FFFFFFFFFFFFFFFFAAAAAAABCD401823DEAD2FFF012FFFFF"
	''.join([input_text[i:(i+8)] for i in range(0,64, 8)][::-1])

### Lifecycle Change ###

Programming the lifecycle means setting the following to address 4000218, which marks the device as entering the "OEM Production" phase (leaving "Customer Delivery"):

	55AA50AF55AA50AF55AA50AF55AA50AF

The following .s19 file can be programmed into address 0x400218 to 0x400228:

	S21440021855AA50AF55AA50AF55AA50AF55AA50AF99

**NOTE BEFORE PROGRAMMING**: Once "OEM Production" is entered, the JTAG & flash passwords take effect. It's a good idea to program a test application before entering this lifecycle. You can also use the debugger to ensure the LOCKn bits are all set to 0 (they are loaded from DCF but have no effect yet, so you can check they were reset to 0). This should only be programmed once you program the passwords, GROUPn defaults, and CENSORSHIP disable (if you don't want censorship on).