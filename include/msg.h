#ifndef INCLUDE_MSG_H_
#define INCLUDE_MSG_H_

#include "types.h"

#define MESSAGE_SIZE 1024
#define ANY -1

typedef
struct msg_t{
	
	char send_msg[MESSAGE_SIZE];		//这里是发送的消息
	char recv_msg[MESSAGE_SIZE];		//这里是接收的消息

	pid_t send_to;		//发送消息到哪里
	pid_t recv_from;	//接收消息从哪里
	
	int	msg_block;		//消息阻塞

}msg_t;

void msg_send(uint32_t from, uint32_t to);

void msg_receive(uint32_t from, uint32_t to);

#endif
