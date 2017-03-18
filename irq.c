// $Id: irq.c 49 2006-09-20 22:03:44Z jlieder $

#include <runtime.h>
#include <idt.h>
#include <irq.h>
#include <io.h>
#include <runtime.h>

#ifdef DEBUG_OUTPUT
#include <console.h>
#include <string.h>
#endif

#include "irq.inc"

static void *irq_handlers[NUMIRQ];

bool irq_set_handler(uint irq, irq_handler_func handler)
{
  if(WITHIN(irq, 0, MAXIRQ))
  {
    irq_handlers[irq] = handler;
    return(TRUE);
  }    
    
  return(FALSE);
}

bool irq_reset_handler(uint irq)
{
  if(WITHIN(irq, 0, MAXIRQ))
  {
    irq_handlers[irq] = NULL;
    return(TRUE);
  }    
    
  return(FALSE);
}

/*
#define PIC_1		0x020
#define PIC_2		0x0A0

#define ICU_OFFSET	0x20
#define ICU_LEN		0x10
#define IRQ_SLAVE	0x0004

#define PIC_1_OCW 0x21
#define PIC_2_OCW 0xA1

#define ENABLE_INTERRUPT( interrupt ) ( interrupt < 8 ) ? \
  outb( PIC_1_OCW, inb( PIC_1_OCW ) & ~( (char)1 << interrupt ) ) : \
  outb( PIC_2_OCW, inb( PIC_2_OCW ) & ~( (char)1 << interrupt ) )
                  
#define DISABLE_INTERRUPT( interrupt ) ( interrupt < 8 ) ? \
  outb( PIC_1_OCW, inb( PIC_1_OCW ) | ( (char)1 << interrupt ) ) : \
  outb( PIC_2_OCW, inb( PIC_2_OCW ) | ( (char)1 << interrupt ) )
*/

void irq_enable_all(void)
{
  // Enable all IRQs  
  io_write(PORT_PIC1+1, 0);
  io_write(PORT_PIC2+1, 0);
}

void irq_disable_all(void)
{
  // Disable all IRQs
  io_write(PORT_PIC1+1, 0xFF);
}

void irq_remap(void)
{
  // Calc IRQ base for master/slave
  
  uint pic1_base = IRQISR;
  uint pic2_base = IRQISR+(NUMIRQ/2);

  // ICW1
  io_write(PORT_PIC1, PIC_ICW1);
  io_write(PORT_PIC2, PIC_ICW1);
  
  // ICW2
  io_write(PORT_PIC1+1, pic1_base);
  io_write(PORT_PIC2+1, pic2_base);
  
  // ICW3
  io_write(PORT_PIC1+1, 4);	// IRQ2 is connection to slave
  io_write(PORT_PIC2+1, 2);	// edge triggered?
  
  // ICW4
  io_write(PORT_PIC1+1, PIC_ICW4);
  io_write(PORT_PIC2+1, PIC_ICW4);

  irq_enable_all();
}

bool irq_init(void *param)
{
  irq_remap();
  
  #define IRQ_SEL 0x08
  #define IRQ_FLAGS 0x8E
  
  #define idt_set_isr(irq, handler) \
    idt_set_entry(IRQISR + irq, (iptr) handler, IRQ_SEL, IRQ_FLAGS)
    
  idt_set_isr(0x0, irq0);
  idt_set_isr(0x1, irq1);
  idt_set_isr(0x2, irq2);
  idt_set_isr(0x3, irq3);
  idt_set_isr(0x4, irq4);
  idt_set_isr(0x5, irq5);
  idt_set_isr(0x6, irq6);
  idt_set_isr(0x7, irq7);

  idt_set_isr(0x8, irq8);
  idt_set_isr(0x9, irq9);
  idt_set_isr(0xA, irq10);
  idt_set_isr(0xB, irq11);
  idt_set_isr(0xC, irq12);
  idt_set_isr(0xD, irq13);
  idt_set_isr(0xE, irq14);
  idt_set_isr(0xF, irq15);
  
  return(TRUE);
}

static char irq_counter[16] = { 0 };

void irq_handler(struct regs *r)
{
  irq_handler_func handler = NULL;
  int int_no = r ? r->int_no : -1;
  
#ifdef DEBUG_OUTPUT
  // if(int_no == -1) dprint("%");
  // dprint("irq_handler()\n");
#endif

  // if(WITHIN(int_no, 0, MAXINTR))
  if(WITHIN(int_no, IRQISR, IRQISR+MAXIRQ))
  {
    int irq_no = int_no - IRQISR;
    handler = irq_handlers[irq_no];
    
    irq_counter[irq_no]++;

#ifdef DEBUG_OUTPUT
    // display last IRQ in upper right
    // corner of the screen

    char strbuf[20];
    int len = snprintf(
      strbuf, 20,
      "[%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c]",
      irq_counter[0] % 10 + '0',
      irq_counter[1] % 10 + '0',
      irq_counter[2] % 10 + '0',
      irq_counter[3] % 10 + '0',
      irq_counter[4] % 10 + '0',
      irq_counter[5] % 10 + '0',
      irq_counter[6] % 10 + '0',
      irq_counter[7] % 10 + '0',
      irq_counter[8] % 10 + '0',
      irq_counter[9] % 10 + '0',
      irq_counter[10] % 10 + '0',
      irq_counter[11] % 10 + '0',
      irq_counter[12] % 10 + '0',
      irq_counter[13] % 10 + '0',
      irq_counter[14] % 10 + '0',
      irq_counter[15] % 10 + '0'
    );

    con_putxy(
      con.columns - len, 0,
      strbuf
    );
#endif
  }
  
  if(handler)
    handler(r);
    
  // Check whether INT# was greater or equal 40 (IRQ8-15)
  // -> send an EOI to the slave controller.
   
  // if(WITHIN(int_no, IRQISR+(MAXIRQ/2), IRQISR+MAXIRQ))
  if((int_no >= 0x28) && (int_no <= 0x2F))
    io_write(PORT_PIC2, PIC_RESET);
    
  io_write(PORT_PIC1, PIC_RESET);
}  
