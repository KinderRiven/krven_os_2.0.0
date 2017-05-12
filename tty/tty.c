#include "tty.h"
#include "vargs.h"
#include "stdio.h"
#include "string.h"
#include "common.h"
#include "console.h"
#include "sys.h"
#include "ui.h"
#include "shell.h"

#define TTY_NUMBER 3

//终端窗口
tty_t tty[TTY_NUMBER];

uint8_t current_tty = 0;

static void tty_print_header(uint8_t num);

//tty命令缓冲区
char tty_command_buf[TTY_BUFFER_SIZE];

//打印tty header
static void tty_print_header(uint8_t num)
{
	printc(c_black, c_light_brown, "tty%d@kinderriven: ", num + 1);
}

//从tty的缓冲区读取数据
void tty_flush(uint8_t num)
{
	//command solver	
	memcpy((void *)tty_command_buf, (void *)tty[num].buf, tty[num].len - 1);
	tty_command_buf[tty[num].len - 1] = '\0';

	//命令的解析交给	
	shell_parse_command(tty_command_buf);

	//缓冲区初始化
	tty[num].buf_size = 0;
	tty[num].point = 0;
	tty[num].len = 0;

	//打印tty header
	tty_print_header(num);
}


//向tty处理队列添加字符
void tty_putc(int num, char c)
{
	tty[num].buf[tty[num].buf_size++] = c;
	tty[num].len++;
}

//向tty处理队列添加字符串
void tty_write(int num, char *cstr)
{
	int len = strlen(cstr), i;
	
	for(i = 0; i < len; i++)
	{
		tty_putc(num, cstr[i]);
	}
}


//tty print
void tty_print(uint8_t num)
{

	//打印
	if(tty[num].point < tty[num].buf_size)
	{
		char c = tty[num].buf[tty[num].point++];
		
		if(c == '\n')
		{
			tty_flush(num);
		}
		//注意对退格符号的处理
		else if(c == '\b')
		{
			if(tty[num].len-2 >= 0)
			{
				tty[num].len -= 2;
				printf("\b \b");
			}
			else
			{
				tty[num].len --;
			}
		}
		//切换tty窗口
		else if(c < 0)
		{
			//切换
			tty_switch(-(c + 1));
		}
		//输出可打印字符
		else 
			printf("%c", c);
	}
}

//tty进程
void tty_start()
{
	
	init_tty();
	
	while(1)
	{
		tty_print(current_tty);		
	}
}

//切换tty
void tty_switch(uint8_t switch_id)
{
	console_copy_to(
		tty[current_tty].console_buffer.video_memory, 
		&tty[current_tty].console_buffer.cursor_x, 
		&tty[current_tty].console_buffer.cursor_y);
	
	//切换	
	current_tty = switch_id;
	
	console_copy_from(
		tty[current_tty].console_buffer.video_memory, 
		&tty[current_tty].console_buffer.cursor_x, 
		&tty[current_tty].console_buffer.cursor_y);
	
}

void init_tty()
{
	int i;	
	
	for(i = TTY_NUMBER - 1; i >= 0; i--)
	{
		tty[i].buf_size = 0;	
		tty[i].point = 0;
		tty[i].len = 0;		

		write_clear();
		ui_welcome();
		tty_print_header(i);
		console_copy_to(tty[i].console_buffer.video_memory, &tty[i].console_buffer.cursor_x, &tty[i].console_buffer.cursor_y);	
	}
	current_tty = 0;
	tty_switch(current_tty);
}
