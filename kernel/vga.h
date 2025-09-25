#pragma once
#include <stdint.h>

void vga_init(uint8_t color);
void vga_clear(void);
void vga_setcolor(uint8_t color);
void vga_setpos(int row, int col);
void vga_putc(char c);
void vga_puts(const char* s);
void vga_write_dec(uint32_t v);
void vga_write_hex(uint32_t v);

