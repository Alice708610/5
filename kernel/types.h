// Basic types for ZephyrOS kernel
// RISC-V 64-bit (lp64 ABI)

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  uint64;
typedef unsigned int   uint32;
typedef unsigned long  uintptr;

typedef char  int8;
typedef short int16;
typedef int   int32;
typedef long  int64;

typedef uint64 pde_t;
typedef uint64 pte_t;
typedef uint64 *pagetable_t;
