# entry.S - RISC-V kernel entry point
# This is where QEMU starts executing after loading kernel at 0x80000000

.section .text.init
.global _start

_start:
    # Set up a stack for C code.
    # Stack pointer grows downward, place it at end of 4KB page
    la      sp, stack0
    li      sp, 4096
    add     sp, sp, sp

    # Jump to start() in start.c
    call    start

    # Should never return from start()
1:  wfi
    j       1b

# Allocate 4KB of initial stack space
.section .bss
.align   3
stack0:
    .space 4096
