#include "derivative.h"
#include "project.h"
#include "sharedmem.h"

//See http://www.nxp.com/assets/documents/data/en/application-notes/AN4805.pdf

#define GetCoreID() ((uint16_t) mfspr(286))

extern uint32_t __shared_mem[];
volatile uint32_t * sharedram_uint32 = __shared_mem;
volatile uint8_t * sharedram_uint8 = __shared_mem;

/*******************************************************************************
Function Name : Get_Gate_status
Engineer      : taken from AN4805
Date          : Feb-29-2016
Parameters    :
Modifies      :
Returns       :
Notes         : return status of the selected Gate
Issues        :
*******************************************************************************/
uint8_t Get_Gate_status(uint8_t gate_no)
{
	return SEMA42.GATE[gate_no].R;
}


/*******************************************************************************
Function Name : Lock_Gate
Engineer      : taken from AN4805
Date          : Feb-29-2016
Parameters    :
Modifies      :
Returns       :
Notes         : Lock selected Gate
Issues        :
*******************************************************************************/
uint8_t Lock_Gate(uint8_t gate_no)
{
	SEMA42.GATE[gate_no].R = GetCoreID()+1;

	return Get_Gate_status(gate_no);
}


/*******************************************************************************
Function Name : Unlock_Gate
Engineer      : taken from AN4805
Date          : Feb-29-2016
Parameters    :
Modifies      :
Returns       :
Notes         : Unlock selected Gate
Issues        :
*******************************************************************************/
uint8_t Unlock_Gate(uint8_t gate_no)
{
	SEMA42.GATE[gate_no].R = GATE_UNLOCK;

	return Get_Gate_status(gate_no);
}


/*******************************************************************************
Function Name : Reset_Gate
Engineer      : taken from AN4805
Date          : Feb-29-2016
Parameters    :
Modifies      :
Returns       :
Notes         : Reset selected Gate
Issues        :
*******************************************************************************/
uint8_t Reset_Gate(uint8_t gate_no)
{
	uint8_t cnt = 0;

	SEMA42.RSTGT.W.R = (0xE2 << 8);

	while(SEMA42.RSTGT.R.B.RSTGSM != 0x01)
	{
		if(cnt++>10) return 1;
	}

	SEMA42.RSTGT.W.R = (0x1D<<8) | gate_no;

	return 0;
}

/*******************************************************************************
Function Name : Shared Nen unut
Engineer      :
Date          : May 31 / 2017
Parameters    :
Modifies      :
Returns       :
Notes         : Hard-coded memory location of 040080000 - make sure matches mem.ld!!!
Issues        :
*******************************************************************************/
void sharedmem_init(void)
{
	uint32_t tmp;

	/* Shared data memory - disable cache over this section */
	SMPU_1.RGD[0].WORD0.R = 0x40080000;
	SMPU_1.RGD[0].WORD1.R = 0x4008FFFF;
	SMPU_1.RGD[0].WORD2.FMT0.R=0xff00fc00;
	SMPU_1.RGD[0].WORD3.B.CI=1;
	SMPU_1.RGD[0].WORD5.B.VLD=1;

	/* Must invalidate cache to ensure anything is flushed, set DCINV bit in L1CSR0. This
	 * requires use of special MTSPR/MFSPR instructions for access. */
	tmp = mfspr(1010);
	mtspr(1010, tmp | 0x02);
}
