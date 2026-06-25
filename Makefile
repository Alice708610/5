K = kernel
CROSS_COMPILE = riscv64-linux-gnu-
AS = $(CROSS_COMPILE)as
LD = $(CROSS_COMPILE)ld
CC = $(CROSS_COMPILE)gcc
CPP = $(CC) -E
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

CFLAGS = -Wall -Werror -O2 -ffreestanding -fno-common -nostdlib -fno-pic -fno-stack-protector -fno-pie -mcmodel=medany -march=rv64imafdc -mabi=lp64d

LDFLAGS = -z max-page-size=0x1000

QEMU = qemu-system-riscv64
CPUS = 3
QEMUOPTS = -machine virt -bios none -kernel $(K)/kernel -m 128M -smp $(CPUS) -nographic

OBJS = \
  $(K)/entry.o \
  $(K)/start.o \
  $(K)/console.o \
  $(K)/exec.o \
  $(K)/kalloc.o \
  $(K)/main.o \
  $(K)/plic.o \
  $(K)/printf.o \
  $(K)/proc.o \
  $(K)/spinlock.o \
  $(K)/string.o \
  $(K)/swtch.o \
  $(K)/syscall.o \
  $(K)/sysproc.o \
  $(K)/trap.o \
  $(K)/uart.o \
  $(K)/vm.o \

# Default target
all: $(K)/kernel

$(K)/kernel: $(OBJS) $(K)/kernel.ld
	$(LD) $(LDFLAGS) -T $(K)/kernel.ld -o $(K)/kernel $(OBJS)
	$(OBJDUMP) -S $(K)/kernel > $(K)/kernel.asm
	$(OBJDUMP) -t $(K)/kernel | sed '1,/Symbol/d' | sort > $(K)/kernel.sym

$(K)/initcode: $(K)/initcode.S
	$(CC) $(CFLAGS) -c $(K)/initcode.S -o $(K)/initcode.o
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o $(K)/initcode.out $(K)/initcode.o
	$(OBJCOPY) -S -O binary $(K)/initcode.out $(K)/initcode

$(K)/%.o: $(K)/%.c
	$(CC) $(CFLAGS) -I$(K) -c -o $@ $<

$(K)/%.o: $(K)/%.S
	$(CC) $(CFLAGS) -I$(K) -c -o $@ $<

clean:
	rm -f $(K)/*.o $(K)/*.d $(K)/*.asm $(K)/*.sym $(K)/kernel $(K)/initcode $(K)/initcode.out fs.img

run: $(K)/kernel $(K)/initcode
	$(QEMU) $(QEMUOPTS)

.PHONY: all clean run
