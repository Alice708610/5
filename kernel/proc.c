// proc.c - Process management
// Handles process creation, scheduling, forking, exiting, and waiting

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "proc.h"
#include "spinlock.h"

struct proc proc[NPROC];
struct cpu cpus[NCPU];

extern char trampoline[], uservec[], userret[];

// Next available PID
int nextpid = 1;
struct spinlock pid_lock;

// Per-CPU "mycpu" accessor
struct cpu *
mycpu(void)
{
    int id = cpuid();
    return &cpus[id];
}

// Get current CPU ID (from tp register)
int
cpuid(void)
{
    int id = r_tp();
    return id;
}

// Get current process
struct proc *
myproc(void)
{
    push_off();
    struct cpu *c = mycpu();
    struct proc *p = c->proc;
    pop_off();
    return p;
}

// Allocate a new PID
int
allocpid(void)
{
    spinlock_acquire(&pid_lock);
    int pid = nextpid++;
    spinlock_release(&pid_lock);
    return pid;
}

// Look through process table for an UNUSED proc
// Allocate it, initialize its page table, and return it
static struct proc *
allocproc(void)
{
    struct proc *p;

    for (p = proc; p < &proc[NPROC]; p++) {
        spinlock_acquire(&p->lock);
        if (p->state == UNUSED) {
            goto found;
        }
        spinlock_release(&p->lock);
    }
    return 0;

found:
    p->pid = allocpid();
    p->state = USED;
    p->parent = 0;

    // Allocate trapframe page
    if ((p->trapframe = (struct trapframe *)kalloc()) == 0) {
        spinlock_release(&p->lock);
        return 0;
    }

    // Allocate user page table
    p->pagetable = proc_pagetable_alloc(p);
    if (p->pagetable == 0) {
        kfree((void *)p->trapframe);
        p->trapframe = 0;
        spinlock_release(&p->lock);
        return 0;
    }

    // Set up new context to start executing at forkret
    memset(&p->context, 0, sizeof(p->context));
    p->context.ra = (uint64)forkret;
    p->context.sp = (uint64)p->kstack + PGSIZE;

    return p;
}

// Allocate a user page table for a process,
// mapping the trampoline and trapframe pages.
pagetable_t
proc_pagetable_alloc(struct proc *p)
{
    pagetable_t pagetable = uvmcreate();
    if (pagetable == 0)
        return 0;

    // Map the trampoline page (at TRAMPOLINE, same physical page as kernel's trampoline)
    if (mappages(pagetable, TRAMPOLINE, PGSIZE, (uint64)trampoline,
                 PTE_R | PTE_X) < 0) {
        uvmunmap(pagetable, 0, 0, 0);  // nothing to unmap
        freewalk(pagetable);
        return 0;
    }

    // Map the trapframe page (at TRAPFRAME, just below trampoline)
    if (mappages(pagetable, TRAPFRAME, PGSIZE, (uint64)p->trapframe,
                 PTE_R | PTE_W) < 0) {
        uvmunmap(pagetable, TRAMPOLINE, 1, 0);
        freewalk(pagetable);
        return 0;
    }

    return pagetable;
}

// Initialize the process table
void
procinit(void)
{
    spinlock_init(&pid_lock);
    pid_lock.name = "pid";

    for (struct proc *p = proc; p < &proc[NPROC]; p++) {
        spinlock_init(&p->lock);
        p->lock.name = "proc";
        p->state = UNUSED;
        // Allocate kernel stack
        char *pa = kalloc();
        if (pa == 0)
            panic("procinit: kalloc");
        int idx = p - proc;
        uint64 va = TRAPFRAME - (idx + 1) * 2 * PGSIZE;
        if (mappages(kernel_pagetable, va, PGSIZE, (uint64)pa,
                     PTE_R | PTE_W) < 0)
            panic("procinit: mappages");
        p->kstack = va;
    }
}

