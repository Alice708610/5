#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "printf.h"

void
main(void)
{
  printf_init();
  printf("\n");
  printf("========================================\n");
  printf("  ZephyrOS - RISC-V 64-bit OS Kernel\n");
  printf("  2026 National OS Competition\n");
  printf("========================================\n");
  printf("\n");
  printf("[boot] ZephyrOS kernel starting...\n");
  printf("[boot] Architecture: RISC-V RV64GC\n");
  printf("[boot] Paging mode: SV39\n");
  printf("[boot] Entering main()...\n");
  printf("[boot] Initializing kernel subsystems...\n");

  // In a full OS, we would initialize:
  // - Physical memory allocator (kinit)
  // - Page table allocator (kvminit)
  // - Trap vectors (tvinit)
  // - UART/Console (consoleinit)
  // - PLIC (plicinit)
  // - Process table (procinit)
  // - Scheduler (scheduler)

  printf("[boot] Subsystem initialization complete.\n");
  printf("[boot] Launching init process...\n");
  printf("\n");
  printf("ZephyrOS# ");
  printf("\n");

  // For now, just idle
  for(;;) {
    // Wait for interrupt
    asm volatile("wfi");
  }
}
