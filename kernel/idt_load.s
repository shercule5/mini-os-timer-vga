# kernel/idt_load.s — lidt wrapper (32-bit)
.code32
.global idt_load
idt_load:
    mov 4(%esp), %eax
    lidt (%eax)
    ret
