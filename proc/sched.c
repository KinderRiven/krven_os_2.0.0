#include "sched.h"
#include "proc.h"
#include "stdio.h"
#include "task.h"

proc_t* current_proc;
static int num = 0;

extern void first_proc_start();

//初始化调度进程
void init_schedule()
{
	current_proc = &(procs[0]);
	first_proc_start();
}

//进行调度主体
void proc_schedule()
{
	//printf("\nSchedule : %d/%d\n", num, proc_num);
	while(1){
		
		current_proc = &(procs[num % proc_num]);
		num = (num + 1) % proc_num;
		
		//不调度发生阻塞的程序
		if(current_proc -> msg_block != 1){
			break;
		}
		else{
			//printk("[%d]", current_proc -> pid);
		}
	}
}

//任务唤醒
void task_schedule()
{

	int i = 0;
	for(i = 0; i < task_size; i++)
	{
		if(task_table[i].status == TASK_WAITING)
		{
			task_table[i].status = TASK_RUNNING;
			
			//创建system进程
			if(task_table[i].level == SYS_TASK)
			{
				task_table[i].pid = new_task_proc((uint32_t) task_table[i].entry);
			}
			else if(task_table[i].level == USER_TASK)
			{
				task_table[i].pid = new_user_proc((uint32_t) task_table[i].entry);
			}
		}
	}	
}

