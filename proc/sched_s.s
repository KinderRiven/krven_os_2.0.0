[BITS 32]
[SECTION .text]

%include "proc.inc"

[EXTERN current_proc]
[EXTERN tss]
[GLOBAL first_proc_start]
first_proc_start:

	mov		esp, [current_proc]				;让esp指向current_proc所指向的进程表
	lldt	[esp + PROC_LDTR]				;更新ldt寄存器
	lea		eax, [esp + REGS_TOP]				
	mov		dword [tss + TSS3_S_SP0], eax	;进入中断时esp指向regs结构体栈顶

	pop		gs
	pop		fs
	pop		es
	pop		ds
	popad									;EDI,ESI,EBP,ESP,EBX,EDX,ECX,EAX.
	
	add		esp, 12							;int_no
											;err_code
											;ret_addr
	iretd									;iretd 弹出eip cs eflags esp ss
