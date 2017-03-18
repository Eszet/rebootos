// $Id: console.c 49 2006-09-20 22:03:44Z jlieder $

#include <console.h>
#include <vga.h>
#include <keyboard.h>
#include <string.h>
#include <io.h>

static const strz newline = "\n";
volatile bool con_hw_use_cursor = FALSE;
struct console con;

void print(const strz s)
{
  con_write(s);
#ifdef SERIAL_OUTPUT
  ser_write(s);
#endif
}

struct window *con_getwnd(void)
{
  // if wnd == NULL select default screen
  return(con.wnd ? con.wnd : &con.scr);
}

color con_gettextcol(void)
{
  return(con_getwnd()->attr & 0xF);
}

color con_getbackcol(void)
{
  return(con_getwnd()->attr & 0xF0 >> 4);
}

color con_setcolor(enum color textcol, enum color backcol)
{
  struct window *wnd = con_getwnd();
  color last_col = wnd->attr;
  wnd->attr = COLOR_ATTR(textcol, backcol);
  return(last_col);
}

struct window *con_selwnd(struct window *wnd)
{
  struct window *last_wnd = con.wnd;

/*
  if(!IS_SCREEN(wnd) && IS_SCREEN(last_wnd))
  {
    con.tmp_x = con_getx(); 
    con.tmp_y = con_gety();
  }
*/

  con.wnd = wnd; // NULL is allowed

/*
  if(IS_SCREEN(wnd) && !IS_SCREEN(last_wnd))
  {
    con_setxy(
      con.tmp_x, con.tmp_y
    );
    
    // con_getwnd()->cur_x = con.tmp_x;
    // con_getwnd()->cur_y = con.tmp_y;
  }
*/
/*
  else
  {
//  ifk(IS_SCREEN(last_wnd) && !IS_SCREEN(wnd))
  new_wnd->cur_x = new_wnd->left;
  new_wnd->cur_y = new_wnd->top;
  }
*/
  
  return(last_wnd);
}

bool con_setwnd(int left, int top, int right, int bottom)
{
  // con_setwnd() should not modify con.scr
  
  if(/* con.wnd && */ left < right && top < bottom)
  {
    struct window *wnd = con_getwnd();

    wnd->left = LIMIT(left, 0, CONSOLE_COLUMNS);
    wnd->top = LIMIT(top, 0, CONSOLE_LINES);
    wnd->right = LIMIT(right, 0, CONSOLE_COLUMNS);
    wnd->bottom = LIMIT(bottom, 0, CONSOLE_LINES);

    wnd->cur_x = wnd->left;
    wnd->cur_y = wnd->top;
    
    return(TRUE);
  }
  
  return(FALSE);
}

void con_clear(void)
{
  struct window *wnd = con_getwnd();
  
  // clear screen or window, unless NULL
  
  color attr = con.wnd->attr;
#ifdef SOFTWARE_CURSOR
  con.sw_cursor_attr = attr;
#endif
  
  uint16 *scr = (void *) SCREEN_START;
  uint16 val = attr << 8 | BLANK_CHAR;
  
  int org_x = wnd->left;
  int org_y = wnd->top;  
  
  for(int y = 0; y < (wnd->bottom - wnd->top); y++)
    for(int x = 0; x < (wnd->right - wnd->left); x++)
    {
      uint16 *ptr = scr + (
        (org_y + y) * CONSOLE_COLUMNS
      ) + org_x + x;

      if(ptr <= SCREEN_END) // security precaution
        *ptr = val;
    }
    
  con_setxy(0, 0); // added
}

bool con_setxy(int x, int y)
{
  struct window *wnd = con_getwnd();
  
  wnd->cur_x = LIMIT(wnd->left + x, wnd->left, wnd->right - 1); ///
  wnd->cur_y = LIMIT(wnd->top + y, wnd->top, wnd->bottom - 1); ///

  return(TRUE);
}

uint8 con_getx(void)
{
  return(con_getwnd()->cur_x - con_getwnd()->left);
}

uint8 con_gety(void)
{
  return(con_getwnd()->cur_y - con_getwnd()->top);
}

void con_writexy(int x, int y, strz s)
{
  if(con_setxy(x, y))
    con_write(s);
}

