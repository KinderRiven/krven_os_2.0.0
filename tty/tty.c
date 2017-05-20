#include "tty.h"
#include "fs.h"
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
tty_t tty_table[TTY_NUMBER];

pid_t tty_pid;

uint8_t current_tty = 0;

static void tty_print_header(uint8_t num);

//打印tty header
static void tty_print_header(uint8_t num)
{
	printc(c_black, c_light_brown, "tty%d@kinderriven:", num + 1);
	printc(c_black, c_cyan, "%s", tty_table[num].fs_buffer.dir_name);
	printf(">");
}

//从tty的缓冲区读取数据
void tty_flush(uint8_t num)
{
	//command solver
	//添加结尾‘/0’	
	tty_table[num].cmd_buf[tty_table[num].pos] = '\0';

	//命令的解析交给	
	shell_parse_command(tty_table[num].cmd_buf, &tty_table[current_tty]);

	//缓冲区初始化
	tty_table[num].buf_size = 0;
	tty_table[num].point = 0;
	tty_table[num].pos = 0;

	//打印tty header
	tty_print_header(num);
}


//向tty处理队列添加字符
void tty_putc(int num, char c)
{
	tty_table[num].buf[tty_table[num].buf_size++] = c;
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
	if(tty_table[num].point < tty_table[num].buf_size)
	{
		char c = tty_table[num].buf[tty_table[num].point++];
		
		if(c == '\n')
		{
			tty_flush(num);
		}
		//注意对退格符号的处理
		else if(c == '\b')
		{
			if(tty_table[num].pos > 0)
			{
				tty_table[num].pos--;
				printf("\b \b");
			}	
		}
		//切换tty窗口
		else if(c < 0)
		{
			//切换
			tty_switch(-(c + 1));
		}
		//输出可打印字符
		else{ 
			printf("%c", c);
			tty_table[num].cmd_buf[tty_table[num].pos++] = c;
		}
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
		 tty_table[current_tty].console_buffer.video_memory, 
		&tty_table[current_tty].console_buffer.cursor_x, 
		&tty_table[current_tty].console_buffer.cursor_y);
	
	//切换	
	current_tty = switch_id;
	
	console_copy_from(
		 tty_table[current_tty].console_buffer.video_memory, 
		&tty_table[current_tty].console_buffer.cursor_x, 
		&tty_table[current_tty].console_buffer.cursor_y);
	
}

void init_tty()
{
	int i;	
	char root_name[] = "/";
	
	for(i = TTY_NUMBER - 1; i >= 0; i--)
	{

		memset((uint8_t *) &tty_table[i], 0, sizeof(tty_t));
		
		//tty_table[i].buf_size = 0;	
		//tty_table[i].point = 0;
		//tty_table[i].pos = 0;
		//char root_name = "/";

		tty_table[i].fs_buffer.update_flag = 1;
		tty_table[i].fs_buffer.current_dir_inode_id = DEF_ROOT_INODE;//super_block.root_inode;
		memcpy((uint8_t *)tty_table[i].fs_buffer.dir_name, (uint8_t *)root_name, strlen(root_name) + 1);

		write_clear();
		ui_welcome();
		tty_print_header(i);
		console_copy_to(tty_table[i].console_buffer.video_memory, 
				&tty_table[i].console_buffer.cursor_x, 
				&tty_table[i].console_buffer.cursor_y);	
	}
	
	current_tty = 0;
	tty_switch(current_tty);

}
