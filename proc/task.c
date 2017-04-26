#include "task.h"
#include "pmm.h"
#include "vmm.h"
#include "sched.h"
#include "string.h"
#include "heap.h"

pid_t now_pid;

//创建内核线程
int32_t kernel_thread(int (*fn)(void *), void *arg){

	//申请一个内核栈
	//这个是每个进程的私有空间
	struct task_struct *new_task = (struct task_struct *)kmalloc(STACK_SIZE);

	bzero(new_task, sizeof(struct task_struct));

	//可调度状态
	new_task -> state = TASK_RUNNABLE;

	//	
	//new_task -> stack = current;
	
	//进程ID
	new_task -> pid = now_pid ++;
	
	//映射表
	new_task -> mm = NULL;

	//栈顶	
	uint32_t *stack_top = (uint32_t *)((uint32_t)new_task + STACK_SIZE);

	*(--stack_top) = (uint32_t) arg;			//参数
	*(--stack_top) = (uint32_t) kthread_exit;	//退出
	*(--stack_top) = (uint32_t) fn;				//函数入口
	
	//内核栈
	new_task -> context.esp = (uint32_t)new_task + STACK_SIZE - sizeof(uint32_t) * 3;
		
	//
	new_task -> context.eflags = 0x200;
	new_task -> next = running_proc_head;
	
	struct task_struct *tail = running_proc_head;
	
	while(tail -> next != running_proc_head){
		tail = tail -> next;
	}

	tail -> next = new_task;
	
	return new_task -> pid;
}

void kthread_exit(){
	
}
