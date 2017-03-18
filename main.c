// $Id: main.c 59 2006-09-27 21:28:09Z jlieder $

#include <kernel.h>
#include <runtime.h>
#include <string.h>
#include <debug.h>
#include <console.h>
#include <module.h>
#include <gdt.h>
#include <idt.h>
#include <isr.h>
#include <irq.h>
#include <pit.h>
#include <keyboard.h>
#include <reboot.h>


static const strz uname = 
  "ReBootOS v2\n";

static const strz kerneldate = 
  "Built " __DATE__ " " __TIME__ "\n";

static const strz kernelrev =
  "$Revision: 59 $\n\n";
  
static const strz purptech =
  "Built on PurpO/S technology(tm)";

void reboot_wd(void);
extern void reboot5(void);

struct menuitem
{
  strz descr;
  rbm func;  
};

const struct menuitem menuitems[] = 
{
  { "plain jmp 0xFFFF0000", reboot1 },
  { "execute int 19h", reboot2 },
  { "via keyboard controller", reboot },
  { "cause triple fault", reboot5 },
  { "reset watchdog (eRBM)", reboot_wd }
};

void reboot_wd(void)
{
  ser_write("reset port0");
}

#define NUM_ITEMS \
  (sizeof(menuitems) / sizeof(menuitems[0]))
  
void update_item(int item, bool sel)
{
  struct window *wnd = con_getwnd();
  
  strz str = menuitems[item].descr;
  int width = (wnd->right - wnd->left);
  int len = strlen(str);
  
  int xpos = (width - len) / 2;
  int ypos = item;
  
  static char space[80];
  memset(space, ' ', width);
  strcpy(&space[xpos], str);
  
  if(sel)
    con_setcolor(textcolor, backcolor);
  else
    con_setcolor(backcolor, textcolor);
  
  con_writexy(xpos, ypos, str);
}

void update_menu(int sel_item)
{
  for(int i = 0; i < NUM_ITEMS; i++)
    update_item(i, i == sel_item);
}

int main(int argc, char *argv[])
{
  /*
  MODULE(NULL, con, NULL);
  */
#ifdef SERIAL_OUTPUT
  MODULE("Serial Port", ser, NULL);
#endif
  MODULE("GDT", gdt, NULL);
  MODULE("IDT", idt, "GDT");
  MODULE("ISR", isr, "IDT");
  MODULE("IRQ", irq, "ISR");
  MODULE("PIT", pit, "IRQ");
  MODULE("Keyboard", kbd, "IRQ");
  
  con_init(NULL);

  print(uname);
  setcolor(red, backcolor);
  print(kerneldate);
  setcolor(textcolor, backcolor);
  print(kernelrev);
  
  // initialize module system
  
  int sel = 0;
  if(mod_init())
  {
    kbd_echo_off();
    
    struct window wnd;
    con_selwnd(&wnd);
    con_setwnd(10, 5, 70, 19);
    con_setcolor(white, dk_grey);
    con_clear();
    
    print("ReBootOS");
    
    con_setxy(
      60 - strlen(purptech), 0
    );

    con_setcolor(textcolor, dk_grey);
    print(purptech);
    
    con_setwnd(10, 6, 70, 19);
    con_setcolor(backcolor, textcolor);
    con_clear();

    while(TRUE)
    {
      update_menu(sel);

      key k = kbd_getkey();
      uint8 scancode = k >> 8;
      
      switch(scancode)
      {
      case 72: // UP
        sel--;
        break;
        
      case 80: // DOWN
        sel++;
        break;
        
      case 28: // RETURN
      {
        strz descr = menuitems[sel].descr;
        rbm func = menuitems[sel].func;

        con_setcolor(backcolor, textcolor);
        con_clear();
        
        con_setxy(0, 10);
        
        print("Attempting RBM: ");
        print(descr);
        print("...");
        
        func();
        con_setcolor(red, textcolor | blink);
        print("Failed!\n");
        break;
      }
      }     
      
      sel %= NUM_ITEMS;
    }
  }
  else
    dprint("mod_init() failed.\n");
  
  return(-1);
}