void con_putxy(int x, int y, strz s)
// Does not obey current window (can print anywhere on
// the screen).
{
  struct window *wnd = con_selwnd(NULL);
  
  // con.tmp_x = wnd->cur_x; wnd->cur_x = x;
  // con.tmp_y = wnd->cur_y; wnd->cur_y = y;
  
  con_sw_hide_cursor();

  con.tmp_x = con_getx();
  con.tmp_y = con_gety();

  con_writexy(x, y, s);
  
  // wnd->cur_x = con.tmp_x;
  // con.tmp_y = 8 /*CON_INVALID*/; wnd->cur_y = con.tmp_y;

  con_setxy(con.tmp_x, con.tmp_y);

  con.tmp_x = CON_INVALID; 
  con.tmp_y = CON_INVALID; 

  con_selwnd(wnd); // restore former window
  
  con_sw_show_cursor();
}

static void con_scroll(void);
static bool con_advance(char c, bool adv);

bool con_write(strz s)
{
  // Does scroll if screen boundary is
  // reached. 
  
  // Returns boolean value indicating
  // whether the screen content has been
  // scrolled.

  bool scrolled = FALSE;
  
  while(*s)
    if(con_putch(*s++, TRUE))
      scrolled = TRUE;
      
  return(scrolled);
}

void con_puts(strz s)
{
  // Does not scroll if screen boundary
  // is reached!
  
  while(*s)
    con_putch(*s++, FALSE);
}

void putch(char c)
{
  // putch() will typically be used by
  // the keyboard interrupt handler
  
  con_putch(c, TRUE);  
}

bool con_putch(char c, bool adv)
{
  struct window *wnd = con_getwnd();
  
  if(WITHIN(c, PRINTABLE_MIN, PRINTABLE_MAX))
  {
    // printable characters
      
    color attr = wnd->attr;
    int x = wnd->cur_x;
    int y = wnd->cur_y;
  
    uint16 *scr = (void *) VIDEO_RAM;
    uint16 *ptr = scr + (y * CONSOLE_COLUMNS) + x;

    uint16 val = attr << 8 | c;
      
    if(WITHIN(ptr, SCREEN_START, SCREEN_END))
      *ptr = val;
  }
  else
  {
    // non printable characters
      
    switch(c)
    {
    case '\a': // BELL
      con_write("(bell)");
      break;
        
    default:
      break;
    }
  }
 
  return(con_advance(c, adv));
}

bool con_newline(bool adv)
{
  struct window *wnd = con_getwnd();

  int new_y = 0;
  bool scrolled = FALSE;

#ifdef SOFTWARE_CURSOR
  con_sw_toggle_cursor(FALSE);
#endif

  wnd->cur_x = wnd->left;
  new_y = wnd->cur_y + 1;
    
  if(adv)
  {
    if(new_y >= wnd->bottom)
    {
      scrolled = TRUE;
      con_scroll();
    }
    
    wnd->cur_y = LIMIT(
      new_y, wnd->top, wnd->bottom - 1
    );
  }
  
  return(scrolled);
}

bool con_advance(char c, bool adv)
{
  bool scrolled = FALSE;
  int new_x = 0;

  struct window *wnd = con_getwnd();

  switch(c)
  {
  default:
    if(wnd->cur_x < wnd->right - 1)
    {
      wnd->cur_x++;
      break;
    }

    // no break, it's intended!
      
  case '\n': // NEWLINE
    if(adv)
      scrolled = con_newline(adv);
      
    break;
    
  case '\b': // BACKSPACE
    new_x = wnd->cur_x - 1;

    new_x = LIMIT(
      new_x, wnd->left, wnd->right - 1
    );
    
    wnd->cur_x = new_x;
    con_putch(' ', FALSE);
    wnd->cur_x = new_x;
  
    break;
      
  case '\t': // TAB
    new_x = wnd->cur_x + TAB_SIZE;
    new_x &= ~TAB_MASK;
    // new_x %= TAB_MASK;

    if(adv && (wnd->cur_x >= wnd->right))
    {
      scrolled = con_newline(adv);
      new_x -= (wnd->right - wnd->left);
    }
    
    wnd->cur_x = new_x;
    
    break;
    
  case 0:
    // do nothing for c == 0
    break;
  }
  
  return(scrolled);
}

