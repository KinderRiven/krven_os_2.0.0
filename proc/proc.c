#include "proc.h"
#include "string.h"
#include "gdt.h"
#include "stdio.h"
#include "ldt.h"
#include "vmm.h"
#include "pmm.h"
#include "sched.h"

//进程表
proc_t procs[PROC_MAX_NUM];

//进程栈
char proc_stack[PROC_MAX_NUM][PROC_STACK_SIZE];


//进程数量
int proc_count = 0;

static void debug_proc();

/*********************************************************
 |      8    |           8        |     8      |   8     |   
 ---------------------------------------------------------
 |Base(24-31)|G|B|0|A|LIMIT(16-19)|1|DPL|S|TYPE|BASE16-23|
 ---------------------------------------------------------
 |       Base (0 - 15)            |		LIMIT(0-15)		 |
 ---------------------------------------------------------
 |            16                  |         16           |
 *********************************************************/

void proc_print_ldt(int num)
{
	ldt_descriptor_t *ldts[2];
	
	//代码描述符号
	ldts[0] = &procs[num].ldts[0];		
	
	//数据描述符号		
	ldts[1] = &procs[num].ldts[1];

	int i;
	
	for(i = 0; i < 2; i++)
	{
		uint32_t base = (((uint32_t) ldts[i] -> base_low) | 
						 ((uint32_t) ldts[i] -> base_middle  << 16) | 
						 ((uint32_t) ldts[i] -> base_high << 24));

		uint32_t limit = (((uint32_t) ldts[i] -> limit_low) |
						  ((uint32_t) (ldts[i] -> granularity & 0xF) << 16));
	
		uint8_t  g  = (ldts[i] -> granularity) & G_MASK;	
		g >>= 7;	
	
		//uint8_t  b  = (ldts[i] -> granularity) & DB_MASK;			
		uint8_t dpl = (ldts[i] -> access) & DPL_MASK;
		dpl >>= 5;

		if(g)
			limit = limit * 4096;

		printf("[BASE]  : 0x%08X (%u)\n", base, base);	
		printf("[LIMIT] : 0x%08X (%u)\n", limit, limit);
		printf("[DPL]   : %d\n", dpl);
		printf("[G]     : %d\n", g);
	}		
}

void init_proc_table()
{
	proc_count = 0;
	int i;

	for(i = 0; i < PROC_MAX_NUM; i++)
	{
		//进程表占用情况为空
		procs[i].take_up = 0;
	}		
}

int get_empty_proc_block()
{
	int i;
	
	for(i = 0; i < PROC_MAX_NUM; i++)
	{
		//分配一个进程块
		if(procs[i].take_up == 0)
		{
			return i;
		}
	}
	
	return -1;
}

//新建一个init进程，以后所有的进程都从这里fork
pid_t new_init_proc(uint32_t fun)
{

	//获得内核内存使用情况	
	//uint32_t limit = get_kernel_mm();

	int idx = get_empty_proc_block();

	//进程表已经满了
	if(idx == PROC_TABLE_FULL)
		return -1;

	//初始化进程控制块
	memset(&procs[idx], 0, sizeof(proc_t));
	
	procs[idx].take_up = 1;
			
	//代码段CS
	ldt_set_descriptor(&procs[idx].ldts[0], 0, 0xFFFFFFFF, //limit, 
				TYPE_RC | DPL_3, DB_32 | G_4096); 
	
	//数据段DS
	ldt_set_descriptor(&procs[idx].ldts[1], 0, 0xFFFFFFFF, //limit, 
				TYPE_RW | DPL_3, DB_32 | G_4096); 
	

	//在GDT中创建LDT描述符
	procs[idx].ldtr = new_ldt_descriptor((uint32_t)procs[idx].ldts, LDT_SIZE * sizeof(ldt_descriptor_t) - 1);	

	//初始化段寄存器
	//CS
	procs[idx].regs.cs = USER_CS_INDEX | TI_1 | RPL_3;

	//DS
	procs[idx].regs.ds = USER_DA_INDEX | TI_1 | RPL_3;
	
	//ES
	procs[idx].regs.es = USER_DA_INDEX | TI_1 | RPL_3;
	
	//FS
	procs[idx].regs.fs = USER_DA_INDEX | TI_1 | RPL_3;
	
	//SS
	procs[idx].regs.ss = USER_DA_INDEX | TI_1 | RPL_3;
	
	//GS
	procs[idx].regs.gs = KERNEL_VIDEO_INDEX   | RPL_3;

	//eip指向函数执行的开始
	procs[idx].regs.eip = (uint32_t) fun;
	
	//内核进程栈的分配	
	procs[idx].regs.esp = (uint32_t) proc_stack[idx] + PROC_STACK_SIZE - 1;	
	
	//用户进程关闭IO权限
	procs[idx].regs.eflags = 0x202;
	procs[idx].pid = idx;

	//优先级
	procs[idx].priority = 1;
	procs[idx].waiting = 0;	
	
	return idx;
}

