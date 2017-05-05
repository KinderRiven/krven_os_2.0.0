#include "sched.h"

proc_t* current_proc;
static int num = 0;

extern void first_proc_start();

void init_schedule()
{
	current_proc = &(procs[0]);
	first_proc_start();
}

void schedule()
{
	current_proc = &(procs[num % proc_num]);
	num = (num + 1) % proc_num;
}

