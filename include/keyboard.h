// $Id: keyboard.h 49 2006-09-20 22:03:44Z jlieder $

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <types.h>

#define KEYBOARD_BUFFER 160

typedef uint16 key;

inline bool kbd_echo_on(void);
inline bool kbd_echo_off(void);

strz kbd_gets(void);
key kbd_getkey(void);

bool kbd_init(void *);

#endif
