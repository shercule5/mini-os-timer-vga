#include "idt.h"
#include "isr.h"
#include <stdint.h>

extern void isr0(); extern void isr1(); extern void isr2(); extern void isr3();
extern void isr4(); extern void isr5(); extern void isr6(); extern void isr7();
extern void isr8();

static const char* exception_name[] = {
  "Divide-by-zero","Debug","NMI","Breakpoint","Overflow","Bound Range Exceeded",
  "Invalid Opcode","Device Not Available","Double Fault"
};

void isrs_install() {
    idt_set_gate(0,(uint32_t)isr0,0x08,0x8E);
    idt_set_gate(1,(uint32_t)isr1,0x08,0x8E);
    idt_set_gate(2,(uint32_t)isr2,0x08,0x8E);
    idt_set_gate(3,(uint32_t)isr3,0x08,0x8E);
    idt_set_gate(4,(uint32_t)isr4,0x08,0x8E);
    idt_set_gate(5,(uint32_t)isr5,0x08,0x8E);
    idt_set_gate(6,(uint32_t)isr6,0x08,0x8E);
    idt_set_gate(7,(uint32_t)isr7,0x08,0x8E);
    idt_set_gate(8,(uint32_t)isr8,0x08,0x8E);
}

/* simple VGA helpers (local so we don’t need another header) */
#define VGA ((uint16_t*)0xB8000)
#define COLS 80
static void puts_xy(const char* s, int row, int col, uint8_t color){
    uint16_t* p = VGA + row*COLS + col;
    while(*s) *p++ = ((uint16_t)color<<8) | (uint8_t)(*s++);
}

/* called from isr_common_stub with pointer to regs on stack */
void isr_handler(struct regs* r) {
    const char* name = (r->int_no < 9) ? exception_name[r->int_no] : "CPU Exception";
    puts_xy("EXCEPTION: ", 10, 20, 0x0C);
    puts_xy(name,        10, 32, 0x0C);
    for(;;) __asm__ __volatile__("hlt");
}
