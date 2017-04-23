org		0100h
jmp		START

BaseOfStack				equ		0100h
BaseOfKernelFile		equ		08000h
OffsetOfKernelFile		equ		0h
BaseOfKernelFilePhyAddr	equ		BaseOfKernelFile * 10h

BaseOfLoader			equ		09000h
OffsetOfLoader			equ		0100h
BaseOfLoaderPhyAddr 	equ		BaseOfLoader * 10h
PageDirBase				equ		100000h				;页目录开始地址
PageTblBase				equ		101000h				;页表开始地址
KernelEntryPointPhyAddr equ		30400h

%include "fat12.inc"
%include "pm.inc"

;---------------------------------------------------------------------------
;									GDT									   ;
;---------------------------------------------------------------------------
HEAD_GDT:		Descriptor 		  	  0, 	    0,		   				   0
;可执行可读写
DESC_FLAT_C:	Descriptor		  	  0,  0fffffh,	DA_CR |DA_32|DA_LIMIT_4K
;读写
DESC_FLAT_RW:	Descriptor		  	  0,  0fffffh,  DA_DRW|DA_32|DA_LIMIT_4K
;可读写
DESC_VIDEO: 	Descriptor		0B8000h,   0ffffh,   		  DA_DRW|DA_DPL3
;--------------------------------------------------------------------------;
GdtLen			equ		$ - HEAD_GDT						;GDT表长度
GdtPtr			dw		GdtLen - 1							;
				dd		BaseOfLoaderPhyAddr + HEAD_GDT		;物理地址
;--------------------------------------------------------------------------;
;GDT选择子
SelectorFlatC	equ		DESC_FLAT_C  - HEAD_GDT
SelectorFlatRW	equ		DESC_FLAT_RW - HEAD_GDT
SelectorVideo	equ		DESC_VIDEO   - HEAD_GDT + SA_RPL3	;请求者特权级

;---------------------------------------------------------------------------;

wRootDirSizeForLoop dw		RootDirSectors

wSectorNo			dw		0
bOdd				db		0
dwKernelSize		dd		0
KernelFileName		db		"KERNEL     ", 0

MESSAGE_LENGTH		equ		9
LOADER_MESSAGE:		db		"[Running]"
SUCCESS_MESSAGE		db		"[SUCCESS]"
ERROR_MESSAGE		db		"[ERROR]  "
PM_MESSAGE			db		"[PM]     "

START:									;开始
SEGMENT_INIT:							;段初始化
					
	mov		ax,	cs						;初始化
	mov		ds,	ax
	mov		es,	ax
	mov		ss,	ax
	mov		sp,	BaseOfStack
	
	mov		dh,	2						;打印一个Loading
	mov		dl, 0
	call	PRINT_MESSAGE

	mov		word [wSectorNo], SectorNoOfRootDirectory
	xor		ah,	ah
	xor		dl, dl
	int		13h

;进行块的读写
SEARCH_IN_ROOT_DIR_BEGIN:
	cmp		word [wRootDirSizeForLoop], 0
										;如果读完所有的目录区	
	jz 		NO_KERNELBIN				;jmp if zero
	dec		word [wRootDirSizeForLoop]	;else go next
	
	mov 	ax,	BaseOfKernelFile			
	mov 	es,	ax						;es:bx加载到内存哪里
	mov		bx,	OffsetOfKernelFile		;
	mov 	ax,	[wSectorNo]				;该读第几个扇区
	mov		cl,	1						;第一次读几个扇区
	call	ReadSector					;
	
	mov		si,	KernelFileName			;ds:si -> "LOADER.BIN"
	mov		di,	OffsetOfKernelFile		;cs:di -> 装载512字节的地址
	cld									;比较指令
	
										;一个目录32字节
	mov		dx,	10h						;512字节 / 32 = 16次 	

;对已经加载进入内存的数据进行匹配
SEARCH_FOR_KERNELBIN:
	cmp		dx,	0						;如果16个条目都读完了加载下一个盘块
	jz		GOTO_NEXT_SECTOR_IN_ROOT_DIR
	dec		dx							;
	mov		cx,	11						;一个目录比较11个字符

;一个一个字节比较
CMP_FILENAME:
	cmp		cx,	0
	jz		FILENAME_FOUND
	dec		cx
	lodsb								;
	cmp		al,	byte [es : di]			;
	jz		GO_ON						;一样就继续
	jmp		DIFFERENT					;发现不一样的字符就不继续
	
GO_ON:
	inc		di							;
	jmp		CMP_FILENAME				;继续读

