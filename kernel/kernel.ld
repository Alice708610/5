// Kernel linker script for QEMU virt (RISC-V 64)
// Kernel loads at 0x80200000 (after OpenSBI at 0x80000000)

OUTPUT_ARCH("riscv")
ENTRY(_entry)

SECTIONS
{
    . = 0x80200000;

    .text : {
        *(.text .text.*)
        . = ALIGN(0x1000);
        _trampoline_start = .;
        *(.trampoline)
        . = ALIGN(0x1000);
        ASSERT(. - _trampoline_start <= 0x1000, "trampoline > 4096");
    }

    .rodata : {
        *(.rodata .rodata.*)
    }

    .data : {
        *(.data .data.*)
    }

    .bss : {
        _bss_start = .;
        *(.sbss .sbss.*)
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(0x1000);
        _bss_end = .;
    }

    _kernel_end = .;
}
