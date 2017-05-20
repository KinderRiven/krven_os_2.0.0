[BITS 32]
[SECTION .text]

%include "sys.inc"

[GLOBAL get_proc_num]
get_proc_num:
	
	push	dword SYS_PROC_NUM
	int	 	SYS_INT_NO
	add		esp, 4
	ret	

[GLOBAL recv_message]
recv_message:
	
	push	dword SYS_RECV_MESSAGE
	int		SYS_INT_NO
	add		esp, 4
	ret

[GLOBAL send_message]
send_message:
	
	push	dword SYS_SEND_MESSAGE
	int		SYS_INT_NO
	add		esp, 4
	ret

[GLOBAL get_proc_pid]
get_proc_pid:
	
	push	dword SYS_PROC_PID
	int		SYS_INT_NO
	add		esp, 4
	ret	

[GLOBAL send_interrupt]
send_interrupt:
	push	dword SYS_SEND_INT
	int		SYS_INT_NO
	add		esp, 4
	ret	


[GLOBAL recv_interrupt]
recv_interrupt:
	push	dword SYS_RECV_INT
	int		SYS_INT_NO
	add		esp, 4
	ret


[GLOBAL exit]
exit:
	push	dword SYS_EXIT
	int		SYS_INT_NO
	add		esp, 4
	ret

[GLOBAL kill]
kill:
	push	dword SYS_KILL
	int		SYS_INT_NO
	add		esp, 4
	ret
