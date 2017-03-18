// $Id: gdt.h 49 2006-09-20 22:03:44Z jlieder $

#ifndef __GDT_H__
#define __GDT_H__

#include <types.h>

struct gdt_entry
{
  uint16 limit_low;
  uint16 base_low;
  uint8 base_mid;
  uint8 access;
  uint8 granularity;
  uint8 base_high;
}
__attribute__((packed));

struct gdt_ptr
{
  uint16 limit;
  void *base;
}
__attribute__((packed));

struct gdt_entry *gdt_set_entry(uint index, iptr base, iptr limit, uint8 access, uint8 gran);

bool gdt_init(void *);

#endif
