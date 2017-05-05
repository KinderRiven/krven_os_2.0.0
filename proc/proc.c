#include "proc.h"
#include "gdt.h"
#include "stdio.h"
#include "ldt.h"
#include "vmm.h"
#include "pmm.h"
#include "sched.h"

proc_t procs[PROC_MAX_NUM];
char proc_stack[PROC_MAX_NUM][PROC_STACK_SIZE];
int proc_num = 0;

static void debug_proc();


//测试

void proc1()
{
	int i, j;
	
	while(1){
		printc(c_black, c_red, "A");
		for(i = 0; i < 10000; i++)
			for(j = 0; j < 1000; j++);
	}

}

void proc2()
{
	int i, j;
	
	while(1){
		printc(c_black, c_green, "B");
		for(i = 0; i < 10000; i++)
			for(j = 0; j < 1000; j++);
	}

}

void proc_start()
{
	//new_proc(proc1);
	//new_proc(proc2);
	
	//从第一个进程开始运行
	current_proc = &procs[0];
	restart();
}

pid_t new_proc(proc_fun fun)
{

	//建立进程的LDT (修改DPL在低权限下运行)

	//代码段CS
	ldt_set_descriptor(&procs[proc_num].ldts[0], 0, 0xFFFFFFFF, 
				TYPE_RC | DPL_1, DB_32 | G_4096); 
	
	//数据段DS
	ldt_set_descriptor(&procs[proc_num].ldts[1], 0, 0xFFFFFFFF, 
				TYPE_RW | DPL_1, DB_32 | G_4096); 
	
	//在GDT中创建LDT描述符
	procs[proc_num].ldtr = new_ldt_descriptor((uint32_t)procs[proc_num].ldts, LDT_SIZE * sizeof(ldt_descriptor_t) - 1);	

	//初始化段寄存器
	procs[proc_num].regs.cs = USER_CS_INDEX | TI_1 | RPL_1;

	procs[proc_num].regs.ds = USER_DA_INDEX | TI_1 | RPL_1;
	procs[proc_num].regs.es = USER_DA_INDEX | TI_1 | RPL_1;
	procs[proc_num].regs.fs = USER_DA_INDEX | TI_1 | RPL_1;
	procs[proc_num].regs.ss = USER_DA_INDEX | TI_1 | RPL_1;
	procs[proc_num].regs.gs = KERNEL_VIDEO_INDEX   | RPL_1;

	procs[proc_num].regs.eip = (uint32_t) fun;	
	procs[proc_num].regs.esp = (uint32_t) proc_stack[proc_num] + PROC_STACK_SIZE - 10;	
	procs[proc_num].regs.eflags = 0x1202;

	//debug_proc(&procs[proc_num]);
	proc_num ++;
	return proc_num - 1;
}

static void debug_proc(proc_t *proc){

	printf("[ds/es/fs/gs] is  0x%04X 0x%04X 0x%04X 0x%04X\n", 
			proc -> regs.ds, proc -> regs.es, proc -> regs.fs, proc -> regs.gs);
	printf("[cs:eip] is 0x%04X : 0x%X (0x%X)\n", proc -> regs.cs, proc -> regs.eip, proc1);
	printf("[ss:esp] is 0x%04X : 0x%X\n", proc -> regs.ss, proc -> regs.esp);
}
