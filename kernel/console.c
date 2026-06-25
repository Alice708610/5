// console.c - Console input/output for ZephyrOS

#include "types.h"
#include "defs.h"

// A simple console: read input from UART, write output to UART.
// No shell yet, just echo input.

#define CONSOLE_BUF 128

static char inputbuf[CONSOLE_BUF];
static int inputr, inputw;

void
consoleinit(void)
{
  // UART already initialized by uart.c
  inputr = inputw = 0;
}

// Called from trap.c when UART interrupt fires
void
consoleintr(int c)
{
  if (c == '\r' || c == '\n') {
    // Echo newline
    uartputc('\n');
    // For now, just print a prompt
    printf("ZephyrOS# ");
  } else if (c == 0x7f || c == '\b') {
    // Backspace
    if (inputw > inputr) {
      inputw--;
      uartputc('\b');
      uartputc(' ');
      uartputc('\b');
    }
  } else if (c >= ' ' && c <= '~') {
    // Printable character
    if (inputw < inputr + CONSOLE_BUF) {
      inputbuf[inputw++ % CONSOLE_BUF] = c;
      uartputc(c);
    }
  }
}
