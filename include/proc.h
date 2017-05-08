#ifndef INCLUDE_PROC_H_
#define INCLUDE_PROC_H_

#include "types.h"
#include "ldt.h"

#define LDT_SIZE 2
#define PROC_STACK_SIZE 1024
#define PROC_MAX_NUM 10

typedef
struct proc_regs_t{

	//需要手动更新的段寄存器
	uint32_t gs;
	uint32_t fs;
	uint32_t es;
	uint32_t ds;
	
	//popad指令压入的段寄存器
	uint32_t edi;
	uint32_t esi;
	uint32_t ebp;
	uint32_t kernel_esp;
	uint32_t ebx;
	uint32_t edx;
	uint32_t ecx;
	uint32_t eax;
	
	//错误号、中断
	uint32_t int_no;	
	uint32_t err_code;	
	
	//发生中断压入的内容
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

//进程表
extern proc_t procs[PROC_MAX_NUM];

//进程栈
extern char proc_stack[PROC_MAX_NUM][PROC_STACK_SIZE];

//正在运行的进程数量
extern int proc_num;

//新建一个权限级为1的进程
pid_t new_task_proc(uint32_t fun);

//新疆一个权限级为3的进程
pid_t new_user_proc(uint32_t fun);

void restart();

void proc_start();

#endif
