// entry.S - Kernel entry point in M-mode
// OpenSBI jumps here at 0x80200000 in S-mode
// We set up per-hart stacks and call start()

.section .text
.globl _entry
_entry:
    // Set up stack: stack0 + hartid * 4096
    la sp, stack0
    li a0, 4096
    csrr a1, mhartid
    addi a1, a1, 1
    mul a0, a0, a1
    add sp, sp, a0

    // Jump to C
    call start

    // Should not return
1:  wfi
    j 1b
