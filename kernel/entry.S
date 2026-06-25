# entry.S - RISC-V kernel entry point
.section .text.init
.global _start

_start:
    # Set up stack (4KB)
    la  sp, stack0
    li  a0, 4096
    add sp, sp, a0

    # Clear BSS
    la  a0, edata
    la  a1, end
1:  bgeu a0, a1, 2f
    sb  zero, 0(a0)
    addi a0, a0, 1
    j    1b
2:
    # Call main()
    call main

    # Should never return
1:  wfi
    j   1b

.section .bss
.align 3
stack0:
    .space 4096
