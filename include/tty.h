#ifndef INCLUDE_TTY_H_
#define INCLUDE_TTY_H_

#include "types.h"

#define TTY_BUFFER_SIZE 1024
#define TTY_FSBUF_SIZE	2048

typedef
struct console_buffer_t{
	
	uint8_t  cursor_x;
	uint8_t  cursor_y;
	uint16_t video_memory[80 * 25];

}console_buffer_t;


//tty对于虚拟文件系统的缓存
typedef
struct fs_buffer_t
{
	//是否需要刷新
	int update_flag;

	//当前的文件夹inode_id
	int current_dir_inode_id;

	//文件头
	char dir_name[128];
	
	//虚拟文件系统缓冲区
	uint8_t fs_buf[TTY_FSBUF_SIZE];
 
} fs_buffer_t;

typedef 
struct tty_t{
	
	uint8_t buf_size;
	uint8_t buf[TTY_BUFFER_SIZE];		//缓冲区大小
	uint8_t point;						//打印指针

	uint8_t pos;						//命令行buffer长度
	uint8_t cmd_buf[TTY_BUFFER_SIZE];	//命令行buffer	
	console_buffer_t console_buffer;	//保存屏幕内存
	fs_buffer_t fs_buffer;

}tty_t;

extern uint8_t current_tty;
extern pid_t tty_pid;

void tty_flush(uint8_t num);
void init_tty();
void tty_putc(int num, char c);
void tty_write(int num, char* cstr);
void tty_switch(uint8_t switch_id);

//开启tty进程
void tty_start();

#endif
