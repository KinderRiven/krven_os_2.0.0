#include "ldt.h"

void ldt_set_descriptor(ldt_descriptor_t *descriptor, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	descriptor -> base_low 		=	(base & 0xFFFF);
	descriptor -> base_middle	=	(base >> 16) & 0xFF;
	descriptor -> base_high		=	(base >> 24) & 0xFF;

	descriptor -> limit_low		=	(limit & 0xFFFF);
	descriptor -> granularity	=	(limit >> 16) & 0x0F;
	
	descriptor -> granularity  |= 	gran;
	descriptor -> access		=	access;
}


