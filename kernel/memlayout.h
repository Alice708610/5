// memlayout.h - Physical and virtual memory layout for ZephyrOS (QEMU virt)

#ifndef MEMLAYOUT_H
#define MEMLAYOUT_H

#include "types.h"

/*
 * QEMU virt machine memory map (128MB DRAM starting at 0x80000000)
 *
 * Physical Memory Layout:
 *   0x80000000 -- kernel text/code (entry point)
 *   ...          -- kernel data, BSS
 *   ...          -- end of kernel (PHYSTOP, ~128MB)
 *   end          -- start of free physical memory
 *
 * Virtual Memory Layout (SV39):
 *   0x0000_0000_0000_0000 -- User address space (0..MAXVA)
 *   Trampoline page at MAXVA (maps to same phys in every process)
 *   0xFFFF_FFFF_8000_0000 -- Kernel text (mapped identically)
 *   0xFFFF_FFFF_8001_0000 -- Kernel data / BSS
 *   ...                        Kernel stack per CPU
 */

// ==================== Physical Memory ====================

// QEMU virt machine: DRAM starts here
#define KERNBASE       0x0000000080000000L
#define PHYSTOP        0x0000000088000000L    // 128MB

// Kernel load address
#define KERNEL_LOAD    KERNBASE

// End of kernel (set by linker script)
extern uint64 _etext[];     // End of .text section
extern uint64 _end[];       // End of all sections

// Free memory starts after kernel
#define FREEMEM_START  ((uint64)_end)

// ==================== Virtual Memory ====================

// Maximum virtual address (user space limit) - SV39: 39 bits = 512GB
// But we use a smaller range to avoid the trampoline area
#define MAXVA         (1L << 38)   // 256GB user space

// Trampoline page: mapped at top of user virtual address space,
// also mapped in kernel. Used for trap return.
#define TRAMPOLINE    (MAXVA - PGSIZE)

// Kernel is identity-mapped at this high address
#define KERNTOP       (0xFFFFFFFFFFFFFFFFL - PGSIZE + 1) // -4KB from top
#define KERNBASE_VA   0xFFFFFFFFFFC00000L  // -4MB from top

// Kernel stack size per CPU
#define KSTACKSIZE    4096

// Per-CPU kernel stacks (in high kernel VA space)
// Stack grows downward; each CPU's stack starts at a separate page
#define KSTACK(p)     (KERNTOP - (p)*KSTACKSIZE)

#endif /* MEMLAYOUT_H */
