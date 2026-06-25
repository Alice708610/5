// exec.c - exec() system call (simplified: no file system, just a stub)

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "proc.h"

// In a full system, exec would load a binary from the file system.
// In this lean kernel, we don't have a file system, so exec returns -1.
// The first user process (init) is loaded directly via userinit().

int
exec(char *path, char **argv)
{
    return -1;
}
