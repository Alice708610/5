// start.c - Early kernel initialization
// Called by entry.S before main()

#include "types.h"
#include "param.h"
#include "riscv.h"
#include "memlayout.h"
#include "spinlock.h"
#include "printf.h"
#include "defs.h"

void main(void);

void
start(void)
{
    // Clear BSS section (zero-initialized data)
    extern char edata[], end[];
    for(char *p = edata; p < end; p++)
        *p = 0;

    // Initialize UART/console first so printf works
    consoleinit();

    printf("\n");
    printf("========================================\n");
    printf("  ZephyrOS - RISC-V 64-bit OS Kernel\n");
    printf("  2026 National Computer System Competition\n");
    printf("========================================\n\n");

    printf("[boot] ZephyrOS starting...\n");
    printf("[boot] Architecture: RISC-V RV64GC (QEMU virt machine)\n");
    printf("[boot] Memory: 128MB DRAM @ 0x80000000\n");
    printf("[boot] CPUs: %d cores\n", NCPU);
    printf("[boot] Paging: SV39 (39-bit virtual address)\n\n");

    printf("[init] Physical memory allocator... ");
    kinit();          // Initialize physical page allocator
    printf("OK\n");

    printf("[init] Kernel page table...       ");
    kvminit();        // Create kernel page table
    kvminithart();    // Load kernel SATP register
    printf("OK\n");

    printf("[init] Trap vector setup...       ");
    tvinit();         // Set trap vector in STVEC
    printf("OK\n");

    printf("[init] PLIC (interrupt ctrl)...   ");
    plicinit();       // Initialize interrupt controller
    plicinithart();   // Enable interrupts on this hart
    printf("OK\n");

    printf("[init] Process table & scheduler...");
    procinit();       // Initialize process table and scheduler
    printf("OK\n");

    printf("[init] Timer interrupt...         ");
    timerinit();      // Start timer interrupts
    printf("OK\n\n");

    printf("[boot] All subsystems initialized.\n");
    printf("[boot] Starting user processes...\n\n");

    userinit();       // Create first user process (init)

    // Enable interrupts and run scheduler
    intr_on();
    scheduler();

    // Should never reach here
    panic("start(): scheduler returned!\n");
}
