#include <stdint.h>
#include "gdt.h"

struct __attribute__((packed)) gdt_entry {
    uint16_t limit_lo;
    uint16_t base_lo;
    uint8_t  base_mid;
    uint8_t  access;
    uint8_t  gran;
    uint8_t  base_hi;
};
struct __attribute__((packed)) gdt_ptr {
    uint16_t limit;
    uint32_t base;
};

static struct gdt_entry gdt[3];
extern void gdt_flush(uint32_t); // ASM

static void set_gate(int i, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran){
    gdt[i].limit_lo = limit & 0xFFFF;
    gdt[i].base_lo  = base & 0xFFFF;
    gdt[i].base_mid = (base >> 16) & 0xFF;
    gdt[i].access   = access;
    gdt[i].gran     = ((limit >> 16) & 0x0F) | (gran & 0xF0);
    gdt[i].base_hi  = (base >> 24) & 0xFF;
}

void gdt_install(void){
    set_gate(0,0,0,0,0);                 // null
    set_gate(1,0,0xFFFFF,0x9A,0xCF);     // code: base 0, limit 4GB, gran 4K, 32-bit
    set_gate(2,0,0xFFFFF,0x92,0xCF);     // data: same

    struct gdt_ptr gp = { .limit = sizeof(gdt)-1, .base = (uint32_t)&gdt };
    gdt_flush((uint32_t)&gp);
}

