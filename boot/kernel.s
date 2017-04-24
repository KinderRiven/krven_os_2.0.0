
MBOOT_HEADER_MAGIC 	equ 	0x1BADB002 	; Multiboot 魔数，由规范决定的

MBOOT_PAGE_ALIGN 	equ 	1 << 0    	; 0 号位表示所有的引导模块将按页(4KB)边界对齐
MBOOT_MEM_INFO 		equ 	1 << 1    	; 1 号位通过 Multiboot 信息结构的 mem_* 域包括可用内存的信息
										; (告诉GRUB把内存空间的信息包含在Multiboot信息结构中)

MBOOT_HEADER_FLAGS 	equ 	MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM 		equ 	- (MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)


[BITS 32] 

section .init.text

dd MBOOT_HEADER_MAGIC 	
dd MBOOT_HEADER_FLAGS   
dd MBOOT_CHECKSUM      

[GLOBAL start] 		
[GLOBAL mboot_ptr_tmp] 	
[EXTERN kern_entry]

start:
	
	cli  			
	
	mov esp, STACK_TOP  	
	mov ebp, 0	
	and esp, 0FFFFFFF0H	
	mov [mboot_ptr_tmp], ebx
	call kern_entry

stop:
	hlt 			 
	jmp stop 		 

;-----------------------------------------------------------------------------

section .init.data			 
stack:	
	times 1024 	db 	0
STACK_TOP equ $-stack-1
mboot_ptr_tmp : dd 0

;-----------------------------------------------------------------------------
