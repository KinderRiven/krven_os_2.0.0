#include "string.h"
#include "sys.h"
#include "stdio.h"
#include "pmm.h"
#include "vmm.h"
#include "mm.h"
#include "msg.h"
#include "proc.h"

pid_t mm_pid;

//初始化
static void mm_init();

//开始fork
static void do_fork();

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

void do_fork(msg_t *msg)
{
	//获得一个空的进程块
	//int idx = get_empty_proc_block();	
	
	//获得父进程的内存块
	//int fpid = msg -> pid;
	
	//原封不动的拷贝内容
	//memcpy((uint8_t *)&procs[idx], (uint8_t *)&procs[fpid], sizeof(procs[fpid]));

}
