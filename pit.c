// $Id: pit.c 49 2006-09-20 22:03:44Z jlieder $

#include <types.h>
#include <pit.h>
#include <irq.h>

#if defined(SOFTWARE_CURSOR) || defined(DEBUG_OUTPUT)
#include <console.h>
#endif

uint64 timer_ticks = 0;

bool pit_set_freq(uint freq) // timer 0
{
  // max. divisor if (freq == 0)
    
  uint16 divisor = 
    (freq == 0) ? 0xFFFF : 1193180 / freq;
    
  io_write(0x43, 0x36); // 3 << 8 | 3 << 1
  io_write(0x40, divisor & 0xFF);
  io_write(0x40, divisor >> 8);  
    
  return(TRUE);
}

void timer_handler(struct regs *r)
{
  timer_ticks++;

#ifdef SOFTWARE_CURSOR
  if((word) timer_ticks % 10 == 0)
    con_sw_toggle_cursor(TRUE); // 2Hz
#endif
  //  strz screen = (strz) 0xB8001;
  //  *screen ^= ~green << 4;
}

bool pit_init(void *param)
{
  if(pit_set_freq(0)) // 18.2Hz
  {
    irq_set_handler(
      IRQ_TIMER, timer_handler
    );
    
    return(TRUE);
  }
  
  return(FALSE);
}
