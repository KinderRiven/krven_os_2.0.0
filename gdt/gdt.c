#include "gdt.h"

#define GDT_LENGTH	5

gdt_ptr_t	gdt_ptr;
gdt_descriptor_t gdt_descriptors[GDT_LENGTH];

static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

void init_gdt()
{
	gdt_ptr.limit = sizeof(gdt_descriptor_t) * GDT_LENGTH - 1;
	gdt_ptr.base  = (uint32_t)&gdt_descriptors;

	//平坦模式
	gdt_set_gate(0,	0,			 0,	   0,	 0);
	gdt_set_gate(1,	0,	0xFFFFFFFF,	0x9A, 0xCF);			//代码段
	gdt_set_gate(2,	0,	0xFFFFFFFF,	0x92, 0xCF);			//数据段
	gdt_set_gate(3,	0,	0xFFFFFFFF,	0xFA, 0xCF);			//用户模式代码段
	gdt_set_gate(4, 0,	0xFFFFFFFF,	0xF2, 0xCF);			//用户模式数据段

	gdt_flush((uint32_t)&gdt_ptr);
}


static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	gdt_descriptors[num].base_low 			=	(base & 0xFFFF);
	gdt_descriptors[num].base_middle		=	(base >> 16) & 0xFF;
	gdt_descriptors[num].base_high			=	(base >> 24) & 0xFF;

	gdt_descriptors[num].limit_low			=	(limit & 0xFFFF);
	gdt_descriptors[num].granularity		=	(limit >> 16) & 0x0F;
	
	gdt_descriptors[num].granularity  	   |= 	gran & 0xF0;
	gdt_descriptors[num].access				=	access;
}



