#include "vga.h"

#define VGA_MEM ((uint16_t*)0xB8000)
#define COLS 80
#define ROWS 25

static int cur_r=0, cur_c=0;
static uint8_t cur_color=0x07;

static inline void outb(uint16_t p, uint8_t v){ __asm__ volatile("outb %0,%1"::"a"(v),"Nd"(p)); }

static void hw_cursor(){
    uint16_t pos=(uint16_t)(cur_r*COLS+cur_c);
    outb(0x3D4,0x0F); outb(0x3D5,(uint8_t)(pos&0xFF));
    outb(0x3D4,0x0E); outb(0x3D5,(uint8_t)(pos>>8));
}

void vga_init(uint8_t color){ cur_color=color; vga_clear(); }
void vga_setcolor(uint8_t c){ cur_color=c; }

void vga_clear(void){
    for(int i=0;i<COLS*ROWS;i++) VGA_MEM[i]=(cur_color<<8)|' ';
    cur_r=cur_c=0; hw_cursor();
}
static void scroll(){
    for(int r=1;r<ROWS;r++)
        for(int c=0;c<COLS;c++)
            VGA_MEM[(r-1)*COLS+c]=VGA_MEM[r*COLS+c];
    for(int c=0;c<COLS;c++)
        VGA_MEM[(ROWS-1)*COLS+c]=(cur_color<<8)|' ';
    if(cur_r>0) cur_r--; hw_cursor();
}
void vga_setpos(int row,int col){ cur_r=row; cur_c=col; hw_cursor(); }

void vga_putc(char ch){
    if(ch=='\n'){ cur_c=0; cur_r++; if(cur_r>=ROWS){ scroll(); cur_r=ROWS-1; } hw_cursor(); return; }
    if(ch=='\r'){ cur_c=0; hw_cursor(); return; }
    VGA_MEM[cur_r*COLS+cur_c]=(cur_color<<8)|(uint8_t)ch;
    cur_c++; if(cur_c>=COLS){ cur_c=0; cur_r++; if(cur_r>=ROWS){ scroll(); cur_r=ROWS-1; } }
    hw_cursor();
}
void vga_puts(const char* s){ while(*s) vga_putc(*s++); }

void vga_write_dec(uint32_t v){
    char buf[11]; int i=0; if(v==0) buf[i++]='0';
    else { char tmp[10]; int j=0; while(v){ tmp[j++]='0'+(v%10); v/=10; } while(j) buf[i++]=tmp[--j]; }
    buf[i]=0; vga_puts(buf);
}
void vga_write_hex(uint32_t v){
    static const char hex[]="0123456789ABCDEF"; vga_puts("0x");
    for(int i=7;i>=0;i--) vga_putc(hex[(v>>(i*4))&0xF]);
}
