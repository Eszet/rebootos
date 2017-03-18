#include <idt.h>
#include <runtime.h>

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void idt_load(); // entry.asm

struct idt_entry *idt_set_entry(uint8 index, iptr base, uint16 sel, uint8 flags)
{
  idt[index].base_low = (base & 0xFFFF);
  idt[index].base_high = (base >> 16)  & 0xFFFF;
  
  idt[index].sel = sel;
  idt[index].always_zero = 0;
  idt[index].flags = flags;

  return(&idt[index]);
}

bool idt_init(void *param)
{
  idtp.limit = sizeof(idt) - 1;
  idtp.base = &idt;
  
  memset(&idt, 0, sizeof(idt));
  
  idt_load();
  return(TRUE);
}
