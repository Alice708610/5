// vm.c - Virtual memory (paging) for ZephyrOS

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

// Create kernel page table
void
kvminit(void)
{
  // Placeholder
}

// Load kernel page table into SATP
void
kvminithart(void)
{
  // Placeholder - in a real OS this would:
  // w_satp(KERNBASE | (uint64)kernel_pagetable >> 12);
}

int
mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, int perm)
{
  // Placeholder
  return 0;
}

int
uvmalloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz)
{
  return newsz;
}

int
uvmcopy(pagetable_t old, pagetable_t new, uint64 start, uint64 sz)
{
  return 0;
}

void
uvmfree(pagetable_t pagetable, uint64 sz)
{
  // Placeholder
}

void
uvmunmap(pagetable_t pagetable, uint64 va, uint64 size, int do_free)
{
  // Placeholder
}

void
uvmdealloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz)
{
  // Placeholder
}

int
copyout(pagetable_t pagetable, uint64 va, char *buf, uint64 len)
{
  // Placeholder
  return 0;
}

int
copyin(pagetable_t pagetable, char *buf, uint64 va, uint64 len)
{
  return 0;
}

int
copyinstr(pagetable_t pagetable, char *buf, uint64 va, int max)
{
  return 0;
}
