// riscv.h - RISC-V CSR definitions and inline functions for ZephyrOS

#ifndef RISCV_H
#define RISCV_H

#include "types.h"

// ==================== CSR Register Numbers ====================

// Machine Status Registers
#define CSR_MSTATUS     0x300
#define CSR_MISA        0x301
#define CSR_MEDELEG     0x302
#define CSR_MIDELEG     0x303
#define CSR_MTVEC       0x305
#define CSR_MCOUNTEREN  0x306
#define CSR_MSCRATCH    0x340
#define CSR_EPC         0x341   // MEPC
#define CSR_CAUSE       0x342   // MCAUSE
#define CSR_TVAL        0x343   // MTVAL
#define CSR_IP          0x344   // MIP

// Supervisor Status Registers
#define CSR_SSTATUS     0x100
#define CSR_SIE         0x104
#define CSR_STVEC       0x105
#define CSR_SCOUNTEREN  0x106
#define CSR_SSCRATCH    0x140
#define CSR_SEPC        0x141
#define CSR_SCAUSE      0x142
#define CSR_STVAL       0x143
#define CSR_SIP         0x144
#define CSR_SATP        0x180

// Machine/Supervisor mode registers
#define CSR_MCYCLE      0xB00
#define CSR_MINSTRET    0xB02
#define CSR_CYCLE       0xC00
#define CSR_TIME        0xC01
#define CSR_INSTRET     0xC02

// ==================== MSTATUS / SSTATUS Fields ====================
#define MSTATUS_SIE    (1L << 1)    // Machine interrupt enable (previous)
#define MSTATUS_SPIE   (1L << 5)    // Previous supervisor IE
#define MSTATUS_MPIE   (1L << 7)    // Previous machine IE
#define MSTATUS_SPP    (1L << 8)    // Previous privilege mode (supervisor=1)
#define MSTATUS_MPP    (3L << 11)   // Machine previous privilege

#define SSTATUS_SIE    (1L << 1)    // Supervisor interrupt enable
#define SSTATUS_SPIE   (1L << 5)    // Previous supervisor IE
#define SSTATUS_SPP    (1L << 8)    // Previous supervisor mode bit

// SIE / SIP fields
#define SIE_SSIE       (1 << 1)     // Software interrupt enable
#define SIE_STIE       (1 << 5)     // Timer interrupt enable
#define SIE_SEIE       (1 << 9)     // External interrupt enable

#define SIP_SSIE       (1 << 1)
#define SIP_STIE       (1 << 5)
#define SIP_SEIE       (1 << 9)

// SCAUSE values: bit 31 = 1 means interrupt, 0 = exception
#define SCAUSE_USER_ECALL       8
#define SCAUSE_SUPERVISOR_ECALL 9

// SATP fields for SV39 paging
#define MAKE_SATP(asid, ppn) (((uint64)(asid) << 44) | (8L << 60) | ((ppn) & 0xFFFFFFFFF))
#define SATP_MODE_SV39 (8L << 60)

// PTE flags for SV39
#define PTE_V   (1L << 0)    // Valid
#define PTE_R   (1L << 1)    // Readable
#define PTE_W   (1L << 2)    // Writable
#define PTE_X   (1L << 3)    // Executable
#define PTE_U   (1L << 4)    // User-accessible
#define PTE_G   (1L << 5)    // Global mapping

// Page size and levels for SV39
#define PGSHIFT    12           // log2(PGSIZE)
#define PGSIZE     (1L << PGSHIFT)   // 4096 bytes
#define PTLEVELS   3            // SV39 has 3 levels of page tables
#define PXMASK     (PGSIZE - 1)

#define PX(level, va) ((((uint64)(va)) >> (PGSHIFT + (level)*9)) & 0x1FF)
#define PGROUNDUP(sz)  (((sz)+PGSIZE-1) & ~(PGSIZE-1))

// PLIC (Platform-Level Interrupt Controller)
#define PLIC_PRIORITY    0x0C000000L
#define PLIC_PENDING     0x0C001000L
#define PLIC_ENABLE(h)   (0x0C002000L + (h) * 0x80)
#define PLIC_THRESHOLD(h) (0x0C200000L + (h) * 0x1000)
#define PLIC_CLAIM(h)    (0x0C200004L + (h) * 0x1000)
#define UART0_IRQ        10

// ==================== Inline CSR Read/Write Functions ====================

static inline uint64 r_mstatus(void) {
  uint64 x;
  asm volatile("csrr %0, mstatus" : "=r" (x));
  return x;
}

static inline void w_mstatus(uint64 x) {
  asm volatile("csrw mstatus, %0" :: "r" (x));
}

