#include "debug.h"
#include "fs.h"
#include "const.h"
#include "string.h"
#include "stdio.h"
#include "hd.h"
#include "msg.h"
#include "sys.h"


//数据缓冲区
static uint8_t mk_buf[FS_BUF_SIZE];

//清理扇区内容
static void clear_sector(int sector_start, int sector_num);

//内存申请
static int inode_alloc_sector(inode_t *);

//获得一个新的节点
static int get_new_inode();

//static uint8_t bits[8] = {
//	
//	1<<0, 
//	1<<0 | 1<<1, 
//	1<<0 | 1<<1 | 1<<2, 
//	1<<0 | 1<<1 | 1<<2 | 1<<3, 
//	1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4, 
//	1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<5, 
//	1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<5 | 1<<6, 
//	1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<5 | 1<<6 | 1<<7
//
//};


////////////////////////////////////////////////////////
//
//4096 * 2 = 8192
//
//start = 0 count = 150
//sector_id = 3 + 0 / 4096 = 3
//sector_num = (150 - 1) / 4096 + 1 = 1
//sector_start = 0 - (0 / 4096) * 4096 = 150
//
//start = 4096 count = 1
//sector_id = 3 + 4096 / 4096 = 4
//sector_num = (1 - 1) / 4096 + 1 = 1
//sector_start = 4096 - (4096 / 4096) * 4096 = 0
//
//start = 8193 count = 10
//sector_id = 3 + 8193 / 4096 = 5
//sector_num = (10 - 1) / 4096 + 1 = 1
//sector_start = 8193 - (8193 / 4096) * 4096 = 1
//
/////////////////////////////////////////////////////////


//update sector_map
//更新扇区的使用情况
int update_sector_map(uint32_t start, uint32_t count)
{
	
	int i, j, k;
	int pos = 0, cnt = count;
	uint8_t *ptr;	

	//在第几个smap扇区
	int sector_id = 3 + start / 4096;

	//需要更新几个扇区
	int sector_num = (count - 1) / 4096 + 1;

	//相对偏移量
	int sector_start = start - (start / 4096) * 4096;

	for(i = 0; i < sector_num; i++)
	{
		fs_read(sector_id, (void *)mk_buf, sizeof(mk_buf));
		ptr = mk_buf;	

		for(j = 0; j < SECTOR_SIZE; j++)
		{
			uint8_t tmp = *ptr;
			for(k = 0; k < 8; k++, pos++)
			{
				if(pos >= sector_start)
				{
					tmp |= (1 << k);	
					cnt--;
				}
				*ptr = tmp;				
				//更新完成	
				if(cnt == 0)
				{
					//fs_print_debug(mk_buf, 32);
					fs_write(sector_id, (void *)mk_buf, sizeof(mk_buf));
					return 1;	
				}
			}
			ptr++;
		}
		sector_id++;	
	}	
	return 0;
}

//为一个inode分配扇区
static int inode_alloc_sector(inode_t *inode)
{
	// inode-map的位置在第三个扇区
	// 0 is boot
	// 1 is super block
	// 2 is imap
	// 3 is smap

	int inode_map_base = 3, i, j, k;
	//计数器
	int num = 0;
	
	//从哪里开始有空位
	int start;
	
	//已经有多少个空位了
	int cnt = 0;
	uint8_t *ptr;

	//找一下空闲的扇区
	for(i = inode_map_base; i < inode_map_base + super_block.smap_sects; i++)
	{
		//printf("%d / %d\n", i - inode_map_base + 1, super_block.smap_sects);

		fs_read(i, (void *)mk_buf, sizeof(mk_buf));
		ptr = mk_buf;	
		
		for(j = 0; j < SECTOR_SIZE; j++){

			//一次找8位			
			uint8_t tmp = *ptr++;

			//printf("%02x\n", tmp);

			for(k = 0; k < 8; k++, num++)
			{
				//说明该位占用了
				if(tmp & (1 << k))
				{
					cnt = 0;
				}
				//该位没有被占用
				else{
					if(cnt == 0)
					{
						start = num;
						cnt = 1;
					}
					else
					{
						cnt++;
					}
				}
				//找到了足够大小的位置
				if(cnt == inode -> sect_num)
				{
					inode -> start_sect = start;
					update_sector_map(start, cnt);
					return 1;
				}
			}
		}	
	}
	return 0;
}

//  0 ~ 15
// 16 ~ 31
// 32 33 34 35
//  0 16 32 48

//写入一个inode数据
// inode_id = 17
// sector_id = 17 / 16 = 1
// sector_offset = (17 - (17 / 16) * 16) * 16 = 16

// inode_id = 35
// sector_id = 35 / 16 = 2
// sector_offset = (35 - (35 / 16) * 16) * 16 = 48 

static int update_inode(int inode_id, inode_t *inode)
{
	//1 + 1 + 1 + smap
	int inode_start_base = 1 + 1 + super_block.imap_sects + super_block.smap_sects;
	
	//512 / 32 = 16
	int pre_sector_inode_num = SECTOR_SIZE / sizeof(inode_t);

	//inode应该在第几扇区读写
	int	sector_id = inode_start_base + inode_id / pre_sector_inode_num;

	//扇区偏移量 	
	int sector_offset = (inode_id - (inode_id / pre_sector_inode_num) * sizeof(inode_t)) * sizeof(inode_t);	

	fs_read(sector_id, (void *)mk_buf, sizeof(mk_buf));
	memcpy(mk_buf + sector_offset, (uint8_t *) inode, sizeof(inode_t));

	fs_write(sector_id, (void *)mk_buf, sizeof(mk_buf));
	
	return 1;
}

