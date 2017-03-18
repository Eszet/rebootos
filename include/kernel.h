#ifndef __KERNEL_H__
#define __KERNEL_H__

// $Id: kernel.h 49 2006-09-20 22:03:44Z jlieder $

#include <types.h>
#include <stdarg.h>

extern void * const _limit;

#define linkage __attribute__((regparm(0)))
#define main _kmain

#define PAGESIZE (0x1000) // page size

struct regs
{
  uint32 gs, fs, es, ds;
  uint32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
  uint32 int_no, err_code;
  uint32 eip, cs, eflags, useresp, ss;
};

// PurpOS Error Codes

#define ERR_UNEXPECTED 1

#endif
