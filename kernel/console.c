// console.c - Console input/output via UART
// Implements a simple line buffer for console input

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "spinlock.h"
#include "proc.h"

#define INPUT_BUF 128

struct {
    struct spinlock lock;
    char buf[INPUT_BUF];
    uint r;  // Read index
    uint w;  // Write index
    uint e;  // Edit index
} cons;

void
consputc(int c)
{
    if (c == '\n')
        uartputc('\r');
    uartputc(c);
}

void
consoleinit(void)
{
    spinlock_init(&cons.lock);
    cons.lock.name = "cons";
}

// Called when a character is received from UART
void
consoleintr(int c)
{
    spinlock_acquire(&cons.lock);

    switch (c) {
    case '\r':
        c = '\n';
        // fall through
    case '\n':
        cons.buf[cons.e++ % INPUT_BUF] = c;
        cons.w = cons.e;  // commit
        wakeup(&cons.r);
        break;
    case 0x7f:  // Backspace (DEL)
    case '\b':
        if (cons.e != cons.w) {
            cons.e--;
            consputc('\b');
            consputc(' ');
            consputc('\b');
        }
        break;
    default:
        if (c != 0 && cons.e - cons.r < INPUT_BUF) {
            cons.buf[cons.e++ % INPUT_BUF] = c;
        }
        break;
    }

    // Echo the character
    if (c != 0x7f && c != '\b')
        consputc(c);

    spinlock_release(&cons.lock);
}

// Read from console into user buffer
// (Simplified: returns one line at a time)
int
consoleread(int user_dst, uint64 dst, int n)
{
    uint target;
    int c;
    char cbuf;

    target = n;
    spinlock_acquire(&cons.lock);
    while (n > 0) {
        // wait until there are chars in the buffer
        while (cons.r == cons.w) {
            if (myproc()->killed) {
                spinlock_release(&cons.lock);
                return -1;
            }
            sleep_lock(&cons.r, &cons.lock);
        }

        c = cons.buf[cons.r++ % INPUT_BUF];

        // Store character for user
        cbuf = c;
        if (copyout(myproc()->pagetable, dst, &cbuf, 1) < 0)
            break;

        dst++;
        --n;

        if (c == '\n')
            break;
    }
    spinlock_release(&cons.lock);

    return target - n;
}

// Write to console from user buffer
int
consolewrite(int user_src, uint64 src, int n)
{
    for (int i = 0; i < n; i++) {
        char c;
        if (copyin(myproc()->pagetable, &c, src + i, 1) < 0)
            return -1;
        consputc(c);
    }
    return n;
}
