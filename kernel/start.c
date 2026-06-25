// start.c - Kernel entry: M-mode setup, then switch to S-mode and call main()
// Called from entry.S after stack setup

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"
#include "param.h"

// Per-hart stack (entry.S uses this)
__attribute__((aligned(16)))
char stack0[4096 * NCPU];

// Timer interval (ticks every ~0.1s at 10MHz)
uint64 timer_interval = 1000000;

void
start(void)
{
    // ---- M-mode setup before entering S-mode ----

    // Set MPP (Previous Privilege) to Supervisor
    uint64 x = r_mstatus();
    x &= ~MSTATUS_MPP_MASK;
    x |= MSTATUS_MPP_S;
    w_mstatus(x);

    // Set return PC to main()
    w_mepc((uint64)main);

    // Disable paging
    w_satp(0);

    // Delegate interrupts/exceptions to S-mode
    w_medeleg(0xffff);
    w_mideleg(0xffff);
    w_sie(r_sie() | SIE_SEIE | SIE_STIE | SIE_SSIE);

    // PMP: allow full access from S-mode
    w_pmpaddr0(0x3fffffffffffffull);
    w_pmpcfg0(0xf);

    // Set up timer interrupt
    timerinit();

    // Save hart ID in tp for S-mode
    w_tp(r_mhartid());

    // Enter S-mode via mret
    asm volatile("mret");
}

void
timerinit(void)
{
    // Enable supervisor timer interrupt
    w_sie(r_sie() | SIE_STIE);

    // Program the first timer interrupt via CLINT
    volatile uint64 *mtime = (uint64 *)CLINT_MTIME;
    volatile uint64 *mtimecmp = (uint64 *)CLINT_MTIMECMP(0);
    *mtimecmp = *mtime + timer_interval;
}
