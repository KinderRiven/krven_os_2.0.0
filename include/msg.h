#ifndef INCLUDE_MSG_H_
#define INCLUDE_MSG_H_

#include "types.h"

#define MESSAGE_SIZE 	128

//硬盘驱动初始化
#define MSG_DEV_OPEN 	0x01

//硬盘读
#define MSG_DEV_READ 	0x02

//硬盘写
#define MSG_DEV_WRITE	0x03

//获取硬盘信息
#define MSG_DEV_INFO	0x04

//获得文件夹内容
#define MSG_FS_DIR		0x10

typedef
struct msg_t{
	
	int type;		//消息类型
	int device;		//设备选择
	int debug;		//debug信息是否打印
	int inode;		//用于虚拟文件系统操作参数
	pid_t pid;		//消息主人
	
	uint8_t *buf;	//指向一段内存区域的指针
	int cnt;		//缓冲区大小
	uint32_t pos;	//用于硬盘读写位置定位

}msg_t;


#endif
