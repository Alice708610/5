# initcode.S - First user program (loaded at virtual address 0)
# Assembled into a flat binary and embedded in the kernel.
# No data section — everything is inline.

.set SYS_write,  9
.set SYS_read,   10
.set SYS_exit,   2
.set SYS_uptime, 8
.set SYS_getpid, 4
.set SYS_fork,   1
.set SYS_wait,   3
.set SYS_sbrk,   6

.section .text
.globl _start
_start:
    # Stack pointer at top of first page (we'll grow it with sbrk)
    li sp, 4096

    # Write welcome message to console
    # Build the message on the stack
    li t0, 0x6570797a   # "zype" (reversed for little-endian)
    sd t0, -8(sp)
    li t0, 0x535f6f7268  # "rhOS_" -> "ZephyrOS"
    sd t0, -16(sp)
    # Actually this is getting too complex. Let's just use inline strings.

    # Simpler approach: write characters one at a time using a loop
    # Message: "ZephyrOS ready\n"
    # Let's just write a single 'H' character to prove it works

    # Write 'H' to stdout
    li a7, SYS_write
    li a0, 1            # fd = stdout
    # Put 'H' (0x48) on the stack
    li t0, 0x48         # 'H'
    sb t0, -8(sp)
    addi a1, sp, -8     # buffer
    li a2, 1            # length
    ecall

    # Write 'i' to stdout
    li a7, SYS_write
    li a0, 1
    li t0, 0x69         # 'i'
    sb t0, -8(sp)
    addi a1, sp, -8
    li a2, 1
    ecall

    # Write newline
    li a7, SYS_write
    li a0, 1
    li t0, 10           # '\n'
    sb t0, -8(sp)
    addi a1, sp, -8
    li a2, 1
    ecall

    # Get PID
    li a7, SYS_getpid
    ecall

    # Print "> " prompt
    li a7, SYS_write
    li a0, 1
    li t0, 0x3e         # '>'
    sb t0, -8(sp)
    li t0, 0x20         # ' '
    sb t0, -7(sp)
    addi a1, sp, -8
    li a2, 2
    ecall

    # Simple echo loop: read 1 byte, echo it back
    # Press 'q' to exit
loop:
    li a7, SYS_read
    li a0, 0
    addi a1, sp, -8
    li a2, 1
    ecall

    beqz a0, loop

    lbu t1, -8(sp)

    # Check 'q' (0x71)
    li t2, 0x71
    beq t1, t2, do_exit

    # Check 'q' uppercase 'Q' (0x51)
    li t2, 0x51
    beq t1, t2, do_exit

    # Echo character back
    li a7, SYS_write
    li a0, 1
    addi a1, sp, -8
    li a2, 1
    ecall

    j loop

do_exit:
    li a7, SYS_exit
    li a0, 0
    ecall
    j .
