#ifndef INCLUDE_CONST_H_
#define INCLUDE_CONST_H_


//扇区大小 512 字节
#define SECTOR_SIZE		512

//扇区位数 512 × 8 位
#define SECTOR_BITS		(SECTOR_SIZE * 8)


/*********************************
 *                               *
 *	 Here is gdt descriptor      *
 *								 *
 *********************************/

#define KERNEL_CS_INDEX 	0x08	// 内核代码段索引
#define KERNEL_DA_INDEX 	0x10	// 内核数据段索引
#define KERNEL_VIDEO_INDEX	0x18	// 屏幕映射段

//系统描述符
#define TYPE_LDT 		0x82		// LDT 局部描述符
#define TYPE_386TSS		0x89		// 386 任务门描述符
#define TYPE_386CGate	0x8C		// 386 调用门描述符
#define TYPE_386IGate	0x8E		// 386 中断门描述符
#define TYPE_386TGate	0x8F		// 386 陷阱门描述符

//普通段描述符
#define TYPE_RW		0x92		//读写
#define TYPE_C		0x98		//执行
#define TYPE_RC		0x9A		//执行读


//段的DPL权限
#define DPL_0	0x00			//DPL
#define DPL_1	0x20
#define DPL_2	0x40
#define DPL_3	0x60

//偏移量(一般为1)
#define DB_16	0x00
#define DB_32	0x40

//G颗粒读
#define G_1		0x00
#define G_4096	0x80 

/*********************************
 *                               *
 *	 Here is ldt descriptor      *
 *								 *
 *********************************/

#define USER_CS_INDEX 0x00		//用户代码段索引
#define USER_DA_INDEX 0x08		//用户数据段索引

/*********************************
 *				                 *
 *			Selector             *
 *                               *
 *********************************/

#define RPL_0	0x00
#define RPL_1	0x01
#define RPL_2	0x02
#define RPL_3	0x03

#define TI_1	0x04

#endif
