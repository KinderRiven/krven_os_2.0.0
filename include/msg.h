#ifndef INCLUDE_MSG_H_
#define INCLUDE_MSG_H_

#include "types.h"

#define MESSAGE_SIZE 	128
#define MSG_DEV_OPEN 	0x01
#define MSG_DEV_READ 	0x02
#define MSG_DEV_WRITE	0x03
#define MSG_DEV_INFO	0x04

#define MSG_BUF_SIZE	128

typedef
struct msg_t{
	
	int type;		//消息类型
	int device;		//设备选择
	
	int debug;		//debug信息是否打印
	//int lock;		//消息锁
	
	pid_t pid;		//消息主人
	
	uint8_t *buf;	//指向一段内存区域的指针
	int cnt;		//缓冲区大小
	uint32_t pos;	//用于硬盘读写位置定位

}msg_t;


#endif
