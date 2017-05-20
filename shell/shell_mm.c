#include "pmm.h"
#include "msg.h"
#include "fs.h"
#include "tty.h"
#include "shell.h"
#include "string.h"
#include "task.h"
#include "stdio.h"
#include "sys.h"
#include "console.h"


void shell_pmm_used(void *args, tty_t *tty)
{
	show_pmm_status();
}

void shell_pmm_alloc(void *args, tty_t *tty)
{

	char (*name)[SHELL_PARAM_SIZE] = args;

	uint32_t num = cstr_to_uint32(name[1]);
	uint32_t addr;

	addr = alloc_pmm((int) num);		
	printf("Allocate %d B of Physical Memory at 0x%08X.\n", num, addr);
	
}

void shell_fork(void *args, tty_t *tty)
{
	fork();
}
