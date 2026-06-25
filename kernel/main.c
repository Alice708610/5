// main.c - ZephyrOS kernel entry point

#include "types.h"
#include "riscv.h"
#include "memlayout.h"
#include "defs.h"
#include "printf.h"

int main(void)
{
    // Initialize UART console first
    consoleinit();
    printf_init();

    printf("\n");
    printf("========================================\n");
    printf("  ZephyrOS - RISC-V 64-bit OS Kernel\n");
    printf("  2026 National OS Competition\n");
    printf("  QEMU virt machine (RV64GC)\n");
    printf("========================================\n\n");

    printf("[boot] ZephyrOS kernel loaded at 0x80000000\n");
    printf("[boot] BSS cleared, stack initialized\n");
    printf("[boot] Console (UART @ 0x10000000) initialized\n\n");

    printf("ZephyrOS# ");

    // Idle loop
    for (;;) {
        asm volatile("wfi");
    }

    return 0;
}
