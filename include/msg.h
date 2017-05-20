#ifndef INCLUDE_MSG_H_
#define INCLUDE_MSG_H_

#include "types.h"

#define MESSAGE_SIZE 	128

//硬盘驱动初始化
//[debug] 决定是否打开debug
#define MSG_DEV_OPEN 	0x01

//硬盘读
//[pos] 选择的扇区号，这里指的LBA模式下的寻址
//[device] 设备号 
//[buf] 指向一段需要读的缓冲区域
#define MSG_DEV_READ 	0x02

//硬盘写
//[pos] 选择的扇区号，这里指的LBA模式下的寻址
//[device] 设备号
//[buf] 指向一段需要写的缓冲区域 
#define MSG_DEV_WRITE	0x03

//获取分区信息
//[device] 一个需要选择的文件系统的magic
#define MSG_DEV_INFO	0x04

//硬盘模块告诉虚拟文件系统准备就绪
#define MSG_HD_READY	0x05

//获得文件夹内容
//[inode] 想获得的文件夹的inode号
#define MSG_FS_DIR		0x10

//建立文件夹
//[inode] 想在哪个文件夹下建立文件的inode号
//[buf] 指向新建文件夹的名称
//[mode] 文件夹的权限
#define MSG_FS_MKDIR	0x11

//建立文件
//[inode] 想在哪个文件夹下建立文件的inode号
//[buf] 指向新建文件的名称
//[mode] 文件的权限
#define MSG_FS_MKFILE 	0x12

//fork进程
#define MSG_FORK		0x20

typedef
struct msg_t{
	
	int type;		//消息类型
	int device;		//设备选择
	int debug;		//debug信息是否打印
	int inode;		//用于虚拟文件系统操作参数
	int mode;		//权限
	pid_t pid;		//消息主人
	
	uint8_t *buf;	//指向一段内存区域的指针
	int cnt;		//缓冲区大小
	uint32_t pos;	//用于硬盘读写位置定位

}msg_t;


#endif
