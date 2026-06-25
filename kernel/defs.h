// defs.h - Kernel function declarations for ZephyrOS

#include "types.h"
#include "param.h"

struct spinlock;
struct proc;
struct trapframe;

// Memory management
void            kinit(void);
void          * kalloc(void);
void            kfree(void *);
void            kvminit(void);
void            kvminithart(void);
pagetable_t     kvmmake(void);
uint64          kvmpa(pagetable_t, uint64);
void            kvmmap(pagetable_t, uint64, uint64, uint64, int);
int             mappages(pagetable_t, uint64, uint64, uint64, int);
int             uvmalloc(pagetable_t, uint64, uint64);
int             uvmcopy(pagetable_t, pagetable_t, uint64, uint64);
void            uvmfree(pagetable_t, uint64);
void            uvmunmap(pagetable_t, uint64, uint64, int);
void            uvmdealloc(pagetable_t, uint64, uint64);
int             copyout(pagetable_t, uint64, char *, uint64);
int             copyin(pagetable_t, char *, uint64, uint64);
int             copyinstr(pagetable_t, char *, uint64, int);

// Process management
struct proc    * myproc(void);
void            procinit(void);
void            userinit(void);
void            scheduler(void);
void            sched(void);
void            yield(void);
void            forkret(void);
pagetable_t     proc_pagetable_alloc(struct proc *);
int             killed(struct proc *);
void    sleep_lock(void *, struct spinlock *);
void    wakeup(void *);

// Trap / interrupt
void            tvinit(void);
void            timerinit(void);
void            intr_on(void);
void            usertrap(void);
void            usertrapret(void);
void            kerneltrap();

// PLIC (interrupt controller)
void            plicinit(void);
void            plicinithart(void);

// Console/UART
void            consoleinit(void);
void            consoleintr(int);
void            panic(char *s);

// Spinlock
void            initlock(struct spinlock *, char *);
void            acquire(struct spinlock *);
void            release(struct spinlock *);
int             holding(struct spinlock *);

// Context switch
void            swtch(struct context *, struct context *);

// System calls
void            syscall();
int             argaddr(int, uint64 *);
int             argint(int, int *);
int             argstr(int, char *, int);
int             fetchstr(uint64, char *, int);
int             fetchaddr(uint64, uint64 *);

// String operations
int             strncmp(const char *, const char *, uint);
char           * strncpy(char *, const char *, int);
uint64          strlen(const char *);
void           * memset(void *, int, uint);
void           * memmove(void *, const void *, uint);
int             memcmp(const void *, const void *, uint);

// Printf
void            printf_init(void);
void            printf(char *, ...);
void            printfputc(int, int);

// CPU info
struct cpu     * mycpu(void);
int             cpuid(void);
