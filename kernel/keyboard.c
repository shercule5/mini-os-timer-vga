// kernel/keyboard.c — minimal PS/2 keyboard IRQ handler using regs_t*
#include <stdint.h>
#include "io.h"
#include "irq.h"
#include "isr.h"

// If your vga.c provides this, we'll use it. Otherwise it's fine if it's missing.
extern void vga_print(const char *s) __attribute__((weak));

#define KBD_DATA_PORT 0x60
#define KBD_STATUS_PORT 0x64
#define KBD_STATUS_OBF 0x01  // Output buffer full

// Optional: tiny helper to print without depending on stdlib
static void kbd_note(const char* s) {
    if (vga_print) vga_print(s);
}

static void on_irq1(regs_t* r) {
    (void)r;

    // Only read when output buffer full
    uint8_t status = inb(KBD_STATUS_PORT);
    if (status & KBD_STATUS_OBF) {
        volatile uint8_t scancode = inb(KBD_DATA_PORT);
        (void)scancode; // consume; extend to decode if you want
    }

    // No explicit EOI here — irq.c sends EOI in irq_ack() after this returns.
}

void keyboard_init(void) {
    irq_register(1, on_irq1);   // IRQ1 = PS/2 keyboard
    kbd_note("keyboard: irq1 handler installed\n");
}
