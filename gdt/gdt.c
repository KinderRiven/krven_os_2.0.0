#include "gdt.h"
#include "stdio.h"
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
	gdt_set_gate(1,	0,	0xFFFFFFFF,	TYPE_RC | DPL_0, DB_32 | G_4096);
	
	//数据段 2 (16)
	gdt_set_gate(2,	0,	0xFFFFFFFF,	TYPE_RW | DPL_0, DB_32 | G_4096);

	//屏幕
	gdt_set_gate(3, 0xC00B8000, 0xFFFF, TYPE_RW | DPL_3, DB_32 | G_1);
	
	gdt_idx = 4;
	gdt_flush((uint32_t)&gdt_ptr);

}


//为一个进程申请一个ldt描述符,并返回索引
uint16_t new_ldt_descriptor(uint32_t base, uint32_t limit)
{
	gdt_set_gate(gdt_idx, base, limit, TYPE_LDT, G_1 | DB_32);
	gdt_idx ++;
	return (gdt_idx - 1) << 3;
}

//添加一个TSS任务门
uint16_t new_tss_descriptor(uint32_t base, uint32_t limit)
{
	gdt_set_gate(gdt_idx, base, limit, TYPE_386TSS, G_1 | DB_32);	
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

void gdt_debug(uint16_t idx)
{
	idx = (idx >> 3);
	gdt_descriptor_t *tmp_descriptor = &gdt_descriptors[idx];

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

	printf("----------------GDT DEBUG----------------\n");	
	printf("[ADD]   : 0x%x\n", tmp_descriptor); 
	printf("[INDEX] : 0x%x (%d)\n", idx,   idx);
	printf("[BASE]  : 0x%x\n", base);
	printf("[LIMIT] : 0x%x (%d)\n", limit, limit);
	printf("[TYPE]  : 0x%x\n", type);
	printf("[DPL]	: 0x%x (%d)\n",dpl, dpl);
	printf("[G]     : 0x%x\n", g);	
	printf("-----------------------------------------\n");

}	

