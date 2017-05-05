#include "sched.h"

proc_t* current_proc;
static int num = 0;

void schedule()
{
	current_proc = &(procs[num % proc_num]);
	num = (num + 1) % proc_num;
}

