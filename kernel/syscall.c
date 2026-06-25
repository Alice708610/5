// syscall.c - System call dispatch

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "proc.h"

// System call numbers
#define SYS_fork    1
#define SYS_exit    2
#define SYS_wait    3
#define SYS_getpid  4
#define SYS_kill    5
#define SYS_sbrk    6
#define SYS_sleep   7
#define SYS_uptime  8
#define SYS_write   9
#define SYS_read    10
#define SYS_exec    11

extern uint64 sys_fork(void);
extern uint64 sys_exit(void);
extern uint64 sys_wait(void);
extern uint64 sys_getpid(void);
extern uint64 sys_kill(void);
extern uint64 sys_sbrk(void);
extern uint64 sys_sleep(void);
extern uint64 sys_uptime(void);
extern uint64 sys_write(void);
extern uint64 sys_read(void);
extern uint64 sys_exec(void);

static uint64 (*syscalls[])(void) = {
    [SYS_fork]   sys_fork,
    [SYS_exit]   sys_exit,
    [SYS_wait]   sys_wait,
    [SYS_getpid] sys_getpid,
    [SYS_kill]   sys_kill,
    [SYS_sbrk]   sys_sbrk,
    [SYS_sleep]  sys_sleep,
    [SYS_uptime] sys_uptime,
    [SYS_write]  sys_write,
    [SYS_read]   sys_read,
    [SYS_exec]   sys_exec,
};

// Fetch the nth argument (n=0..5) as an int
// On RISC-V, syscall args are in a0-a5
int
argint(int n, int *ip)
{
    struct proc *p = myproc();
    uint64 val;
    switch (n) {
    case 0: val = p->trapframe->a0; break;
    case 1: val = p->trapframe->a1; break;
    case 2: val = p->trapframe->a2; break;
    case 3: val = p->trapframe->a3; break;
    case 4: val = p->trapframe->a4; break;
    case 5: val = p->trapframe->a5; break;
    default: return -1;
    }
    *ip = (int)val;
    return 0;
}

// Fetch the nth argument as a pointer (uint64 address)
int
argaddr(int n, uint64 *ip)
{
    struct proc *p = myproc();
    switch (n) {
    case 0: *ip = p->trapframe->a0; break;
    case 1: *ip = p->trapframe->a1; break;
    case 2: *ip = p->trapframe->a2; break;
    case 3: *ip = p->trapframe->a3; break;
    case 4: *ip = p->trapframe->a4; break;
    case 5: *ip = p->trapframe->a5; break;
    default: return -1;
    }
    return 0;
}

// Fetch the nth word-sized system call argument as a null-terminated string
int
argstr(int n, char *buf, int max)
{
    uint64 addr;
    if (argaddr(n, &addr) < 0)
        return -1;
    return fetchstr(addr, buf, max);
}

// Fetch a uint64 from user virtual address
int
fetchaddr(uint64 addr, uint64 *ip)
{
    struct proc *p = myproc();
    if (addr >= p->sz || addr + sizeof(uint64) > p->sz)
        return -1;
    if (copyin(p->pagetable, (char *)ip, addr, sizeof(uint64)) < 0)
        return -1;
    return 0;
}

// Fetch a string from user space
int
fetchstr(uint64 addr, char *buf, int max)
{
    struct proc *p = myproc();
    if (copyinstr(p->pagetable, buf, addr, max) < 0)
        return -1;
    return 0;
}

void
syscall(void)
{
    int num;
    struct proc *p = myproc();

    num = p->trapframe->a7;
    if (num > 0 && num < (int)(sizeof(syscalls) / sizeof(syscalls[0])) && syscalls[num]) {
        p->trapframe->a0 = syscalls[num]();
    } else {
        printf("syscall: unknown sys call %d\n", num);
        p->trapframe->a0 = -1;
    }
}
