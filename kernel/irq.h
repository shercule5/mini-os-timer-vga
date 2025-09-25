#pragma once
#include <stdint.h>
#include "isr.h"   // regs_t

typedef void (*irq_handler_t)(regs_t *r);

#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI   0x20

void irq_init(void);
void irq_remap(uint8_t master_offset, uint8_t slave_offset);
void irq_install_handler(int irq, irq_handler_t handler);
void irq_uninstall_handler(int irq);
void irq_disable(uint8_t irq);
void irq_enable(uint8_t irq);
void irq_ack(int irq);
int  irq_has_handler(int irq);

static inline void irq_register(int irq, irq_handler_t h){ irq_install_handler(irq, h); }
