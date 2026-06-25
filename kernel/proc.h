// proc.h - Process structures

#ifndef _PROC_H_
#define _PROC_H_

#include "types.h"
#include "spinlock.h"
#include "param.h"
#include "riscv.h"
#include "memlayout.h"

// Saved registers for kernel context switches
struct context {
    uint64 ra;
    uint64 sp;
    uint64 s0;
    uint64 s1;
    uint64 s2;
    uint64 s3;
    uint64 s4;
    uint64 s5;
    uint64 s6;
    uint64 s7;
    uint64 s8;
    uint64 s9;
    uint64 s10;
    uint64 s11;
};

// Per-CPU state
struct cpu {
    struct proc *proc;          // The process running on this cpu, or null
    struct context context;     // swtch() here to enter scheduler()
    int noff;                   // Depth of push_off() nesting
    int intena;                 // Were interrupts enabled before push_off()?
};

// Process states
enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Trapframe for user/kernel transitions (matches trampoline.S offsets)
struct trapframe {
    uint64 kernel_satp;     // 0  - kernel page table
    uint64 kernel_sp;       // 8  - top of process's kernel stack
    uint64 kernel_trap;     // 16 - usertrap()
    uint64 epc;             // 24 - saved user program counter
    uint64 kernel_hartid;   // 32 - saved kernel tp
    uint64 ra;              // 40
    uint64 sp;              // 48
    uint64 gp;              // 56
    uint64 tp;              // 64
    uint64 t0;              // 72
    uint64 t1;              // 80
    uint64 t2;              // 88
    uint64 s0;              // 96
    uint64 s1;              // 104
    uint64 a0;              // 112
    uint64 a1;              // 120
    uint64 a2;              // 128
    uint64 a3;              // 136
    uint64 a4;              // 144
    uint64 a5;              // 152
    uint64 a6;              // 160
    uint64 a7;              // 168
    uint64 s2;              // 176
    uint64 s3;              // 184
    uint64 s4;              // 192
    uint64 s5;              // 200
    uint64 s6;              // 208
    uint64 s7;              // 216
    uint64 s8;              // 224
    uint64 s9;              // 232
    uint64 s10;             // 240
    uint64 s11;             // 248
    uint64 t3;              // 256
    uint64 t4;              // 264
    uint64 t5;              // 272
    uint64 t6;              // 280
};

// Process
struct proc {
    struct spinlock lock;

    enum procstate state;
    void *chan;                  // If non-zero, sleeping on chan
    int killed;
    int xstate;                  // Exit status
    int pid;

    pagetable_t pagetable;       // User page table
    uint64 sz;                   // Size of process memory (bytes)
    struct trapframe *trapframe; // Trapframe page
    struct context context;      // Kernel context for swtch()
    uint64 kstack;               // Virtual address of kernel stack
    struct proc *parent;         // Parent process

    char name[16];               // Name for debugging
};

// Kernel stack layout: each proc gets one page, starting below TRAPFRAME
// Process 0: TRAPFRAME - 2*PGSIZE, Process 1: TRAPFRAME - 3*PGSIZE, ...
#define KSTACK(p) (TRAPFRAME - (p + 1) * 2 * PGSIZE)

// Per-CPU array
extern struct cpu cpus[NCPU];

#endif // _PROC_H_
