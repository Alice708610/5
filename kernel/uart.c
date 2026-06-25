// uart.c - UART driver for QEMU virt machine (NS16550A compatible)

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

// The QEMU virt machine's UART (NS16550A compatible)
#define UART0 0x10000000L

// UART register offsets
#define RHR 0   // Receive Holding Register (read)
#define THR 1   // Transmit Holding Register (write)
#define IER 1   // Interrupt Enable Register
#define FCR 2   // FIFO Control Register
#define LCR 3   // Line Control Register
#define MCR 4   // Modem Control Register
#define LSR 5   // Line Status Register
#define IER_RX_ENABLE  0x01
#define FCR_FIFO_ENABLE 0x01
#define FCR_FIFO_CLEAR  0x06
#define LCR_8N1        0x03

static int uart_initialized = 0;

void
uartputc(int c)
{
  volatile unsigned char *uart = (volatile unsigned char *)UART0;
  // Wait for Transmit Holding Register empty
  while ((uart[LSR] & 0x20) == 0)
    ;
  uart[THR] = c & 0xff;
}

int
uartgetc(void)
{
  volatile unsigned char *uart = (volatile unsigned char *)UART0;
  if ((uart[LSR] & 0x01) != 0) {
    return uart[RHR];
  }
  return -1;
}

void
consoleinit(void)
{
  volatile unsigned char *uart = (volatile unsigned char *)UART0;

  // Disable interrupts
  uart[IER] = 0x00;
  // Enable FIFO, clear buffers
  uart[FCR] = FCR_FIFO_ENABLE | FCR_FIFO_CLEAR;
  // 8 bits, no parity, one stop bit (8N1)
  uart[LCR] = LCR_8N1;
  // IRQs enabled, RTS/DSR set
  uart[MCR] = 0x00;

  uart_initialized = 1;
}

// Console interrupt handler - called from trap.c
void
consoleintr(int c)
{
  if (c >= 0) {
    // Echo back
    uartputc(c);
  }
}
