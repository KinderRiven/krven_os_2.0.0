#include "sched.h"
#include "pmm.h"

//可以被调度的队列
struct task_struct *running_proc_head = NULL;

//等待调度的队列
struct task_struct *wait_proc_head = NULL;

//当前任务
struct task_struct *current = NULL;

//初始化调度,把内核主进程加入
void init_sched(){

	current = (struct task_struct*)(kern_stack_top - STACK_SIZE);	

	current -> state = TASK_RUNNABLE;
	current -> pid = now_pid ++;
	//current -> stack = current;
	current -> mm = NULL;

	current -> next = current;
	running_proc_head = current;

}

//调度下一个
void schedule(){

	if(current) {
		change_task_to(current->next);
	}	
}

//修改任务
void change_task_to(struct task_struct *next){

	if(current != next){
		struct task_struct *prev = current;
		current = next;
		switch_to(&(prev -> context), &(current->context));	//
	}
}




