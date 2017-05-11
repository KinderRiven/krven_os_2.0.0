#ifndef INCLUDE_HD_H_
#define INCLUDE_HD_H_

#include "types.h"

//Device寄存器的宏定义
//0 ~ 3 LBA 模式下的24-27位
//		CHS 模式下的磁头号
//4 DRV 0 主设备
//		1 从设备
//5 is always 1
//6	L	0 CHS模式
//		1 LBA模式
//7	is always 1	
#define MAKE_DEVICE_REG(lba, drv, lba_highest) ((lba << 6) | 			\
												(drv << 4) | 			\
												(lba_highest & 0xF) | 0xA0)

#define REG_DATA		0x1F0

#define REG_FEATURE		0x1F1
#define REG_ERROR		REG_FEATURE

#define REG_COUNT		0x1F2
#define REG_LAB_LOW		0x1F3
#define REG_LAB_MID		0x1F4
#define REG_LAB_HIGH	0x1F5	
#define REG_DEVICE		0x1F6

#define REG_CMD			0x1F7
#define REG_STATUS		REG_CMD

#define REG_DEV_CTRL	0x3F6

//命令寄存器组成
typedef
struct hd_cmd
{							//Primary (主设备)
	uint8_t data;			//1F0h
	uint8_t feature;		//1F1h
	uint8_t count;			//1F2h
	uint8_t lba_low;		//1F3h
	uint8_t lba_mid;		//1F4h
	uint8_t lba_high;		//1F5h
	uint8_t device;			//1F6h
	uint8_t command;		//1F7h

} hd_cmd;

//硬盘驱动进程的id号
extern pid_t hd_pid;

//初始化硬盘驱动进程
void init_hd();

//硬盘驱动进程入口
void hd_task();

//
void hd_identify(uint8_t drive);

#endif
