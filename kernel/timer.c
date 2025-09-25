#include "timer.h"
#include <stdint.h>
#include "io.h"
#include "irq.h"
#include "isr.h"

#define PIT_BASE 1193182u

static volatile uint32_t s_hz = 100;
static volatile uint64_t s_ticks = 0;
static volatile uint32_t s_tick_in_sec = 0;
static volatile uint32_t s_seconds = 0;

static void pit_callback(regs_t* r){
    (void)r;
    s_ticks++;
    if(++s_tick_in_sec >= s_hz){
        s_tick_in_sec = 0;
        s_seconds++;
    }
}

void timer_init(uint32_t hz){
    if(hz==0) hz=100;
    s_hz = hz;

    irq_install_handler(0, pit_callback);

    uint32_t divisor = PIT_BASE / s_hz;
    if(!divisor) divisor = 1;

    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

uint64_t timer_ticks(void){ return s_ticks; }
uint32_t timer_hz(void){ return s_hz; }
uint32_t timer_seconds(void){ return s_seconds; }

