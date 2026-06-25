// riscv.h - RISC-V constants, CSR accessors, and macros
// For ZephyrOS kernel (RV64, SV39)

#ifndef _RISCV_H_
#define _RISCV_H_

#include "types.h"

// Page constants
#define PGSIZE      4096
#define PGSHIFT     12
#define PGROUNDUP(sz)   (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#define PGROUNDDOWN(a)  ((a) & ~(PGSIZE - 1))

// SV39 page table
#define PXMASK          0x1FF
#define PXSHIFT(level)  (PGSHIFT + (level) * 9)
#define PX(level, va)   (((uint64)(va) >> PXSHIFT(level)) & PXMASK)

// PTE bits
#define PTE_V (1L << 0)
#define PTE_R (1L << 1)
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4)
#define PTE_A (1L << 6)
#define PTE_D (1L << 7)

// PA <-> PTE conversion
#define PA2PTE(pa)  ((((uint64)(pa)) >> 12) << 10)
#define PTE2PA(pte) (((pte) >> 10) << 12)
#define PTE_FLAGS(pte) ((pte) & 0x3FF)

#define NPTENTRIES (PGSIZE / sizeof(uint64))

// SATP for SV39
#define SATP_SV39 (8L << 60)
#define MAKE_SATP(pagetable) (SATP_SV39 | (((uint64)(pagetable)) >> 12))

// SCAUSE
#define SCAUSE_INTERRUPT  (1L << 63)
#define SCAUSE_TIMER      5
#define SCAUSE_ECALL_U    8

// SSTATUS bits
#define SSTATUS_SIE   (1L << 1)
#define SSTATUS_SPIE  (1L << 5)
#define SSTATUS_SPP   (1L << 8)
#define SSTATUS_SUM   (1L << 18)

// SIE bits
#define SIE_STIE  (1L << 5)
#define SIE_SEIE  (1L << 9)
#define SIE_SSIE  (1L << 1)

// MSTATUS bits
#define MSTATUS_MPP_MASK   (3L << 11)
#define MSTATUS_MPP_S      (1L << 11)

// Which hart (core) is this?
// (mhartid in M-mode, tp in S-mode)

// --- CSR accessors (inline assembly) ---

static inline uint64
r_mstatus(void) {
    uint64 x;
    asm volatile("csrr %0, mstatus" : "=r"(x));
    return x;
}

static inline void
w_mstatus(uint64 x) {
    asm volatile("csrw mstatus, %0" : : "r"(x));
}

static inline uint64
r_sstatus(void) {
    uint64 x;
    asm volatile("csrr %0, sstatus" : "=r"(x));
    return x;
}

static inline void
w_sstatus(uint64 x) {
    asm volatile("csrw sstatus, %0" : : "r"(x));
}

static inline uint64
r_scause(void) {
    uint64 x;
    asm volatile("csrr %0, scause" : "=r"(x));
    return x;
}

static inline uint64
r_stval(void) {
    uint64 x;
    asm volatile("csrr %0, stval" : "=r"(x));
    return x;
}

static inline uint64
r_sepc(void) {
    uint64 x;
    asm volatile("csrr %0, sepc" : "=r"(x));
    return x;
}

static inline void
w_sepc(uint64 x) {
    asm volatile("csrw sepc, %0" : : "r"(x));
}

static inline uint64
r_sip(void) {
    uint64 x;
    asm volatile("csrr %0, sip" : "=r"(x));
    return x;
}

static inline void
w_sip(uint64 x) {
    asm volatile("csrw sip, %0" : : "r"(x));
}

static inline uint64
r_sie(void) {
    uint64 x;
    asm volatile("csrr %0, sie" : "=r"(x));
    return x;
}

static inline void
w_sie(uint64 x) {
    asm volatile("csrw sie, %0" : : "r"(x));
}

static inline uint64
r_stvec(void) {
    uint64 x;
    asm volatile("csrr %0, stvec" : "=r"(x));
    return x;
}

static inline void
w_stvec(uint64 x) {
    asm volatile("csrw stvec, %0" : : "r"(x));
}

static inline uint64
r_satp(void) {
    uint64 x;
    asm volatile("csrr %0, satp" : "=r"(x));
    return x;
}

static inline void
w_satp(uint64 x) {
    asm volatile("csrw satp, %0" : : "r"(x));
}

static inline uint64
r_tp(void) {
    uint64 x;
    asm volatile("mv %0, tp" : "=r"(x));
    return x;
}

static inline void
w_tp(uint64 x) {
    asm volatile("mv tp, %0" : : "r"(x));
}

static inline uint64
r_sp(void) {
    uint64 x;
    asm volatile("mv %0, sp" : "=r"(x));
    return x;
}

static inline uint64
r_mhartid(void) {
    uint64 x;
    asm volatile("csrr %0, mhartid" : "=r"(x));
    return x;
}

static inline void
w_mepc(uint64 x) {
    asm volatile("csrw mepc, %0" : : "r"(x));
}

static inline void
w_medeleg(uint64 x) {
    asm volatile("csrw medeleg, %0" : : "r"(x));
}

static inline void
w_mideleg(uint64 x) {
    asm volatile("csrw mideleg, %0" : : "r"(x));
}

static inline void
w_pmpaddr0(uint64 x) {
    asm volatile("csrw pmpaddr0, %0" : : "r"(x));
}

static inline void
w_pmpcfg0(uint64 x) {
    asm volatile("csrw pmpcfg0, %0" : : "r"(x));
}

static inline void
w_sscratch(uint64 x) {
    asm volatile("csrw sscratch, %0" : : "r"(x));
}

static inline uint64
r_sscratch(void) {
    uint64 x;
    asm volatile("csrr %0, sscratch" : "=r"(x));
    return x;
}

static inline void
w_scounteren(uint64 x) {
    asm volatile("csrw scounteren, %0" : : "r"(x));
}

static inline void
sfence_vma(void) {
    asm volatile("sfence.vma zero, zero");
}

static inline uint64
r_time(void) {
    uint64 x;
    asm volatile("rdtime %0" : "=r"(x));
    return x;
}

// Read cycle counter
static inline uint64
r_cycle(void) {
    uint64 x;
    asm volatile("rdcycle %0" : "=r"(x));
    return x;
}

// Assembly helper to invoke ecall
static inline uint64
ecall_syscall(void) {
    uint64 ret;
    asm volatile(
        "ecall"
        : "=a"(ret)
        : "a"(17)
        : "memory"
    );
    return ret;
}

#endif // _RISCV_H_
