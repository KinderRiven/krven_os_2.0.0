#include "msg.h"
#include "proc.h"
#include "types.h"
#include "string.h"
#include "stdio.h"

static void insert_msg_queue(proc_t *proc_from, proc_t *proc_to);
static void msg_debug(proc_t *proc);
static void msg_copy(msg_t *msg_a, msg_t *msg_b);

static void msg_copy(msg_t *msg_to, msg_t *msg_from)
{
	msg_to -> int_no = msg_from -> int_no;
	
	int i;
	for(i = 0; i < MESSAGE_SIZE; i++)
	{
		msg_to -> message[i] = msg_from -> message[i];
	}

}


static void msg_debug(proc_t *proc)
{

}

//发送消息的函数
void msg_send(proc_t* proc_from, pid_t send_to, msg_t *msg)
{
	proc_t *proc_to = &procs[send_to];

	//如果正在等待接收消息
	if(proc_to -> msg_block && 
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
				//消息传递
				msg_copy(msg, tmp1 -> msg);
				
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
