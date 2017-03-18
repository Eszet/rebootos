// $Id: io.h 49 2006-09-20 22:03:44Z jlieder $

#ifndef __IO_H__
#define __IO_H__

#include <types.h>

// Port-Mapped I/O is extremely Intel-specific, thus all
// declarations use Intel Architecture (ia_*) data types.
// Use io_read/io_write for readability's sake.

#define io_read io_port_inb
#define io_write io_port_outb

extern void io_port_outb(ia_word port, ia_byte data);
extern ia_byte io_port_inb(ia_word port);

#endif
