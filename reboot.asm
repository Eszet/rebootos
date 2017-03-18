; $Id: reboot.asm 49 2006-09-20 22:03:44Z jlieder $

global reboot1, reboot2, reboot4

section .text

reboot1:
	jmp 0xFFFF0000
	ret

reboot2:
	int 19h
	ret

reboot4:
	ret
