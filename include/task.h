#ifndef INCLUDE_TASK_H_
#define INCLUDE_TASK_H_

#include "types.h"
#include "pmm.h"
#include "vmm.h"

typedef
enum task_state{

	TASK_UNINIT = 0,
	TASK_SLEEPING = 1,
	TASK_RUNNABLE = 2,
	TASK_ZOMBIE = 3,

} task_state;


//上下文
struct context{
	
	uint32_t esp;
	uint32_t ebp;
	uint32_t ebx;
	uint32_t esi;
	uint32_t edi;
	uint32_t eflags;
	
};

//进程内存地址
struct mm_struct{
	pgd_t *pgd_dir;
};

//
struct task_struct{
	
	volatile task_state state;	//进程状态
	pid_t pid;					//进程标识符
	void *stack;				//进程内核栈地址
	struct mm_struct *mm;		//进程内存地址映像
	struct context context;		//进程切换需要的上下文信息
	struct task_struct *next;	//链表指针
};

extern pid_t now_pid;

//线程创建
int32_t kernel_thread(int (*fn)(void *), void *arg);

//线程退出
void kthread_exit();

#endif
