; $Id: boot.asm 49 2006-09-20 22:03:44Z jlieder $

bits 16
org 7C00h

init:
	jmp start			; hop to startup code

; Data Segment

msg01	db "Loading PurpOS...", 7, 0
msg02	db "Rebooting...", 0
dot	db ".", 0
colon	db "*", 0

fs_name	db 13, 10, "File System ", 0
ki_name	db 13, 10, "Kernel ", 0

msg03	db "Failed to enable A20 gate!", 0
bootdrv db 0

flp_spt    dw 0x0012
flp_heads  dw 0x0002
flp_bps    dw 0x0200

abs_sector db 0
abs_head   db 0
abs_track  db 0

kernel_entry equ 10000h		; kernel entry point

loc_meminfo equ 7000h
memcount   db 0

sec_fsinfo equ 1		; fs info in 2nd sector
sec_kernel equ 2
len_kernel equ 40		; max. 12K kernel code

; Code Segment (16-bit)

start:
	mov ax, 0
	mov ds, ax
	mov es, ax

	mov [bootdrv], dl	; save bootdrive

	mov ax, 9000h
	mov ss, ax		; init stack segment
	mov sp, 0FFFFh		; init stack pointer
	mov bp, sp

	mov si, msg01
	call print

enable_a20:
	mov ax, 2401h
	int 15h

	mov cx, 3

	mov si, fs_name
	call print

	mov cx, sec_kernel-sec_fsinfo
	mov ax, sec_fsinfo
	mov bx, 7C00h+flp_bps	; es=07C0:bx=0200
	call load_sectors

	mov si, ki_name
	call print


	push es
	mov ax, kernel_entry/16
	mov es, ax

	mov cx, len_kernel
	mov ax, sec_kernel
	mov bx, 0 ; kernel_entry % 16 ; es=0000:bx=1000
	call load_sectors	; FFFF-1000 = 61439B
				; max. kernel image size
	pop es			; new es was temporary

	call stop_motor		; stop the floppy motor

%if 1
mem_size_real:
	mov ebx, 0
	mov edi, loc_meminfo

.find_next:
	mov eax, 0e820h
	mov edx, 534d4150h
	mov ecx, 20h

	int 15h
	jc .done

	inc byte [memcount]
	add edi, 20h

	or ebx, ebx
	jne .find_next

.done:
%endif
	xor ax, ax
	mov ds, ax		; clear ds (for lgdt)
	
	cli
	lgdt [gdtr]		; load descriptor

	mov eax, cr0
	or al, 1
	mov cr0, eax		; enable protected mode

	jmp csel:pm		; jump into pm code, clear pl
	hlt			; processor halt


stop_motor:
	xor al, al
	mov dx, 3F2h
	out dx, al

	ret

; --------------------------------------------------------------------

; load_sectors - read cx sectors from disk starting with sector ax
;                into memory location at es:bx
; input: cx, ax, es:bx

load_sectors:
.main:				; main load loop
	mov di, 5
.loop:				; re-read sector
	push ax
	push bx
	push cx

	call lba_chs

	mov ah, 0x02
	mov al, 1

	mov ch, [abs_track]
	mov cl, [abs_sector]
	mov dh, [abs_head]
	mov dl, [bootdrv]

	int 13h
	jnc .success

	xor ax, ax
	int 13h
	dec di

	pop cx
	pop bx
	pop ax

	jnz .loop
	int 18h

.success:
	mov si, colon
	call print

	pop cx
	pop bx
	pop ax

	add bx, [flp_bps]
	inc ax

	loop .main

	ret


; lba_chs - convert LBA address to CHS address
; input: ax
; output: abs_sector, abs_head, abs_track

lba_chs:
	xor dx, dx
	div word [flp_spt]
	inc dl

	mov byte [abs_sector], dl
	xor dx, dx
	div word [flp_heads]

	mov byte [abs_head], dl
	mov byte [abs_track], al

	ret


; Function: print(si=addr_of(strz))
; Prints string on screen via BIOS interrupt.

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


; Function: reboot()
;
; Reboots the system by programming the keyboard
; controller.

reboot:
	magic equ 1234h

	; mov ax, 040h
	; mov es, ax
	; mov bx, 72h
	; mov dword [es:bx], magic

	mov si, msg02
	call print		; reboot message

	mov al, 0FEh
	out 64h, al		; reset computer
	
	jmp 0FFFFh:0		; alternate RBM

	hlt			; jmp $


; Code Segment (32-bit)

bits 32
pm:
	mov ax, dsel
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov esp, 090000h

	call kernel_entry	; code in 2nd sector

	hlt


; Global Descriptor Table

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
