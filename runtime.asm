; $Id: runtime.asm 59 2006-09-27 21:28:09Z jlieder $

bits 32


; Data Segment

segment .data

%define VIDEO_RAM 0A0000h+18000h	; 0B8000h
screen	equ 80*25

textcol	db 07h
backcol	db 00h

oldccol db 0
con_cursor dd 0

s0_base	equ 03F8h		; we also could read this from 0:400h
s1_base equ 02F8h

sbase	equ s0_base		; set the default port (S0)
sready	db 0			; serial port has been initialized?


; Code Segment

segment .text

global memset, memcpy
global con_cursor
; global con_clear, con_write
; global con_setcolor, con_setxy
global rsetcolor
global rprint
; global con_hw_show_cursor, con_hw_hide_cursor
; global con_sw_toggle_cursor
global reboot, halt, yield
global ser_init, ser_write
global getsysmem

global reboot5

memset:
	push ebp
	mov ebp, esp

	push eax
	push ecx
	push esi

	mov edi, [ebp+8]
	mov eax, [ebp+12]
	mov ecx, [ebp+16]

	rep stosb

	pop esi
	pop ecx
	pop eax

	mov esp, ebp
	pop ebp

	ret	

memcpy:
	push ebp
	mov ebp, esp

	push ecx
	push edi
	push esi

	mov esi, [ebp+8]
	mov edi, [ebp+12]
	mov ecx, [ebp+16]

	mov edx, ecx
	and ecx, 0xFFFFFFFC
	shr ecx, 2
	rep movsd

	mov ecx, edx
	and ecx, 3
	rep movsb

	pop esi
	pop edi
	pop edx
	pop ecx

	mov esp, ebp
	pop ebp

	ret

rsetcolor:				; set text and background color
	push ebp
	mov ebp, esp

	mov al, [ebp+8]
	mov bl, [ebp+12]

	mov [textcol], al
	mov [backcol], bl

	mov esp, ebp
	pop ebp

	ret

con_setxy:
	push ebp
	mov ebp, esp

	mov eax, [con_cursor]
	mov al, [oldccol]
	mov [VIDEO_RAM+eax*2+1], al

	xor ax, ax
	xor bx, bx

	mov bl, byte [ebp+8]  ; x
	mov al, byte [ebp+12] ; y

	mov cl, 80
	mul cl
	add ax, bx

	mov [con_cursor], eax
	ret

con_sw_toggle_cursor:
	mov ebx, [con_cursor]
	mov al, [VIDEO_RAM+ebx*2+1]
	xor al, ~(2 << 4)
	mov [VIDEO_RAM+ebx*2+1], al

	ret

con_clear:				; clear screen function
	pusha
	cld

	mov ax, 0			; clear charachter
	mov ax, [backcol]		; load background color
	shl ax, 4
	or ax, [textcol]		; add text color attribute
	shl ax, 8

	mov [oldccol], ah

	mov edi, VIDEO_RAM
	mov ecx, screen
	rep stosw

	mov dx, 3D4h
	mov al, 14
	out dx, al

	mov dx, 3D5h
	mov al, 0
	out dx, al

	mov dx, 3D4h
	mov al, 15
	out dx, al

	mov dx, 3D5h
	mov al, 0
	out dx, al

con_hw_hide_cursor:
	mov dx, 3d4h
	mov al, 0ah
	out dx, al

	mov dx, 3d5h
	mov al, 1 << 5
	out dx, al	

	; mov dword [con_cursor], 0
	; popa
	ret

rprint:
con_write:			; kernel print function
	; pusha			; saves ebp as well
	push ebp		; caller resp. for registers? (cdecl)
	mov ebp, esp
	mov esi, [ebp+8]	; load parameter (char*)

	mov dl, 0
	mov dl, [backcol]
	shl dl, 4
	or dl, [textcol]	; compute text/bg color

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

	mov ecx, [con_cursor]
	mov ah, dl ; 0Fh	; text color/background
	mov [VIDEO_RAM+ecx*2], ax ; al
	inc dword [con_cursor]
	jmp short .loop

.quit:
	; popa
	pop ebp
	ret


newline:
	pusha

	; ((eax/80)+1)*80

	mov eax, [con_cursor]
	mov bh, 80

	div bh
	inc eax
	mov cl, al		; save current line

	mov bh, 80
	mul bh

	mov [con_cursor], eax

	; call con_scroll

	; mov dx, 3D4h
	; mov al, 15
	; out dx, al

	; mov dx, 3D5h
	; mov al, 10
	; out dx, al
	
	popa
	ret

ser_init:
	push eax
	push edx

	mov dx, sbase

	mov al, 0h		; interrupt off
	inc dx			; sbase+1
	out dx, al

	mov al, 80h		; dlab on
	add dx, 2		; sbase+3
	out dx, al

	mov al, 0Ch		; baud rate (low) 9.600bps
	mov dx, sbase		; sbase
	out dx, al

	mov al, 0h		; baud rate (high)
	inc dx			; sbase+1
	out dx, al

	mov al, 03h		; 8n1
	add dx, 2		; sbase+3
	out dx, al

	mov al, 0C7h ; 0	; fifo control
	dec dx			; sbase+2
	out dx, al

	mov al, 0Bh		; activate dtr, rts and out2 
	add dx, 2		; sbase+4
	out dx, al

	; sti

	pop edx
	pop eax
	ret

ser_write:				; serial port print function
	push ebp		; caller resp. for registers? (cdecl)
	mov ebp, esp
	mov esi, [ebp+8]	; load parameter (char*)

	; mov al, [sready]
	; test al, al
	; jnz .skip
	;
	; call init_serial

.skip:
	mov dx, sbase
	cld

.loop:
	lodsb
	test al, al
	jz .quit		; end of string?

	out dx, al		; send char to serial port
	jmp short .loop

.quit:
	; popa
	pop ebp
	ret

getsysmem:
	push ebp
	mov ebp, esp

	; push eax
	; push edx

	mov eax, 0
	mov edx, 1EFFFCh

.loop:
	inc eax
	mov dword [edx], 0AAAAAAAAh

	mov ebx, [edx]
	add edx, 0x100000
	cmp ebx, 0xAAAAAAAA
	je .loop

	; pop edx
	; pop eax

	mov esp, ebp
	pop ebp

	ret

yield:
	sti
	hlt
	ret

reboot:
	mov al, 0FEh
	out 64h, al

halt:				; stop all activity
	cli
	hlt

null_idt_descr:
	dw 0
	dw 0, 0

reboot5:
	lidt [null_idt_descr]
	int 0
