[BITS 32]

[section .text]
[GLOBAL	_start]				;连接器识别
[EXTERN kernel_entry]

_start:		
	mov		esp, STACK_TOP
	call	kernel_entry
	jmp	$
	

[section .bss]
stack:
	resb	1024 * 2
STACK_TOP 	equ	 $ - stack - 1
