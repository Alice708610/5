K = kernel
CROSS_COMPILE = riscv64-linux-gnu-
AS = as
LD = ld
CC = gcc
CPP =  -E
OBJCOPY = objcopy
OBJDUMP = objdump

CFLAGS = -Wall -Werror -O2 -ffreestanding -fno-common -nostdlib -fno-pic -fno-stack-protector -fno-pie -mcmodel=medany -march=rv64imafdc -mabi=lp64d

LDFLAGS = -z max-page-size=0x1000

QEMU = qemu-system-riscv64
CPUS = 3
QEMUOPTS = -machine virt -bios none -kernel /kernel -m 128M -smp  -nographic

OBJS = \
	/entry.o \
	/console.o \
	/kalloc.o \
	/kernelvec.o \
	/main.o \
	/plic.o \
	/printf.o \
	/proc.o \
	/spinlock.o \
	/string.o \
	/swtch.o \
	/trap.o \
	/uart.o \
	/vm.o \

all: /kernel

/kernel:  /kernel.ld
	  -T /kernel.ld -o /kernel 
	 -S /kernel > /kernel.asm
	 -t /kernel | sed '1,/Symbol/d' | sort > /kernel.sym

/initcode: /initcode.S
	  -c /initcode.S -o /initcode.o
	  -N -e start -Ttext 0 -o /initcode.out /initcode.o
	 -S -O binary /initcode.out /initcode

/%.o: /%.c
	  -I -c -o $@ $<

/%.o: /%.S
	  -I -c -o $@ $<

clean:
	rm -f /*.o /*.d /*.asm /*.sym /kernel /initcode /initcode.out fs.img

run: /kernel
	 

.PHONY: all clean run
