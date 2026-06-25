/* kernel.ld - Linker script for ZephyrOS kernel (RISC-V 64, QEMU virt) */

OUTPUT_ARCH("riscv")
ENTRY(entry)

/*
 * Memory layout for QEMU virt machine:
 *   Physical RAM: 0x80000000 - 0x88000000 (128MB)
 *   Kernel loads at 0x80000000
 */
MEMORY
{
  RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS
{
  /* Start of kernel code at physical address 0x80000000 */
  .text : {
    *(.text.init)    /* entry.S first */
    *(.text .text.*)
  } > RAM

  .rodata : {
    *(.rodata .rodata.*)
  } > RAM

  .data : {
    *(.data .data.*)
  } > RAM

  .sdata : {
    __global_pointer$ = . + 0x800;
    *(.sdata .sdata.*)
  } > RAM

  .sbss : {
    *(.sbss .sbss.*)
  } > RAM

  .bss : {
    *(.bss .bss.*)
    *(COMMON)
  } > RAM

  _bss_end = ALIGN(8);

  /* Symbols used by entry.S for BSS clearing */
  edata = _bss_end;
  end   = _bss_end;
  _end  = ALIGN(8);
}