;没有找到
DIFFERENT:
	and		di,	0FFE0h					;一个条目20h长度
	add		di,	20h						;从下一个条目开始
	mov		si,	KernelFileName			;
	jmp		SEARCH_FOR_KERNELBIN		;

GOTO_NEXT_SECTOR_IN_ROOT_DIR:
	add		word [wSectorNo],	1		;
	jmp		SEARCH_IN_ROOT_DIR_BEGIN	;

NO_KERNELBIN:
	mov		dl,	2
	mov		dh,	3						;NO LOADERBIN PRINT	
	call 	PRINT_MESSAGE				;打印NO Load
	jmp 	$

FILENAME_FOUND:
	mov		ax,	RootDirSectors
	and		di,	0FFF0h
	
	push	eax
	mov		eax,	[es:di+01Ch]
	mov		dword	[dwKernelSize], eax
	pop		eax
	add		di,		01Ah
	mov		cx,		word [es:di]
	push	cx
	add		cx,	ax
	add		cx,	DeltaSectorNo
	mov		ax,	BaseOfKernelFile
	mov		es,	ax
	mov		bx,	OffsetOfKernelFile
	mov		ax,	cx

GOON_LOADING_FILE:
	mov		cl, 1	
	call	ReadSector
	pop		ax
	call	GetFATEntry
	cmp		ax,	0FFFh
	jz		FILE_LOADED
	push	ax
	mov		dx,	RootDirSectors
	add		ax,	dx
	add		ax,	DeltaSectorNo
	add		bx,	[BPB_BytsPerSec]
	jmp		GOON_LOADING_FILE

FILE_LOADED:
	call	KillMotor
	mov		dl, 1
	mov		dh,	3
	call	PRINT_MESSAGE

;---------------------------------------------------------;
;				从这里跳入保护模式						  ;
;---------------------------------------------------------;

PREPARE_PM:
	
	lgdt	[GdtPtr]							;gdt指针
	cli
	
	in		al,  92h
	or		al,	 00000010b	
	out		92h, al
	
	mov		eax, cr0
	or		eax, 1
	mov		cr0, eax

	jmp		dword SelectorFlatC:(BaseOfLoaderPhyAddr+PM_START)

KillMotor:
	push	dx
	mov		dx,	03F2h
	mov		al,	0
	out		dx,	al
	pop		dx
	ret

;---------------------------------------------------------;
;                 将LOADER加载入内存					  ;
;---------------------------------------------------------;
GetFATEntry:
	push	es
	push	bx
	push	ax
	mov		ax,	BaseOfKernelFile
	sub		ax,	0100h
	mov		es,	ax
	pop		ax
	mov		byte [bOdd], 0
	mov		bx,	3
	mul		bx
	mov		bx, 2
	div		bx
	cmp		dx, 0
	jz		EVEN
	mov		byte [bOdd], 1
EVEN:
	xor		dx,	dx
	mov		bx,	[BPB_BytsPerSec]
	div		bx
	push	dx
	mov		bx,	0
	add		ax,	SectorNoOfFAT1
	mov		cl,	2
	call	ReadSector
	pop		dx
	add		bx,	dx
	mov		ax,	[es:bx]
	cmp		byte [bOdd], 1
	jnz		EVEN_2
	shr		ax, 4

EVEN_2:
	and		ax, 0FFFh

GET_FAT_ENRY_OK:
	pop		bx
	pop		es
	ret		

;---------------------------------------------------------;
;				 调用int10中断						  	  ;
; es : bp = 显示字符串的地址   					 	      ;
; cx 串长度                                               ;
; dh, dl = 起始行列                                       ;
; bh 页号  bl 属性值									  ;
; ah 0x13  al 显示模式									  ;
;---------------------------------------------------------;
CLEAR_CONSOLE:
	mov		ax,	0600h
	mov		bx,	0700h
	mov		cx,	0
	mov		dx,	0184fh
	int		10h
	ret
	
;---------------------------------------------------------;
;	13h读盘中断说明          	 	;       			  ;
; ah =   00h , dl = 驱动器号 	 	;      复位软驱		  ;
;---------------------------------------------------------;
; ah =   02h , al = 要读扇区数		;	   从磁盘读	      ;
; ch = 柱面号, cl = 起始扇区号		;	   入es:bx 		  ;
; dh = 磁头号, dl = 驱动器号  		;	   所指向的		  ;
; es:bx -> 数据加载的地方     		;		缓冲区  	  ;
;---------------------------------------------------------;
ReadSector:
	push 	bp						;保存bp寄存器
	mov  	bp, sp					;复制栈顶指针到bp寄存器
	sub		esp,2					
	mov  	byte [bp - 2],	cl		;从cl获得要要读几个扇区存入栈
	
	push 	bx						;	
	mov  	bl, [BPB_SecPerTrk]		;18
	div  	bl						;al存商 ah存余数
	inc		ah						;
	mov		cl,	ah					;起始扇区
	mov		dh,	al
	shr		al,	1					;
	mov		ch,	al					;柱面号
	and		dh, 1					;磁头号
	pop		bx
	
	mov		dl,	[BS_DrvNum]			;驱动器号一般为0

