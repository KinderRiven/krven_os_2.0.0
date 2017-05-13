[BITS 32]
[SECTION .text]

; -------------------------------------- ;
; 一个字 = 2个字节						 ;
; 1 word = 2 bytes (16 bits)			 ;
; 1 double word = 2 word  (32 bits)		 ;
; -------------------------------------- ;

[GLOBAL port_read]

; void port_read(uint32_t port, void *buf, int n)

port_read:

	; --------
	; |   n  |
	; |  buf |
	; | port |
	; [  ret | <- esp
	; -------- 

	mov		edx, [esp + 4]		; port	
	mov		edi, [esp + 8]		; buf
	mov		ecx, [esp + 12]		; n

	shr		ecx, 1				; n = n / 2
	
	cld
	rep		insw				;读一个字出来
	ret

[GLOBAL port_write]

; void port_write(uint32_t port, void *buf, int n)

port_write:

	; --------
	; |   n  |
	; |  buf |
	; | port |
	; [  ret | <- esp
	; -------- 

	mov		edx, [esp + 4]		; port	
	mov		edi, [esp + 8]		; buf
	mov		ecx, [esp + 12]		; n

	shr		ecx, 1				; n = n / 2
	
	cld
	rep		outsw				;读一个字出来
	ret
