// vm.c - Virtual memory management (SV39 page tables)

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"

pagetable_t kernel_pagetable;

extern char _kernel_end[];  // from linker.ld
extern char _trampoline_start[];

// Walk the page table to find the PTE for a virtual address.
// If alloc != 0, create missing page table pages.
pte_t *
walk(pagetable_t pagetable, uint64 va, int alloc)
{
    if (va >= (1L << 39))  // beyond SV39 range
        return 0;

    for (int level = 2; level > 0; level--) {
        pte_t *pte = &pagetable[PX(level, va)];
        if (*pte & PTE_V) {
            pagetable = (pagetable_t)PTE2PA(*pte);
        } else {
            if (!alloc || (pagetable = (pagetable_t)kalloc()) == 0)
                return 0;
            memset(pagetable, 0, PGSIZE);
            *pte = PA2PTE((uint64)pagetable) | PTE_V;
        }
    }
    return &pagetable[PX(0, va)];
}

// Create PTEs for virtual addresses starting at va that refer to
// physical addresses starting at pa.
int
mappages(pagetable_t pagetable, uint64 va, uint64 size, uint64 pa, int perm)
{
    uint64 a, last;
    pte_t *pte;

    a = PGROUNDDOWN(va);
    last = PGROUNDDOWN(va + size - 1);
    for (;;) {
        if ((pte = walk(pagetable, a, 1)) == 0)
            return -1;
        if (*pte & PTE_V)
            panic("mappages: remap");
        *pte = PA2PTE(pa) | perm | PTE_V;
        if (a == last)
            break;
        a += PGSIZE;
        pa += PGSIZE;
    }
    return 0;
}

// Remove mappings from a page table (va to va+npages*PGSIZE)
void
uvmunmap(pagetable_t pagetable, uint64 va, uint64 npages, int do_free)
{
    for (uint64 a = va; a < va + npages * PGSIZE; a += PGSIZE) {
        pte_t *pte = walk(pagetable, a, 0);
        if (pte == 0)
            panic("uvmunmap: walk");
        if ((*pte & PTE_V) == 0)
            panic("uvmunmap: not mapped");
        if (PTE_FLAGS(*pte) == PTE_V)
            panic("uvmunmap: not a leaf");
        if (do_free) {
            uint64 pa = PTE2PA(*pte);
            kfree((void *)pa);
        }
        *pte = 0;
    }
}

// Create an empty user page table
pagetable_t
uvmcreate(void)
{
    pagetable_t pagetable = (pagetable_t)kalloc();
    if (pagetable == 0)
        return 0;
    memset(pagetable, 0, PGSIZE);
    return pagetable;
}

// Load user program into a new page table at virtual address 0
void
uvmfirst(pagetable_t pagetable, uchar *src, uint sz)
{
    char *mem;
    if (sz > PGSIZE)
        panic("uvmfirst: more than a page");
    mem = kalloc();
    if (mem == 0)
        panic("uvmfirst: kalloc failed");
    memset(mem, 0, PGSIZE);
    memmove(mem, src, sz);
    if (mappages(pagetable, 0, PGSIZE, (uint64)mem, PTE_R | PTE_W | PTE_X | PTE_U) < 0)
        panic("uvmfirst: mappages failed");
}

// Allocate page table entries from oldsz to newsz
uint64
uvmalloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz)
{
    char *mem;
    uint64 a;

    if (newsz < oldsz)
        return oldsz;

    oldsz = PGROUNDUP(oldsz);
    a = oldsz;
    for (; a < newsz; a += PGSIZE) {
        mem = kalloc();
        if (mem == 0) {
            uvmdealloc(pagetable, a, oldsz);
            return 0;
        }
        memset(mem, 0, PGSIZE);
        if (mappages(pagetable, a, PGSIZE, (uint64)mem, PTE_R | PTE_W | PTE_U) != 0) {
            kfree(mem);
            uvmdealloc(pagetable, a, oldsz);
            return 0;
        }
    }
    return newsz;
}

// Deallocate from oldsz to newsz
void
uvmdealloc(pagetable_t pagetable, uint64 oldsz, uint64 newsz)
{
    if (newsz >= oldsz)
        return;

    if (PGROUNDUP(newsz) < PGROUNDUP(oldsz)) {
        int npages = (PGROUNDUP(oldsz) - PGROUNDUP(newsz)) / PGSIZE;
        uvmunmap(pagetable, PGROUNDUP(newsz), npages, 1);
    }
}

// Recursively free page table pages
void
freewalk(pagetable_t pagetable)
{
    for (int i = 0; i < 512; i++) {
        pte_t pte = pagetable[i];
        if (pte & PTE_V) {
            pagetable_t child = (pagetable_t)PTE2PA(pte);
            if (pte & (PTE_R | PTE_W | PTE_X))
                panic("freewalk: leaf");
            freewalk(child);
            pagetable[i] = 0;
        }
    }
    kfree((void *)pagetable);
}

// Free a process's page table and all its physical memory
void
proc_freepagetable(pagetable_t pagetable, uint64 sz)
{
    uvmunmap(pagetable, TRAMPOLINE, 1, 0);
    uvmunmap(pagetable, TRAPFRAME, 1, 0);
    if (sz > 0) {
        uvmunmap(pagetable, 0, PGROUNDUP(sz) / PGSIZE, 1);
    }
    freewalk(pagetable);
}

