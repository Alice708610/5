# ZephyrOS - Top-level Makefile
# 2026 National College Student Computer System Capability Competition
# RISC-V 64-bit OS Kernel for QEMU virt machine

QEMU = qemu-system-riscv64

QEMU_OPTS = -machine virt -bios default -kernel kernel/kernel.bin -m 128M -smp 1 -nographic

.PHONY: all clean run debug gdb kernel

all: kernel

kernel:
	$(MAKE) -C kernel

run: kernel
	@echo "============================================"
	@echo "  ZephyrOS - RISC-V 64 OS Kernel"
	@echo "  2026 System Capability Contest"
	@echo "============================================"
	$(QEMU) $(QEMU_OPTS)

debug: kernel
	@echo "Starting QEMU in debug mode (waiting for GDB on port 1234)..."
	$(QEMU) $(QEMU_OPTS) -S -gdb tcp::1234

gdb:
	riscv64-linux-gnu-gdb kernel/kernel.elf

clean:
	$(MAKE) -C kernel clean
