// $Id: reboot.h 49 2006-09-20 22:03:44Z jlieder $

#ifndef __REBOOT_H__
#define __REBOOT_H__

typedef void (*rbm)(void);

extern void reboot1(void); // jump
extern void reboot2(void); // int
extern void reboot4(void); // tf#

#endif
