// trampoline.S - User/kernel transition code
// This page is mapped at TRAMPOLINE virtual address in both kernel and user page tables.
// uservec: saves user registers to trapframe, loads kernel context, jumps to usertrap()
// userret: restores user registers from trapframe, switches to user page table, sret

#include "riscv.h"
#include "memlayout.h"

.section .trampoline
.globl trampoline
trampoline:
.align 4

.globl uservec
uservec:
    // TRAPFRAME is at TRAMPOLINE - PGSIZE
    // sscratch holds the address of trapframe (set by userret before sret)
    // Swap a0 and sscratch so we have the trapframe address
    csrrw a0, sscratch, a0

    // Save user registers to trapframe
    sd ra,   40(a0)
    sd sp,   48(a0)
    sd gp,   56(a0)
    sd tp,   64(a0)
    sd t0,   72(a0)
    sd t1,   80(a0)
    sd t2,   88(a0)
    sd s0,   96(a0)
    sd s1,  104(a0)
    sd a1,  120(a0)
    sd a2,  128(a0)
    sd a3,  136(a0)
    sd a4,  144(a0)
    sd a5,  152(a0)
    sd a6,  160(a0)
    sd a7,  168(a0)
    sd s2,  176(a0)
    sd s3,  184(a0)
    sd s4,  192(a0)
    sd s5,  200(a0)
    sd s6,  208(a0)
    sd s7,  216(a0)
    sd s8,  224(a0)
    sd s9,  232(a0)
    sd s10, 240(a0)
    sd s11, 248(a0)
    sd t3,  256(a0)
    sd t4,  264(a0)
    sd t5,  272(a0)
    sd t6,  280(a0)

    // Save the user a0 (was in sscratch) into trapframe
    csrr t0, sscratch
    sd t0, 112(a0)

    // Load kernel page table from trapframe
    ld t0, 0(a0)       // kernel_satp
    // Load kernel sp
    ld sp, 8(a0)       // kernel_sp
    // Load kernel trap handler address
    ld t1, 16(a0)      // kernel_trap (usertrap)
    // Load kernel hartid
    ld t2, 32(a0)      // kernel_hartid
    csrw tp, t2

    // Set sscratch to the trapframe address for next user trap
    csrw sscratch, a0

    // Switch to kernel page table
    csrw satp, t0
    sfence.vma zero, zero

    // Jump to usertrap()
    jr t1

.globl userret
userret:
    // a0 = trapframe, a1 = user pagetable (satp)

    // Restore kernel satp field first (not needed, but keep consistent)
    // Switch to user page table
    csrw satp, a1
    sfence.vma zero, zero

    // Save kernel satp in trapframe for next entry
    // (already saved, so skip)

    // Restore user registers from trapframe
    ld ra,   40(a0)
    ld sp,   48(a0)
    ld gp,   56(a0)
    ld tp,   64(a0)
    ld t0,   72(a0)
    ld t1,   80(a0)
    ld t2,   88(a0)
    ld s0,   96(a0)
    ld s1,  104(a0)
    ld a1,  120(a0)
    ld a2,  128(a0)
    ld a3,  136(a0)
    ld a4,  144(a0)
    ld a5,  152(a0)
    ld a6,  160(a0)
    ld a7,  168(a0)
    ld s2,  176(a0)
    ld s3,  184(a0)
    ld s4,  192(a0)
    ld s5,  200(a0)
    ld s6,  208(a0)
    ld s7,  216(a0)
    ld s8,  224(a0)
    ld s9,  232(a0)
    ld s10, 240(a0)
    ld s11, 248(a0)
    ld t3,  256(a0)
    ld t4,  264(a0)
    ld t5,  272(a0)
    ld t6,  280(a0)

    // Restore user a0 last (it's in trapframe at offset 112)
    // Set sscratch to trapframe address for next entry
    csrw sscratch, a0
    ld a0, 112(a0)

    // Return to user mode
    sret
