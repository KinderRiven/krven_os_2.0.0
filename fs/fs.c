#include "debug.h"
#include "fs.h"
#include "const.h"
#include "string.h"
#include "stdio.h"
#include "hd.h"
#include "msg.h"
#include "sys.h"

pid_t fs_pid;

//是否进行文件系统的安装
static uint8_t to_mkfs = 0;

static uint8_t install_sp = 1;
static uint8_t install_imap = 1;
static uint8_t install_smap = 1;
static uint8_t install_inode = 1;
static uint8_t install_root  = 1;


uint32_t fs_root;
uint8_t fs_buf[FS_BUF_SIZE];
hd_part_t hd_part;
super_block_t super_block;

//安装文件系统
static void mkfs();

//fs初始化	
static void fs_init();

//按超级块内容格式打印内容
static void print_super_block(super_block_t *sp);

//打印扇区内容（num个字节）
static void fs_print_sector(int sector_no, int num);

//打印dir目录
static void fs_print_dir(int);

void fs_print_debug(void *ptr, int num)
{
	int i;
	uint8_t *p = ptr;
	
	for(i = 0; i < num; i++)
	{
		printf("%02x ", *p++);
		
		if((i + 1) % 16 == 0)
		{
			printf("\n");
		}
	}
}

static void print_super_block(super_block_t *sp)
{
	printf("[magic]      : %6d ",  sp -> magic);
	printf("[inode_num]  : %6d ",  sp -> inode_num);
	printf("[inode_sects]: %6d\n", sp -> inode_sects);
	printf("[sect_num]   : %6d ",  sp -> sect_num);
	printf("[imap_sects] : %6d ",  sp -> imap_sects);
	printf("[smap_sects] : %6d\n", sp -> smap_sects);
	printf("[1st_sect]   : %6d ",  sp -> first_data_sect);
	printf("[root_inode] : %6d\n", sp -> root_inode);
}

//初始化主要进行

//1. 初始化硬盘
//2. 获得分区表信息
//3. 获得超级块
static void fs_init()
{
	
	//获得文件系统的pid
	get_proc_pid(&fs_pid);
	
	msg_t msg;
	pid_t pid;

	get_proc_pid(&pid);
	
	//打开设别指令
	msg.type = MSG_DEV_OPEN;
	
	//次设备号为0
	msg.device = 0;

	//打印设别信息
	msg.debug = 1;

	//打开设备，进行硬盘信息初始化
	send_message(pid, hd_pid, &msg);	
	
	//获取分区信息
	msg.type = MSG_DEV_INFO;
	msg.device = PART_TYPE_KOS;
	msg.buf = (uint8_t *) &hd_part;

	//获取分区信息
	send_message(pid, hd_pid, &msg);
	fs_root = hd_part.start_lba;

	fs_read(1, (void *)&super_block, sizeof(super_block));
	print_super_block(&super_block);
}

static void fs_print_sector(int sector_no, int num)
{
	int i;	
	fs_read(sector_no, (void *)fs_buf, sizeof(fs_buf));

	for(i = 0; i < num; i++)
	{
		printf("%02x ", fs_buf[i]);
		if((i + 1) % 16 == 0)
		{
			printf("\n");
		}
	}

}

//打印目录
static void fs_print_dir(int inode_id)
{
	inode_t inode = find_inode(inode_id);

	int start_sector = inode.start_sect;
	int sector_num = inode.sect_num;

	//printf("[%d %d]\n", start_sector, sector_num);	
	
	int i, j;
	int pre_sector_entry_size = SECTOR_SIZE / sizeof(dir_entry_t);
	dir_entry_t *ptr;

	//读一个扇区的目录项
	for(i = start_sector; i < start_sector + sector_num; i++)
	{
		fs_read(i, (void *) fs_buf, sizeof(fs_buf));
		ptr = (dir_entry_t *) fs_buf;
		
		for(j = 0; j < pre_sector_entry_size; j++){
			//目录项为空
			if(ptr -> inode_id == 0){
				return;
			}
			else{
				printf("[name]  : %s",  ptr -> name);
				printf(" ");
				printf("[inode] : %d\n", ptr -> inode_id);
			}
			ptr++;
		}
		
	}
}


