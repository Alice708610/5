// spinlock.c - Spinlock implementation

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "proc.h"
#include "spinlock.h"

// CPU state for push_off/pop_off
struct cpu cpus[NCPU];

void
spinlock_init(struct spinlock *lk)
{
    lk->locked = 0;
    lk->name = "spinlock";
    lk->cpu = 0;
}

// Acquire the lock, spinning until we get it.
// Also disable interrupts on this CPU to avoid deadlock.
void
spinlock_acquire(struct spinlock *lk)
{
    push_off();
    if (spinlock_holding(lk))
        panic("acquire");

    // RISC-V atomic swap: __sync_lock_test_and_set
    while (__sync_lock_test_and_set(&lk->locked, 1) != 0)
        ;

    // Tell the compiler not to reorder loads/stores across this point
    __sync_synchronize();

    lk->cpu = mycpu();
}

void
spinlock_release(struct spinlock *lk)
{
    if (!spinlock_holding(lk))
        panic("release");

    lk->cpu = 0;

    __sync_synchronize();

    // Release the lock (atomic store)
    __sync_lock_release(&lk->locked);

    pop_off();
}

int
spinlock_holding(struct spinlock *lk)
{
    int r;
    push_off();
    r = (lk->locked && lk->cpu == mycpu());
    pop_off();
    return r;
}

// push_off / pop_off: nestable interrupt disable
void
push_off(void)
{
    int old = r_sstatus() & SSTATUS_SIE;
    w_sstatus(r_sstatus() & ~SSTATUS_SIE);
    struct cpu *c = mycpu();
    if (c->noff == 0)
        c->intena = old;
    c->noff += 1;
}

void
pop_off(void)
{
    struct cpu *c = mycpu();
    if ((r_sstatus() & SSTATUS_SIE) != 0)
        panic("pop_off - interruptible");
    if (c->noff < 1)
        panic("pop_off");
    c->noff -= 1;
    if (c->noff == 0 && c->intena)
        w_sstatus(r_sstatus() | SSTATUS_SIE);
}
