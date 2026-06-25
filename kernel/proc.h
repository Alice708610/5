// proc.h - Process table and process structure for ZephyrOS

#ifndef PROC_H
#define PROC_H

#include "spinlock.h"
#include "types.h"

#define NPROC 64

enum procstate { UNUSED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

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
