// types.h - Basic types for ZephyrOS (RISC-V 64-bit)

typedef signed char         int8;
typedef unsigned char       uint8;
typedef short               int16;
typedef unsigned short      uint16;
typedef int                 int32;
typedef unsigned int        uint32;
typedef long                int64;
typedef unsigned long       uint64;

typedef uint64              pte_t;          // Page table entry
typedef uint64              pde_t;          // Page directory entry
typedef uint64 *            pagetable_t;    // Page table root pointer

// Context for swtch() - saved registers
struct context {
    uint64 ra;
    uint64 sp;

    // callee-saved
    uint64 s0;
    uint64 s1;
    uint64 s2;
    uint64 s3;
    uint64 s4;
    uint64 s5;
    uint64 s6;
    uint64 s7;
    uint64 s8;
    uint64 s9;
    uint64 s10;
    uint64 s11;
};
