#include "task.h"
#include "string.h"
#include "stdio.h"
#include "ui.h"

task_t task_table[MAX_TASK_SIZE];

uint32_t task_size = 0;

void init_task_table()
{
	task_size = 0;
}

tid_t add_new_task(task_level level, uint32_t entry)
{
	
	//运行等级
	task_table[task_size].level = level;
	
	//任务入口
	task_table[task_size].entry = entry;
	
	//任务等待
	task_table[task_size].status = TASK_WAITING;

	task_table[task_size].tid = task_size;
	
	task_size++;
	
	//返回任务的id
	return task_size - 1;
}

char table_header[10][10] = {"Id", "Pid", "Level", "Status"}; 

void show_task_table()
{

	int i, j, line_width = 10;
	for(i = 0; i < 4; i++)
	{
		printf("%s", table_header[i]);
		for(j = 0; j < line_width - strlen(table_header[i]); j++)
			printf(" ");
	}
	printf("\n");
	
	for(i = 0; i < task_size; i++)
	{
		printf("%-10d%-10d%-10d%-10d\n", task_table[i].tid, task_table[i].pid, task_table[i].level, task_table[i].status);
	}	
}
