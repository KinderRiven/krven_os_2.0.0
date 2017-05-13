#ifndef INCLUDE_SYSCALL_H_
#define INCLUDE_SYSCALL_H_

#include "types.h"

#include "sys_proc.h"		//0
#include "sys_write.h"		//1

#define SYSCALL_MAX_SIZE 512
#define SYS_INT_NO 48

//无参调用
#define SYS_PROC_NUM 1
#define SYS_WRITE_CLEAR 2

//单参调用
//write
#define SYS_WRITE 51
//获得进程ID号
#define SYS_PROC_PID 52

//双参调用
#define SYS_ALERT_WRITE_COLOR 101
#define SYS_SEND_INT	102
#define SYS_RECV_INT	103	

//三参数调用
#define SYS_WRITE_COLOR	 151
#define SYS_SEND_MESSAGE 152
#define SYS_RECV_MESSAGE 153

extern uint32_t sys_call_table[SYSCALL_MAX_SIZE];

void init_sys_call();
#endif