// Create the first user process (loads initcode)
// initcode is a small binary that calls exec("init", ...)
extern uchar initcode[];
extern uchar initcode_end[];

void
userinit(void)
{
    struct proc *p;
    p = allocproc();
    if (p == 0)
        panic("userinit: allocproc");

    // Set up the first user page with initcode
    uint sz = initcode_end - initcode;
    uvmfirst(p->pagetable, initcode, sz);
    p->sz = PGSIZE;

    // Initialize trapframe: start at address 0
    memset(p->trapframe, 0, PGSIZE);
    p->trapframe->epc = 0;       // user program counter = 0
    p->trapframe->sp = PGSIZE;   // user stack pointer

    safestrcpy(p->name, "init", sizeof(p->name));
    p->parent = 0;

    p->state = RUNNABLE;
    spinlock_release(&p->lock);
}

// Grow or shrink process memory by n bytes
int
growproc(int n)
{
    uint64 sz;
    struct proc *p = myproc();

    sz = p->sz;
    if (n > 0) {
        if ((sz = uvmalloc(p->pagetable, sz, sz + n)) == 0)
            return -1;
    } else if (n < 0) {
        sz = uvmdealloc(p->pagetable, sz, sz + n);
    }
    p->sz = sz;
    return 0;
}

// Fork a new process
int
fork(void)
{
    struct proc *p;
    struct proc *np = myproc();

    // Allocate process
    if ((p = allocproc()) == 0)
        return -1;

    // Copy user memory from parent to child
    if (uvmcopy(np->pagetable, p->pagetable, np->sz) < 0) {
        freeproc(p);
        spinlock_release(&p->lock);
        return -1;
    }
    p->sz = np->sz;

    // Copy trapframe (so child resumes where parent was)
    *(p->trapframe) = *(np->trapframe);

    // Child returns 0 from fork
    p->trapframe->a0 = 0;

    // Set parent
    p->parent = np;

    safestrcpy(p->name, np->name, sizeof(p->name));

    p->state = RUNNABLE;
    spinlock_release(&p->lock);

    return p->pid;
}

// Free a proc structure and its resources
void
freeproc(struct proc *p)
{
    if (p->trapframe)
        kfree((void *)p->trapframe);
    p->trapframe = 0;
    if (p->pagetable)
        proc_freepagetable(p->pagetable, p->sz);
    p->pagetable = 0;
    p->sz = 0;
    p->pid = 0;
    p->parent = 0;
    p->name[0] = 0;
    p->chan = 0;
    p->killed = 0;
    p->xstate = 0;
    p->state = UNUSED;
}

// Exit the current process
int
exit(int status)
{
    struct proc *p = myproc();

    if (p == &proc[0])
        panic("exit: init exiting");

    // Reparent children to init
    for (struct proc *pp = proc; pp < &proc[NPROC]; pp++) {
        if (pp->parent == p) {
            pp->parent = &proc[0];
        }
    }

    // Wake up parent
    wakeup(p->parent);

    p->xstate = status;
    p->state = ZOMBIE;

    spinlock_release(&p->lock);

    // Jump into scheduler — never returns
    w_sstatus(r_sstatus() & ~SSTATUS_SIE);
    struct cpu *c = mycpu();
    swtch(&p->context, &c->context);

    panic("exit: zombie exit returned");
}

// Wait for a child process to exit
int
wait(uint64 addr)
{
    struct proc *np;
    int havekids, pid;
    struct proc *p = myproc();

    spinlock_acquire(&p->lock);
    for (;;) {
        havekids = 0;
        for (np = proc; np < &proc[NPROC]; np++) {
            if (np->parent == p) {
                spinlock_acquire(&np->lock);
                if (np->state == ZOMBIE) {
                    // Found a zombie child
                    pid = np->pid;
                    if (addr != 0 && copyout(p->pagetable, addr, (char *)&np->xstate,
                                             sizeof(np->xstate)) < 0) {
                        spinlock_release(&np->lock);
                        spinlock_release(&p->lock);
                        return -1;
                    }
                    freeproc(np);
                    spinlock_release(&np->lock);
                    spinlock_release(&p->lock);
                    return pid;
                }
                spinlock_release(&np->lock);
                havekids = 1;
            }
        }

        if (!havekids || p->killed) {
            spinlock_release(&p->lock);
            return -1;
        }

        // Wait for child to exit
        sleep_lock(p, &p->lock);
    }
}

