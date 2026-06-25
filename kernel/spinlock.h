// spinlock.h - Spinlock definition

#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "types.h"

struct cpu;

struct spinlock {
  volatile int locked;   // Is the lock held?
  char *name;            // Name of lock (for debugging)
  struct cpu *cpu;       // The cpu holding the lock
};

#endif
