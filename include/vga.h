// $Id$

#ifndef __VGA_H__
#define __VGA_H__

#include <types.h>

bool vga_setblink(bool enable);
void vga_cursor_on(void);
void vga_cursor_off(void);
int vga_cursor_getx(void);
int vga_cursor_gety(void);
void vga_cursor_setxy(int cursor);

#endif
