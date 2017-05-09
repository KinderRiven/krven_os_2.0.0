[BITS 32]
[SECTION .text]

%include "sys.inc"

;屏幕打印系统调用
[GLOBAL write]
write:	
	
	mov		eax, SYS_WRITE
	mov		ebx, esp
	int		SYS_INT_NO
	ret

[GLOBAL write_clear]
write_clear:

	mov		eax, SYS_WRITE_CLEAR
	mov		ebx, esp
	int		SYS_INT_NO
	ret

;修改打印颜色系统调用
[GLOBAL alert_write_color]
alert_write_color:
	
	mov		eax, SYS_ALERT_WRITE_COLOR
	mov		ebx, esp
	int		SYS_INT_NO
	ret

[GLOBAL write_color]
write_color:
	
	mov		eax, SYS_WRITE_COLOR
	mov		ebx, esp
	int		SYS_INT_NO
	ret
