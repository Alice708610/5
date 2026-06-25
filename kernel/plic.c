// plic.c - PLIC (Platform-Level Interrupt Controller) driver for QEMU virt

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

// QEMU virt machine PLIC mapped at 0x0c000000
#define PLIC_BASE 0x0c000000L

// PLIC register offsets
#define PLIC_PRIORITY(id)    (PLIC_BASE + (id) * 4)
#define PLIC_PENDING(id)     (PLIC_BASE + 0x1000 + ((id) / 32) * 4)
#define PLIC_ENABLE(hart, id) (PLIC_BASE + 0x2000 + (hart) * 0x80 + ((id) / 32) * 4)
#define PLIC_THRESHOLD(hart) (PLIC_BASE + 0x200000 + (hart) * 0x1000)
#define PLIC_CLAIM(hart)     (PLIC_BASE + 0x200004 + (hart) * 0x1000)

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
