// $Id: irq.h 49 2006-09-20 22:03:44Z jlieder $

#ifndef __IRQ_H__
#define __IRQ_H__

#include <kernel.h>

#define PORT_PIC1 0x20
#define PORT_PIC2 0xA0

#define PIC_ICW1 0x11
#define PIC_ICW4 0x01
#define PIC_RESET 0x20

#define NUMIRQ 0x10
#define MAXIRQ (NUMIRQ-1)

#define IRQISR 0x40 // IRQ0 == ISR64

typedef void (*irq_handler_func)(struct regs *r);

bool irq_set_handler(uint irq, irq_handler_func handler);
bool irq_reset_handler(uint irq);

bool irq_init(void *);
void irq_handler(struct regs *r);

#endif
