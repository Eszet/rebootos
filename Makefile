#
# $Id: Makefile 49 2006-09-20 22:03:44Z jlieder $
#
# (c) 2006 by jlieder
#

-include Makefile.conf

SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
DEPS=$(patsubst %.o,%.d,$(OBJS))
ASMOBJS=runtime.o io.o reboot.o
CFLAGS= -Wall -nostdinc -std=c99 -ffreestanding -I./include $(addprefix -D,$(DEFINES)) # -02 -fomit-frame-pointer
LDFLAGS= --strip-debug --gc-sections # -t # --print-map
CC= gcc
NASM= nasm

all:	kernel floppy rebootos.flp

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.s: %.c
	$(CC) -fverbose-asm $(CFLAGS) -S -o $@ $<

%.o: %.asm
	$(NASM) -f elf -o $@ $<

%.bin: %.asm
	$(NASM) -f bin -o $@ $<

kernel: kernel.elf kernel.bin

kernel.elf: entry.o $(ASMOBJS) $(OBJS)
	ld --oformat elf32-i386 -T kernel.ld -o kernel.elf $^

kernel.bin: entry.o $(ASMOBJS) $(OBJS)
	ld $(LDFLAGS) -T kernel.ld -s -o kernel.bin $^
	
kernel.map: entry.o $(ASMOBJS) $(OBJS)
	ld --print-map -T kernel.ld -o kernel.tmp $^ >kernel.map
	less kernel.map
	
map: kernel.map

deps: $(SRCS)
	$(CC) $(CFLAGS) -MD -E $(SRCS) > /dev/null
	
clean:
	rm -f *.bin *.elf *.o rebootos.flp
	
realclean: clean
	rm *~ *.d

rebootos.flp: boot.bin kernel.bin
	dd if=/dev/zero of=rebootos.flp bs=512 count=2880
	dd if=boot.bin of=rebootos.flp bs=512 count=1 conv=notrunc
	dd if=fs_info of=rebootos.flp bs=512 seek=1 count=1 conv=notrunc
	dd if=kernel.bin of=rebootos.flp bs=512 seek=2 conv=notrunc

floppy: boot.bin kernel.bin
	dd if=boot.bin of=/dev/fd0 bs=512 count=1
	dd if=/dev/zero of=/dev/fd0 bs=512 seek=1 count=1
	dd if=kernel.bin of=/dev/fd0 bs=512 seek=2

run: rebootos.flp
	qemu -fda rebootos.flp -m 64 -serial stdio -nographic
	
debug: kernel.elf
	gdb --symbols=kernel.elf

-include $(DEPS)
