// kernel/kernel.c — safe boot path (no interrupts by default), with heartbeat

#include <stdint.h>
#include "io.h"
#include "irq.h"
#include "timer.h"

/* Toggle this to 1 once your IDT/IRQ stubs are definitely correct. */
#define ENABLE_TIMER_AND_IRQS 0

/* Weak names so either *_install or *_init works. */
void gdt_install(void) __attribute__((weak));
void gdt_init(void)    __attribute__((weak));
void idt_install(void) __attribute__((weak));
void idt_init(void)    __attribute__((weak));

/* Tiny VGA text writer */
static volatile uint16_t* const VGA = (uint16_t*)0xB8000;
static int cur_row = 0, cur_col = 0;
static uint8_t attr = 0x07;

static void vga_putc(char c) {
    if (c == '\n') { cur_col = 0; if (++cur_row >= 25) cur_row = 24; return; }
    VGA[cur_row * 80 + cur_col] = (uint16_t)c | ((uint16_t)attr << 8);
    if (++cur_col >= 80) { cur_col = 0; if (++cur_row >= 25) cur_row = 24; }
}
static void vga_print(const char* s) { while (*s) vga_putc(*s++); }
static void vga_print_u32(uint32_t x) {
    char b[11]; int i = 0; if (x == 0){ vga_putc('0'); return; }
    while (x && i < 10){ b[i++] = (char)('0' + (x % 10)); x /= 10; }
    while (i--) vga_putc(b[i]);
}

/* Simple delay so we can show progress without PIT interrupts */
static void busy_delay(uint32_t loops) {
    for (volatile uint32_t i = 0; i < loops; ++i) {
        __asm__ volatile ("" ::: "memory");
    }
}

static inline void unmask_irq0(void) {
    uint8_t m = inb(0x21);
    outb(0x21, (uint8_t)(m & ~0x01));  /* enable IRQ0 on master PIC */
}

static void setup_gdt_idt(void) {
    if (gdt_install) gdt_install(); else if (gdt_init) gdt_init();
    if (idt_install) idt_install(); else if (idt_init) idt_init();
}

static int have_idt_setup(void) {
    return (idt_install != 0) || (idt_init != 0);
}

void kmain(void) {
    vga_print("kmain: hello from C\n");
    setup_gdt_idt();

#if ENABLE_TIMER_AND_IRQS
    if (!have_idt_setup()) {
        vga_print("kmain: NO IDT init symbol; cannot enable IRQs.\n");
    } else {
        vga_print("kmain: IDT present; enabling timer IRQ0 @ 100 Hz...\n");
        irq_init();          /* PIC remap + masks */
        unmask_irq0();       /* allow IRQ0 */
        timer_init(100);     /* PIT @ 100 Hz */
        __asm__ volatile("sti");   /* enable interrupts */
        vga_print("kernel: timer online, printing every 1s...\n");
    }
#endif

    /* Heartbeat loop that never uses interrupts (safe on any IDT state). */
    uint32_t soft_seconds = 0;
    for (;;) {
#if ENABLE_TIMER_AND_IRQS
        /* If interrupts are enabled and PIT is running, prefer real timer. */
        if (have_idt_setup()) {
            static uint64_t last = 0;
            uint64_t t = timer_ticks();
            if (t - last >= timer_hz()) {
                last = t;
                vga_print("uptime: ");
                vga_print_u32(timer_seconds());
                vga_print("s\n");
            }
        } else
#endif
        {
            /* Fallback: software heartbeat so you know we didn’t hang. */
            busy_delay(50u * 1000u * 1000u);   /* tune if too fast/slow */
            vga_print("soft-uptime: ");
            vga_print_u32(++soft_seconds);
            vga_print("s\n");
        }
        __asm__ volatile("hlt");
    }
}
