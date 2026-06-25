// main.c - Main kernel entry point (called from start.c in S-mode)

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "proc.h"
#include "spinlock.h"

volatile int started = 0;

void
main(void)
{
    if (r_tp() == 0) {
        // Hart 0: initialize everything

        // Console and UART
        uartinit();
        printfinit();
        consoleinit();
        printf("\n");
        printf("ZephyrOS 1.0 - RISC-V 64-bit OS Kernel\n");
        printf("2026 National Computer System Capability Competition\n");
        printf("\n");

        // Physical page allocator
        kinit();

        // Virtual memory
        kvminit();       // set up kernel page table
        kvminithart();   // turn on paging

        // Interrupt controller
        plicinit();
        plicinithart();

        // Trap vectors
        trapinit();

        // Process table
        procinit();

        // First user process
        userinit();

        started = 1;
    } else {
        // Other harts: wait for hart 0 to finish init
        while (started == 0)
            ;

        kvminithart();
        plicinithart();
        trapinithart();
    }

    // Enter scheduler — never returns
    scheduler();
}
