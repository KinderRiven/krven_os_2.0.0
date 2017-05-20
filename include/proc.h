#ifndef INCLUDE_PROC_H_
#define INCLUDE_PROC_H_

#include "types.h"
#include "ldt.h"
#include "msg.h"

#define LDT_SIZE 2
#define PROC_STACK_SIZE 8192
#define PROC_MAX_NUM 10
#define MAX_PROC_NAME 24

#define ANY -1
#define PROC_DEBUG 0
#define PROC_TABLE_FULL -1

#define HD_INTERRUPT 0xFFFF

#define NO_INT 		0
#define HAS_INT		1
#define WAIT_INT  	2

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
	
	//call返回
	uint32_t retaddr;

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
enum proc_level
{
	KERNEL_PROC = 0,
	SYS_PROC = 1,
	USER_PROC = 3

} proc_level;

typedef
enum proc_status
{
	PROC_EXIT = 0,
	PROC_RUNNING = 1,
	PROC_BLOCK = 2

} proc_status;

typedef
struct proc_t{
	
	proc_regs_t regs;					//寄存器保存的值 (18 * 4 = 72字节)
	uint16_t ldtr;						//ldtr寄存器的值 (2字节)
	ldt_descriptor_t ldts[LDT_SIZE];	//2个
	
	pid_t pid;							//进程id号
	tid_t tid;							//对应的任务号
	char name[MAX_PROC_NAME];			//进程名称

										//以下是进程的消息处理内容
	msg_t *msg;							//消息结构体

	int int_status;						//0无中断 1有中断 2等待中断
	int interrupt_id;					//中断号

	pid_t send_to;						//想发送消息去哪里
	pid_t recv_from;					//想从哪里接收到消息

	int	msg_block;						//是否有消息阻塞	
	struct proc_t *msg_head;			//消息接收
	struct proc_t *msg_next;			//链表指针	

	int take_up;						//进程表是否为空
	proc_level level;					//进程等级
	proc_status status;					//进程状态
	
}proc_t;

//进程表
extern proc_t procs[PROC_MAX_NUM];

//进程栈
extern char proc_stack[PROC_MAX_NUM][PROC_STACK_SIZE];

//正在运行的进程数量
extern int proc_count;

//现实当前内存中运行的进程表
void show_proc_table();

//初始化进程表
void init_proc_table();

//获得一个新的进程块
int get_empty_proc_block();

//新建一个权限级为1的进程
pid_t new_task_proc(uint32_t fun);

//新建一个权限级为3的进程
pid_t new_user_proc(uint32_t fun);

//新建一个初始化进程(权限级为3)
pid_t new_init_proc(uint32_t fun);

//DEBUG
void proc_print_ldt(int num);

//进程通信函数

//消息发送
void msg_send(proc_t *proc_from, pid_t send_to, msg_t *msg);
//消息接收（阻塞）
void msg_receive(proc_t *proc_to, pid_t recv_from, msg_t *msg);

//产生中断
void msg_send_interrupt(pid_t pid, int interrupt_id);

//接收中断（阻塞）
void msg_recv_interrupt(pid_t pid, int interrupt_id);
#endif
