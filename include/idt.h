// $Id: idt.h 49 2006-09-20 22:03:44Z jlieder $

#ifndef __IDT_H__
#define __IDT_H__

#include <types.h>

#define MAXINTR 255

struct idt_entry
{
  uint16 base_low;
  uint16 sel;
  uint8 always_zero;
  uint8 flags;
  uint16 base_high;
}
__attribute__((packed));

struct idt_ptr
{
  uint16 limit;
  void *base; // uint32
}
__attribute__((packed));

struct idt_entry *idt_set_entry(uint8 index, iptr base, uint16 sel, uint8 flags);

bool idt_init(void *);

#endif
