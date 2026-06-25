// proc.h - Process table and process structure for ZephyrOS

#ifndef PROC_H
#define PROC_H

#include "spinlock.h"
#include "types.h"

#define NPROC 64

enum procstate { UNUSED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Saved user registers for traps (user → kernel transitions)
struct trapframe {
  // General-purpose registers
  uint64 ra;       // return address (x1)
  uint64 sp;       // stack pointer (x2)
  uint64 gp;       // global pointer (x3)
  uint64 tp;       // thread pointer (x4)
  uint64 t0;       // x5
  uint64 t1;       // x6
  uint64 t2;       // x7
  uint64 s0;       // x8 / fp
  uint64 s1;       // x9
  uint64 a0;       // x10  (syscall number / return value)
  uint64 a1;       // x11  (syscall arg 1)
  uint64 a2;       // x12  (syscall arg 2)
  uint64 a3;       // x13  (syscall arg 3)
  uint64 a4;       // x14  (syscall arg 4)
  uint64 a5;       // x15  (syscall arg 5)
  uint64 a6;       // x16  (syscall arg 5)
  uint64 a7;       // x17  (syscall arg 6)
  uint64 s2;       // x18
  uint64 s3;       // x19
  uint64 s4;       // x20
  uint64 s5;       // x21
  uint64 s6;       // x22
  uint64 s7;       // x23
  uint64 s8;       // x24
  uint64 s9;       // x25
  uint64 s10;      // x26
  uint64 s11;      // x27
  uint64 t3;       // x28
  uint64 t4;       // x29
  uint64 t5;       // x30
  uint64 t6;       // x31

  // Supervisor-mode registers saved by hardware on trap entry
  uint64 epc;      // program counter when trap happened
  uint64 status;   // sstatus register
  uint64 cause;    // scause register
};

// Per-process data
struct proc {
  struct spinlock lock;
  enum procstate state;        // Process state
  int pid;                     // Process ID
  pagetable_t pagetable;       // Page table
  uint64 sz;                   // Size of process memory (bytes)
  int killed;                  // If non-zero, has been killed
  char name[16];               // Process name (debugging)
  void *chan;                  // If non-zero, sleeping on chan
  int exitno;                  // Exit status
  struct context context;      // swtch() here to run process
  struct trapframe *trapframe; // Saved user registers
};

struct cpu {
  struct context context;       // swtch() here to scheduler
  struct proc *proc;           // Current running process
  struct spinlock lock;        // Interrupt safety (for this cpu)
  int noff;                    // Push_off nesting depth
  int intena;                  // Were interrupts enabled before push_off?
  char name[16];               // CPU name (debugging)
};

extern struct proc proc[NPROC];
extern struct cpu cpus[NCPU];

#endif
