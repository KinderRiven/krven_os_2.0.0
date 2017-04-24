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
extern void gdt_flush(uint32_t);

#endif