//新建一个进程
pid_t new_task_proc(uint32_t fun)
{

	//获得一个空的进程块
	int idx = get_empty_proc_block();
	
	//进程表已经满了
	if(idx == PROC_TABLE_FULL)
		return -1;
	
	memset(&procs[idx], 0, sizeof(proc_t));
	procs[idx].take_up = 1;
	
	//建立进程的LDT (修改DPL在低权限下运行)
	//代码段CS
	//0 ~ 4G
	ldt_set_descriptor(&procs[idx].ldts[0], 0, 0xFFFFFFFF, 
				TYPE_RC | DPL_1, DB_32 | G_4096); 
	
	//数据段DS
	//0 ~ 4G
	ldt_set_descriptor(&procs[idx].ldts[1], 0, 0xFFFFFFFF, 
				TYPE_RW | DPL_1, DB_32 | G_4096); 
	
	//在GDT中创建LDT描述符
	procs[idx].ldtr = new_ldt_descriptor((uint32_t)procs[idx].ldts, LDT_SIZE * sizeof(ldt_descriptor_t) - 1);	

	//初始化段寄存器
	
	//CS
	procs[idx].regs.cs = USER_CS_INDEX | TI_1 | RPL_1;
	
	//DS
	procs[idx].regs.ds = USER_DA_INDEX | TI_1 | RPL_1;
	
	//ES
	procs[idx].regs.es = USER_DA_INDEX | TI_1 | RPL_1;
	
	//FS
	procs[idx].regs.fs = USER_DA_INDEX | TI_1 | RPL_1;
	
	//SS
	procs[idx].regs.ss = USER_DA_INDEX | TI_1 | RPL_1;
	
	//GS
	procs[idx].regs.gs = KERNEL_VIDEO_INDEX   | RPL_1;

	procs[idx].regs.eip = (uint32_t) fun;	
	procs[idx].regs.esp = (uint32_t) proc_stack[idx] + PROC_STACK_SIZE - 1;	
	
	//系统进程拥有IO权限
	procs[idx].regs.eflags = 0x1202;

	procs[idx].pid = idx;
	
	procs[idx].priority = 1;
	procs[idx].waiting = 0;	
	
	return idx;
}

//创建一个用户进程
pid_t new_user_proc(uint32_t fun)
{

	//获得一个空的进程控制块	
	int idx = get_empty_proc_block();

	if(idx == PROC_TABLE_FULL)
		return -1;
	
	memset(&procs[idx], 0, sizeof(proc_t));
	procs[idx].take_up = 1;
	
	//建立进程的LDT (修改DPL在低权限下运行)
	
	//代码段CS
	ldt_set_descriptor(&procs[idx].ldts[0], 0, 0xFFFFFFFF, 
				TYPE_RC | DPL_3, DB_32 | G_4096); 
	
	//数据段DS
	ldt_set_descriptor(&procs[idx].ldts[1], 0, 0xFFFFFFFF, 
				TYPE_RW | DPL_3, DB_32 | G_4096); 
	

	//在GDT中创建LDT描述符
	procs[idx].ldtr = new_ldt_descriptor((uint32_t)procs[idx].ldts, LDT_SIZE * sizeof(ldt_descriptor_t) - 1);	

	//初始化段寄存器
	//cs
	procs[idx].regs.cs = USER_CS_INDEX | TI_1 | RPL_3;

	//DS
	procs[idx].regs.ds = USER_DA_INDEX | TI_1 | RPL_3;
	
	//ES
	procs[idx].regs.es = USER_DA_INDEX | TI_1 | RPL_3;
	
	//FS
	procs[idx].regs.fs = USER_DA_INDEX | TI_1 | RPL_3;
	
	//SS
	procs[idx].regs.ss = USER_DA_INDEX | TI_1 | RPL_3;
	
	//GS
	procs[idx].regs.gs = KERNEL_VIDEO_INDEX   | RPL_3;

	procs[idx].regs.eip = (uint32_t) fun;	
	procs[idx].regs.esp = (uint32_t) proc_stack[idx] + PROC_STACK_SIZE - 1;	
	
	//用户进程关闭IO权限
	procs[idx].regs.eflags = 0x202;
	procs[idx].pid = idx;
	
	//调度算法
	procs[idx].priority = 1;
	procs[idx].waiting = 0;	
	
	return idx;
}

//表头
static char table_header[10][10] = {"TID", "PID", "LEVEL", "STATUS", "NAME"}; 

void show_proc_table()
{
	int i, j, line_width = 10;
	for(i = 0; i < 5; i++)
	{
		printc(c_black, c_light_magenta, "%s", table_header[i]);
		for(j = 0; j < line_width - strlen(table_header[i]); j++)
			printf(" ");
	}
	printf("\n");
	
	for(i = 0; i < PROC_MAX_NUM; i++)
	{
		//一个进程代表一个进程块	
		if(procs[i].take_up == 1){
			
			printc(c_black, c_light_red, "%-10d%-10d", procs[i].tid, procs[i].pid);
			
			switch(procs[i].level)
			{	
				case KERNEL_PROC: 
					printc(c_black, c_red, "KERENL    ");	
					break;
				case SYS_PROC:
					printc(c_black, c_cyan, "SYS       ");
					break;
				case USER_PROC:
					printc(c_black, c_magenta, "USER      ");
					break;
				default:
					printf(c_black, c_light_red, "ERROR     ");
					break;	
			}
		
			switch(procs[i].status)
			{	
				case PROC_EXIT: 
					printc(c_black, c_red, "STOP      ");	
					break;
				case PROC_RUNNING:
					printc(c_black, c_green, "RUNNING   ");
					break;
				case PROC_BLOCK:
					printc(c_black, c_light_brown, "BLOCK     ");
					break;
				default:
					printf("ERROR     ");
					break;	
			}
		
			printc(c_black, c_cyan, "%s", procs[i].name);
			printf("\n");
		
		}
	}
}

static void debug_proc(proc_t *proc){

	printf("[DS/ES/FS/GS] is  0x%04X 0x%04X 0x%04X 0x%04X\n", proc -> regs.ds, proc -> regs.es, proc -> regs.fs, proc -> regs.gs);
	printf("[CS:EIP] is 0x%04X : 0x%X\n", proc -> regs.cs, proc -> regs.eip);
	printf("[SS:ESP] is 0x%04X : 0x%X\n", proc -> regs.ss, proc -> regs.esp);

}
