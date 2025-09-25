#include "pic.h"

#define PIC1      0x20
#define PIC2      0xA0
#define PIC1_CMD  PIC1
#define PIC1_DAT  (PIC1+1)
#define PIC2_CMD  PIC2
#define PIC2_DAT  (PIC2+1)
#define PIC_EOI   0x20

static inline void outb(uint16_t p, uint8_t v){ __asm__ volatile("outb %0,%1"::"a"(v),"Nd"(p)); }
static inline uint8_t inb(uint16_t p){ uint8_t r; __asm__ volatile("inb %1,%0":"=a"(r):"Nd"(p)); return r; }

void pic_remap(uint8_t off1, uint8_t off2){
    uint8_t a1 = inb(PIC1_DAT);
    uint8_t a2 = inb(PIC2_DAT);

    outb(PIC1_CMD, 0x11);  // init, expect ICW4
    outb(PIC2_CMD, 0x11);

    outb(PIC1_DAT, off1);  // vector offset for master
    outb(PIC2_DAT, off2);  // vector offset for slave

    outb(PIC1_DAT, 0x04);  // tell master there is a slave at IRQ2
    outb(PIC2_DAT, 0x02);  // tell slave its cascade identity

    outb(PIC1_DAT, 0x01);  // 8086 mode
    outb(PIC2_DAT, 0x01);

    outb(PIC1_DAT, a1);    // restore masks
    outb(PIC2_DAT, a2);
}

void pic_send_eoi(uint8_t irq){
    if (irq >= 8) outb(PIC2_CMD, PIC_EOI);
    outb(PIC1_CMD, PIC_EOI);
}
