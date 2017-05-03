#include "proc.h"
#include "gdt.h"
#include "stdio.h"
#include "ldt.h"
#include "vmm.h"
#include "pmm.h"

proc_t procs[1];
proc_t *current_proc;

uint8_t proc_stack[PROC_STACK_SIZE];

static void debug_proc();

int proc1()
{
	//while(1);
	printf("Proc A is running!\n");
	
	while(1){}
	
	return 0;
}

void init_proc(int num)
{

	//建立进程的LDT (修改DPL在低权限下运行)

	//代码段CS
	ldt_set_descriptor(&procs[num].ldts[0], 0, 0xFFFFFFFF, 
				DESC_TYPE_RC | DESC_S_1 | DESC_DPL_3 | DESC_ALWAYS_1, 
				DESC_G_4096 | DESC_DB | DESC_AVL); 
	
	//数据段DS
	ldt_set_descriptor(&procs[num].ldts[1], 0, 0xFFFFFFFF, 
				DESC_TYPE_RW | DESC_S_1 | DESC_DPL_3 | DESC_ALWAYS_1, 
				DESC_G_4096 | DESC_DB | DESC_AVL); 
	
	//printf("%x %x\n", DESC_TYPE_RC | DESC_S_1 |DESC_DPL_3 | DESC_ALWAYS_1,
	//	DESC_TYPE_RC | DESC_S_1 | DESC_DPL_0 |DESC_ALWAYS_1);
	
	//在GDT中创建LDT描述符
	procs[num].ldtr = new_ldt_descriptor((uint32_t)procs[num].ldts, LDT_SIZE * sizeof(ldt_descriptor_t) - 1);	

	//初始化段寄存器
	procs[num].regs.cs = LDT_INDEX_TEXT | SEG_TIL_1 | SEG_DPL_3;

	procs[num].regs.ds = LDT_INDEX_DATA | SEG_TIL_1 | SEG_DPL_3;
	procs[num].regs.es = LDT_INDEX_DATA | SEG_TIL_1 | SEG_DPL_3;
	procs[num].regs.fs = LDT_INDEX_DATA | SEG_TIL_1 | SEG_DPL_3;
	procs[num].regs.ss = LDT_INDEX_DATA | SEG_TIL_1 | SEG_DPL_3;
	procs[num].regs.gs = LDT_INDEX_DATA | SEG_TIL_1 | SEG_DPL_3;

	procs[num].regs.eip = (uint32_t) proc1;	
	procs[num].regs.esp = (uint32_t) proc_stack + PROC_STACK_SIZE;	
	procs[num].regs.eflags = 0x1202;

	current_proc = &procs[0];
	
	//debug_proc(current_proc);
	//gdt_debug(0x08);
	//gdt_debug(0x10);
	//gdt_debug(0x18);
	//gdt_debug(procs[num].ldtr);
	//printf("%x %x\n", &procs[num].ldts[0], &procs[num].ldts[1]); 
	
	//ldt_debug(&procs[num].ldts[0]);
	//gdt_debug(0x08);
	
	//ldt_debug(&procs[num].ldts[1]);
	//gdt_debug(0x10);
	//gdt_debug(0x08);

	//while(1);	
	//debug_proc(current_proc);
	
	printf("0x%x", procs[num].regs.eip);
	while(1);

	restart();
	while(1){};

}

static void debug_proc(proc_t *proc){

	printf("[ds/es/fs/ss/gs] is  0x%04X 0x%04X 0x%04X 0x%04X 0x%04X\n", 
			proc -> regs.ds, proc -> regs.es, proc -> regs.fs,
			proc -> regs.ss, proc -> regs.gs);
	printf("cs:eip is 0x%04x : 0x%x\n", proc -> regs.cs, proc -> regs.eip);
	
}
