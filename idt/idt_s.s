[BITS 32]
[SECTION .text]

[EXTERN kernel_stack_top]
[EXTERN current_proc]
[EXTERN tss]

;用于标记当前进程是否在内核中
;用于可重入处理
[EXTERN k_reenter]

%include "proc.inc"
%include "sys.inc"
%include "const.inc"

;刷行idtr寄存器
[GLOBAL idt_flush]
idt_flush:
	mov	eax, [esp + 4]
	lidt [eax]
	ret


;用多行宏定义去定义更加方便
;---------------------------------------------------------;

;没有错误代码的中断
[EXTERN isr_handler]
%macro ISR_NOERRCODE 1
[GLOBAL isr%1]
isr%1:

	push dword 0			;压入错误号
	push dword %1			;压入中断号

	call	save
	push	eax
	
	;到这里的内核栈应该是这样的
	; ------- 高
	; | ret | 
	; | eax | <- esp
	; ------- 低

	call	isr_handler
	add		esp, 4
	
	;返回	
	ret	

%endmacro

;---------------------------------------------------------;

;有错误代码的中断
%macro ISR_ERRCODE 1
[GLOBAL isr%1]
isr%1:
							
	push dword %1			;压入中断号
	
	call	save
	push	eax

	call	isr_handler
	add		esp, 4

	;返回	
	ret
	
%endmacro

;---------------------------------------------------------;

[EXTERN irq_handler]
%macro IRQ_MASTER	2
[GLOBAL irq%1]
irq%1:
	
	push dword 0			;压入错误号			
	push dword %2			;压入中断号

	call	save
	push	eax	
	
	;这里开始屏蔽同类中断
	in		al,	INT_M_CTLMASK
	or		al, (1 << %1)
	out	INT_M_CTLMASK, al
	
	;开中断，可重入		
	sti
	call	irq_handler
	
	;关中断，还原现场
	cli
	add		esp, 4
	
	;解除同类中断
	in		al, INT_M_CTLMASK
	and		al, ~(1 << %1)
	out		INT_M_CTLMASK, al

	;返回	
	ret

%endmacro

;---------------------------------------------------------;

%macro IRQ_SLAVE	2
[GLOBAL irq%1]
irq%1:
	
	push dword 0			;压入错误号			
	push dword %2			;压入中断号

	call	save
	push	eax	

	call	irq_handler
	add		esp, 4
	
	;返回	
	ret

%endmacro

;---------------------------------------------------------;


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
IRQ_MASTER	 0,    32	; 时钟中断
IRQ_MASTER   1,    33 	; 键盘
IRQ_MASTER   2,    34 	; 与 IRQ9 相接，MPU-401 MD 使用
IRQ_MASTER   3,    35 	; 串口设备
IRQ_MASTER   4,    36 	; 串口设备
IRQ_MASTER   5,    37 	; 建议声卡使用
IRQ_MASTER   6,    38 	; 软驱传输控制使用
IRQ_MASTER   7,    39 	; 打印机传输控制使用
IRQ_SLAVE    8,    40 	; 即时时钟
IRQ_SLAVE    9,    41 	; 与 IRQ2 相接，可设定给其他硬件
IRQ_SLAVE   10,    42 	; 建议网卡使用
IRQ_SLAVE   11,    43 	; 建议 AGP 显卡使用
IRQ_SLAVE   12,    44 	; 接 PS/2 鼠标，也可设定给其他硬件
IRQ_SLAVE   13,    45 	; 协处理器使用
IRQ_SLAVE   14,    46 	; IDE0 传输控制使用
IRQ_SLAVE   15,    47 	; IDE1 传输控制使用
						; sys_call 16,48


;寄存器的存储过程

;---------------------------------------------------------;
save:
	
	; 在执行之前已经压入了3部分内容
	; 1. ss esp eflags cs eip	
	; 2. err_code int_no
	; 3. retaddr

	; 这里已经压入了retaddr
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
	; 保存esp的当前指针到eax,以后我们还需要访问寄存器的内容说不定
	; 如：打印出错信息等
	mov		eax, esp

	inc		dword [k_reenter]				; if(++k_reenter != 0)							
	cmp		dword [k_reenter], 0
	jne		.save_reenter

	mov		esp, kernel_stack_top			; 切换到内核栈
	push	restart							; 为ret做准备
	jmp		[eax + RET_ADDR]				; 根据retaddr进行返回

;---------------------------------------------------------;

.save_reenter:
	;mov		bh, 0Fh
	;mov		bl, '?'
	;mov		[gs : (80 * 0 + 0) * 2], bx
	push	restart_reenter	
	jmp		[eax + RET_ADDR]

;---------------------------------------------------------;

; 这里开始是离开内核栈的操作
; 最终通过iret指令进行任务切换

; 如果在内核中进行镶嵌跳转，不需要进行esp的切换
restart:
	
	; 离开内核栈
	mov		esp, [current_proc]
	lldt	[esp + PROC_LDTR]
	lea		eax, [esp + REGS_TOP]
	mov		dword [tss + TSS3_S_SP0], eax

;---------------------------------------------------------;

; 否则需要进行esp的切换
restart_reenter:

	; 出内核 k_reenter -= 1	
	dec		dword [k_reenter]
	
	; 恢复现场
	pop		gs
	pop		fs
	pop		es
	pop		ds
	popad
	add		esp, 12
	iretd

;---------------------------------------------------------;
	
[GLOBAL sys_call]
[EXTERN sys_call_table]
sys_call:

	; eax里面放着存在的中断号
	push	dword 0
	push	dword 48
	
	call	save
	
	; 返回之后，eax里存放寄存器栈顶
	; 找到调用号
	; 找到之前的栈指针
	; 目前栈指向内容为
	; -------
	; |  4  |
	; |  3  |
	; |  2  |
	; |  1  | <-- ebx
	; | ret |
	; | id  | <-- ecx
	; -------
	mov		ecx, [eax + REGS_ESP]

	; eax存放着系统调用号
	mov		eax, [ecx]
	
	; ebx指向调用传参
	mov		ebx, ecx
	add		ebx, 8

	; 压入寄存器参数
	; 中断号
	; 根据中断号选择压入参数的个数
	cmp		eax,	SYS_TREBLE
	ja		.3
	cmp		eax,	SYS_DOUBLE
	ja		.2
	cmp		eax,	SYS_SINGLE
	ja		.1
	cmp		eax, 	SYS_ZERO
	ja		.0

	;三个参数的从这里走
.3:
	push	dword [ebx + 8]
	push	dword [ebx + 4]
	push	dword [ebx]

	sti	
	call	[sys_call_table + eax * 4]
	cli	

	add		esp, 12
	ret
	;两个参数的从这里走
.2:
	push	dword [ebx + 4]
	push	dword [ebx]
	
	sti
	call	[sys_call_table + eax * 4]
	cli

	add		esp, 8
	ret
	;一个参数的从这里走
.1:
	push	dword [ebx]
	
	sti
	call	[sys_call_table + eax * 4]
	cli	

	add		esp, 4
	ret
	;无参数的从这里走
.0:

	sti
	call	[sys_call_table + eax * 4]
	cli

	ret

.end:

