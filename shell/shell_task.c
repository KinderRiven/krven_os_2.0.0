#include "msg.h"
#include "fs.h"
#include "tty.h"
#include "shell.h"
#include "string.h"
#include "task.h"
#include "stdio.h"
#include "sys.h"
#include "number.h"
#include "console.h"

void shell_show_task(void *args, tty_t *tty)
{
	show_task_table();	
}

void shell_show_ldt(void *args, tty_t *tty)
{
	char (*name)[SHELL_PARAM_SIZE] = args;
	
	uint32_t num = cstr_to_uint32(name[1]);

	proc_print_ldt(num);  
}
