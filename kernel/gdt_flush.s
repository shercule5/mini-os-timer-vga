.code32
.global gdt_flush
gdt_flush:
    mov 4(%esp), %eax
    lgdt (%eax)

    mov $0x10, %ax       # data selector (index 2)
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    ljmp $0x08, $1f      # far jump to reload CS (code selector index 1)
1:
    ret

