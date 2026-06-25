// defs.h - Function declarations for ZephyrOS kernel
// Lean kernel: console + VM + kalloc + traps + processes + syscalls
// No file system or virtio disk — keeps the kernel small and bug-free

#ifndef _DEFS_H_
#define _DEFS_H_

#include "types.h"

struct context;
struct proc;
struct spinlock;
struct trapframe;

// --- start.c ---
void    start(void);
void    timerinit(void);

// --- main.c ---
void    main(void);

// --- kalloc.c ---
void    kinit(void);
void   *kalloc(void);
void    kfree(void *pa);
void    freerange(void *pa_start, void *pa_end);

// --- vm.c ---
extern pagetable_t kernel_pagetable;
void    kvminit(void);
void    kvminithart(void);
pagetable_t uvmcreate(void);
void    uvmfirst(pagetable_t, uchar *, uint);
uint64  uvmalloc(pagetable_t, uint64, uint64);
void    uvmdealloc(pagetable_t, uint64, uint64);
int     uvmcopy(pagetable_t, pagetable_t, uint64);
void    uvmunmap(pagetable_t, uint64, uint64, int);
void    freewalk(pagetable_t);
uint64  walkaddr(pagetable_t, uint64);
pte_t  *walk(pagetable_t, uint64, int);
int     mappages(pagetable_t, uint64, uint64, uint64, int);
void    uvmclear(pagetable_t, uint64);
int     copyin(pagetable_t, char *, uint64, uint64);
int     copyinstr(pagetable_t, char *, uint64, uint64);
int     copyout(pagetable_t, uint64, char *, uint64);
void    proc_freepagetable(pagetable_t, uint64);

// --- string.c ---
int     strlen(const char *);
int     strncmp(const char *, const char *, uint);
char *  strncpy(char *, const char *, int);
int     strcmp(const char *, const char *);
char *  safestrcpy(char *, const char *, int);
void *  memmove(void *, const void *, uint);
void *  memset(void *, int, uint);
int     memcmp(const void *, const void *, uint);

// --- uart.c ---
void    uartinit(void);
void    uartputc(int);
int     uartgetc(void);
void    uartintr(void);

// --- console.c ---
void    consoleinit(void);
void    consputc(int);
void    consoleintr(int);
int     consoleread(int, uint64, int);
int     consolewrite(int, uint64, int);

// --- printf.c ---
void    printf(char *fmt, ...);
void    panic(char *s);
void    printfinit(void);

// --- plic.c ---
void    plicinit(void);
void    plicinithart(void);
int     plic_claim(void);
void    plic_complete(int);

// --- trap.c ---
void    usertrap(void);
void    usertrapret(void);
void    kerneltrap(void);
void    trapinit(void);
void    trapinithart(void);
void    clockintr(void);
int     devintr(void);

// --- proc.c ---
void    procinit(void);
void    userinit(void);
int     growproc(int);
int     fork(void);
int     exit(int) __attribute__((noreturn));
int     wait(uint64);
void    scheduler(void) __attribute__((noreturn));
int     cpuid(void);
void    yield(void);
void    procdump(void);
struct proc *myproc(void);
struct cpu *mycpu(void);
void    swtch(struct context *, struct context *);
void    freeproc(struct proc *);
void    forkret(void);
pagetable_t proc_pagetable_alloc(struct proc *);
int     killed(struct proc *);
void    sleep_lock(void *, struct spinlock *);
void    wakeup(void *);

// --- exec.c ---
int     exec(char *, char **);

// --- syscall.c ---
void    syscall(void);
int     argint(int, int *);
int     argaddr(int, uint64 *);
int     argstr(int, char *, int);
int     fetchaddr(uint64, uint64 *);
int     fetchstr(uint64, char *, int);

// --- sysproc.c ---
uint64  sys_exit(void);
uint64  sys_getpid(void);
uint64  sys_fork(void);
uint64  sys_wait(void);
uint64  sys_sbrk(void);
uint64  sys_sleep(void);
uint64  sys_uptime(void);
uint64  sys_kill(void);

// --- spinlock.c ---
void    spinlock_init(struct spinlock *);
void    spinlock_acquire(struct spinlock *);
void    spinlock_release(struct spinlock *);
int     spinlock_holding(struct spinlock *);
void    push_off(void);
void    pop_off(void);

#endif // _DEFS_H_
