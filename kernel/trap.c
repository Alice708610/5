// trap.c - Trap (interrupt/exception) handling for ZephyrOS

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"

// Trap vector base address
extern void kernelvec(void);

// Initialize trap vector for this hart
void
tvinit(void)
{
  // Set STVEC to kernelvec (in kernelvec.S)
  w_stvec((uint64)kernelvec);
}

// Enable timer interrupts
void
timerinit(void)
{
  // Set the timer compare register to fire in 1 second
  // QEMU virt: CLINT timer at 0x02000000
  uint64 interval = 10000000;  // ~10ms at 1GHz
  w_sbi_set_timer(interval);
}

// Enable interrupts
void
intr_on(void)
{
  w_sstatus(r_sstatus() | SSTATUS_SIE);
}

// Disable interrupts
void
intr_off(void)
{
  w_sstatus(r_sstatus() & ~SSTATUS_SIE);
}

// Called from kernelvec.S on trap/interrupt
void
kerneltrap(void)
{
  uint64 scause = r_scause();
  uint64 stval = r_stval();
  uint64 sepc = r_sepc();

  if ((scause & 0x8000000000000000L) && ((scause & 0xff) == 5)) {
    // Timer interrupt
    // Acknowledge and set next timer
    uint64 interval = 10000000;
    w_sbi_set_timer(interval);
    // Yield to scheduler
    yield();
  } else if (scause & 0x8000000000000000L) {
    // Other external interrupt
    // For now, just ignore
  } else {
    // Exception - panic
    printf("kerneltrap: unexpected scause 0x%p\n", scause);
    printf("            sepc=0x%p stval=0x%p\n", sepc, stval);
    panic("kerneltrap");
  }

  // Return from trap
  w_sepc(sepc);
}

// Called from usertrap() in user mode (not implemented yet)
void
usertrap(void)
{
  // Should not happen yet
  panic("usertrap not implemented");
}

void
usertrapret(void)
{
  panic("usertrapret not implemented");
}
