/* Multiboot v1 header + proper stack + visible marker + entry */

.set ALIGN,    1<<0
.set MEMINFO,  1<<1
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

/* Multiboot header (must be in a loadable, allocated section).
   Our linker script places this FIRST inside .text. */
    .section .multiboot, "a"
    .align 4
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

/* Reserve a simple 16 KiB kernel stack in .bss and use it. */
    .section .bss
    .align 16
stack_bottom:
    .skip 16 * 1024
stack_top:

    .section .text
    .globl _start
_start:
    /* Set up a known-good stack for C code */
    mov $stack_top, %esp
    xor %ebp, %ebp

    /* Drop a big 'K' at the top-left so we know we entered the kernel */
    mov $0xB8000, %edi
    mov $'K', %al
    mov $0x07, %ah       /* light gray on black */
    mov %ax, (%edi)

    /* Jump into the C kernel */
    call kmain

.hang:
    hlt
    jmp .hang
