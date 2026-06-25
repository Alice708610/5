// kalloc.c - Physical page allocator
// Manages free memory between _kernel_end and PHYSTOP

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"

extern char _kernel_end[];  // Defined by linker.ld

struct run {
    struct run *next;
};

struct {
    struct spinlock lock;
    struct run *freelist;
} kmem;

void
kinit(void)
{
    spinlock_init(&kmem.lock);
    kmem.lock.name = "kmem";
    freerange(_kernel_end, (void *)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
    char *p;
    p = (char *)PGROUNDUP((uint64)pa_start);
    for (; p + PGSIZE <= (char *)pa_end; p += PGSIZE)
        kfree(p);
}

// Free a page (add to freelist)
void
kfree(void *pa)
{
    if (((uint64)pa % PGSIZE) != 0 || (char *)pa < _kernel_end || (uint64)pa >= PHYSTOP)
        panic("kfree");

    // Fill with junk to catch dangling pointers
    memset(pa, 1, PGSIZE);

    struct run *r = (struct run *)pa;

    spinlock_acquire(&kmem.lock);
    r->next = kmem.freelist;
    kmem.freelist = r;
    spinlock_release(&kmem.lock);
}

// Allocate a page
void *
kalloc(void)
{
    struct run *r;

    spinlock_acquire(&kmem.lock);
    r = kmem.freelist;
    if (r)
        kmem.freelist = r->next;
    spinlock_release(&kmem.lock);

    if (r)
        memset((char *)r, 5, PGSIZE);  // fill with junk

    return r;
}
