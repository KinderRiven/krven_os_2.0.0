[BITS 32]

;--------------------------------------------
REGS_GS 	EQU 	0			  	;0
REGS_FS		EQU		REGS_GS  + 4	;4
REGS_ES		EQU 	REGS_FS  + 4	;8
REGS_DS		EQU		REGS_ES  + 4	;12

;popad
REGS_EDI	EQU		REGS_DS  + 4	;16
REGS_ESI	EQU		REGS_EDI + 4	;20
REGS_EBP	EQU		REGS_ESI + 4	;24
KERNEL_ESP	EQU		REGS_EBP + 4	;28
REGS_EBX	EQU		KERNEL_ESP + 4	;32
REGS_EDX	EQU		REGS_EBX + 4	;36
REGS_ECX	EQU		REGS_EDX + 4	;40
REGS_EAX	EQU		REGS_ECX + 4	;44

REGS_ADDR	EQU		REGS_EAX + 4	;48

REGS_EIP	EQU		REGS_ADDR+ 4	;52
REGS_CS		EQU		REGS_EIP + 4	;56
REGS_FLAG	EQU		REGS_CS	 + 4	;60
REGS_ESP	EQU		REGS_FLAG+ 4	;64
REGS_SS		EQU		REGS_ESP + 4	;68
REGS_TOP	EQU		REGS_SS	 + 4	;72
PROC_LDTR	EQU		REGS_SS  + 4	;72
;----------------------------------------------	
TSS3_S_SP0	EQU		4
;----------------------------------------------

[EXTERN current_proc]
[EXTERN tss]
[GLOBAL restart]
restart:

	mov		esp, [current_proc]				;让esp指向current_proc所指向的进程表
	lldt	[esp + PROC_LDTR]				;更新ldt寄存器
	lea		eax, [esp + REGS_TOP]				
	mov		dword [tss + TSS3_S_SP0], eax

	pop		gs
	pop		fs
	pop		es
	pop		ds
	popad									;EDI,ESI,EBP,ESP,EBX,EDX,ECX,EAX.
	
	add		esp, 4
	iretd									;iretd 弹出eip cs eflags esp ss
