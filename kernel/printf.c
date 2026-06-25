// printf.c - Formatted print for ZephyrOS kernel

#include "types.h"
#include "riscv.h"
#include "defs.h"

static char digits[] = "0123456789abcdef";

static void
printint(int xx, int base, int sign)
{
  char buf[16];
  int i;
  uint64 x;

  if (sign && (sign = (xx < 0)))
    x = (uint64)(-xx);
  else
    x = (uint64)xx;

  i = 0;
  do {
    buf[i++] = digits[x % base];
  } while ((x /= base) != 0);

  if (sign)
    buf[i++] = '-';

  while (--i >= 0)
    uartputc(buf[i]);
}

static void
printptr(uint64 x)
{
  int i;
  char buf[17];

  buf[0] = '0';
  buf[1] = 'x';
  for (i = 15; i >= 2; i--) {
    buf[i] = digits[x & 0xf];
    x >>= 4;
  }
  for (i = 0; i < sizeof(buf); i++)
    uartputc(buf[i]);
}

void
printf_init(void)
{
  // Nothing to init
}

// Print to the console. Only understands %d, %x, %p, %s.
void
printf(char *fmt, ...)
{
  __builtin_va_list ap;
  int i, c;
  char *s;
  long l;

  __builtin_va_start(ap, fmt);
  for (i = 0; fmt[i]; i++) {
    c = fmt[i] & 0xff;
    if (c != '%') {
      uartputc(c);
      continue;
    }
    c = fmt[++i] & 0xff;
    if (c == 'd') {
      printint(__builtin_va_arg(ap, int), 10, 1);
    } else if (c == 'l') {
      l = __builtin_va_arg(ap, long);
      if (l < 0) {
        l = -l;
        uartputc('-');
      }
      // Print positive value
      {
        char tmp[20];
        int j = 0;
        do { tmp[j++] = digits[l % 10]; } while ((l /= 10) > 0);
        while (--j >= 0) uartputc(tmp[j]);
      }
    } else if (c == 'x') {
      printint(__builtin_va_arg(ap, int), 16, 0);
    } else if (c == 'p') {
      printptr(__builtin_va_arg(ap, uint64));
    } else if (c == 's') {
      s = __builtin_va_arg(ap, char *);
      if (s == 0)
        s = "(null)";
      while (*s) {
        uartputc(*s++);
      }
    } else if (c == '%') {
      uartputc('%');
    } else {
      uartputc('%');
      uartputc(c);
    }
  }
  __builtin_va_end(ap);
}

void
panic(char *s)
{
  printf("panic: ");
  printf(s);
  printf("\n");
  for (;;)
    asm volatile("wfi");
}
