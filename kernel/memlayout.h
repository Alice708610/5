// memlayout.h - Memory layout for QEMU virt (RISC-V 64)

#ifndef _MEMLAYOUT_H_
#define _MEMLAYOUT_H_

// Physical addresses (QEMU virt machine)
#define PHYSTOP   0x88000000L   // Top of physical RAM (128MB from 0x80000000)

#define UART0     0x10000000L   // NS16550A UART
#define VIRTIO0   0x10001000L   // VirtIO MMIO disk
#define PLIC      0x0c000000L   // Platform-Level Interrupt Controller
#define CLINT     0x02000000L   // Core Local Interruptor

#define CLINT_MTIMECMP(hartid)  (CLINT + 0x4000 + 8*(hartid))
#define CLINT_MTIME             (CLINT + 0xBFF8)

// PLIC registers
#define PLIC_PRIORITY   (PLIC + 0x0)
#define PLIC_PENDING    (PLIC + 0x1000)
#define PLIC_SENABLE(hart)   (PLIC + 0x2080 + (hart)*0x100)
#define PLIC_SPRIORITY(hart) (PLIC + 0x201000 + (hart)*0x2000)
#define PLIC_SCLAIM(hart)    (PLIC + 0x201004 + (hart)*0x2000)

// Virtual addresses (kernel)
// TRAMPOLINE is at the very top of the address space (used for user/kernel transitions)
#define TRAMPOLINE  0xffffffe000000000UL
#define TRAPFRAME   (TRAMPOLINE - PGSIZE)

#endif // _MEMLAYOUT_H_
