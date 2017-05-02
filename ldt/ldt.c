#include "ldt.h"
#include "stdio.h"

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

void ldt_debug(ldt_descriptor_t *ldt_descriptor)
{
	ldt_descriptor_t *tmp_descriptor = ldt_descriptor;

	//基地地址
	uint32_t base = (uint32_t) tmp_descriptor -> base_low | 
						((uint32_t)tmp_descriptor -> base_middle << 16) | 
							((uint32_t)tmp_descriptor -> base_high << 24);
	
	//limit
	uint32_t limit = (uint32_t) tmp_descriptor -> limit_low | 
						((uint32_t) (tmp_descriptor -> granularity & 0x0F) << 16);

	uint8_t type = (uint8_t) tmp_descriptor -> access & 0xF; 
	uint8_t dpl  = (uint8_t) (tmp_descriptor -> access >> 5) & 0x03; 
	uint8_t g = (uint8_t) (tmp_descriptor -> granularity >> 7) & 0x01;

	printf("----------------LDG DEBUG----------------\n");	
	printf("[ADDR]  : 0x%x\n", (uint32_t) tmp_descriptor);
	printf("[BASE]  : 0x%x\n", base);
	printf("[LIMIT] : 0x%x (%d)\n", limit, limit);
	printf("[TYPE]  : 0x%x\n", type);
	printf("[DPL]	: 0x%x (%d)\n",dpl, dpl);
	printf("[G]     : 0x%x\n", g);	
	printf("-----------------------------------------\n");

}	
