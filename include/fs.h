#ifndef INCLUDE_FS_H_
#define INCLUDE_FS_H_

#include "types.h"

#define MAX_FILENAME 24

#define KOS_FS_LV1 0x6666

#define INODE_MAX_NUM 	4096

//一个超级块对应一个文件系统
typedef 
struct super_block_t
{

	uint32_t magic;
	uint32_t inode_num;			//inode节点的数量 4096 = 512 × 8
	uint32_t inode_sects;		//inode所占扇区总数	
	uint32_t sect_num;			//扇区总数
	uint32_t imap_sects;		//inode-map占用的扇区数 1
	uint32_t smap_sects;		//sector-map占用的扇区数
	uint32_t first_data_sect;	//第一块数据扇区
	uint32_t root_inode;		//root对应的inode号
	

} super_block_t;

typedef
struct inode_t
{
	uint32_t mode;			//读写权限
	uint32_t size;			//文件大小
	uint32_t start_sect;	//开始扇区
	uint32_t sect_num;		//所占扇区数	

} inode_t;

typedef
struct dir_entry_t
{
	
	uint32_t inode_id;
	char name[MAX_FILENAME];	
	
} dir_entry_t;


void mkfs();

#endif
