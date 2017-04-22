[BITS 32]

[section .text]
[GLOBAL	start]				;连接器识别
[EXTERN kernel_entry]		;导入入口函数

start:	
	jmp 	$

[section .bss]
stack:
	resb	1024
STACK_TOP equ $-stack-1
