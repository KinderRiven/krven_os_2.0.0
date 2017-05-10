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

extern pid_t hd_pid;

void init_hd();

void hd_task();

#endif
