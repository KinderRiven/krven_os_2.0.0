#include "shell.h"
#include "string.h"
#include "task.h"
#include "stdio.h"
#include "sys.h"

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
		write("Test Writing!");
	}
	else
	{
		printf("Can not find order : %s", args);
	}
	printf("\n");
}
