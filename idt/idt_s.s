[BITS 32]
[SECTION .text]

[EXTERN kernel_stack_top]
[EXTERN current_proc]
[EXTERN tss]

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

INT_NO		EQU		REGS_ADDR+ 4	;52
ERR_CODE	EQU		INT_NO	 + 4	;56

REGS_EIP	EQU		ERR_CODE + 4	;60
REGS_CS		EQU		REGS_EIP + 4	;64
REGS_FLAG	EQU		REGS_CS	 + 4	;68
REGS_ESP	EQU		REGS_FLAG+ 4	;72
REGS_SS		EQU		REGS_ESP + 4	;76
REGS_TOP	EQU		REGS_SS	 + 4	;84
PROC_LDTR	EQU		REGS_SS  + 4	;84
;----------------------------------------------	
TSS3_S_SP0	EQU		4
;----------------------------------------------

;刷行idtr寄存器
[GLOBAL idt_flush]
idt_flush:
	mov	eax, [esp + 4]
	lidt [eax]
	ret


;用多行宏定义去定义更加方便
;没有错误代码的中断
%macro ISR_NOERRCODE 1
[GLOBAL isr%1]
isr%1:
	cli						;关中断
	push 0					;压入错误号
	push %1					;压入中断号
	jmp	isr_common_stub		
%endmacro

;有错误代码的中断
%macro ISR_ERRCODE 1
[GLOBAL isr%1]
isr%1:						
	cli						;自动压入错误号
	push %1					;压入中断号
	jmp	isr_common_stub		
%endmacro

%macro IRQ	2
[GLOBAL irq%1]
irq%1:
	cli
	push 0					;压入错误号			
	push %2					;压入中断号
	jmp	 irq_common_stub
%endmacro

; 定义中断处理函数
ISR_NOERRCODE  0 	; 0 #DE 除 0 异常
ISR_NOERRCODE  1 	; 1 #DB 调试异常
ISR_NOERRCODE  2 	; 2 NMI
ISR_NOERRCODE  3 	; 3 BP 断点异常 
ISR_NOERRCODE  4 	; 4 #OF 溢出 
ISR_NOERRCODE  5 	; 5 #BR 对数组的引用超出边界 
ISR_NOERRCODE  6 	; 6 #UD 无效或未定义的操作码 
ISR_NOERRCODE  7 	; 7 #NM 设备不可用(无数学协处理器) 
ISR_ERRCODE    8 	; 8 #DF 双重故障(有错误代码) 
ISR_NOERRCODE  9 	; 9 协处理器跨段操作
ISR_ERRCODE   10 	; 10 #TS 无效TSS(有错误代码) 
ISR_ERRCODE   11 	; 11 #NP 段不存在(有错误代码) 
ISR_ERRCODE   12 	; 12 #SS 栈错误(有错误代码) 
ISR_ERRCODE   13 	; 13 #GP 常规保护(有错误代码) 
ISR_ERRCODE   14 	; 14 #PF 页故障(有错误代码) 
ISR_NOERRCODE 15 	; 15 CPU 保留 
ISR_NOERRCODE 16 	; 16 #MF 浮点处理单元错误 
ISR_ERRCODE   17 	; 17 #AC 对齐检查 
ISR_NOERRCODE 18 	; 18 #MC 机器检查 
ISR_NOERRCODE 19 	; 19 #XM SIMD(单指令多数据)浮点异常


; 20~31 Intel 保留
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

ISR_NOERRCODE 255