// Copy a page table (for fork)
int
uvmcopy(pagetable_t old, pagetable_t new, uint64 sz)
{
    pte_t *pte;
    uint64 pa, i;
    uint flags;
    char *mem;

    for (i = 0; i < sz; i += PGSIZE) {
        if ((pte = walk(old, i, 0)) == 0)
            panic("uvmcopy: pte should exist");
        if ((*pte & PTE_V) == 0)
            panic("uvmcopy: page not present");
        pa = PTE2PA(*pte);
        flags = PTE_FLAGS(*pte);
        if ((mem = kalloc()) == 0)
            goto err;
        memmove(mem, (char *)pa, PGSIZE);
        if (mappages(new, i, PGSIZE, (uint64)mem, flags) != 0) {
            kfree(mem);
            goto err;
        }
    }
    return 0;

err:
    uvmunmap(new, 0, i / PGSIZE, 1);
    return -1;
}

// Mark a PTE as non-user-accessible (for guard pages)
void
uvmclear(pagetable_t pagetable, uint64 va)
{
    pte_t *pte = walk(pagetable, va, 0);
    if (pte == 0)
        panic("uvmclear");
    *pte &= ~PTE_U;
}

// Translate a user virtual address to physical
uint64
walkaddr(pagetable_t pagetable, uint64 va)
{
    pte_t *pte;
    uint64 pa;

    if (va >= (1L << 39))
        return 0;

    pte = walk(pagetable, va, 0);
    if (pte == 0)
        return 0;
    if ((*pte & PTE_V) == 0)
        return 0;
    if ((*pte & PTE_U) == 0)
        return 0;
    pa = PTE2PA(*pte);
    return pa;
}

// Copy from kernel to user
int
copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len)
{
    uint64 n, va0, pa0;

    while (len > 0) {
        va0 = PGROUNDDOWN(dstva);
        pa0 = walkaddr(pagetable, va0);
        if (pa0 == 0)
            return -1;
        n = PGSIZE - (dstva - va0);
        if (n > len)
            n = len;
        memmove((void *)(pa0 + (dstva - va0)), src, n);
        len -= n;
        src += n;
        dstva = va0 + PGSIZE;
    }
    return 0;
}

// Copy from user to kernel
int
copyin(pagetable_t pagetable, char *dst, uint64 srcva, uint64 len)
{
    uint64 n, va0, pa0;

    while (len > 0) {
        va0 = PGROUNDDOWN(srcva);
        pa0 = walkaddr(pagetable, va0);
        if (pa0 == 0)
            return -1;
        n = PGSIZE - (srcva - va0);
        if (n > len)
            n = len;
        memmove(dst, (void *)(pa0 + (srcva - va0)), n);
        len -= n;
        dst += n;
        srcva = va0 + PGSIZE;
    }
    return 0;
}

// Copy a null-terminated string from user to kernel
int
copyinstr(pagetable_t pagetable, char *dst, uint64 srcva, uint64 max)
{
    uint64 n, va0, pa0;
    int got_null = 0;

    while (got_null == 0 && max > 0) {
        va0 = PGROUNDDOWN(srcva);
        pa0 = walkaddr(pagetable, va0);
        if (pa0 == 0)
            return -1;
        n = PGSIZE - (srcva - va0);
        if (n > max)
            n = max;

        char *p = (char *)(pa0 + (srcva - va0));
        while (n > 0) {
            if (*p == '\0') {
                *dst = '\0';
                got_null = 1;
                break;
            } else {
                *dst = *p;
            }
            --n;
            --max;
            p++;
            dst++;
        }
        srcva = va0 + PGSIZE;
    }
    if (got_null)
        return 0;
    return -1;
}

// --- Kernel page table setup ---

// Add a mapping from va to pa with given permissions
static void
kvmmap(pagetable_t pagetable, uint64 va, uint64 pa, uint64 sz, int perm)
{
    if (mappages(pagetable, va, sz, pa, perm) < 0)
        panic("kvmmap");
}

// Create the kernel page table
void
kvminit(void)
{
    kernel_pagetable = (pagetable_t)kalloc();
    memset(kernel_pagetable, 0, PGSIZE);

    // Map UART
    kvmmap(kernel_pagetable, UART0, UART0, PGSIZE, PTE_R | PTE_W);

    // Map VirtIO (for future use)
    kvmmap(kernel_pagetable, VIRTIO0, VIRTIO0, PGSIZE, PTE_R | PTE_W);

    // Map PLIC
    kvmmap(kernel_pagetable, PLIC, PLIC, 0x4000000, PTE_R | PTE_W);

    // Map CLINT
    kvmmap(kernel_pagetable, CLINT, CLINT, 0x10000, PTE_R | PTE_W);

    // Map kernel text (executable, read-only)
    kvmmap(kernel_pagetable, KERNLINK, KERNLINK, (uint64)_trampoline_start - KERNLINK,
           PTE_R | PTE_X);

    // Map kernel data and free memory (read-write)
    kvmmap(kernel_pagetable, (uint64)_trampoline_start, (uint64)_trampoline_start,
           PHYSTOP - (uint64)_trampoline_start, PTE_R | PTE_W);

    // Map the trampoline (read-execute, at TRAMPOLINE virtual address)
    kvmmap(kernel_pagetable, TRAMPOLINE, (uint64)_trampoline_start, PGSIZE,
           PTE_R | PTE_X);
}

// Switch to kernel page table
void
kvminithart(void)
{
    w_satp(MAKE_SATP(kernel_pagetable));
    sfence_vma();
}
