[BITS 32]
[SECTION .text]

%include "sys.inc"

;屏幕打印系统调用
[GLOBAL write]
write:
	push	dword esp	
	push	dword SYS_WRITE
	int		SYS_INT_NO
	add		esp, 8
	ret

[GLOBAL write_clear]
write_clear:

	push	dword esp
	push	dword SYS_WRITE_CLEAR
	int		SYS_INT_NO
	add		esp, 8
	ret

;修改打印颜色系统调用
[GLOBAL alert_write_color]
alert_write_color:

	push	dword esp	
	push	dword SYS_ALERT_WRITE_COLOR
	int		SYS_INT_NO
	add		esp, 8
	ret

[GLOBAL write_color]
write_color:

	push	dword esp	
	push	dword SYS_WRITE_COLOR
	int		SYS_INT_NO
	add		esp, 8
	ret
