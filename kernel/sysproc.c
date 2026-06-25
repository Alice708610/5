// sysproc.c - System call implementations for process-related calls

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "proc.h"

uint64
sys_exit(void)
{
    int n;
    if (argint(0, &n) < 0)
        n = 0;
    exit(n);
    return 0;  // not reached
}

uint64
sys_getpid(void)
{
    return myproc()->pid;
}

uint64
sys_fork(void)
{
    return fork();
}

uint64
sys_wait(void)
{
    uint64 p;
    if (argaddr(0, &p) < 0)
        return -1;
    return wait(p);
}

uint64
sys_kill(void)
{
    int pid;
    if (argint(0, &pid) < 0)
        return -1;
    // return kill(pid);  // kill not fully implemented in this lean version
    return -1;
}

uint64
sys_sbrk(void)
{
    int n;
    if (argint(0, &n) < 0)
        return -1;
    uint64 oldsz = myproc()->sz;
    if (growproc(n) < 0)
        return -1;
    return oldsz;
}

uint64
sys_sleep(void)
{
    int n;
    if (argint(0, &n) < 0)
        return -1;

    extern uint64 ticks;
    extern struct spinlock ticks_lock;

    uint ticks0;
    spinlock_acquire(&ticks_lock);
    ticks0 = ticks;
    while (ticks - ticks0 < n) {
        if (myproc()->killed) {
            spinlock_release(&ticks_lock);
            return -1;
        }
        sleep_lock(&ticks, &ticks_lock);
    }
    spinlock_release(&ticks_lock);
    return 0;
}

uint64
sys_uptime(void)
{
    extern uint64 ticks;
    extern struct spinlock ticks_lock;

    uint xticks;
    spinlock_acquire(&ticks_lock);
    xticks = ticks;
    spinlock_release(&ticks_lock);
    return xticks;
}

uint64
sys_write(void)
{
    // Write to console (fd = 1 or 2)
    // a0 = fd, a1 = buf, a2 = n
    struct proc *p = myproc();
    int fd = p->trapframe->a0;
    uint64 buf = p->trapframe->a1;
    int n = p->trapframe->a2;

    if (fd == 1 || fd == 2) {
        return consolewrite(1, buf, n);
    }
    return -1;
}

uint64
sys_read(void)
{
    // Read from console (fd = 0)
    struct proc *p = myproc();
    int fd = p->trapframe->a0;
    uint64 buf = p->trapframe->a1;
    int n = p->trapframe->a2;

    if (fd == 0) {
        return consoleread(1, buf, n);
    }
    return -1;
}

uint64
sys_exec(void)
{
    // Not fully implemented in this lean version
    return -1;
}
