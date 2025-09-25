# boot/boot.s — Multiboot v1 header + minimal 32-bit entry (GAS/AT&T)

.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

    # Put header in its own allocatable section; linker script will place it first.
    .section .multiboot, "a", @progbits
    .balign 8
    .globl multiboot_header
multiboot_header:
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

    .section .text
    .code32
    .globl start
    .extern kmain

start:
    cli
    call kmain

.hang:
    hlt
    jmp .hang
