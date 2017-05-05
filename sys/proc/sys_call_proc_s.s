[BITS 32]
[SECTION .text]

[GLOBAL get_proc_num]
get_proc_num:

	mov	 	eax, 0
	int	 48
	ret		
