.section .text.entry
.global _start

_start:
    ldr     x0, =__stack_end
    mov     sp, x0
    
    bl      main

1:  
    wfe
    b       1b