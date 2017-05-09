[BITS 32]
[SECTION .text]

%include "sys.inc"

[GLOBAL get_proc_num]
get_proc_num:
	mov		eax, SYS_PROC_NUM	;系统调用号
	mov		ebx, esp
	int	 	SYS_INT_NO
	ret	

[GLOBAL recv_message]
recv_message:	
	mov		eax, SYS_RECV_MESSAGE
	mov		ebx, esp
	int		SYS_INT_NO
	ret

[GLOBAL send_message]
send_message:
	mov		eax, SYS_SEND_MESSAGE
	mov		ebx, esp
	int		SYS_INT_NO
	ret
	
