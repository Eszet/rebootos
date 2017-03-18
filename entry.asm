; $Id$

bits 32

extern _kmain
extern rprint, halt
extern rsetcolor
extern yield
global entry, __limit

extern gdt, gdtp, idtp
global gdt_load, idt_load

segment .data

__limit	dd 0

aiieeeee db 10, "Aiieeeee!!!", 10, 0

_argc	dd 1			; first parameter for _kmain
_param1	db "kernel", 0
_argv	dd _param1		; second parameter for _kmain

segment .text

entry:
	; pusha			; precaution (push all)

	mov ebp, esp

	push _argv
	push dword [_argc]	; push arguments in reverse order

	call _kmain		; call the kernel's main function
	; popa			; restore previous "safe" state

	push dword 4
	push dword 0
	call rsetcolor

	push aiieeeee
	call rprint		; print a message of desperation

	; int 3			; test interrupt handling

	sti
.idle:
	hlt			; call yield() function to
	jmp .idle		; save some power ;)

	cli
	hlt			; unless something went wrong
				; we shouldn't end up here...

extern csel, dsel, esel, ssel

addr dd gdt_load.jump
_cs  dw 0

gdt_load:
	mov eax, 0

	cmp dword [csel], 0	; check the gdt_entries first
	jz .return

	cmp dword [dsel], 0
	jz .return

	cmp dword [ssel], 0
	jz .return

	lgdt [gdtp]		; no checks are being conducted!

	cmp dword [esel], 0
	jz .skip_es

	mov ebx, [esel]
	sub ebx, gdt
	mov es, bx
	mov fs, bx		; fs, gs = es
	mov gs, bx

.skip_es:
	mov ebx, [ssel]
	sub ebx, gdt
	mov ss, bx

	mov ebx, [csel]
	sub ebx, gdt
	mov [_cs], bx

	jmp far [addr]
.jump:
	mov ebx, [dsel]
	sub ebx, gdt
	mov ds, bx

	mov eax, 1

.return:
	ret

	
idt_load:
	lidt [idtp]
	ret


; Interrupt Service Routine Macro

%macro isr 1
%define no %1
global isr %+ no
isr %+ no:
	cli

; exceptions 8, 10..14 already push an error code
%if (no != 8) && ((no < 10) || (no > 14))
	push byte 0
%endif
	push byte no

	jmp isr_stub
%endmacro

%assign isr_no 0
%rep 32
	isr isr_no
%assign isr_no isr_no+1
%endrep


; Interrupt Request Macro

%define first_irq 40h ; IRQ0 == ISR64

%macro irq 2
global irq %+ %1
irq %+ %1:
	cli

	push byte 0		; dummy error code
	push byte %2

	jmp irq_stub
%endmacro

%assign irq_no 0
%rep 16
%assign isr_no irq_no+first_irq
	irq irq_no, isr_no
%assign irq_no irq_no+1
%endrep


extern fault_handler
extern irq_handler


%macro int_stub 2
%1:				; isr_stub
	pusha
	push ds
	push es
	push fs
	push gs

; reload data segments according to dsel and esel

; do es first as we want to touch the data segment
; selector as late as possible

%if 0
	mov ebx, [esel]
	cmp ebx, 0
	jz .skip_es

	sub ebx, gdt
	mov es, bx

.skip_es:
	mov eax, [dsel]
	sub eax, gdt
	mov ds, ax
	mov fs, ax
	mov gs, ax
%endif

	mov ax, 16
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov eax, esp
	push eax

	mov eax, %2 		; fault_handler
	call eax		; this call preserves eip

	pop eax
	pop gs
	pop fs
	pop es
	pop ds
	popa

	; add esp, 4
	add esp, 8		; frame pointer et al?

	iret
%endmacro

int_stub isr_stub, fault_handler
int_stub irq_stub, irq_handler
