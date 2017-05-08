[BITS 32]
[SECTION .text]

%include "sys.inc"

[GLOBAL get_proc_num]
get_proc_num:

	mov		eax, SYS_PROC_NUM	;系统调用号
	int	 	SYS_INT_NO
	ret		
