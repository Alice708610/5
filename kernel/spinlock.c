// spinlock.c - Spinlock implementation for ZephyrOS

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "defs.h"

void
initlock(struct spinlock *lk, char *name)
{
  lk->name = name;
  lk->locked = 0;
  lk->cpu = 0;
}

void
acquire(struct spinlock *lk)
{
  // Disable interrupts to prevent deadlock with timer interrupt
  push_off();

  // RISC-V atomic exchange (amoadd.w d, a2, a1)
  // On riscv64: use amoswap.d
  while (__sync_lock_test_and_set(&lk->locked, 1) != 0)
    ;

  // Memory barrier
  __sync_synchronize();

  lk->cpu = mycpu();
}

void
release(struct spinlock *lk)
{
  lk->cpu = 0;

  // Memory barrier
  __sync_synchronize();

  __sync_lock_release(&lk->locked);

  pop_off();
}

int
holding(struct spinlock *lk)
{
  int r;
  r = lk->locked && lk->cpu == mycpu();
  return r;
}
