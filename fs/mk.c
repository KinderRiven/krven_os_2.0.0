#include "debug.h"
#include "fs.h"
#include "const.h"
#include "string.h"
#include "stdio.h"
#include "hd.h"
#include "msg.h"
#include "sys.h"

static uint8_t mk_buf[FS_BUF_SIZE];

static void inode_alloc_sector(inode_t *);

static uint8_t bits[8] = {
	1<<0, 
	1<<0 | 1<<1, 
	1<<0 | 1<<1 | 1<<2, 
	1<<0 | 1<<1 | 1<<2 | 1<<3, 
	1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4, 
	1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<5, 
	1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<5 | 1<<6, 
	1<<0 | 1<<1 | 1<<2 | 1<<3 | 1<<4 | 1<<5 | 1<<6 | 1<<7
};
//4096 * 2 = 8192

//start = 0 count = 150
//sector_id = 3 + 0 / 4096 = 3
//sector_num = (150 - 1) / 4096 + 1 = 1
//sector_start = 0 - (0 / 4096) * 4096 = 150

//start = 4096 count = 1
//sector_id = 3 + 4096 / 4096 = 4
//sector_num = (1 - 1) / 4096 + 1 = 1
//sector_start = 4096 - (4096 / 4096) * 4096 = 0

//start = 8193 count = 10
//sector_id = 3 + 8193 / 4096 = 5
//sector_num = (10 - 1) / 4096 + 1 = 1
//sector_start = 8193 - (8193 / 4096) * 4096 = 1
void update_sector_map(uint32_t start, uint32_t count)
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
					return;	
				}
			}
			ptr++;
		}
		sector_id++;	
	}	
}

//为一个inode分配扇区
static void inode_alloc_sector(inode_t *inode)
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
	int start = -1;
	
	//已经有多少个空位了
	int cnt = 0;
	uint8_t *ptr;

	//找一下空闲的扇区
	for(i = inode_map_base; i < inode_map_base + super_block.smap_sects; i++)
	{
		printf("%d / %d\n", i - inode_map_base + 1, super_block.smap_sects);

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
				if(cnt == inode -> sect_num)
				{
					printf("%d %d\n", start, cnt);
					return;
				}
			}
		}	
	}
}

//新建一个文件
void mk_file(char name[], int size, int mode)
{
	inode_t inode;

	//分配空闲扇区
	inode_alloc_sector(&inode);
	
	//文件大小
	inode.size = size;

	//文件权限
	inode.mode = mode;
}


//新建一个文件夹
void mk_dir(char name[], int mode)
{
	inode_t inode;

	inode.sect_num = 10;

	//分配空闲扇区
	inode_alloc_sector(&inode);
	
	//文件权限
	inode.mode = mode;

}
