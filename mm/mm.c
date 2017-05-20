#include "stdio.h"
#include "pmm.h"
#include "vmm.h"
#include "mm.h"
#include "msg.h"
#include "proc.h"

pid_t mm_pid;

//初始化
static void mm_init();

static void mm_init()
{
	//获得mm任务的进程号
	get_proc_pid(&mm_pid);
}

void mm_task()
{
	msg_t msg;	
		
	mm_init();
	
	while(1)
	{
		recv_message(ANY, mm_pid, &msg);
		switch(msg.type)
		{
			case MSG_FORK:
				do_fork(&msg);
				break;
			default:
				printc(c_black, c_light_red, "MM TASK ERROR!\n");
		}
	}
}

void do_fork()
{
		
}
