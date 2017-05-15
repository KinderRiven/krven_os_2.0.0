#ifndef INCLUDE_HD_H_
#define INCLUDE_HD_H_

#include "types.h"

//分区类型
//空
#define PART_TYPE_EMPTY 	0x00
//FAT12
#define PART_TYPE_FAT12		0x01
//EXTRA
#define PART_TYPE_EXTRA		0x05
//LINUX
#define PART_TYPE_LINUX		0x83
//KOS
#define PART_TYPE_KOS		0x99



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

//最多有几个分区
#define MAX_HD_PART		10

//分区表的位置
#define PART_TABLE_LOCAL 0x1BE

//分区表的大小
#define PART_TABLE_SIZE	 4

//分区表项的大小

//ATA操作参数
#define ATA_IDENTIFY	0xEC
#define ATA_READ		0x20
#define ATA_WRITE		0x30

//硬盘寄存器常量
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
struct hd_cmd_t
{							//Primary (主设备)
	uint8_t data;			//1F0h
	uint8_t feature;		//1F1h
	uint8_t count;			//1F2h
	uint8_t lba_low;		//1F3h
	uint8_t lba_mid;		//1F4h
	uint8_t lba_high;		//1F5h
	uint8_t device;			//1F6h
	uint8_t command;		//1F7h

} hd_cmd_t;



typedef
struct hd_part_t
{
	uint8_t status;			//80h  = 可引导
							//00h  = 不可引导
							//其他 = 不合法

	uint8_t start_head;		//起始磁头号
	uint8_t start_sector;	//起始扇区号（高2位为柱面号8,9）
	uint8_t start_cylinder;	//起始磁柱
	
	uint8_t type;			//分区类型
	
	uint8_t end_head;		//结束磁头号（高2位位柱面号8,9）
	uint8_t end_sector;		//结束扇区号
	uint8_t end_cylinder; 	//结束磁柱

	uint32_t start_lba;		//起始lba号
	uint32_t sector_num;	//扇区总数

} hd_part_t;

//硬盘驱动进程的id号
extern pid_t hd_pid;

//硬盘驱动进程入口
void hd_task();

//打印分区信息
void hd_print_part();
#endif
