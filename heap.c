// $Id: heap.c 49 2006-09-20 22:03:44Z jlieder $

#include <heap.h>
#include <debug.h>

#define HEAP_START 0x100000 // 1M

static unsigned char *heap_ptr = 
  (unsigned char *) HEAP_START;

void *kmalloc(size block)
{
  void *ptr = heap_ptr;
//dprint("kmalloc(): size %d, ptr %x\n", block, ptr);
  heap_ptr += block;
  return(ptr);
}
    
void kfree(void *ptr)
{
}
