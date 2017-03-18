#ifndef __RUNTIME_H__
#define __RUNTIME_H__

// $Id: runtime.h 49 2006-09-20 22:03:44Z jlieder $

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WITHIN(val, min, max) \
  ((val >= min) && (val <= max))
  
#define ABS(a) \
  (a > 0 ? a : -a)

// Memory Operations

extern void memset(void *buf, char c, size count);
extern void memcpy(void *dest, const void *src, size count);

// Serial I/O

extern void ser_init(void *);
extern void ser_write(const strz);

#define sprint ser_write

// System Functions

extern size getsysmem(void);

extern void yield(void);
extern void halt(void);
extern void reboot(void);

#ifdef __cplusplus
}
#endif

#endif
