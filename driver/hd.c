#include "hd.h"
#include "stdio.h"
#include "sys.h"
#include "msg.h"
#include "idt.h"

pid_t hd_pid;

//硬盘中断发生的时候进行的处理
void hd_handler()
{
	
}

//初始化硬盘驱动程序
void init_hd()
{
	register_interrupt_handler(IRQ14, hd_handler);	
}

void hd_task()
{

	msg_t msg;
	
	//初始化硬盘驱动程序
	init_hd();

	//获得硬盘驱动程序pid
	get_proc_pid(&hd_pid);	

	while(1)
	{
		recv_message(ANY, hd_pid, &msg);
	}
}
