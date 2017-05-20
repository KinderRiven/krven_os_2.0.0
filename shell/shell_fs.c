#include "msg.h"
#include "fs.h"
#include "tty.h"
#include "shell.h"
#include "string.h"
#include "task.h"
#include "stdio.h"
#include "sys.h"
#include "console.h"

static uint8_t shell_fsbuf[SHELL_BUF_SIZE];

//打印目录
static void print_dir_entries(void *dir_entries);

//检查重名
static int check_name_repeat(tty_t *tty, char *name);

//带有缓冲的文件夹更新
static void update_current_dir(tty_t *tty);


static void print_dir_entries(void *dir_entries)
{
	dir_entry_t *dir_entry = (dir_entry_t *) dir_entries;
	int i, j, total = MAX_DIR_ENTRIES_NUM;

	for(i = 0; i < total; i++)
	{
		if(dir_entry -> inode_id != 0)
		{
			if(dir_entry -> type == FS_TYPE_FILE)
				printc(c_black, c_cyan, "%s", dir_entry -> name);
			
			else if(dir_entry -> type == FS_TYPE_FOLDER)
				printc(c_black, c_light_brown, "%s", dir_entry -> name);
		
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

//更新当前tty虚拟文件系统缓存
static void update_current_dir(tty_t *tty)
{

	if(tty -> fs_buffer.update_flag == 0)
		return;	

	tty -> fs_buffer.update_flag = 0;

	msg_t msg;
	msg.type = MSG_FS_DIR;
	msg.inode = tty -> fs_buffer.current_dir_inode_id;
	msg.buf = shell_fsbuf;
	msg.pid = tty_pid;

	send_message(tty_pid, fs_pid, &msg);
	
	msg_t msg_finish;	
	recv_message(fs_pid, tty_pid, &msg_finish);
	
	memcpy((uint8_t *)tty -> fs_buffer.fs_buf,	(uint8_t *) shell_fsbuf, sizeof(shell_fsbuf));
	
}

static int check_name_repeat(tty_t *tty, char *name)
{
	update_current_dir(tty);	

	dir_entry_t *dir_entry = (dir_entry_t *) tty -> fs_buffer.fs_buf;
	int i, total = MAX_DIR_ENTRIES_NUM;

	for(i = 0; i < total; i++)
	{
		if(dir_entry -> inode_id != 0)
		{
			//发现重名的
			if(strcmp(dir_entry -> name, name) == 0)
				return 0;
		}
		else
			break;
	
		dir_entry++;
	}

	return 1;

}

void shell_mkdir(void *args, tty_t *tty)
{

	char (*name)[SHELL_PARAM_SIZE] = args;
	
	msg_t msg;
	msg.type = MSG_FS_MKDIR;
	msg.inode = tty -> fs_buffer.current_dir_inode_id;
	msg.mode = 0;	

	memcpy(shell_fsbuf, (uint8_t *) name[1], strlen(name[1]) + 1);
	msg.buf = shell_fsbuf;
	msg.pid = tty_pid;

	if(check_name_repeat(tty, (char *)shell_fsbuf) == 0){
		printf("Tha name is repeat : %s", shell_fsbuf);
		printf("\n");
	}
	else{	
		
		send_message(tty_pid, fs_pid, &msg);
		msg_t msg_finish;	
		recv_message(fs_pid, tty_pid, &msg_finish);	
		tty -> fs_buffer.update_flag = 1;	
	}
}

void shell_mkfile(void *args, tty_t *tty)
{

	char (*name)[SHELL_PARAM_SIZE] = args;
	
	msg_t msg;
	msg.type = MSG_FS_MKFILE;
	msg.inode = tty -> fs_buffer.current_dir_inode_id;
	msg.cnt = 512;
	msg.mode = 0;
	
	memcpy(shell_fsbuf, (uint8_t *) name[1], strlen(name[1]) + 1);
	msg.buf = shell_fsbuf;
	
	msg.pid = tty_pid;
	
	if(check_name_repeat(tty, (char *)shell_fsbuf) == 0){
		printf("Tha name is repeat : %s", shell_fsbuf);
		printf("\n");
	}
	else{	
		
		send_message(tty_pid, fs_pid, &msg);
		msg_t msg_finish;	
		recv_message(fs_pid, tty_pid, &msg_finish);	
		tty -> fs_buffer.update_flag = 1;	
	
	}
	
}

void shell_ls(void *args, tty_t *tty)
{

	update_current_dir(tty);
	print_dir_entries((void *) tty -> fs_buffer.fs_buf);	
}

void shell_cd(void *args, tty_t *tty)
{
	
	char opt1[] = "/";

	//获取当前目录
	update_current_dir(tty);
	char (*name)[SHELL_PARAM_SIZE] = args;
	dir_entry_t *dir_entry = (dir_entry_t *) tty -> fs_buffer.fs_buf;
	int i, total = MAX_DIR_ENTRIES_NUM;

	for(i = 0; i < total; i++)
	{
		if(dir_entry -> inode_id != 0)
		{
			//找到要进入的文件夹
			if(strcmp(dir_entry -> name, name[1]) == 0){	
	
				if(dir_entry -> type == FS_TYPE_FOLDER)
				{
					//进入上级目录
					if(strcmp(dir_entry -> name, parent_dir_name) == 0){
						
						int l = strlen(tty -> fs_buffer.dir_name);
						int j;
						for(j = l - 2; j >= 0; j--)
						{
							if(tty -> fs_buffer.dir_name[j] == '/')
							{
								tty -> fs_buffer.dir_name[j + 1] = '\0';
								break;
							}	
						}	
						
						tty -> fs_buffer.current_dir_inode_id = dir_entry -> inode_id;								
						tty -> fs_buffer.update_flag = 1;
						return;	
					}
					else if(strcmp(dir_entry -> name, current_dir_name) == 0)
					{
						tty -> fs_buffer.update_flag = 1;
						return;
					}
					//进入普通目录
					else{
						
						strcat(tty -> fs_buffer.dir_name, name[1]);
						strcat(tty -> fs_buffer.dir_name, opt1);
						tty -> fs_buffer.current_dir_inode_id = dir_entry -> inode_id;								
						tty -> fs_buffer.update_flag = 1;
						return;
					
					}
				}
				else
				{
					printc(c_black, c_brown, "%s", name[1]);
					printc(c_black, c_brown, " is a file.\n");
					return;
				}
			}
		}
		else
			break;
		dir_entry++;
	}
	
	printc(c_black, c_brown, "%s", name[1]);
	printc(c_black, c_brown, " does not existed.\n");	

}
