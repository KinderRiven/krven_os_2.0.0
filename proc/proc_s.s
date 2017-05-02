LDTR 	equ		68	;17 * 4

[EXTERN current_procs]
[GLOBAL restart]
restart:
	mov		esp, [current_procs]
	lldt	[esp + LDTR]

	pop		gs
	pop		fs
	pop		es
	jmp		$
	pop		ds
	popad		
	add		esp, 4
	
	;iretd 弹出eip cs eflags
	iretd

