// PIC + IRQ dispatch
#include "irq.h"
#include "io.h"
#include "isr.h"

static irq_handler_t s_handlers[16];
static uint8_t s_pic1_mask = 0xFF;
static uint8_t s_pic2_mask = 0xFF;

static inline void pic_write_masks(void){
    outb(PIC1_DATA, s_pic1_mask);
    outb(PIC2_DATA, s_pic2_mask);
}

void irq_remap(uint8_t master_offset, uint8_t slave_offset){
    uint8_t a1 = inb(PIC1_DATA), a2 = inb(PIC2_DATA);
    outb(PIC1_CMD, 0x11); outb(PIC2_CMD, 0x11);
    outb(PIC1_DATA, master_offset); outb(PIC2_DATA, slave_offset);
    outb(PIC1_DATA, 0x04); outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01); outb(PIC2_DATA, 0x01);
    outb(PIC1_DATA, a1);   outb(PIC2_DATA, a2);
    s_pic1_mask = a1; s_pic2_mask = a2;
}

void irq_init(void){
    irq_remap(0x20, 0x28);
    s_pic1_mask = 0xFF; s_pic2_mask = 0xFF; pic_write_masks();
    for(int i=0;i<16;i++) s_handlers[i]=0;
}

void irq_disable(uint8_t irq){
    if(irq<8) s_pic1_mask |= (uint8_t)(1u<<irq);
    else if(irq<16) s_pic2_mask |= (uint8_t)(1u<<(irq-8));
    else return;
    pic_write_masks();
}
void irq_enable(uint8_t irq){
    if(irq<8) s_pic1_mask &= (uint8_t)~(1u<<irq);
    else if(irq<16) s_pic2_mask &= (uint8_t)~(1u<<(irq-8));
    else return;
    pic_write_masks();
}

void irq_install_handler(int irq, irq_handler_t handler){
    if((unsigned)irq>15) return;
    s_handlers[irq]=handler;
    irq_enable((uint8_t)irq);
}
void irq_uninstall_handler(int irq){
    if((unsigned)irq>15) return;
    s_handlers[irq]=0; irq_disable((uint8_t)irq);
}
int irq_has_handler(int irq){ return (unsigned)irq<16 && s_handlers[irq]!=0; }

void irq_ack(int irq){
    if(irq>=8) outb(PIC2_CMD, PIC_EOI);
    outb(PIC1_CMD, PIC_EOI);
}

static inline void irq_dispatch(int irq, regs_t* r){
    irq_handler_t h = (irq>=0 && irq<16)? s_handlers[irq] : 0;
    if(h) h(r);
    irq_ack(irq);
}

void irq_common_handler_regs(int irq, regs_t* r){ irq_dispatch(irq, r); }
void irq_handler(int irq, regs_t* r){ irq_dispatch(irq, r); }   // shim for your stubs

