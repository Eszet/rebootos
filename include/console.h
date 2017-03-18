#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <types.h>
#include <runtime.h>

#define VGA_LINEAR_LOCATION 0xA0000
#define VGA_TEXT_OFFSET 0x18000

#define VIDEO_RAM (uint16 *) \
  (VGA_LINEAR_LOCATION+VGA_TEXT_OFFSET)

#define CONSOLE_LINES 25
#define CONSOLE_COLUMNS 80

#define SCREEN_START VIDEO_RAM
#define SCREEN_END \
  (VIDEO_RAM+CONSOLE_COLUMNS*CONSOLE_LINES) // *2)

#define CON_INVALID (-1)
#define BLANK_CHAR ' '

#define TAB_SIZE 8
#define TAB_MASK (TAB_SIZE-1)

#define PRINTABLE_MIN 32
#define PRINTABLE_MAX 128

#define COLOR_ATTR(t, b) (((b & 0xF) << 4) | (t & 0xF))
#define IS_SCREEN(wnd) ((wnd == NULL) || (wnd == &con.scr))

#define LIMIT(a, min, max) \
  ((a) < (min) ? (min) : ((a) > (max) ? (max) : (a)))

typedef uint8 color;

enum color {
  black = 0,
  blue,
  green,
  cyan,
  red,
  magenta,
  brown,
  lt_grey,
  dk_grey,
  lt_blue,
  lt_green,
  lt_cyan,
  lt_red,
  lt_magenta,
  lt_brown,
  white
};

#define textcolor black
#define backcolor lt_grey
#define blink 0x8

struct window
{
  strz name;
  
  int left, top;
  int right, bottom;

  int cur_x;
  int cur_y;
    
  color attr;
};

struct console
{
  bool sw_cursor;
  color sw_cursor_attr;
  
  int columns;
  int lines;
  
  int tmp_x;
  int tmp_y;
  
  struct window scr;
  struct window *wnd;
};

extern volatile bool con_hw_use_cursor;
extern struct console con;

bool con_init(void *);

void print(const strz s);

color con_setcolor(enum color textcol, enum color backcol);
struct window *con_selwnd(struct window *wnd);

bool con_setwnd(int left, int top, int right, int bottom);

void con_clear(void);
bool con_setxy(int x, int y);

uint8 con_getx(void);
uint8 con_gety(void);

void con_writexy(int x, int y, strz s);
void con_putxy(int x, int y, strz s);

bool con_write(strz s);
int con_read(strz buf);

void putch(char c); 
bool con_putch(char c, bool adv);

bool con_sw_show_cursor(void);
bool con_sw_hide_cursor(void);
void con_sw_toggle_cursor(bool toggle);

void con_hw_show_cursor(void);
void con_hw_hide_cursor(void);
int con_hw_cursor_getx(void);
int con_hw_cursor_gety(void);
void con_hw_cursor_setxy(int x, int y);
inline void con_hw_update_cursor(void);

void con_debug_print(strz fname, strz s);

#define echo_on kbd_echo_on
#define echo_off kbd_echo_off
#define gets kbd_gets

#define setcolor con_setcolor
#define clrscr con_clear
#define kprint con_write

#define read con_read

#endif
