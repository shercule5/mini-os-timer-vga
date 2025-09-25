#pragma once
#include <stdint.h>

typedef struct regs {
    uint32_t edi, esi, ebp, esp_dummy, ebx, edx, ecx, eax;
    uint32_t int_no;
    uint32_t err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} regs_t;
