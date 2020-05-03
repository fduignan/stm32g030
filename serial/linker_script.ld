MEMORY
{
    flash : org = 0x08000000, len = 64k  
    ram : org = 0x20000000, len = 8k
}
  
SECTIONS
{
        
	. = ORIGIN(flash);
        .text : {
		  *(.vectors); /* The interrupt vectors */
		  *(.text);

		
		  . = ALIGN(4);
        /* All of the following is necessary for constructors for static/global objects */
        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);      		
        
        
        *(.rodata);
        *(.comment);
        *(.ARM.attributes);        
        
        } >flash
        
	. = ORIGIN(ram);
        .data : {
	  INIT_DATA_VALUES = LOADADDR(.data);
	  INIT_DATA_START = .;
	    *(.data);
	  INIT_DATA_END = .;
	  . = ALIGN(4);
        } >ram AT>flash
	BSS_START = .;
	.bss : {	  
	    *(.bss);
	    . = ALIGN(4);
	} > ram
	BSS_END = .;
	
}
