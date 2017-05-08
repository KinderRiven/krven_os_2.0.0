#include "sys.h"
#include "stdio.h"

uint32_t sys_call_table[SYSCALL_MAX_SIZE];
static void set_sys_call(int num, uint32_t sys_call);

void init_sys_call()
{
	//查看内存中进程数量的系统调用
	set_sys_call(SYS_PROC_NUM, (uint32_t) sys_get_proc_num);
	
	//查看
	set_sys_call(SYS_WRITE, (uint32_t) sys_write);

}

static void set_sys_call(int num, uint32_t sys_call)
{
	sys_call_table[num] = sys_call;		
}

