#include "gdt.h"
#define GDT_LENGTH	50

uint8_t	gdt_idx;
gdt_ptr_t	gdt_ptr;
gdt_descriptor_t gdt_descriptors[GDT_LENGTH];

static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

void init_gdt()
{
	gdt_ptr.limit = sizeof(gdt_descriptor_t) * GDT_LENGTH - 1;
	gdt_ptr.base  = (uint32_t)&gdt_descriptors;

	//表头为空 0 0
	gdt_set_gate(0,	0,	0,	0,	0);
	
	//代码段 1 (8)
	gdt_set_gate(1,	0,	0xFFFFFFFF,	DESC_TYPE_RC | DESC_S_1 | DESC_DPL_0 | DESC_ALWAYS_1, DESC_G_4096 | DESC_DB | DESC_AVL);
	
	//数据段 2 (16)
	gdt_set_gate(2,	0,	0xFFFFFFFF,	DESC_TYPE_RW | DESC_S_1 | DESC_DPL_0 | DESC_ALWAYS_1, DESC_G_4096 | DESC_DB | DESC_AVL);	

	gdt_idx = 3;
	gdt_flush((uint32_t)&gdt_ptr);
}


//为一个进程申请一个ldt描述符,并返回索引
uint16_t new_ldt_descriptor(uint32_t base, uint32_t limit)
{
	gdt_set_gate(gdt_idx, base, limit, ACCESS_LDT, DESC_G_1 | DESC_AVL);
	gdt_idx ++;
	return (gdt_idx - 1) << 3;
}

//添加一个TSS任务门
uint16_t new_tss_descriptor(uint32_t base, uint32_t limit)
{
	gdt_set_gate(gdt_idx, base, limit, ACCESS_386TSS, DESC_G_1 | DESC_AVL);	
	gdt_idx ++;
	return (gdt_idx - 1) << 3;
}

static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	gdt_descriptors[num].base_low 			=	(base & 0xFFFF);
	gdt_descriptors[num].base_middle		=	(base >> 16) & 0xFF;
	gdt_descriptors[num].base_high			=	(base >> 24) & 0xFF;

	gdt_descriptors[num].limit_low			=	(limit & 0xFFFF);
	gdt_descriptors[num].granularity		=	(limit >> 16) & 0x0F;
	
	gdt_descriptors[num].granularity  	   |= 	gran;
	gdt_descriptors[num].access				=	access;
}



