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
#include "project.h"

#include "cw308t_mpc5748g.h"
#include "sharedmem.h"
#include "tinyaes128.h"

void user_setkey(uint8_t *k);
void user_enc(uint8_t *pt);

__attribute__ ((section(".set_key")))
void call_setkey(uint8_t *k)
{
	user_setkey(k);
}

__attribute__ ((section(".enc")))
void call_enc(uint8_t *pt)
{
	user_enc(pt);
}
//Start in .text section now
__attribute__ ((section(".text")))

void user_setkey(uint8_t *k)
{
	//your AES setkey here
	LED_SETUP(LED_USER1);
	LED_ON(LED_USER1);
}

void user_enc(uint8_t *pt)
{
	//your AES enc here
	LED_SETUP(LED_USER2);
	LED_TOGGLE(LED_USER2);
}
/**************************** Serial I/O Functions ************************/

/************************************ Main ***********************************/
//int main(void)
int main(void)
{
	user_setkey(0);
	return 0;
}

/********************  End of Main ***************************************/


/******************** Machine Check Exception ***************************/
