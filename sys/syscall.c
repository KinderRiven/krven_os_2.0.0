#include "sys.h"
#include "stdio.h"

sys_call_t sys_call_table[SYSCALL_MAX_SIZE];
static void set_sys_call(int num, sys_call_t sys_call);

void init_sys_call()
{
	//查看内存中进程数量的系统调用
	set_sys_call(0, sys_get_proc_num);
}

static void set_sys_call(int num, sys_call_t sys_call)
{
	sys_call_table[num] = sys_call;		
}

