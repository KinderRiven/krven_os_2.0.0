#ifndef INCLUDE_GDT_H_
#define INCLUDE_GDT_H_

#include "types.h"

typedef
struct gdt_descriptor_t
{
	uint16_t	limit_low;			//Limit  0 ~ 15
	uint16_t	base_low;			//Base	 0 ~ 15
	uint8_t		base_middle;		//Limit	16 ~ 24
	uint8_t		access;				//P DPL DT TYPE
	uint8_t		granularity;		// G D 0 AVL Limit
	uint8_t		base_high;			//Base

}__attribute__((packed)) gdt_descriptor_t;

typedef
struct gdt_ptr_t
{
	uint16_t	limit;				//段大小
	uint32_t	base;				//Base
}__attribute__((packed)) gdt_ptr_t;


void init_gdt();

//在GDT中添加一个LDT系统描述符
uint16_t new_ldt_descriptor(uint32_t base, uint32_t limit);

//在GDT中添加一个TSS描述系统描述符
uint16_t new_tss_descriptor(uint32_t base, uint32_t limit);

extern void gdt_flush(uint32_t);

//代码段
#define DESC_TEXT_INDEX	0x08		

//数据段
#define DESC_DATA_INDEX 0x10

//SEG
#define SEG_TIL_0	0X00
#define SEG_TIL_1 	0x04
#define SEG_DPL_0	0x00
#define SEG_DPL_1	0x01
#define SEG_DPL_2	0x02
#define SEG_DPL_3	0x03

//ACCESS
#define ACCESS_LDT 		0x82	//局部LDT描述副
#define ACCESS_TaskGate	0x85	//任务门类型
#define ACCESS_386TSS	0x89	//可用386任务状态段类型值
#define ACCESS_386CGate 0x8C	//386 调用门类型值
#define ACCESS_386IGate 0x8E	//386 中断门类型值
#define ACCESS_386TGate	0x8F	//386 陷阱门类型值

//TYPE 存取权限 4位
#define DESC_TYPE_RW 0x02		//读写
#define DESC_TYPE_RC 0x0A		//执行、读

//S=1 (数据段，代码段)
//s=0 （系统段)
#define DESC_S_1 0x10			
#define DESC_S_0 0x00				

//DPL (0 ~ 3)
#define DESC_DPL_0	0x00
#define DESC_DPL_1	0x20
#define DESC_DPL_2	0x40
#define DESC_DPL_3	0x60

//总是为1
#define DESC_ALWAYS_1 0x80 

//Granularity

//AVL（通常被省略）
#define DESC_AVL 0x00

//G（颗粒度)
#define DESC_G_1 0x00			//1B ~ 1MB
#define DESC_G_4096 0x80		//4KB ~ 4GB

//D/B
#define DESC_DB 0x40

#endif
