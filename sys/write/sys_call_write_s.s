[BITS 32]
[SECTION .text]

%include "sys.inc"

;屏幕打印系统调用
[GLOBAL write]
write:	
	
	;系统调用号
	mov		eax, SYS_WRITE
	mov		ebx, [esp + 4]
	int		48
	ret

;修改打印颜色系统调用
[GLOBAL alert_write_color]
alert_write_color:
	
	;系统调用号
	mov		eax, SYS_ALERT_WRITE_COLOR
	mov		ebx, [esp + 4]
	mov		ecx, [esp + 8]
	int		48
	ret
