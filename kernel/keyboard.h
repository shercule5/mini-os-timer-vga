#pragma once
#include <stdint.h>
void keyboard_init(void);
int  kbd_getch(void);   // blocking getchar (ASCII; returns 0 for non-printables)

