[GLOBAL debug_print_right]
debug_print_right:
	mov	ah, 0Fh
	mov	al, [esp + 4]
	mov	[gs : (80 * 24 + 79) * 2], ax
	ret

[GLOBAL debug_print_left]
debug_print_left:
	mov	ah, 0Fh
	mov	al, [esp + 4]
	mov	[gs : (80 * 24 + 0) * 2], ax
	ret
	
