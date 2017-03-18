// $Id$

#include <vga.h>
#include <io.h>

// http://www.acid.org/info/xbin/x_tut.htm

bool vga_setblink(bool enable)
{
  io_read(0x3DA);
  io_write(0x3C0, 0x10 + 0x20);
  uint8 val = io_read(0x3C1);

  val = enable ? 
    val | 0x08 : 
    val & ~0x08;

  io_write(0x3C0, val);

/*
  REIMAR
  if(enable)
    io_write(0x3D8, 0x29);
*/
  
  return(TRUE);
}

void vga_cursor_on(void)
{
  io_write(0x3D4, 0x0A);
  uint8 val = io_read(0x3D5);
  io_write(0x3D5, val & ~(1 << 5));
}

void vga_cursor_off(void)
{
  io_write(0x3D4, 0x0A);
  uint8 val = io_read(0x3D5);
  io_write(0x3D5, val | (1 << 5));
}

int vga_cursor_getx(void)
{
  int cursor;
  io_write(0x3D4, 0x0E);
  cursor = io_read(0x3D5) << 8;
  io_write(0x3D4, 0x0F);
  cursor |= io_read(0x3D5);
  
  return(cursor);
}

int vga_cursor_gety(void)
{
  int cursor;
  io_write(0x3D4, 0x0E);
  cursor = io_read(0x3D5) << 8;
  io_write(0x3D4, 0x0F);
  cursor |= io_read(0x3D5);
  
  return(cursor);
}

void vga_cursor_setxy(int cursor)
{
  io_write(0x3D4, 0x0E);
  io_write(0x3D5, cursor >> 8);
  io_write(0x3D4, 0x0F);
  io_write(0x3D5, cursor & 0xFF);
}
