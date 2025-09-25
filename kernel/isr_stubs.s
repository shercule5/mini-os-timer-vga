# kernel/isr_stubs.s — 32-bit exception + IRQ stubs
.code32

# ---------------- Exceptions (ISRs) 0..19 ----------------
# For exceptions WITHOUT an error code, push a fake 0 so the
# common stub stack layout is always [err_code][int_no] ...
.macro ISR_NOERR n
.global isr\n
isr\n:
    cli
    pushl $0          # fake err_code
    pushl $\n         # int_no
    jmp isr_common_stub
.endm

# For exceptions WITH an error code, CPU already pushed err_code.
.macro ISR_ERR n
.global isr\n
isr\n:
    cli
    pushl $\n         # int_no (err_code already on stack)
    jmp isr_common_stub
.endm

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19

.global isr_common_stub
isr_common_stub:
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    mov $0x10, %ax       # kernel data selector
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    pushl %esp           # pass pointer to regs
    call isr_handler
    add $4, %esp

    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa
    add $8, %esp         # pop [err_code][int_no]
    sti
    iret

# ---------------- IRQs 0..15 (vectors 32..47) ----------------
# After PIC remap to 0x20/0x28
.macro IRQ_STUB n
.global irq\n
irq\n:
    cli
    pushl $0                 # dummy err_code
    .set VEC, 32+\n
    pushl $VEC               # int_no = 32+n
    jmp irq_common_stub
.endm

IRQ_STUB 0
IRQ_STUB 1
IRQ_STUB 2
IRQ_STUB 3
IRQ_STUB 4
IRQ_STUB 5
IRQ_STUB 6
IRQ_STUB 7
IRQ_STUB 8
IRQ_STUB 9
IRQ_STUB 10
IRQ_STUB 11
IRQ_STUB 12
IRQ_STUB 13
IRQ_STUB 14
IRQ_STUB 15

.global irq_common_stub
irq_common_stub:
    pusha
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs

    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    pushl %esp
    call irq_handler
    add $4, %esp

    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa
    add $8, %esp          # pop [err_code][int_no]
    sti
    iret
