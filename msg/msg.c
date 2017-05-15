#include "msg.h"
#include "proc.h"
#include "types.h"
#include "string.h"
#include "stdio.h"
#include "hd.h"

static void insert_msg_queue(proc_t *proc_from, proc_t *proc_to);
static void msg_copy(msg_t *msg_a, msg_t *msg_b);


//拷贝函数
static void msg_copy(msg_t *msg_to, msg_t *msg_from)
{
	//msg_to -> type 	 = msg_from -> type;
	//msg_to -> device = msg_from -> device;
	memcpy((uint8_t *) msg_to, (uint8_t *) msg_from, sizeof(msg_t));
}

//发送一个中断
void msg_send_interrupt(pid_t pid, int interrupt_id)
{

	proc_t *proc = &procs[pid];

	//没中断,将中断加入队列
	if(proc -> int_status == NO_INT)
	{
		proc -> int_status = HAS_INT;
		proc -> interrupt_id = interrupt_id;	
	}

	//有中断,不响应新的中断内容
	else if(proc -> int_status == HAS_INT);

	//等待中断,取消阻塞
	else if(proc -> int_status == WAIT_INT && interrupt_id == proc -> interrupt_id)
	{
		proc -> int_status = NO_INT;
		proc -> msg_block = 0;
	}
}

//等待接收一个中断
void msg_recv_interrupt(pid_t pid, int interrupt_id)
{
	
	proc_t *proc = &procs[pid];
	
	//没有等待的中断，申明需要等待的中断
	if(proc -> int_status == NO_INT)
	{
		proc -> int_status = 2;
		proc -> interrupt_id = interrupt_id;
		proc -> msg_block = 1;
	}

	//已经有一个中断在了,解除阻塞
	else if(proc -> int_status == HAS_INT && proc -> interrupt_id == interrupt_id)
	{
		proc -> int_status = 0;	
	}
	//已经有一个中断了，还出现这个问题，那就是出错了
	else if(proc -> int_status == WAIT_INT)
	{
		printk("Error!\n");
	}
	
}

//发送消息的函数
void msg_send(proc_t* proc_from, pid_t send_to, msg_t *msg)
{
	proc_t *proc_to = &procs[send_to];

	//如果正在等待接收消息
	//如果发送的对象没有等待中断，并且已经阻塞的
	if(proc_to->int_status != WAIT_INT && proc_to -> msg_block && 
		(proc_to -> recv_from == proc_from -> pid || proc_to -> recv_from == ANY))
	{
		msg_copy(proc_to -> msg, msg);
		//解除阻塞
		proc_to -> msg_block = 0;					
	}
	//否则加入消息队列，并对自己产生一个阻塞
	else
	{
		//阻塞自己,并保存消息
		proc_from -> msg_block = 1;	
		proc_from -> send_to = send_to;
		proc_from -> msg = msg;
		
		//插入到消息队列
		insert_msg_queue(proc_from, proc_to);				
	}
}

//接收消息的函数
void msg_receive(proc_t *proc_to, pid_t recv_from, msg_t *msg)
{

	if(proc_to -> msg_block == 1)
	{
		printk("Error!\n");
	}	

	//先检查消息队列查看是否有像接收的消息
	if(proc_to -> msg_head != NULL)
	{
		proc_t *tmp1 = proc_to -> msg_head;
		proc_t *tmp2;
		while(tmp1 != NULL)
		{
			//找到可以接收的消息
			if(recv_from == ANY || recv_from == tmp1 -> pid)
			{
				//消息传递, 解除阻塞
				msg_copy(msg, tmp1 -> msg);
				tmp1 -> msg_block = 0;				

				//修改链表
				if(tmp1 == proc_to -> msg_head)
				{
					proc_to -> msg_head = tmp1 -> msg_next;		
				}	
				else
				{
					tmp2 -> msg_next = tmp1 -> msg_next;
				}
				return;
			}
			tmp2 = tmp1;
			tmp1 = tmp1 -> msg_next;
		}
	}		
	//如果没有消息可以接收,那么对自己产生一个阻塞
	proc_to -> msg_block = 1;
	proc_to -> recv_from = recv_from;
	proc_to -> msg = msg;
}

static void insert_msg_queue(proc_t *proc_from, proc_t *proc_to)
{
	if(proc_to -> msg_head == NULL)
	{
		proc_to -> msg_head = proc_from;
		proc_from -> msg_next = NULL;
	}
	else
	{
		proc_t *tmp1 = proc_to -> msg_head;
		proc_t *tmp2;
		while(tmp1 != NULL)
		{
			tmp2 = tmp1;
			tmp1 = tmp1 -> msg_next;
		}
		tmp2 -> msg_next = proc_from;
		proc_from -> msg_next = NULL;
	}
}
