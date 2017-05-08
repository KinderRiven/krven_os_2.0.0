#include "msg.h"
#include "proc.h"
#include "types.h"
#include "string.h"

static void insert_msg_queue(proc_t *proc_from, proc_t *proc_to);

//发送消息的函数
void msg_send(uint32_t pid_from, uint32_t pid_to)
{
	proc_t *proc_from = &procs[pid_from];
	proc_t *proc_to = &procs[pid_to];

	//如果正在等待接收消息
	if(proc_to -> msg.msg_block && 
		(proc_to -> msg.recv_from == pid_from || proc_to -> msg.recv_from == ANY))
	{
		memcpy(proc_to -> msg.recv_msg, proc_from -> msg.send_msg, MESSAGE_SIZE); 
		//解除阻塞
		proc_to -> msg.msg_block = 0;					
	}
	//否则加入消息队列，并对自己产生一个阻塞
	else
	{
		//阻塞自己	
		proc_from -> msg.msg_block = 1;	
		proc_from -> msg.send_to = pid_to;
		//插入到消息队列
		insert_msg_queue(proc_from, proc_to);				
	}	
}

//接收消息的函数
void msg_reveice(uint32_t pid_from, uint32_t pid_to)
{
	proc_t *proc_to = &procs[pid_to];

	//先检查消息队列查看是否有像接收的消息
	if(proc_to -> msg_head != NULL)
	{
		proc_t *tmp1 = proc_to -> msg_head;
		proc_t *tmp2;
		while(tmp1 != NULL)
		{
			//找到可以接收的消息
			if(pid_from == ANY || pid_from == tmp1 -> pid)
			{
				//消息传递
				memcpy(proc_to -> msg.recv_msg, tmp1 -> msg.send_msg, MESSAGE_SIZE);
				//修改链表
				if(tmp1 == proc_to -> msg_head)
				{
					proc_to -> msg_head = tmp1 -> msg_next;		
				}	
				else
				{
					tmp2 -> msg_next = tmp1 -> msg_next;
				}
			}
			tmp2 = tmp1;
			tmp1 = tmp1 -> msg_next;
		}
	}		
	//如果没有消息可以接收,那么对自己产生一个阻塞
	proc_to -> msg.msg_block = 1;
	proc_to -> msg.recv_from = pid_from;
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
