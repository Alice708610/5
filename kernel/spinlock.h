// spinlock.h - Spinlock structure

#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include "types.h"

struct spinlock {
    uint locked;       // Is the lock held?
    char *name;        // Name of lock (for debugging)
    struct cpu *cpu;   // The cpu holding the lock
};

#endif // _SPINLOCK_H_
