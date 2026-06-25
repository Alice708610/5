// plic.c - PLIC (Platform-Level Interrupt Controller) driver
// For QEMU virt machine

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"

void
plicinit(void)
{
    // Set UART interrupt priority to 1 (non-zero = enabled)
    volatile uint32 *priority = (uint32 *)PLIC_PRIORITY;
    priority[10] = 1;  // UART0 is IRQ 10

    // (No virtio disk in this lean kernel)
}

void
plicinithart(void)
{
    int hart = r_tp();

    // Enable UART interrupt for this hart's S-mode
    volatile uint32 *enable = (uint32 *)PLIC_SENABLE(hart);
    enable[10 / 32] |= (1 << (10 % 32));

    // Set S-mode priority threshold to 0 (accept all)
    volatile uint32 *threshold = (uint32 *)PLIC_SPRIORITY(hart);
    *threshold = 0;
}

// Claim a pending interrupt
int
plic_claim(void)
{
    int hart = r_tp();
    volatile uint32 *claim = (uint32 *)PLIC_SCLAIM(hart);
    return *claim;
}

// Complete handling an interrupt
void
plic_complete(int irq)
{
    int hart = r_tp();
    volatile uint32 *claim = (uint32 *)PLIC_SCLAIM(hart);
    *claim = irq;
}
