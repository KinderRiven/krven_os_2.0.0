#ifndef INCLUDE_PROC_H_
#define INCLUDE_PROC_H_

#include "types.h"
#include "ldt.h"

#define LDT_SIZE 2
#define PROC_STACK_SIZE 4096

typedef
struct proc_regs_t{

	//pop start in here
	uint32_t gs;
	uint32_t fs;
	uint32_t es;
	uint32_t ds;
	
	//popad start in here
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t kernel_esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	
	uint32_t retaddr;
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;
	uint32_t esp;
	uint32_t ss;

} proc_regs_t;

typedef
struct proc_t{
	
	proc_regs_t regs;					//寄存器保存的值 (18 * 4 = 72字节)
	uint16_t ldtr;						//ldtr寄存器的值 (2字节)
	ldt_descriptor_t ldts[LDT_SIZE];	//2个
	pid_t pid;							//进程id号

}proc_t;

void init_proc(int num);
void restart();

#endif
