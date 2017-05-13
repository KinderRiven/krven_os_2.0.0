#include "sys.h"
#include "stdio.h"

uint32_t sys_call_table[SYSCALL_MAX_SIZE];
static void set_sys_call(int num, uint32_t sys_call);

void init_sys_call()
{
	//1.查看内存中进程数量的系统调用
	set_sys_call(SYS_PROC_NUM, (uint32_t) sys_get_proc_num);
	//2.清屏
	set_sys_call(SYS_WRITE_CLEAR, (uint32_t) sys_write_clear);
	
	//51.内核打印函数
	set_sys_call(SYS_WRITE, (uint32_t) sys_write);
	set_sys_call(SYS_PROC_PID, (uint32_t) sys_get_proc_pid);

	//102.发送中断
	set_sys_call(SYS_SEND_INT, (uint32_t) sys_send_interrupt);
	
	//103.接收中断
	set_sys_call(SYS_RECV_INT, (uint32_t) sys_recv_interrupt);

	//151.内核打印函数
	set_sys_call(SYS_WRITE_COLOR, (uint32_t) sys_write_color);
	
	//152
	set_sys_call(SYS_SEND_MESSAGE, (uint32_t) sys_send_message);
	
	//153
	set_sys_call(SYS_RECV_MESSAGE, (uint32_t) sys_recv_message);
}

static void set_sys_call(int num, uint32_t sys_call)
{
	sys_call_table[num] = sys_call;		
}

