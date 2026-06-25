// proc.c - Process management for ZephyrOS

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

struct proc proc[NPROC];
struct cpu cpus[NCPU];

// Per-CPU data: indexed by hartid, points to the current cpu struct
static struct cpu *cpus_hart[NCPU];

// Return current hart ID (CPU number)
int
cpuid(void)
{
  int id = r_mhartid();
  return id;
}

// Return pointer to the current CPU's struct cpu
struct cpu *
mycpu(void)
{
  int id = cpuid();
  struct cpu *c = &cpus[id];
  // Initialize per-CPU pointer on first use
  if (!cpus_hart[id])
    cpus_hart[id] = c;
  return c;
}

void
procinit(void)
{
  struct proc *p;
  for (p = proc; p < &proc[NPROC]; p++) {
    initlock(&p->lock, "proc");
  }
}

// Create first user process (init)
void
userinit(void)
{
  // Not implemented yet - just a placeholder
}

// Scheduler: pick a runnable process and run it
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();

  for (;;) {
    // Enable interrupts on this processor
    intr_on();

    // Loop over process table looking for a runnable process
    for (p = proc; p < &proc[NPROC]; p++) {
      acquire(&p->lock);
      if (p->state == RUNNABLE) {
        // Switch to that process
        p->state = RUNNING;
        c->proc = p;
        swtch(&c->context, &p->context);
        c->proc = 0;
      }
      release(&p->lock);
    }
  }
}

// Give up the CPU for one scheduling round
void
yield(void)
{
  struct proc *p = myproc();
  acquire(&p->lock);
  p->state = RUNNABLE;
  sched();
  release(&p->lock);
}

// Switch to scheduler
void
sched(void)
{
  struct proc *p = myproc();
  if (p->state == RUNNING)
    panic("sched running");
  if (r_sstatus() & SSTATUS_SIE)
    panic("sched interruptible");
  swtch(&p->context, &mycpu()->context);
}

// Sleep on a channel
void
sleep_lock(void *chan, struct spinlock *lk)
{
  struct proc *p = myproc();
  acquire(&p->lock);
  release(lk);
  p->chan = chan;
  p->state = SLEEPING;
  sched();
  p->chan = 0;
  release(&p->lock);
  acquire(lk);
}

// Wake up all processes sleeping on a channel
void
wakeup(void *chan)
{
  struct proc *p;
  for (p = proc; p < &proc[NPROC]; p++) {
    if (p->state == SLEEPING && p->chan == chan) {
      acquire(&p->lock);
      p->state = RUNNABLE;
      release(&p->lock);
    }
  }
}

struct proc *
myproc(void)
{
  push_off();
  struct cpu *c = mycpu();
  struct proc *p = c->proc;
  pop_off();
  return p;
}
