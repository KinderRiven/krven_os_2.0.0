#ifndef INCLUDE_FS_H_
#define INCLUDE_FS_H_

#include "types.h"
#include "proc.h"
#include "sys.h"
#include "msg.h"

#define MAX_FILENAME 24

#define KOS_FS_LV1 0x6666

#define FOLDER_SIZE		1024
#define INODE_MAX_NUM 	4096
#define FS_BUF_SIZE		512

#define FS_TYPE_FILE	1
#define FS_TYPE_FOLDER	2

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


//size = 32
typedef
struct inode_t
{
	uint32_t mode;			//读写权限
	uint32_t size;			//文件大小
	uint32_t start_sect;	//开始扇区
	uint32_t sect_num;		//所占扇区数	
	
	uint32_t v0;			//保留
	uint32_t v1;			
	uint32_t v2;
	uint32_t v3;

} inode_t;

typedef
struct dir_entry_t
{
	//4	
	uint32_t inode_id;
	
	//24
	char name[MAX_FILENAME];	
	
	//4
	uint32_t type;
	
} dir_entry_t;

// 1024 / 32 = 32
#define MAX_DIR_ENTRIES_NUM (FOLDER_SIZE / sizeof(dir_entry_t))

//fs.c
extern pid_t fs_pid;
extern uint32_t fs_root;
extern super_block_t super_block;

void fs_task();
void fs_print_debug(void *, int);

//write.c
void write_to_hd(int pos);
void fs_write(int sector_no, void *buf, int buf_size);

//read.c
void read_from_hd(int pos);
void fs_read(int sector_no, void *buf, int buf_size);

//mk.c
int mk_file(char name[], int size, int mode, int dir);
int mk_dir(char name[], int mode, int dir);

int update_sector_map(uint32_t, uint32_t);

//建立一个新的inode节点，返回inode id
int mk_inode(int size, int mode);

//根据inode_id找到inode array的内容
inode_t find_inode(int inode_id);

void add_dir_entry(int, dir_entry_t *);

//opt.c
void get_dir_entries(msg_t *msg);

#endif


