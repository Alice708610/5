// kalloc.c - Physical page allocator for ZephyrOS

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"

void
kinit(void)
{
  // Initialize free list - placeholder
}

void *
kalloc(void)
{
  // Placeholder: just return a static buffer
  static char buf[4096];
  return buf;
}

void
kfree(void *pa)
{
  // Placeholder
}
