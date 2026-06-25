# kernelvec.S - Kernel trap vector for ZephyrOS
# Called when a trap/interrupt occurs while in kernel mode (S-mode)
#
# Saves registers on the stack, calls kerneltrap(), then restores.

.globl kernelvec

kernelvec:
    # Save caller-saved registers on the kernel stack
    addi    sp, sp, -256     # Allocate trap frame space

    sd      ra,   0(sp)
    sd      sp,   8(sp)
    sd      gp,  16(sp)
    sd      tp,  24(sp)
    sd      t0,  32(sp)
    sd      t1,  40(sp)
    sd      t2,  48(sp)
    sd      s0,  56(sp)     # s0 = fp
    sd      s1,  64(sp)
    sd      a0,  72(sp)
    sd      a1,  80(sp)
    sd      a2,  88(sp)
    sd      a3,  96(sp)
    sd      a4, 104(sp)
    sd      a5, 112(sp)
    sd      a6, 120(sp)
    sd      a7, 128(sp)
    sd      s2, 136(sp)
    sd      s3, 144(sp)
    sd      s4, 152(sp)
    sd      s5, 160(sp)
    sd      s6, 168(sp)
    sd      s7, 176(sp)
    sd      s8, 184(sp)
    sd      s9, 192(sp)
    sd     s10, 200(sp)
    sd     s11, 208(sp)

    # Call kerneltrap() - it will handle the trap
    call    kerneltrap

    # Restore registers from stack
    ld      ra,   0(sp)
    ld      sp,   8(sp)
    ld      gp,  16(sp)
    ld      tp,  24(sp)
    ld      t0,  32(sp)
    ld      t1,  40(sp)
    ld      t2,  48(sp)
    ld      s0,  56(sp)
    ld      s1,  64(sp)
    ld      a0,  72(sp)
    ld      a1,  80(sp)
    ld      a2,  88(sp)
    ld      a3,  96(sp)
    ld      a4, 104(sp)
    ld      a5, 112(sp)
    ld      a6, 120(sp)
    ld      a7, 128(sp)
    ld      s2, 136(sp)
    ld      s3, 144(sp)
    ld      s4, 152(sp)
    ld      s5, 160(sp)
    ld      s6, 168(sp)
    ld      s7, 176(sp)
    ld      s8, 184(sp)
    ld      s9, 192(sp)
    ld     s10, 200(sp)
    ld     s11, 208(sp)

    addi    sp, sp, 256       # Free trap frame

    sret                     # Return from supervisor trap