//获得一个空闲的inode节点
static int get_new_inode()
{
	int imap_start_base = 2;
	int i, j, num = 0;
	uint8_t *ptr;
	
	//读写imap区块
	fs_read(imap_start_base, (void *)mk_buf, sizeof(mk_buf));			

	ptr = (uint8_t *) mk_buf;
	
	for(i = 0; i < SECTOR_SIZE; i++)
	{
		uint8_t tmp = *ptr;		

		for(j = 0; j < 8; j++, num++)
		{
			//位置为空
			if((tmp &(1 << j)) == 0)
			{
				tmp |= (1 << j);
				*ptr = tmp;
				fs_write(imap_start_base, (void *)mk_buf, sizeof(mk_buf));
				return num;
			}			
		}

		ptr++;
	}
	return -1;
}


//为文件/文件夹建立inode节点
//8192 / 4096 = 2
int mk_inode(int size, int mode)
{
	int id;
	inode_t inode;

	//inode-map
	id = get_new_inode(); 

	//size
	//sect_num
	inode.size = size; 							// B
	inode.sect_num = size / SECTOR_SIZE;		// 10
	
	//分配空闲扇区
	//start_sect
	inode_alloc_sector(&inode);
	
	//文件权限
	//mode
	inode.mode = mode;

	//将inode信息写入inode区域
	update_inode(id, &inode);

	clear_sector(inode.start_sect, inode.sect_num);

	//printf("[inode] : %5d ", id);
	//printf("[size]   : %5d ", inode.sect_num);
	//printf("[start]  : %5d\n", inode.start_sect);

	return id;
}

//通过inode号查找文件信息
inode_t find_inode(int inode_id)
{
	inode_t inode;

	//1 + 1 + 1 + smap
	int inode_start_base = 1 + 1 + super_block.imap_sects + super_block.smap_sects;
	
	//512 / 32 = 16
	int pre_sector_inode_num = SECTOR_SIZE / sizeof(inode_t);

	//inode应该在第几扇区读写
	int	sector_id = inode_start_base + inode_id / pre_sector_inode_num;

	//inode的扇区偏移	
	int sector_offset = (inode_id - (inode_id / pre_sector_inode_num) * sizeof(inode_t)) * sizeof(inode_t);	

	fs_read(sector_id, (void *)mk_buf, sizeof(mk_buf));
	
	memcpy((uint8_t *) &inode, mk_buf + sector_offset, sizeof(inode_t));

	//printf("[%d %d]\n", inode.start_sect, inode.sect_num);
	return inode;
}

//向一个文件夹添加目录项目
void add_dir_entry(int dir_inode_id, dir_entry_t *dir_entry)
{
	inode_t inode = find_inode(dir_inode_id);

	int start_sector = inode.start_sect;
	int sector_num = inode.sect_num;
	int i, j ;
	int pre_sector_entry_size = SECTOR_SIZE / sizeof(dir_entry_t);
	dir_entry_t *ptr;

	//读一个扇区的目录项
	for(i = start_sector; i < start_sector + sector_num; i++)
	{
		fs_read(i, (void *) mk_buf, sizeof(mk_buf));
		ptr = (dir_entry_t *) mk_buf;
		
		for(j = 0; j < pre_sector_entry_size; j++)
		{
			//目录项为空
			if(ptr -> inode_id == 0)
			{
				//写入目录项
				memcpy((uint8_t *)ptr, (uint8_t *) dir_entry, sizeof(dir_entry_t));
				fs_write(i, mk_buf, sizeof(mk_buf));
				return;
			}
			ptr++;
		}
		
	}
}

//清空扇区
static void clear_sector(int sector_start, int sector_num)
{
	memset(mk_buf, 0, sizeof(mk_buf));
	int i;
	
	for(i = sector_start; i < sector_start + sector_num; i++)
	{
		fs_write(i, mk_buf, sizeof(mk_buf));		
	}		
}

//新建一个文件
// 1. size (B)
// 2. mode (0/1/3)
int mk_file(char name[], int size, int mode, int dir)
{
	
	//为文件分配了inode和存储扇区
	int inode_id = mk_inode(size, mode);
	dir_entry_t dir_entry;

	//添加目录项
	memcpy((uint8_t *) dir_entry.name, (uint8_t *) name, strlen(name) + 1);
	dir_entry.inode_id = inode_id;
	dir_entry.type = FS_TYPE_FILE;	

	add_dir_entry(dir, &dir_entry);
	
	return 1;
}



//新建一个文件夹
int mk_dir(char name[], int mode, int dir)
{
	int inode_id = mk_inode(FOLDER_SIZE, mode);
	dir_entry_t dir_entry;

	//添加目录项
	memcpy((uint8_t *) dir_entry.name, (uint8_t *) name, strlen(name) + 1);
	dir_entry.inode_id = inode_id;	
	dir_entry.type = FS_TYPE_FOLDER;

	add_dir_entry(dir, &dir_entry);

	//添加当前目录项	
	memcpy((uint8_t *) dir_entry.name, (uint8_t *) current_dir_name, strlen(current_dir_name) + 1);
	dir_entry.inode_id = inode_id;
	dir_entry.type = FS_TYPE_FOLDER;	
	add_dir_entry(inode_id, &dir_entry);

	//添加上级目录项
	memcpy((uint8_t *) dir_entry.name, (uint8_t *) parent_dir_name, strlen(parent_dir_name) + 1);
	dir_entry.inode_id = dir;
	add_dir_entry(inode_id, &dir_entry);
	
	return 1;
}
