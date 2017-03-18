; $Id$

bits 32

segment .text

global io_port_inb
global io_port_outb

io_port_inb:
	push ebp
	mov ebp, esp

	mov eax, 0
	mov dx, [ebp+8]
	in al, dx

	mov esp, ebp
	pop ebp

	ret

io_port_outb:
	push ebp
	mov ebp, esp

	mov al, [ebp+12]
	mov dx, [ebp+8]
	out dx, al

	mov esp, ebp
	pop ebp

	ret