static inline uint64 r_sstatus(void) {
  uint64 x;
  asm volatile("csrr %0, sstatus" : "=r" (x));
  return x;
}

static inline void w_sstatus(uint64 x) {
  asm volatile("csrw sstatus, %0" :: "r" (x));
}

static inline uint64 r_mie(void) {
  uint64 x;
  asm volatile("csrr %0, mie" : "=r" (x));
  return x;
}

static inline void w_mie(uint64 x) {
  asm volatile("csrw mie, %0" :: "r" (x));
}

static inline uint64 r_sie(void) {
  uint64 x;
  asm volatile("csrr %0, sie" : "=r" (x));
  return x;
}

static inline void w_sie(uint64 x) {
  asm volatile("csrw sie, %0" :: "r" (x));
}

static inline uint64 r_mip(void) {
  uint64 x;
  asm volatile("csrr %0, mip" : "=r" (x));
  return x;
}

static inline void w_mip(uint64 x) {
  asm volatile("csrw mip, %0" :: "r" (x));
}

static inline uint64 r_sip(void) {
  uint64 x;
  asm volatile("csrr %0, sip" : "=r" (x));
  return x;
}

static inline void w_sip(uint64 x) {
  asm volatile("csrw sip, %0" :: "r" (x));
}

// Trap vector address
static inline void w_stvec(uint64 x) {
  asm volatile("csrw stvec, %0" :: "r" (x));
}

static inline uint64 r_stvec(void) {
  uint64 x;
  asm volatile("csrr %0, stvec" : "=r" (x));
  return x;
}

// Exception program counter
static inline void w_sepc(uint64 x) {
  asm volatile("csrw sepc, %0" :: "r" (x));
}

static inline uint64 r_sepc(void) {
  uint64 x;
  asm volatile("csrr %0, sepc" : "=r" (x));
  return x;
}

// Cause register
static inline uint64 r_scause(void) {
  uint64 x;
  asm volatile("csrr %0, scause" : "=r" (x));
  return x;
}

// Trap value (faulting address)
static inline uint64 r_stval(void) {
  uint64 x;
  asm volatile("csrr %0, stval" : "=r" (x));
  return x;
}

// Scratch register (for saving tp on trap entry)
static inline void w_sscratch(uint64 x) {
  asm volatile("csrw sscratch, %0" :: "r" (x));
}

// Address translation (SATP - Supervisor Address Translation and Protection)
static inline void w_satp(uint64 x) {
  asm volatile("csrw satp, %0" :: "r" (x));
}

static inline uint64 r_satp(void) {
  uint64 x;
  asm volatile("csrr %0, satp" : "=r" (x));
  return x;
}

// Thread pointer (tp) register
static inline uint64 r_tp(void) {
  uint64 x;
  asm volatile("mv %0, tp" : "=r"(x));
  return x;
}

static inline void w_tp(uint64 x) {
  asm volatile("mv tp, %0" :: "r"(x));
}

// Read current hart ID from mhartid CSR
static inline uint64 r_mhartid(void) {
  uint64 x;
  asm volatile("csrr %0, mhartid" : "=r" (x));
  return x;
}

// Timer counter (mtime in CLINT, or time CSR)
static inline uint64 r_time(void) {
  uint64 x;
  // Use the 'time' CSR (available in S-mode on QEMU virt)
  asm volatile("csrr %0, time" : "=r" (x));
  return x;
}

// Machine exception PC
static inline void w_mepc(uint64 x) {
  asm volatile("csrw mepc, %0" :: "r" (x));
}

static inline uint64 r_mepc(void) {
  uint64 x;
  asm volatile("csrr %0, mepc" : "=r" (x));
  return x;
}

// Machine scratch
static inline void w_mscratch(uint64 x) {
  asm volatile("csrw mscratch, %0" :: "r" (x));
}

// Machine cause
static inline uint64 r_mcause(void) {
  uint64 x;
  asm volatile("csrr %0, mcause" : "=r" (x));
  return x;
}

// Machine trap value
static inline uint64 r_mtval(void) {
  uint64 x;
  asm volatile("csrr %0, mtval" : "=r" (x));
  return x;
}

// Machine trap vector
static inline void w_mtvec(uint64 x) {
  asm volatile("csrw mtvec, %0" :: "r" (x));
}

// Machine ISA
static inline uint64 r_misa(void) {
  uint64 x;
  asm volatile("csrr %0, misa" : "=r" (x));
  return x;
}

// ==================== Interrupt Enable Helpers ====================

// Read machine-level sstatus (used to check SIE bit)
static inline uint64 r_sstatus_raw(void) {
  return r_sstatus();
}

// Alias for checking SIE in sched()
#define r_sstatus() r_sstatus()

#endif /* RISCV_H */
