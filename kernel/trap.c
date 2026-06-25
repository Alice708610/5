// trap.c - Trap handling for user and kernel mode

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "proc.h"
#include "spinlock.h"

extern char trampoline[], uservec[], userret[];
extern void kernelvec(void);  // defined in kernelvec.S
extern uint64 timer_interval; // defined in start.c

// Global ticks counter (for sleep/uptime)
uint64 ticks;
struct spinlock ticks_lock;

void
trapinit(void)
{
    spinlock_init(&ticks_lock);
    ticks_lock.name = "ticks";
    w_stvec((uint64)kernelvec);  // Set kernel trap vector
}

void
trapinithart(void)
{
    w_stvec((uint64)kernelvec);
}

// Timer interrupt from CLINT
void
clockintr(void)
{
    // Re-arm the timer
    int hart = r_tp();
    volatile uint64 *mtime = (uint64 *)CLINT_MTIME;
    volatile uint64 *mtimecmp = (uint64 *)CLINT_MTIMECMP(hart);
    uint64 next = *mtime + timer_interval;
    *mtimecmp = next;

    // Increment global tick counter
    spinlock_acquire(&ticks_lock);
    ticks++;
    spinlock_release(&ticks_lock);

    // Wake up sleeping processes
    wakeup(&ticks);
}

// Device interrupt handler
int
devintr(void)
{
    uint64 scause = r_scause();

    // Check if it's an interrupt
    if ((scause & SCAUSE_INTERRUPT) == 0)
        return 0;

    // Timer interrupt
    if ((scause & ~SCAUSE_INTERRUPT) == SCAUSE_TIMER) {
        clockintr();
        return 1;
    }

    // External interrupt (UART, etc.)
    if ((scause & ~SCAUSE_INTERRUPT) == 9) {
        int irq = plic_claim();
        if (irq == 10) {
            // UART interrupt
            uartintr();
        } else if (irq) {
            printf("unexpected interrupt irq=%d\n", irq);
        }
        if (irq)
            plic_complete(irq);
        return 1;
    }

    return 0;
}

// User trap handler (called from trampoline.S uservec)
void
usertrap(void)
{
    // Set kernel trap vector
    w_stvec((uint64)kernelvec);

    struct proc *p = myproc();

    // Make sure we came from user mode
    if ((r_sstatus() & SSTATUS_SPP) != 0)
        panic("usertrap: not from user mode");

    // Save user PC in trapframe
    p->trapframe->epc = r_sepc();

    uint64 scause = r_scause();

    if (scause == SCAUSE_ECALL_U) {
        // System call
        if (p->killed)
            exit(-1);

        // Advance PC past the ecall instruction
        p->trapframe->epc += 4;

        // Enable interrupts during syscall
        w_sstatus(r_sstatus() | SSTATUS_SIE);

        // Dispatch syscall (a7 = syscall number, a0-a5 = args)
        syscall();

    } else if (devintr() != 0) {
        // Device interrupt handled
    } else {
        printf("usertrap(): unexpected scause %p pid=%d\n", scause, p->pid);
        printf("            sepc=%p stval=%p\n", r_sepc(), r_stval());
        p->killed = 1;
    }

    // If process was killed, exit
    if (p->killed)
        exit(-1);

    // Give up CPU on timer interrupt (round-robin scheduling)
    if (scause == (SCAUSE_INTERRUPT | SCAUSE_TIMER))
        yield();

    // Return to user
    usertrapret();
}

// Return to user space (sets up trampoline and sret)
void
usertrapret(void)
{
    struct proc *p = myproc();

    // Disable interrupts while setting up return
    w_sstatus(r_sstatus() & ~SSTATUS_SIE);

    // Set up the trampoline's user trap vector (stvec)
    // so next trap goes to uservec in trampoline.S
    uint64 trampoline_uservec = TRAMPOLINE + (uservec - trampoline);
    w_stvec(trampoline_uservec);

    // Set up trapframe values for return to user
    p->trapframe->kernel_satp = r_satp();         // kernel page table
    p->trapframe->kernel_sp = (uint64)p->kstack;  // process's kernel stack
    p->trapframe->kernel_trap = (uint64)usertrap;
    p->trapframe->kernel_hartid = r_tp();

    // Set up sstatus for SRET
    uint64 x = r_sstatus();
    x &= ~SSTATUS_SPP;  // Clear SPP to 0 (return to U-mode)
    x |= SSTATUS_SPIE;  // Enable interrupts after SRET
    w_sstatus(x);

    // Set SEPC to the saved user PC
    w_sepc(p->trapframe->epc);

    // Set SATP to user page table
    uint64 satp = MAKE_SATP(p->pagetable);

    // Jump to userret in trampoline.S
    uint64 trampoline_userret = TRAMPOLINE + (userret - trampoline);
    uint64 fn = trampoline_userret;
    // RISC-V: jalr r1, r2 (jump to register)
    asm volatile(
        "jalr r0, %0"
        : : "r"(fn), "a0"(p->trapframe), "a1"(satp)
        : "memory"
    );
}

// Kernel trap handler (interrupts in kernel mode)
void
kerneltrap(void)
{
    uint64 sepc = r_sepc();
    uint64 sstatus = r_sstatus();
    uint64 scause = r_scause();

    // Make sure we came from S-mode
    if ((sstatus & SSTATUS_SPP) == 0)
        panic("kerneltrap: not from supervisor mode");

    // Interrupts must be off
    if (sstatus & SSTATUS_SIE)
        panic("kerneltrap: interrupts enabled");

    if (devintr() == 0) {
        printf("kerneltrap(): unexpected scause %p\n", scause);
        printf("             sepc=%p stval=%p\n", r_sepc(), r_stval());
        panic("kerneltrap");
    }

    // Restore state and return
    w_sepc(sepc);
    w_sstatus(sstatus);
}
