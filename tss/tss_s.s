[BITS 32]
[SECTION .text]

[GLOBAL tss_flush]
tss_flush:

	xor		eax, eax	
	mov		eax, [esp + 4]
	ltr		[eax]
	ret

