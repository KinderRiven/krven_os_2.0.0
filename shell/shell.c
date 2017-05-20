#include "msg.h"
#include "fs.h"
#include "tty.h"
#include "pmm.h"
#include "shell.h"
#include "string.h"
#include "task.h"
#include "stdio.h"
#include "sys.h"
#include "console.h"

//static uint8_t shell_buf[SHELL_BUF_SIZE];

//参数数组
static char shell_params[10][SHELL_PARAM_SIZE];

//参数数量
static int  shell_param_num;

//打印文件夹
static void print_dir_entries(void *dir_entries);

static int shell_handler_size = 0;

shell_handler_t shell_handlers[SHELL_HANDLER_SIZE];

//命令列表
static char command_table[SHELL_HANDLER_SIZE][24];

static void set_shell_handler(char command[], shell_handler_t shell_handler)
{
	memcpy((uint8_t *)command_table[shell_handler_size], (uint8_t *) command, strlen(command));
	shell_handlers[shell_handler_size] = shell_handler;
	
	shell_handler_size++;
}

void init_shell()
{
	set_shell_handler("task", shell_show_task);		
	
	set_shell_handler("ls", shell_ls);
	
	set_shell_handler("clear", shell_clear);
	
	set_shell_handler("mkfile", shell_mkfile);
	
	set_shell_handler("mkdir", shell_mkdir);
	
	set_shell_handler("cd", shell_cd);
	
	set_shell_handler("pmm", shell_pmm_used);

	set_shell_handler("ldt", shell_show_ldt);

	set_shell_handler("alloc", shell_pmm_alloc);
}


static void shell_split_command(char *args)
{
	int l = strlen(args);
	int i, flag = 0, pos = 0;

	shell_param_num = 0;

	for(i = 0; i <= l; i++)
	{
		//一段参数结束了
		if(args[i] == ' ' || args[i] == '\0')
		{
			if(flag)
			{
				shell_params[shell_param_num][pos] = '\0';
				shell_param_num++;
				pos = 0;
				flag = 0;
			}
		}
		else
		{
			flag = 1;
			shell_params[shell_param_num][pos++] = args[i];
		}		
	}

}

//进行第一个字段比较，进行命令分类
static int shell_get_command_id(char *args)
{
	int i;

	if(shell_param_num == 0) return -1;

	for(i = 0; i < shell_handler_size; i++)
	{
		if(strcmp(command_table[i], shell_params[0]) == 0)
			return i;
	}
	return -1;	
}

//命令解析
void shell_parse_command(uint8_t *args, tty_t *tty)
{

	shell_split_command((char *)args);
	
	int command_id = shell_get_command_id((char *)args);
	printf("\n");
	
	if(command_id != -1)
	{
		shell_handlers[command_id]((void *) shell_params, tty);
	}
	else if(shell_param_num > 0)
	{ 
		printc(c_black, c_light_red, "Can not find command : %s.\n", shell_params[0]);
	}
}

void shell_clear(void *args, tty_t *tty)
{
	write_clear();
}