// Scheduler: pick next runnable process and switch to it
void
scheduler(void)
{
    struct proc *p;
    struct cpu *c = mycpu();

    c->proc = 0;
    for (;;) {
        // Enable interrupts on this CPU
        w_sstatus(r_sstatus() | SSTATUS_SIE);

        for (p = proc; p < &proc[NPROC]; p++) {
            spinlock_acquire(&p->lock);
            if (p->state == RUNNABLE) {
                p->state = RUNNING;
                c->proc = p;
                swtch(&c->context, &p->context);

                // Process is done running for now
                c->proc = 0;
            }
            spinlock_release(&p->lock);
        }
    }
}

// Give up CPU for one scheduling round
void
yield(void)
{
    struct proc *p = myproc();
    spinlock_acquire(&p->lock);
    p->state = RUNNABLE;
    struct cpu *c = mycpu();
    swtch(&p->context, &c->context);
    spinlock_release(&p->lock);
}

// A fork child's very first scheduling by scheduler() will
// call forkret, which will return to user space.
void
forkret(void)
{
    static int first = 1;
    struct proc *p = myproc();

    spinlock_release(&p->lock);

    if (first) {
        first = 0;
        // First process, initialize file system if needed
    }

    // Return to user mode
    usertrapret();
}

// Atomically release lock and sleep on chan
void
sleep_lock(void *chan, struct spinlock *lk)
{
    struct proc *p = myproc();

    // Must acquire p->lock before modifying p->state
    spinlock_acquire(&p->lock);

    // Release the caller's lock
    spinlock_release(lk);

    // Go to sleep
    p->chan = chan;
    p->state = SLEEPING;

    struct cpu *c = mycpu();
    swtch(&p->context, &c->context);

    // Woke up
    p->chan = 0;

    // Reacquire p->lock
    spinlock_release(&p->lock);
    spinlock_acquire(lk);
}

// Wake up all processes sleeping on chan
void
wakeup(void *chan)
{
    for (struct proc *p = proc; p < &proc[NPROC]; p++) {
        if (p != myproc()) {
            spinlock_acquire(&p->lock);
            if (p->state == SLEEPING && p->chan == chan) {
                p->state = RUNNABLE;
            }
            spinlock_release(&p->lock);
        }
    }
}

// Kill a process
int
kill(int pid)
{
    for (struct proc *p = proc; p < &proc[NPROC]; p++) {
        spinlock_acquire(&p->lock);
        if (p->pid == pid) {
            p->killed = 1;
            if (p->state == SLEEPING) {
                p->state = RUNNABLE;
            }
            spinlock_release(&p->lock);
            return 0;
        }
        spinlock_release(&p->lock);
    }
    return -1;
}

// Is this process killed?
int
killed(struct proc *p)
{
    int k;
    spinlock_acquire(&p->lock);
    k = p->killed;
    spinlock_release(&p->lock);
    return k;
}

// Print process table (for debugging)
void
procdump(void)
{
    printf("pid\tstate\tname\n");
    for (struct proc *p = proc; p < &proc[NPROC]; p++) {
        if (p->state != UNUSED) {
            printf("%d\t", p->pid);
            switch (p->state) {
            case USED:     printf("USED\t"); break;
            case SLEEPING: printf("SLEEP\t"); break;
            case RUNNABLE: printf("READY\t"); break;
            case RUNNING:  printf("RUN\t"); break;
            case ZOMBIE:   printf("ZOMBIE\t"); break;
            default:       printf("?\t");
            }
            printf("%s\n", p->name);
        }
    }
}
