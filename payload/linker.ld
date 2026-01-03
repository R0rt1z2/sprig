ENTRY(_start)

SECTIONS
{
    . = 0x62F00000;
    
    .text : {
        KEEP(*(.text.entry))
        *(.text*)
    }
    
    .rodata : { *(.rodata*) }
    .data : { *(.data*) }
    
    .bss : ALIGN(16) {
        __bss_start = .;
        *(.bss*)
        *(COMMON)
        __bss_end = .;
    }
    
    . = ALIGN(16);
    __stack_start = .;
    .stack : {
        . = . + 0x10000;  /* 64KB should be enough? idk */
    }
    __stack_end = .;
}