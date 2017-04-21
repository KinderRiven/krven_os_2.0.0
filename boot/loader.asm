org		0100h
jmp		START

BaseOfStack			equ		0100h
BaseOfKernelFile	equ		08000h
OffsetOfKernelFile	equ		0h

%include "fat12.inc"

wRootDirSizeForLoop dw		RootDirSectors

wSectorNo			dw		0
bOdd				db		0
dwKernelSize		dd		0
KernelFileName		db		"KERNEL     ", 0

MESSAGE_LENGTH		equ		9
LOADER_MESSAGE:		db		"[Running]"
SUCCESS_MESSAGE		db		"[SUCCESS]"
ERROR_MESSAGE		db		"[ ERROR ]"

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
	push	ax
	push	bx
	mov		ah,	0Eh
	mov		al, '.'
	mov		bl,	0Fh
	int		10h
	pop		bx
	pop		ax

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
	jmp		BaseOfKernelFile:OffsetOfKernelFile

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
