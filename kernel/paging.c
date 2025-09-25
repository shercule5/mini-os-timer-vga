#include <stdint.h>
#include "paging.h"

static uint32_t __attribute__((aligned(4096))) page_dir[1024];
static uint32_t __attribute__((aligned(4096))) page_tbl0[1024];

void paging_init(void){
    // Identity-map 0..4MB with 4KB pages (present + writable)
    for(int i=0;i<1024;i++) page_tbl0[i] = (i*0x1000) | 0x003;
    for(int i=0;i<1024;i++) page_dir[i]  = 0x000;  // not present
    page_dir[0] = ((uint32_t)page_tbl0) | 0x003;

    // Load CR3 with page directory
    __asm__ volatile("mov %0, %%cr3" :: "r"(page_dir));

    // Enable paging bit (PG) in CR0
    uint32_t cr0; __asm__ volatile("mov %%cr0, %0":"=r"(cr0));
    cr0 |= 0x80000000; // PG
    __asm__ volatile("mov %0, %%cr0"::"r"(cr0));
}
