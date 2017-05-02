#ifndef INCLUDE_TSS_H_
#define INCLUDE_TSS_H_

#include "types.h"

typedef
struct tss_t
{
	uint32_t backlink;
	
						//不同权限级的栈
	uint32_t esp0;
	uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;

	uint32_t cr3;		//页表	
	uint32_t eip;
	uint32_t flags;		//标志位

	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esi;
	uint32_t edi;
	
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;

	uint16_t trap;
	uint16_t iobase;

}tss_t;


extern tss_t tss;			//对于单核的处理器只有一个tss足够了
							//Linux下就是这样的
void init_tss();			//初始化tss
void tss_flush(uint32_t);	//刷新tr寄存器

#endif