void fs_task()
{
	fs_init();

	if(to_mkfs)
		mkfs();
	
	/*****************************
   	**                          **
	** 		 Read a sector	    **
    **                          **
	*****************************/
	
	//int i = 0;
	//for(i = 0; i < 20; i++){
	//	fs_print_sector(i, 32);
	//}

	//fs_read(super_block.first_data_sect, (void *)fs_buf, sizeof(fs_buf));	
	//fs_print_dir((dir_entry_t *) fs_buf);

	//新建根目录
	//mk_file("v1", 1024, 0, 1);
	//mk_file("C program", 1024, 0, 1);
	//mk_file("zxc", 1024, 0, 1);
	
	//for(i = 10; i < 30; i++)
	//	find_inode(i);

	//mk_file("hello.cpp", 4096, 0, super_block.root_inode);
	//mk_file("yy.cpp", 512, 0, super_block.root_inode);
	//mk_file("os.txt", 6666, 0, super_block.root_inode);

	fs_print_dir(super_block.root_inode);

	while(1);	
}

//建立文件系统
static void mkfs()
{

	sleep(50);
	int i, j;

	//printf("LBA : %d %d\n", hd_part.start_lba, hd_part.sector_num);

	/*****************************
   	**                          **
	** 		Super Block	        **
    **                          **
	*****************************/
	
	///////////////////////////// start  :  length
	//        frist data	   // (14 + n) (x)
	/////////////////////////////
	//		  inodes		   // 14 (n)
	/////////////////////////////
	//      sector map		   // 03 (11)
	/////////////////////////////
	//		inode map		   // 02 (1)
	/////////////////////////////
	//		Super Block		   // 01 (1)
	///////////////////////////// 
	//		Boot Sector	   	   // 0
	///////////////////////////// 


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
	super_block.smap_sects = super_block.sect_num / (512 * 8) + 1;

	//第一个数据扇区 boot + imap + smap
	super_block.first_data_sect = (1 + 1 + 
			super_block.imap_sects + super_block.smap_sects + super_block.inode_sects);

	//根目录的inode号
	super_block.root_inode = 1;

	//写入一个超级块
	if(install_sp){
	
		printf("Start install super-block (1)\n");
		fs_write(1, (void *)&super_block, sizeof(super_block));	
		print_super_block(&super_block);
	
	}

	//uint8_t *ptr = (uint8_t *)&super_block;
	
	//for(i = 0; i < sizeof(super_block); i++)
	//{
	//	printf("%02x ", *ptr++);
	//	if((i + 1) % 16 == 0)
	//		printf("\n");
	//}


	/*****************************
   	**                          **
	** 		  inode map	    	**
    **                          **
	*****************************/

	if(install_imap){

		printf("Start install inode map (1)\n"); 
	
		memset(fs_buf, 0, sizeof(fs_buf));

		//inode的使用情况
		//1 is root dir
		for(i = 0; i < 1; i++){
			fs_buf[0] |= (1 << i);	
		}

		fs_write(2, (void *)fs_buf, sizeof(fs_buf));
	}

	/*****************************
   	**                          **
	** 	 	  sector  map	   	**
    **                          **
	*****************************/

	if(install_smap){

		printf("Start install sector map (3)\n");
		memset(fs_buf, 0, sizeof(fs_buf));
		
		for(i = 3, j = 1; i < 3 + super_block.smap_sects; i++)
		{
			printf("[%d / %d]\n", j++, super_block.smap_sects);
			fs_write(i, (void *)fs_buf, sizeof(fs_buf));		
		}
		
		update_sector_map(0, super_block.first_data_sect);		
	}
	
	/*****************************
   	**                          **
	** 	 	 inode  array	   	**
    **                          **
	*****************************/

	if(install_inode){
	
		memset(fs_buf, 0, sizeof(fs_buf));
		printf("Start install sector map (%d)\n", 3 + super_block.smap_sects);

		for(i = 3 + super_block.smap_sects, j = 1; 
				i < 3 + super_block.smap_sects + super_block.inode_sects; i++)
		{
			printf("[%d / %d]\n", j++, super_block.inode_sects);
			fs_write(i, (void *)fs_buf, sizeof(fs_buf));		
		}
	}



	/*****************************
   	**                          **
	** 	 	 Root   Dir	    	**
    **                          **
	*****************************/

	//1 + 1 + imap + smap + inum
	if(install_root){

		memset(fs_buf, 0, sizeof(fs_buf));	
	
		printf("Start isntall root (%d - %d)\n", super_block.first_data_sect, 
				3 + super_block.smap_sects + super_block.inode_sects);
	
		//建立根目录
		int root_dir_inode_id = mk_inode(FOLDER_SIZE, 0);
		printf("Root inode-id is : %d\n", root_dir_inode_id);
		dir_entry_t root_entry;
		
		char name[] = ".";
		root_entry.inode_id = 1;	
		memcpy((uint8_t *) root_entry.name, (uint8_t *) name, sizeof(name));	
	
		//添加目录项	
		add_dir_entry(root_dir_inode_id, &root_entry);	
	}
}
