#include "fs.h"
#include "const.h"
#include "string.h"
#include "stdio.h"
#include "hd.h"
#include "msg.h"
#include "sys.h"

//建立文件系统
void mkfs()
{
	msg_t msg;
	pid_t pid;

	hd_part_t hd_part;
	get_proc_pid(&pid);
	
	//打开设别指令
	msg.type = MSG_DEV_OPEN;
	//次设备号为0
	msg.device = 0;

	//打开设备，建立分区表
	send_message(pid, hd_pid, &msg);	

	//获取分区信息
	msg.type = MSG_DEV_INFO;
	msg.device = PART_TYPE_KOS;
	msg.buf = (uint8_t *) &hd_part;

	//获取分区信息
	send_message(pid, hd_pid, &msg);
	
	/*****************************
   	**                          **
	** 		Super Block	        **
    **                          **
	*****************************/

	/////////////////////////////
	//		  inodes		   //
	/////////////////////////////
	//      sector map		   //
	/////////////////////////////
	//		inode map		   //	
	/////////////////////////////
	//		Super Block		   //
	/////////////////////////////
	//		Boot Sector		   //
	/////////////////////////////

	super_block_t super_block;
	
	//标记为KOS操作系统
	super_block.magic = KOS_FS_LV1;
	
	//inode的数量最大为4096个
	super_block.inode_num = INODE_MAX_NUM;

	//inode所占扇区总数 4096 * sizeof(node_t) / 512
	super_block.inode_sects = super_block.inode_num * sizeof(inode_t) / SECTOR_SIZE;
	
	//扇区总数从分区表获得
	super_block.sect_num = hd_part.sector_num;	

	//用于存放inode-map的扇区数量
	super_block.imap_sects = 1;
		
	//用与存放sector-map的扇区数量
	super_block.smap_sects = super_block.sect_num / INODE_MAX_NUM + 1;

	//第一个数据扇区
	super_block.first_data_sect = (1 + 1 + 
			super_block.imap_sects + super_block.smap_sects + super_block.inode_sects);
	
	while(1){}

}
