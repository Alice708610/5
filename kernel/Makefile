# ZephyrOS Kernel Makefile
# Target: RISC-V 64 (QEMU virt machine)

CROSS_COMPILE = riscv64-linux-gnu-
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)gcc -c
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

CFLAGS = -Wall -Werror -O2 -fno-common -fno-pic -fno-stack-protector \
         -fno-pie -ffreestanding -mcmodel=medany -march=rv64imafdc \
         -mabi=lp64d -I.

LDFLAGS = -z max-page-size=0x1000

# C source files (non-empty only)
C_SRCS := main.c console.c uart.c printf.c string.c spinlock.c proc.c \
          trap.c plic.c swtch.c kalloc.c vm.c

# Assembly source files
ASM_SRCS := entry.S swtch.S kernelvec.S

# Object files
C_OBJS := $(C_SRCS:.c=.o)
ASM_OBJS := $(ASM_SRCS:.S=.o)
OBJS := $(ASM_OBJS) $(C_OBJS)

KERNEL = kernel

all: $(KERNEL)

$(KERNEL): $(OBJS) kernel.ld
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $(OBJS)
	$(OBJDUMP) -S $@ > kernel.asm
	$(OBJDUMP) -t $@ | grep ' text' | sort > kernel.sym

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(AS) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(KERNEL) kernel.asm kernel.sym initcode.out fs.img

run: $(KERNEL)
	qemu-system-riscv64 -machine virt -bios none -kernel $(KERNEL) -nographic -smp 2 -m 128M

.PHONY: all clean run
