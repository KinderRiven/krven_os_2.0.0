#include "tss.h"
#include "gdt.h"
#include "string.h"
#include "stdio.h"

tss_t tss;
uint16_t tss_ptr;

void init_tss(){

	//初始化为0
	memset(&tss, 0, sizeof(tss));
	
	//向GDT申请一个TSS系统描述符
	tss_ptr = new_tss_descriptor((uint32_t)&tss, sizeof(tss) - 1);
	
	tss.ss0 = DESC_DATA_INDEX;
	tss.iobase = sizeof(tss);
	//刷新tr寄存器
	tss_flush((uint32_t)&tss_ptr);
	
}