; 32～255 用户自定义
IRQ	  0,    32	; 时钟中断
IRQ   1,    33 	; 键盘
IRQ   2,    34 	; 与 IRQ9 相接，MPU-401 MD 使用
IRQ   3,    35 	; 串口设备
IRQ   4,    36 	; 串口设备
IRQ   5,    37 	; 建议声卡使用
IRQ   6,    38 	; 软驱传输控制使用
IRQ   7,    39 	; 打印机传输控制使用
IRQ   8,    40 	; 即时时钟
IRQ   9,    41 	; 与 IRQ2 相接，可设定给其他硬件
IRQ  10,    42 	; 建议网卡使用
IRQ  11,    43 	; 建议 AGP 显卡使用
IRQ  12,    44 	; 接 PS/2 鼠标，也可设定给其他硬件
IRQ  13,    45 	; 协处理器使用
IRQ  14,    46 	; IDE0 传输控制使用
IRQ  15,    47 	; IDE1 传输控制使用
				; sys_call 16,48

[GLOBAL isr_common_stub]
[EXTERN isr_handler]
isr_common_stub:
	
	; 进入中断
	; 若特权级发生变化 依次压入: (error code) ss esp eflags cs eip
	; 否则压入 :  (error code) eflags cs eip
	
	; 从这里开始进行现场的保护	
	sub		esp, 4
	pushad
	push	ds
	push	es
	push	fs	
	push	gs

	; 恢复到内核态
	mov		dx, ss
	mov		ds, dx
	mov		es,	dx

	; debug函数
	;inc	byte [gs:0]
	;mov	al, 0x20
	;out	0x20, al

	; 恢复到内核栈
	mov		eax, esp
	mov		esp, kernel_stack_top

	; 压入寄存器参数
	push	eax
	call	isr_handler

	; 离开内核栈
	mov		esp, [current_proc]
	lldt	[esp + PROC_LDTR]
	lea		eax, [esp + REGS_TOP]
	mov		dword [tss + TSS3_S_SP0], eax

	; 恢复现场
	pop		gs
	pop		fs
	pop		es
	pop		ds
	popad
	add		esp, 12
	iretd

[GLOBAL irq_common_stub]
[EXTERN irq_handler]

irq_common_stub:
	
	; 进入中断
	; 若特权级发生变化 依次压入: (error code) ss esp eflags cs eip
	; 否则压入 :  (error code) eflags cs eip
	
	; 从这里开始进行现场的保护	
	sub		esp, 4
	pushad
	push	ds
	push	es
	push	fs	
	push	gs

	; 恢复到内核态
	mov		dx, ss
	mov		ds, dx
	mov		es,	dx

	; debug函数
	;inc	byte [gs:0]
	;mov	al, 0x20
	;out	0x20, al

	; 恢复到内核栈
	mov		eax, esp
	mov		esp, kernel_stack_top

	; 压入寄存器参数
	push	eax
	call	irq_handler

	; 离开内核栈
	mov		esp, [current_proc]
	lldt	[esp + PROC_LDTR]
	lea		eax, [esp + REGS_TOP]
	mov		dword [tss + TSS3_S_SP0], eax

	; 恢复现场
	pop		gs
	pop		fs
	pop		es
	pop		ds
	popad
	add		esp, 12
	iretd

[GLOBAL sys_call]
[EXTERN sys_call_handler]
sys_call:

	cli
	; eax里面放着存在的中断号
	push	0
	push	48	
	
	; 进入中断
	; 若特权级发生变化 依次压入: (error code) ss esp eflags cs eip
	; 否则压入 :  (error code) eflags cs eip
	
	; 从这里开始进行现场的保护	
	sub		esp, 4
	pushad
	push	ds
	push	es
	push	fs	
	push	gs

	; 恢复到内核态
	mov		dx, ss
	mov		ds, dx
	mov		es,	dx

	; 恢复到内核栈
	mov		esp, kernel_stack_top

	; 压入寄存器参数
	push	eax
	call	sys_call_handler

	; 离开内核栈
	mov		esp, [current_proc]
	lldt	[esp + PROC_LDTR]
	lea		eax, [esp + REGS_TOP]
	mov		dword [tss + TSS3_S_SP0], eax

	; 恢复现场
	pop		gs
	pop		fs
	pop		es
	pop		ds
	popad
	add		esp, 12
	iretd

.end:

