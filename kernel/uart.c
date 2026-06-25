// uart.c - NS16550A UART driver (QEMU virt machine)
// The UART is memory-mapped at 0x10000000

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"

// UART registers (offset from UART0 base)
#define RHR 0  // Receive Holding Register (read)
#define THR 0  // Transmit Holding Register (write)
#define IER 1  // Interrupt Enable Register
#define FCR 2  // FIFO Control Register
#define ISR 2  // Interrupt Status Register (read)
#define LCR 3  // Line Control Register
#define LSR 5  // Line Status Register

#define LSR_RX_READY  0x01
#define LSR_TX_IDLE   0x20

// Read a UART register
static inline uchar
uart_r(int reg)
{
    volatile uchar *p = (uchar *)UART0;
    return p[reg];
}

// Write a UART register
static inline void
uart_w(int reg, uchar val)
{
    volatile uchar *p = (uchar *)UART0;
    p[reg] = val;
}

void
uartinit(void)
{
    // Disable interrupts
    uart_w(IER, 0x00);

    // Enable FIFO, clear them, with 14-byte threshold
    uart_w(FCR, 0x07);

    // Set word length to 8 bits (LCR=3)
    uart_w(LCR, 0x03);

    // Enable RX interrupts
    uart_w(IER, 0x01);
}

void
uartputc(int c)
{
    // Wait for transmit to be ready
    while ((uart_r(LSR) & LSR_TX_IDLE) == 0)
        ;
    uart_w(THR, (uchar)c);
}

int
uartgetc(void)
{
    if (uart_r(LSR) & LSR_RX_READY) {
        return uart_r(RHR);
    }
    return -1;
}

void
uartintr(void)
{
    // Read and process characters from UART
    while (1) {
        int c = uartgetc();
        if (c == -1)
            break;
        consoleintr(c);
    }
}
