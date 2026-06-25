// printf.c - Kernel printf implementation (no floating point, no stdlib)

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "spinlock.h"

// We can't use <stdarg.h> because of -nostdinc,
// so use __builtin_va_list directly (GCC/Clang built-in).
typedef __builtin_va_list va_list;
#define va_start(ap, last)     __builtin_va_start(ap, last)
#define va_arg(ap, type)       __builtin_va_arg(ap, type)
#define va_end(ap)             __builtin_va_end(ap)

static struct spinlock printf_lock;

static char digits[] = "0123456789abcdef";

static void
printint(long long n, int base, int sign)
{
    char buf[32];
    int i = 0;
    uint64 x;

    if (sign && (sign = (n < 0)))
        x = -n;
    else
        x = n;

    do {
        buf[i++] = digits[x % base];
        x /= base;
    } while (x != 0 && i < (int)sizeof(buf));

    if (sign)
        buf[i++] = '-';

    while (--i >= 0)
        consputc(buf[i]);
}

void
printf(char *fmt, ...)
{
    spinlock_acquire(&printf_lock);

    va_list ap;
    va_start(ap, fmt);

    for (char *p = fmt; *p; p++) {
        if (*p != '%') {
            consputc(*p);
            continue;
        }
        p++;

        switch (*p) {
        case 'd':
            printint(va_arg(ap, int), 10, 1);
            break;
        case 'x':
            printint(va_arg(ap, unsigned int), 16, 0);
            break;
        case 'l':
            p++;
            if (*p == 'd') {
                printint(va_arg(ap, long), 10, 1);
            } else if (*p == 'x') {
                printint(va_arg(ap, unsigned long), 16, 0);
            } else if (*p == 'u') {
                printint(va_arg(ap, unsigned long), 10, 0);
            } else {
                p--;
            }
            break;
        case 'u':
            printint(va_arg(ap, unsigned int), 10, 0);
            break;
        case 'p':
            printint(va_arg(ap, uint64), 16, 0);
            break;
        case 'c':
            consputc(va_arg(ap, int));
            break;
        case 's': {
            char *s = va_arg(ap, char *);
            if (s == 0)
                s = "(null)";
            for (; *s; s++)
                consputc(*s);
            break;
        }
        case '%':
            consputc('%');
            break;
        default:
            consputc('%');
            consputc(*p);
            break;
        }
    }

    va_end(ap);
    spinlock_release(&printf_lock);
}

void
panic(char *s)
{
    printf("\npanic: ");
    printf(s);
    printf("\n");
    for (;;)
        ;
}

void
printfinit(void)
{
    spinlock_init(&printf_lock);
}
