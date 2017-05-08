#ifndef INCLUDE_TTY_H_
#define INCLUDE_TTY_H_

#include "types.h"

#ifndef TTY_BUFFER_SIZE
	#define TTY_BUFFER_SIZE 1024
#endif
typedef
struct console_buffer_t{
	
	uint8_t  cursor_x;
	uint8_t  cursor_y;
	uint16_t video_memory[80 * 25];

}console_buffer_t;

typedef 
struct tty_t{
	
	uint8_t buf_size;
	uint8_t buf[TTY_BUFFER_SIZE];		//缓冲区大小
	uint8_t point;						//打印指针
	uint8_t len;						//长度(用于退格计数)
	console_buffer_t console_buffer;	//保存屏幕内存

}tty_t;

extern uint8_t current_tty;

void tty_flush(uint8_t num);
void init_tty();
void tty_putc(int num, char c);
void tty_write(int num, char* cstr);
void tty_switch(uint8_t switch_id);

//开启tty进程
void tty_start();

#endif
