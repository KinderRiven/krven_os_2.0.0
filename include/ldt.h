#ifndef INCLUDE_LDT_H_
#define INCLUDE_LDT_H_

#include "types.h"

typedef
struct ldt_descriptor_t{
	
	uint16_t 	limit_low;
	uint16_t	base_low;
	uint8_t		base_middle;
	uint8_t		access;
	uint8_t		granularity;
	uint8_t		base_high;

} ldt_descriptor_t;
//__attribute__((packed)) ldt_descriptor_t;	//按实际占用大小进行对齐

void ldt_set_descriptor(ldt_descriptor_t *descriptor, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

void ldt_debug(ldt_descriptor_t *);

#define LDT_INDEX_TEXT 0x00
#define LDT_INDEX_DATA 0x08

#endif
