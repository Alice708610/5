# kernel/Makefile - Build ZephyrOS kernel

TOOLPREFIX = riscv64-linux-gnu-
CC = $(TOOLPREFIX)gcc
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
OBJDUMP = $(TOOLPREFIX)objdump

CFLAGS = -Wall -Werror -O2 -fno-omit-frame-pointer -ggdb
CFLAGS += -march=rv64gc -mabi=lp64
CFLAGS += -ffreestanding -nostdinc -nostdlib -fno-builtin -fno-stack-protector
CFLAGS += -fno-common -fvisibility=hidden
CFLAGS += -I.
CFLAGS += -MD -MP

LDFLAGS = -z max-page-size=4096 -nostdlib

# Kernel objects (excluding initcode)
KOBJS = \
	entry.o \
	start.o \
	main.o \
	string.o \
	uart.o \
	plic.o \
	printf.o \
	console.o \
	spinlock.o \
	kalloc.o \
	vm.o \
	trap.o \
	proc.o \
	syscall.o \
	sysproc.o \
	exec.o \
	kernelvec.o \
	trampoline.o \
	swtch.o \
	initcode.o

all: kernel.bin

# Build initcode binary from initcode.S
initcode.bin: initcode.S
	$(CC) $(CFLAGS) -c initcode.S -o initcode_tmp.o
	$(OBJCOPY) -O binary initcode_tmp.o initcode.bin

# Convert initcode binary into an object file with symbol "initcode"
initcode.o: initcode.bin
	$(OBJCOPY) -I binary -O elf64-littleriscv -B riscv \
		--redefine-sym _binary_initcode_bin_start=initcode \
		--redefine-sym _binary_initcode_bin_end=initcode_end \
		--redefine-sym _binary_initcode_bin_size=initcode_size \
		initcode.bin initcode.o
	rm -f initcode_tmp.o

# Link the kernel ELF
kernel.elf: $(KOBJS) kernel.ld
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $(KOBJS)
	$(OBJDUMP) -S $@ > kernel.asm

# Create raw binary for QEMU
kernel.bin: kernel.elf
	$(OBJCOPY) -O binary $< $@

# Pattern rules
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.d *.elf *.bin *.asm *.sym initcode_tmp.o

-include *.d
