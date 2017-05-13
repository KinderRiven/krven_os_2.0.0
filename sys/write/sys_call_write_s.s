[BITS 32]
[SECTION .text]

%include "sys.inc"

; fun(1, 2, 3, 4)
; 等价与
; push 4
; push 3
; push 2
; push 
; call fun 
; 然而 call fun 又等价与
; push retaddr
; jmp  fun
; 最终栈的内容为

; ----- 高
; | 4 |
; | 3 |
; | 2 |
; | 1 |
; |ret| <-esp
; ----- 低


;屏幕打印系统调用
[GLOBAL write]
write:
	push	dword SYS_WRITE
	int		SYS_INT_NO
	add		esp, 4
	ret

[GLOBAL write_clear]
write_clear:

	push	dword SYS_WRITE_CLEAR
	int		SYS_INT_NO
	add		esp, 4
	ret

;修改打印颜色系统调用
[GLOBAL alert_write_color]
alert_write_color:

	push	dword SYS_ALERT_WRITE_COLOR
	int		SYS_INT_NO
	add		esp, 4
	ret

[GLOBAL write_color]
write_color:

	push	dword SYS_WRITE_COLOR
	int		SYS_INT_NO
	add		esp, 4
	ret
