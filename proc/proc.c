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

//新建一个进程
pid_t new_task_proc(uint32_t fun)
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
	procs[proc_num].regs.esp = (uint32_t) proc_stack[proc_num] + PROC_STACK_SIZE - 1;	
	procs[proc_num].regs.eflags = 0x1202;

	//debug_proc(&procs[proc_num]);
	procs[proc_num].pid = proc_num++;
	return proc_num - 1;
}

//创建一个用户进程
pid_t new_user_proc(uint32_t fun)
{
	//建立进程的LDT (修改DPL在低权限下运行)
	
	//代码段CS
	ldt_set_descriptor(&procs[proc_num].ldts[0], 0, 0xFFFFFFFF, 
				TYPE_RC | DPL_3, DB_32 | G_4096); 
	
	//数据段DS
	ldt_set_descriptor(&procs[proc_num].ldts[1], 0, 0xFFFFFFFF, 
				TYPE_RW | DPL_3, DB_32 | G_4096); 
	

	//在GDT中创建LDT描述符
	procs[proc_num].ldtr = new_ldt_descriptor((uint32_t)procs[proc_num].ldts, LDT_SIZE * sizeof(ldt_descriptor_t) - 1);	

	//初始化段寄存器
	procs[proc_num].regs.cs = USER_CS_INDEX | TI_1 | RPL_3;

	procs[proc_num].regs.ds = USER_DA_INDEX | TI_1 | RPL_3;
	procs[proc_num].regs.es = USER_DA_INDEX | TI_1 | RPL_3;
	procs[proc_num].regs.fs = USER_DA_INDEX | TI_1 | RPL_3;
	procs[proc_num].regs.ss = USER_DA_INDEX | TI_1 | RPL_3;
	procs[proc_num].regs.gs = KERNEL_VIDEO_INDEX   | RPL_3;

	procs[proc_num].regs.eip = (uint32_t) fun;	
	procs[proc_num].regs.esp = (uint32_t) proc_stack[proc_num] + PROC_STACK_SIZE - 1;	
	
	//用户进程关闭IO权限
	procs[proc_num].regs.eflags = 0x202;
	procs[proc_num].pid = proc_num++;
	
	//printf("return\n");
	return proc_num - 1;
}

static void debug_proc(proc_t *proc){

	printf("[ds/es/fs/gs] is  0x%04X 0x%04X 0x%04X 0x%04X\n", 
			proc -> regs.ds, proc -> regs.es, proc -> regs.fs, proc -> regs.gs);
	printf("[cs:eip] is 0x%04X : 0x%X\n", proc -> regs.cs, proc -> regs.eip);
	printf("[ss:esp] is 0x%04X : 0x%X\n", proc -> regs.ss, proc -> regs.esp);
}
