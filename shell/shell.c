#include "shell.h"
#include "string.h"
#include "task.h"
#include "stdio.h"
#include "sys.h"
#include "console.h"

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
	else
	{
		printc(c_black, c_light_red, "Can't find command : %s", args);
		printf("\n");
	}
}
