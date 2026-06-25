// plic.c - PLIC (Platform-Level Interrupt Controller) driver for QEMU virt

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

// PLIC_BASE and register offsets are already defined in riscv.h

void
plicinit(void)
{
  // Set priority for UART (interrupt ID 10 on QEMU virt)
  *(volatile uint32 *)(PLIC_PRIORITY(10)) = 1;

  // No actual interrupt sources enabled yet
}

void
plicinithart(void)
{
  int hart = cpuid();

  // Enable UART interrupt (ID 10) for this hart
  volatile uint32 *enable = (volatile uint32 *)(PLIC_ENABLE(hart, 10));
  *enable |= (1 << (10 % 32));

  // Set threshold to 0 (accept all priorities)
  *(volatile uint32 *)(PLIC_THRESHOLD(hart)) = 0;
}

// Called from trap.c to claim an interrupt
uint32
plic_claim(void)
{
  int hart = cpuid();
  uint32 irq = *(volatile uint32 *)(PLIC_CLAIM(hart));
  return irq;
}

void
plic_complete(uint32 irq)
{
  int hart = cpuid();
  *(volatile uint32 *)(PLIC_CLAIM(hart)) = irq;
}
