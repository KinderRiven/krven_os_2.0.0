#include "msg.h"
#include "fs.h"
#include "tty.h"
#include "shell.h"
#include "string.h"
#include "task.h"
#include "stdio.h"
#include "sys.h"
#include "console.h"

static uint8_t shell_buf[SHELL_BUF_SIZE];

//打印文件夹
static void print_dir_entries(void *dir_entries);

void shell_parse_command(char *args)
{
	printf("\n");
	if(strcmp(args, "task") == 0)
	{
		show_task_table();
	}
	else if(strcmp(args, "proc") == 0)
	{
		get_proc_num();
	}
	else if(strcmp(args, "write") == 0)
	{
		write("Test Writing!\n");
	}
	else if(strcmp(args, "color") == 0)
	{
		write_color("Hello World!\n", c_black, c_red);
	}
	else if(strcmp(args, "clear") == 0)
	{
		write_clear();
	}
	else if(strcmp(args, "ls") == 0)
	{
		msg_t msg;
		msg.type = MSG_FS_DIR;
		msg.inode = super_block.root_inode;
		msg.buf = shell_buf;
		msg.pid = tty_pid;

		send_message(tty_pid, fs_pid, &msg);
	
		msg_t msg_finish;	
		recv_message(fs_pid, tty_pid, &msg_finish);	
		print_dir_entries((void *)shell_buf);
	}
	else
	{
		printc(c_black, c_light_red, "Can't find command : %s", args);
		printf("\n");
	}
}

static void print_dir_entries(void *dir_entries)
{
	dir_entry_t *dir_entry = (dir_entry_t *) dir_entries;
	int i, j, total = MAX_DIR_ENTRIES_NUM;

	for(i = 0; i < total; i++)
	{
		if(dir_entry -> inode_id != 0)
		{
			printc(c_black, c_cyan, "%s", dir_entry -> name);
		
			for(j = 0; j < 15 - strlen(dir_entry -> name); j++)	
				printf(" ");
		}
		else
			break;
	
		dir_entry++;
		if((i + 1) % 5 == 0)
			printf("\n");
	}
	printf("\n");
}
