bits 16
org 7C00h

	jmp start

; Data Segment

msg1	db "Loading Executive...", 13, 10, 0
msg2	db "Rebooting...", 13, 10, 0
msg3	db "ReBootOS v2 PME-", 10, 0
msg4	db "CPU Vendor: ", 0
dot	db ".", 0

bootdrv db 0
vendor	times 16 db 0

start:
	mov [bootdrv], dl	; save bootdrive

	mov ax, 0x9000
	mov ss, ax		; init stack segment
	mov sp, 0xFFFF		; init stack pointer

	mov si, msg1
	call print

	; jmp reboot
	
	cli
	lgdt [gdtr]

	mov eax, cr0
	or al, 1
	mov cr0, eax		; enable protected mode

	jmp csel:pm		; jump into pm code, clear pl
	hlt			; processor halt

print:
	mov ah, 0Eh
.loop:
	lodsb
	or al, al
	jz .done
	mov bl, 09h
	xor bh, bh
	int 10h
	jmp .loop
.done:
	ret

reboot:
magic equ 1234h
	mov ax, 040h
	mov es, ax
	mov bx, 72h
	mov dword [es:bx], magic

	mov si, msg2
	call print

	mov al, 0FEh
	out 64h, al
	
	; jmp 0FFFFh:0

	hlt
	; jmp $

bits 32
pm:
	mov ax, dsel
	mov ds, ax
	mov ss, ax
	mov esp, 090000h

	call clrscr

	mov eax, 0
	cpuid

	mov [vendor], ebx
	mov [vendor+4], edx
	mov [vendor+8], ecx

	mov esi, msg3
	call kprint

	; times 2 call newline

	mov esi, msg4
	call kprint

	mov esi, vendor
	call kprint

	call newline

	; call kernel
	hlt

%define VIDEO_RAM 0B8000h
cursor	dd 0
screen	equ 80*25

clrscr:				; clear screen function
	pusha
	cld

	mov eax, 0h
	mov edi, VIDEO_RAM
	mov ecx, screen
	rep stosw

	popa
	ret

kprint:				; kernel print function
	pusha
	cld

.loop:
	lodsb
	test al, al
	jz .quit

	cmp al, 10
	jne .skip
	call newline
	jmp .loop
.skip:

	mov ecx, [cursor]
	mov ah, 0Fh		; text color/background
	mov [VIDEO_RAM+ecx*2], ax; al
	inc dword [cursor]
	jmp short .loop

.quit:
	popa
	ret


newline:
	pusha

	mov eax, [cursor]
	mov bh, 80

	div bh
	inc eax

	mov bh, 80
	mul bh

	mov [cursor], eax

	; ((ecx/80)+1)*80
	
	popa
	ret

gdtr:
	dw gdt_end - gdt - 1
	dd gdt
gdt:
	dd 0, 0
csel equ $-gdt
	dw 0FFFFh
	dw 0000h
	db 00h
	db 9Ah
	db 0CFh
	db 00h
dsel equ $-gdt
	dw 0FFFFh
	dw 0000h
	db 00h
	db 92h
	db 0CFh
	db 00h
gdt_end


times 510-($-$$) db 0
dw 0xAA55			; bootsector magic


kernel:
;	incbin "kernel.bin"
