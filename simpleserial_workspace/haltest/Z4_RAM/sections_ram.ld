/*
** ###################################################################
**     Processor:           MPC5748G with 256 KB m_data
**     Compiler:            GNU C Compiler
**
**     Abstract:
**         Linker file for the GNU C Compiler
**
**     Copyright 2017 NXP
**     All rights reserved.
**
**     THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
**     IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
**     OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
**     IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
**     INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
**     (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
**     SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
**     HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
**     STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
**     IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
**     THE POSSIBILITY OF SUCH DAMAGE.
**
**     http:                 www.nxp.com
**
** ###################################################################
*/

SECTIONS
{

     /*========================== Text ==========================*/


    .set_key :
    {
    	KEEP(*(.set_key))
    } > m_setkey

    .enc :
    {
    	KEEP(*(.enc))
    } > m_enc

    .text :
    {
        *(.text.startup)
        *(.text)
        *(.text.*)
        KEEP(*(.init))
        KEEP(*(.fini))
        /* discard boot header in RAM target.*/
        . = ALIGN(16);
    } > m_data

/* migration to version v1.2
   define section PREINIT_ARRAY */

.preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } > m_data


/* end section PREINIT_ARRAY */
/*  migration to version v1.2
    define section INIT_ARRAY*/

  .init_array     :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array ))
    PROVIDE_HIDDEN (__init_array_end = .);
  } > m_data


/* end section INIT_ARRAY */

/* migration to version v1.2
   define section FINI_ARRAY */


  .fini_array     :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array ))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } > m_data



/* end  section FINI_ARRAY */
/* migration to version v1.2
   define section DTORS */

  .dtors          :
  {
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
  } > m_data

/* end section DTORS */
/* migration to version v1.2
   define section CTORS */

  .ctors          :
  {
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    /* We don't want to include the .ctor section from
       the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */
    KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
  } > m_data

/* end section CTORS */


    /* Section for storing functions that needs to execute from RAM */
    .code_ram :
    {
        . = ALIGN(4);
        __CODE_RAM = .;
        *(.code_ram)               /* Custom section for storing code in RAM */
        __CODE_ROM = .;            /* Symbol is used by start-up for data initialization. */
        __CODE_END = .;            /* No copy */
        . = ALIGN(4);
    } > m_data

    .rodata :
    {
        *(.rodata)
        *(.rodata.*)
    } > m_data

    .eh_frame_hdr : { *(.eh_frame_hdr) } > m_data
    .eh_frame     : { KEEP (*(.eh_frame)) } > m_data

    __TEXT_END = .;                 /* Define a global symbol at end of code. */
    __DATA_ROM = .;                 /* Symbol is used by startup for data initialization. */
    __DATA_END = __DATA_ROM;        /* No copy */

    /*========================== DATA ==========================*/
    .data   :
    {
        . = ALIGN(4);
        __DATA_RAM = .;
        __data_start__ = .;         /* Create a global symbol at data start. */
        *(.data)
        *(.data.*)
        *(.sdata2)
        *(.sdata2.*)
        *(.sdata)
        *(.sdata.*)
        . = ALIGN(4);
        __data_end__ = .;           /* Define a global symbol at data end. */
    }  > m_data

    .bss   (NOLOAD)  :
    {
        __BSS_START = .;
        *(.sbss)
        *(.sbss.*)
        *(.bss)
        *(.bss.*)
        *(.sbss2)
        *(.sbss2.*)
        *(COMMON)
        __BSS_END = .;
    } > m_data

    .stack (NOLOAD) : ALIGN(16)
    {
        __HEAP = . ;
        PROVIDE (_end = . );
        PROVIDE (end = . );
        . += __HEAP_SIZE ;
        __HEAP_END = . ;
        _stack_end = . ;
        . +=  __STACK_SIZE ;
        _stack_addr = . ;
        __SP_INIT = . ;
        . += 4;
    } > m_data
    
    .sdata  :
    {
      *(.sdata)
      *(.sdata.*)
    } > m_data
    

/* Labels for Copying Initialised Data from Flash to RAM */
__DATA_SRAM_ADDR  = ADDR(.data);
__SDATA_SRAM_ADDR = ADDR(.sdata);
__DATA_SIZE   = SIZEOF(.data);
__SDATA_SIZE  = SIZEOF(.sdata);
__DATA_ROM_ADDR  = LOADADDR(.data);
__SDATA_ROM_ADDR = LOADADDR(.sdata);
   
/* Labels Used for Initialising SRAM ECC */
__SRAM_SIZE = SRAM_SIZE;
__SRAM_BASE_ADDR = SRAM_BASE_ADDR;
__BSS_SIZE    = __BSS_END - __BSS_START;

}