.GoOnReading:
	mov		ah,	2					;02h
	mov		al,	byte [bp - 2]		;要读几个磁面
	int		13h
	jc		.GoOnReading			;jmp if error

	add 	esp, 2				;栈恢复
	pop		bp
	ret

;dh is hight
;dl	is meesage no
PRINT_MESSAGE:
	mov		ax,	MESSAGE_LENGTH
	mul		dl
	add		ax,	LOADER_MESSAGE
	mov		bp,	ax
	mov		ax,	ds
	mov		es,	ax
	mov		cx,	MESSAGE_LENGTH
	mov		ax,	01301h
	mov		bx,	000ch
	mov		dl,	0
	int		10h
	ret
	
[SECTION .s32]
[BITS 32]
PM_START:												;如果进入了保护模式打印一个P
	mov		ax,	SelectorVideo
	mov		gs,	ax
	mov		ah, 0Fh
	mov		al, 'P'
	mov		[gs:((80 * 24 + 79) * 2)], ax
PM_INIT:												;对寄存器进行初始化
	mov		 ax, SelectorFlatRW
	mov		 ds, ax
	mov		 es, ax
	mov		 fs, ax
	mov		 ss, ax
	mov		esp, TopOfStack
	call	SetupPaging
	call	InitKernel
	jmp		PM_JMP
;---------------------------------------------------------;
;				开启分页内存管理从这里开始				  ;
;---------------------------------------------------------;

SetupPaging:
	mov		 ax, SelectorFlatRW							;初始化页表目录
	mov		 es, ax
	mov		edi, PageDirBase
	xor		eax, eax								
	mov		eax, PageTblBase|PG_P|PG_USU|PG_RWW			;第一个页表地址
.1:
	stosd
	add		eax, 4096									;每次加4096	
	loop	.1

	mov		ecx, 1024 * 1024							;1024 * 1024个页表
	mov		edi, PageTblBase
	xor		eax, eax
	mov		eax, PG_P | PG_USU | PG_RWW
.2:
	stosd
	add		eax, 4096
	loop	.2

	mov		eax, PageDirBase
	mov		cr3, eax
	mov		eax, cr0
	or		eax, 80000000h
	mov		cr0, eax
	jmp		short .3
.3:
	nop
	ret

;---------------------------------------------------------;
;				从这里开始装入内核代码					  ;
;---------------------------------------------------------;
InitKernel:
	xor		esi, esi
	mov		 cx, word [BaseOfKernelFilePhyAddr + 2Ch]
	movzx	ecx, cx
	mov		esi, [BaseOfKernelFilePhyAddr + 1Ch]
	add		esi, BaseOfKernelFilePhyAddr

.Begin:
	mov		eax, [esi + 0]
	cmp		eax, 0
	jz		.NoAction
	push	dword[esi + 010h]
	mov		eax, [esi + 04h]
	add		eax, BaseOfKernelFilePhyAddr
	push	eax
	push	dword[esi + 08h]
	call	MemCpy
	add		esp, 12

.NoAction:
	add		esi, 020h
	dec		ecx
	jnz		.Begin
	ret
;---------------------------------------------------------;
;														  ;
;---------------------------------------------------------;
MemCpy:
	push	ebp
	mov		ebp, esp
	push	esi
	push	edi
	push	ecx
	mov		edi, [ebp + 8]	; Destination
	mov		esi, [ebp + 12]	; Source
	mov		ecx, [ebp + 16]	; Counter
.1:
	cmp		ecx, 0		; 判断计数器
	jz	.2		; 计数器为零时跳出

	mov		al, [ds:esi]	
	inc		esi			
	mov		byte [es:edi], al	
	inc		edi

	dec		ecx		
	jmp	.1
.2:
	mov		eax, [ebp + 8]
	pop		ecx
	pop		edi
	pop		esi
	mov		esp, ebp
	pop		ebp
	ret			; 函数结束，返回
;---------------------------------------------------------;
;				把管理权限交给内核						  ;
;---------------------------------------------------------;
PM_JMP:
	jmp		SelectorFlatC:KernelEntryPointPhyAddr
;栈
StackSpace:
	times	1024 	db		0
TopOfStack	equ		BaseOfLoaderPhyAddr + $
