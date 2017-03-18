// $Id: isr.c 49 2006-09-20 22:03:44Z jlieder $

#include <runtime.h>
#include <kernel.h>
#include <idt.h>
#include <isr.h>

#ifdef DEBUG_OUTPUT
#include <console.h>
#include <string.h>
#include <debug.h>
#endif

static strz exception_descr[] =
{
  "Division By Zero",
  "Debug Exception",
  "NMI",
  "Breakpoint",
  "Into Detected Overflow",
  "Out Of Bounds",
  "Invalid Opcode",
  "No Coprocessor",
  "Double Fault",
  "Coprocessor Segment Overrun",
  "Bad TSS",
  "Segment No Present",
  "Stack Fault",
  "General Protection Fault",
  "Page Fault",
  "Unknown Interrupt",
  "Coprocessor Fault",
  "Alignment Check",
  "Machine Check",
  "Reserved Exception",
  "Reserved Exception",
  "Reserved Exception",
  "Reserved Exception",
  "Reserved Exception",
  "Reserved Exception",
  "Reserved Exception",
  "Reserved Exception",
  "Reserved Exception",
  "Reserved Exception",
  "Reserved Exception",
  "Reserved Exception",
  "Reserved Exception",
  "Reserved Exception"
};

#include "isr.inc" // external ISRs, isr0..isr31

bool isr_init(void *param)
{
#define ISR_SEL   0x08
#define ISR_FLAGS 0x8E

#define idt_set_isr(intr, handler) \
  idt_set_entry(intr, (iptr) handler, ISR_SEL, ISR_FLAGS)
  
  idt_set_isr(0x00, isr0);
  idt_set_isr(0x01, isr1);
  idt_set_isr(0x02, isr2);
  idt_set_isr(0x03, isr3);
  idt_set_isr(0x04, isr4);
  idt_set_isr(0x05, isr5);
  idt_set_isr(0x06, isr6);
  idt_set_isr(0x07, isr7);

  idt_set_isr(0x08, isr8);
  idt_set_isr(0x09, isr9);
  idt_set_isr(0x0A, isr10);
  idt_set_isr(0x0B, isr11);
  idt_set_isr(0x0C, isr12);
  idt_set_isr(0x0D, isr13);
  idt_set_isr(0x0E, isr14);
  idt_set_isr(0x0F, isr15);

  idt_set_isr(0x10, isr16);
  idt_set_isr(0x11, isr17);
  idt_set_isr(0x12, isr18);
  idt_set_isr(0x13, isr19);
  idt_set_isr(0x14, isr20);
  idt_set_isr(0x15, isr21);
  idt_set_isr(0x16, isr22);
  idt_set_isr(0x17, isr23);

  idt_set_isr(0x18, isr24);
  idt_set_isr(0x19, isr25);
  idt_set_isr(0x1A, isr26);
  idt_set_isr(0x1B, isr27);
  idt_set_isr(0x1C, isr28);
  idt_set_isr(0x1D, isr29);
  idt_set_isr(0x1E, isr30);
  idt_set_isr(0x1F, isr31);

#undef idt_set_isr

  return(TRUE);
}

void fault_handler(struct regs *r)
{
#ifdef DEBUG_OUTPUT
  dprint("\nfault_handler():\n");
#endif
  
  int int_no = r->int_no;

  if(WITHIN(int_no, 0, 31))
  {
#ifdef DEBUG_OUTPUT
    char strbuf[300];
    
    snprintf(
      strbuf, 300,
      "Caught Interrupt %uh\n"
      "Error Code: %u\n"
      "Exception#: %s\n"
      "EIP=%x\n"
/*      
      "EAX=%x  EBX=%x  ECX=%x  EDX=%x\n"
      "ESP=%x  EBP=%x  ESI=%x  EDI=%x\n"
      "CS=%x   DS=%x\n"
      "ES=%x   SS=%s\n"
      "FS=%x   GS=%x\n"
*/
      "System Halted!\n",
      int_no, r->err_code,
      exception_descr[int_no],
      r->eip
/*
      r->eip,
      r->eax, r->ebx, r->ecx, r->edx,
      r->esp, r->ebp, r->esi, r->edi,
      r->cs, r->ds, r->es, r->ss, r->fs, r->gs
*/
    );
    
    print(strbuf);
#endif
   
    // halt();  
  }
}
