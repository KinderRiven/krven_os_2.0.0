[GLOBAL switch_to]

; switch_to(prev -> context, current->context)
switch_to:
	
	mov	eax, [esp + 4]		;context -> prev
							; 保存当前的寄存器内容到
							; context -> pre指向的PCB块
	mov [eax + 0],  esp		; esp
	mov	[eax + 4],  ebp		; ebp
	mov [eax + 8],  ebx		; ebx
	mov	[eax + 12], esi		; esi
	mov [eax + 16], edi		; edi
	
	pushf					; 标志寄存器内容进栈
	pop ecx					; 标志寄存器保存到ecx中
	mov	[eax + 20], ecx		; eflags保存标志寄存器内容 
	mov	eax, [esp + 8]		; 指向context -> current的内容

	mov	esp, [eax + 0]		;
	mov ebp, [eax + 4]		;
	mov ebx, [eax + 8]		;
	mov esi, [eax + 12]		;
	mov edi, [eax + 16]		;
	mov eax, [eax + 20]		;
	push eax				;
	popf					;标志寄存器置数字
	ret