void con_scroll_up(int y, bool clear)
{
  struct window *wnd = con_getwnd();
  
  if(wnd && y > wnd->top && y < wnd->bottom)
  {
    color attr = con.wnd->attr;
    uint16 *scr = (void *) VIDEO_RAM;
  
    uint16 val = attr << 8 | ' ';
    
    for(int x = wnd->left; x < wnd->right; x++) 
    {
      uint16 *ptr = scr + (y * CONSOLE_COLUMNS) + x;
      *(ptr - CONSOLE_COLUMNS) = *ptr;
      
      // check limits?

      if(clear) *ptr = val;
    }
  }
}

void con_scroll(void)
{
  struct window *wnd = con_getwnd();
  
  int min_y = wnd->top + 1;
  int max_y = wnd->bottom - 1;
  
  for(int y = min_y; y < max_y; y++)
    con_scroll_up(y, FALSE);
    
  con_scroll_up(max_y, TRUE);
}

int con_read(strz buf)
{
  kbd_echo_on();

#ifndef SOFTWARE_CURSOR  
  con_hw_show_cursor();
  con_hw_cursor_setxy(
    con_getx(), con_gety()
  );
#endif

  strz s = gets();
  strcpy(buf, s);

#ifndef SOFTWARE_CURSOR  
  con_hw_hide_cursor();
#endif

  return(strlen(s));
}

// Console Software Cursor

bool con_sw_show_cursor(void)
{
  con_sw_toggle_cursor(FALSE);
  return(con.sw_cursor = TRUE);
}

bool con_sw_hide_cursor(void)
{
  con_sw_toggle_cursor(FALSE);
  return(con.sw_cursor = FALSE);
}

void con_sw_toggle_cursor(bool toggle)
{
  struct window *wnd = con_getwnd();
  
  if(wnd && con.sw_cursor)
  {
    // color attr = con.wnd->attr;
    uint16 *scr = (void *) VIDEO_RAM;
    
    int x = wnd->cur_x;
    int y = wnd->cur_y;
  
    color *ptr = (color *) (scr + (y * CONSOLE_COLUMNS) + x) 
      + 1; // blink color, + 0 blink character (experimental)

    if(toggle)
    {
      con.sw_cursor_attr = *ptr;
      *ptr ^= ~COLOR_ATTR(0, green);
    }
    else  // restore original attr
      *ptr = con.sw_cursor_attr; 
      // applies to con_newline()
  }
}

// Console Hardware Cursor

void con_hw_show_cursor(void)
{
  con_hw_use_cursor = TRUE;
  vga_cursor_on();
}

void con_hw_hide_cursor(void)
{
  con_hw_use_cursor = FALSE;
  vga_cursor_off();
}

int con_hw_cursor_getx(void)
{
  int cursor = vga_cursor_getx();
  return(cursor % con.columns);
}

int con_hw_cursor_gety(void)
{
  int cursor = vga_cursor_gety();
  return(cursor / con.columns);
}

void con_hw_cursor_setxy(int x, int y)
{
  int cursor = y * con.columns + x;
  vga_cursor_setxy(cursor);    
}

inline void con_hw_update_cursor(void)
{
#ifndef SOFTWARE_CURSOR
  if(!con_hw_use_cursor)
    return;

  struct window *wnd = con_getwnd();
      
  con_hw_cursor_setxy(
    wnd->cur_x, wnd->cur_y
  );
#endif
}

void con_debug_print(strz fname, strz s)
{
  print(fname);
  print(": ");
  print(s);
}

bool con_init(void *param)
{
  con.columns = CONSOLE_COLUMNS;
  con.lines = CONSOLE_LINES;
  
  con.tmp_x = CON_INVALID;
  con.tmp_y = CON_INVALID;
  
  con.wnd = &con.scr; // use default screen
  con.sw_cursor_attr = 0;

#ifdef SOFTWARE_CURSOR  
  con_hw_hide_cursor();
  con_sw_show_cursor();
#else
  con_sw_hide_cursor();
//con_hw_show_cursor();
  con_hw_cursor_setxy(0, 0);
#endif

  con_hw_hide_cursor();

  con_setwnd(0, 0, con.columns, con.lines);
  con_setcolor(textcolor, backcolor);
  con_clear();
  
  con.wnd = NULL; // switch back to NULL so
  // con.scr cannot be modified anymore...
  
  vga_setblink(TRUE);
  
  return(TRUE);
}
