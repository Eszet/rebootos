// $Id: gdt.c 49 2006-09-20 22:03:44Z jlieder $

#include <types.h>
#include <gdt.h>

// Global Descriptor Table and GDT Pointer
  
struct gdt_entry gdt[3];
struct gdt_ptr gdtp;

// Global Selectors for Kernel-Land
//
// xsel should point to the appropriate gdt entries 
// prior to the call to gdt_load().

struct gdt_entry *csel = NULL;	// selector for code segment  (CS)
struct gdt_entry *dsel = NULL;	// selector for data segment  (DS)
struct gdt_entry *esel = NULL;	// selector for extra segment (ES)
struct gdt_entry *ssel = NULL;	// selector for stack segment (SS)

extern bool gdt_load();

struct gdt_entry *gdt_set_entry(uint index, iptr base, iptr limit, uint8 access, uint8 gran)
{
  gdt[index].base_low = (base & 0xFFFF);
  gdt[index].base_mid = (base >> 16) & 0xFF;
  gdt[index].base_high = (base >> 24) & 0xFF;
  
  gdt[index].limit_low = (limit & 0xFFFF);
  gdt[index].granularity = ((limit >> 16) & 0x0F);
  
  gdt[index].granularity |= gran << 4;
  gdt[index].access = access;
  
  return(&gdt[index]);
}

bool gdt_init(void *param)
{
  gdtp.limit = sizeof(gdt) - 1;
  gdtp.base = &gdt;
  
  // null segment descriptor, this is mandatory
  gdt_set_entry(0, 0, 0, 0, 0);

  csel = gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xC);
  dsel = esel = ssel 
       = gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xC);
  
  return(gdt_load());
}
