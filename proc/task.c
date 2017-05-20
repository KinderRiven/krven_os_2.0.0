#include "task.h"
#include "hd.h"
#include "fs.h"
#include "string.h"
#include "stdio.h"
#include "ui.h"

task_t task_table[MAX_TASK_SIZE];

uint32_t task_size = 0;

void init_task_table()
{
	task_size = 0;
}

//添加一个新任务
tid_t add_new_task(task_level level, uint32_t entry, char *name)
{
	
	//运行等级
	task_table[task_size].level = level;
	
	//任务入口
	task_table[task_size].entry = entry;

	//任务名称
	strcpy(task_table[task_size].name, name);
	
	//任务等待
	task_table[task_size].status = TASK_WAITING;

	//Task id
	task_table[task_size].tid = task_size;

	task_size++;
	
	//返回任务的id
	return task_size - 1;
}

static char table_header[10][10] = {"TID", "PID", "LEVEL", "STATUS", "NAME"}; 

void show_task_table()
{

	int i, j, line_width = 10;
	for(i = 0; i < 5; i++)
	{
		printc(c_black, c_light_magenta, "%s", table_header[i]);
		for(j = 0; j < line_width - strlen(table_header[i]); j++)
			printf(" ");
	}
	printf("\n");
	
	for(i = 0; i < task_size; i++)
	{
		printc(c_black, c_light_red, "%-10d%-10d", task_table[i].tid, task_table[i].pid);
		
		switch(task_table[i].level)
		{	
			case KERNEL_TASK : 
				printc(c_black, c_red, "KERENL    ");	
				break;
			case SYS_TASK:
				printc(c_black, c_cyan, "SYS       ");
				break;
			case USER_TASK:
				printc(c_black, c_magenta, "USER      ");
				break;
			default:
				printf(c_black, c_light_red, "ERROR     ");
				break;	
		}
		switch(task_table[i].status)
		{	
			case TASK_STOP : 
				printc(c_black, c_red, "STOP      ");	
				break;
			case TASK_RUNNING:
				printc(c_black, c_green, "RUNNING   ");
				break;
			case TASK_WAITING:
				printc(c_black, c_light_brown, "WAITING   ");
				break;
			default:
				printf("ERROR     ");
				break;	
		}
		printc(c_black, c_cyan, "%s", task_table[i].name);
		printf("\n");
	}	
}
