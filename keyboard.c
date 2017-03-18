// $Id: keyboard.c 49 2006-09-20 22:03:44Z jlieder $

#include <keyboard.h>
#include <runtime.h>
#include <console.h>
#include <debug.h>
#include <string.h>
#include <irq.h>
#include <io.h>

typedef const key keymap[128];

keymap keymap_us =
{
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8',
    /* 9 */
    '9', '0', '-', '=', '\b',	/* Backspace */
    '\t',			/* Tab */
    'q', 'w', 'e', 'r',	
    /* 19 */
    't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			
    /* 29   - Control */
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	
    /* 39 */
    '\'', '`',  0,		/* Left shift */
    '\\', 'z', 'x', 'c', 'v', 'b', 'n',			
    /* 49 */
    'm', ',', '.', '/',   0,				/* Right shift */
    '*',
    0,	/* Alt */
    ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	
    /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
    '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
    '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

#define CURRENT_KEYMAP (*kbd_current_keymap) 

static keymap *kbd_current_keymap = &keymap_us;
static int magic_count = 0;

static bool kbd_echo = TRUE;

static char key_buffer[KEYBOARD_BUFFER + 1];
static volatile int key_buf_count = 0;
static volatile bool key_input_complete = TRUE;
static volatile key key_last = 0;

// ----------------------------------------------------------------------

inline bool kbd_echo_on(void)
{
  return(kbd_echo = TRUE);
}

inline bool kbd_echo_off(void)
{
  return(kbd_echo = FALSE);
}

strz kbd_gets(void)
{
  key_input_complete = FALSE;
  key_buf_count = 0;
  
#ifndef SOFTWARE_CURSOR
  con_hw_update_cursor();
#endif
  
  while(!key_input_complete)
    yield(); // yield() for poor people
    
  // print(itoa(key_buf_count, 10));

  key_buffer[key_buf_count] = 0;
  key_input_complete = TRUE; // FALSE;
  
  return(key_buffer);
}

key kbd_getkey(void)
{
  while(key_last == 0)
    yield();
  
  key res = key_last;  
  key_last = 0;
  
  return(res);
}

// ----------------------------------------------------------------------

void keyboard_handler(struct regs *r)
{
  uint8 scancode = io_read(0x60);
  
  if(scancode & 0x80)
  {
  }
  else
  {
    char buffer[6];
    char c = 0;
    int skip_print = 0;
    
    if(WITHIN(scancode, 0, 127))
      c = CURRENT_KEYMAP[scancode];
      
    key_last = scancode << 8 | c;
  
    if(WITHIN(c, 0, 127))
    {
      if(kbd_echo)
        snprintf(buffer, 6, "%c", c);
      
      if(c == '\n')
        key_input_complete = TRUE;
      else 
      if(c == '\b')
      {
        if(key_buf_count > 0)
          key_buf_count--;
        else
          skip_print = 1;
      }
      else
      {
        if((key_buf_count < KEYBOARD_BUFFER) && !key_input_complete)
        {
          if(c == '\b') // go back if BACKSP was pressed
            key_buf_count--;
          else
          if(c != 0) // intercept unassigned keys
            key_buffer[key_buf_count++] = c;
        }
        else
          key_input_complete = TRUE;
      }  
    }
    else
      if(kbd_echo)
        snprintf(buffer, 6, "<%d>", c);

    if(kbd_echo && !skip_print)
      print(buffer);

#ifndef SOFTWARE_CURSOR
    con_hw_update_cursor();
#endif

    /*
    Process magic PurpOS key sequence 
    <ScrollLock>, <ScrollLock>, <Del>
    */
    
#define KEY_SCROLL_LOCK 70
#define KEY_DEL 83
    
    if(scancode == KEY_SCROLL_LOCK)
    {
      if(magic_count == 0 || magic_count == 1)
        magic_count++;
    }
    else      
    if(magic_count == 2 && scancode == KEY_DEL)
      magic_count++;
    else
      magic_count = 0;
    
    // if(scancode == 0x3F+7) // F12
    if(magic_count >= 3)
      reboot();
  }
}

bool kbd_init(void *param)
{
  bool success =
    irq_set_handler(1, keyboard_handler);

  if(success)
  {
    // wait for keyboard to become ready
  
    while(io_read(0x64) & 2);
  
    // set caps lock and scroll lock
  
    io_write(0x60, 0xED);
    io_write(0x60, 0x05);
  
    if(io_read(0x60) != 0xFA)
      dprint("Setting LEDs failed.\n");
  }
    
  return(success);
}
