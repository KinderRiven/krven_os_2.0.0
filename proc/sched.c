#include "sched.h"
#include "hd.h"
#include "fs.h"
#include "string.h"
#include "proc.h"
#include "stdio.h"
#include "task.h"

proc_t* current_proc;
static int proc_ptr = 0;
int k_reenter;

extern void first_proc_start();

//初始化调度进程
void init_schedule()
{
	k_reenter = -1;
	current_proc = &(procs[0]);
	first_proc_start();
}

//进行调度主体
void proc_schedule()
{
	while(1){
		
		//当前的进程
		current_proc = &(procs[proc_ptr]);
		
		//往后推一个
		proc_ptr = (proc_ptr + 1) % PROC_MAX_NUM;
	
		//不调度发生阻塞的程序
		//不调度空的进程块
		if(current_proc -> msg_block != 1 && current_proc -> take_up != 0){
			task_table[current_proc -> tid].status = 1;
			break;
		}
		else{
			task_table[current_proc -> tid].status = 2;
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
					//如果是初始化进程
			if(strcmp(task_table[i].name, "INIT") == 0)
			{
				task_table[i].status = TASK_RUNNING;
				task_table[i].pid = new_init_proc((uint32_t) task_table[i].entry);	
			}
			//如果是非初始化进程
			else{

				//转入调度
				task_table[i].status = TASK_RUNNING;
				
				//创建系统进程	
				if(task_table[i].level == SYS_TASK)
				{
					//为proc和task建立双向映射
					task_table[i].pid = new_task_proc((uint32_t) task_table[i].entry);
					procs[task_table[i].pid].tid = task_table[i].tid;
					
					if(strcmp(task_table[i].name, "HD") == 0)
					{
						hd_pid = task_table[i].pid;
					}
					else if(strcmp(task_table[i].name, "FS") == 0)
					{
						fs_pid = task_table[i].pid;
					}
	
				}
				//创建用户进程
				else if(task_table[i].level == USER_TASK)
				{
					task_table[i].pid = new_user_proc((uint32_t) task_table[i].entry);
					procs[task_table[i].pid].tid = task_table[i].tid;
				}
			}
		}
	}	
}

