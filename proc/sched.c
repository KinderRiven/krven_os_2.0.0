#include "sched.h"
#include "mm.h"
#include "hd.h"
#include "tty.h"
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
void proc_line_schedule()
{
	while(1){
		
		//当前的进程
		current_proc = &(procs[proc_ptr]);
		
		//往后推一个
		proc_ptr = (proc_ptr + 1) % PROC_MAX_NUM;
	
		//不调度发生阻塞的程序
		//不调度空的进程块
		if(current_proc -> msg_block != 1 && current_proc -> take_up != 0){
			current_proc -> status = PROC_RUNNING;
			break;
		}
		
		//如果被阻塞了
		else if(current_proc -> msg_block == 1){
			current_proc -> status = PROC_BLOCK;
		}
	}
}

void proc_priority_schedule()
{
	int i;
	int mark;
	double max_level = 0;

	//线性检测
	for(i = 0; i < PROC_MAX_NUM; i++)
	{
		//可调度非空进程块
		if(procs[i].msg_block != 1 && procs[i].take_up != 0)
		{
			procs[i].status = PROC_RUNNING;
			procs[i].waiting++;
			double tmp = 1.0 * procs[i].waiting / procs[i].priority; 
			if(tmp > max_level)
			{
				max_level = tmp;
				mark = i;
			}
		}
		else
		{
			procs[i].status = PROC_BLOCK;
		}
	}

	//选择最终的调度任务
	current_proc = &procs[mark];
	procs[mark].waiting = 0;
}

//任务唤醒
void task_line_schedule()
{

	int i = 0;

	//扫一次任务表
	for(i = 0; i < task_size; i++)
	{
		//如果任务正在等待
		if(task_table[i].status == TASK_WAITING)
		{
			//如果是初始化进程
			if(strcmp(task_table[i].name, "INIT") == 0)
			{
				task_table[i].pid = new_init_proc((uint32_t) task_table[i].entry);	
			}
			//如果是非初始化进程
			else{

				//创建系统进程	
				if(task_table[i].level == SYS_TASK)
				{
					//为proc和task建立双向映射
					task_table[i].pid = new_task_proc((uint32_t) task_table[i].entry);
					
					if(strcmp(task_table[i].name, "HD") == 0)
					{
						hd_pid = task_table[i].pid;
					}
					else if(strcmp(task_table[i].name, "FS") == 0)
					{
						fs_pid = task_table[i].pid;
					}
					else if(strcmp(task_table[i].name, "TTY") == 0)
					{
						tty_pid = task_table[i].pid;	
					}
					else if(strcmp(task_table[i].name, "MM") == 0)
					{
						mm_pid = task_table[i].pid;
					}
				}
				//创建用户进程
				else if(task_table[i].level == USER_TASK)
				{
					task_table[i].pid = new_user_proc((uint32_t) task_table[i].entry);
				}
			}

			if(task_table[i].pid != -1)
			{	
				//转入调度
				task_table[i].status = TASK_RUNNING;
				task_table[i].tid = i;			
	
				//复制一些内容，名称、优先级等信息
				strcpy(procs[task_table[i].pid].name, task_table[i].name);	
				procs[task_table[i].pid].tid = task_table[i].tid;	
				procs[task_table[i].pid].level = task_table[i].level;
			}
		}
	}	
}

