/* Entry Point */
ENTRY(_start)

/* define heap and stack size */
__HEAP_SIZE            = 0 ;
__STACK_SIZE           = 4096 ;

SRAM_SIZE =  512K;    
/* Define SRAM Base Address */
SRAM_BASE_ADDR = 0x40020000;
    
MEMORY
{
	m_setkey :  org = 0x40030000, len = 0x100
	m_enc :		org = 0x40030100, len = 0x100
    m_data :	org = 0x40030200, len = 0xFE00
}

