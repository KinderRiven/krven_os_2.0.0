#ifndef INCLUDE_TTY_H_
#define INCLUDE_TTY_H_

#include "types.h"

#ifndef TTY_BUFFER_SIZE
	#define TTY_BUFFER_SIZE 1024
#endif
typedef
struct console_buffer_t{
	uint8_t cursor_x;
	uint8_t cursor_y;
}console_buffer_t;

typedef 
struct tty_t{
	
	uint8_t buf_size;
	uint8_t buf[TTY_BUFFER_SIZE];	//缓冲区大小
	uint8_t point;					//打印指针
	uint8_t len;					//长度(用于退格计数)

}tty_t;

extern uint8_t current_tty;

void parse_command(uint8_t num);
void init_tty();
void tty_putc(int num, char c);
void tty_write(int num, char* cstr);

int tty_thread(void *arg);

#endif
