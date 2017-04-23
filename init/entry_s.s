[BITS 32]

[section .text]
[GLOBAL	_start]				;连接器识别

_start:	
	jmp	$
[section .bss]
stack:
	resb	1024
STACK_TOP equ $-stack-1
