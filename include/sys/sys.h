#ifndef INCLUDE_SYSCALL_H_
#define INCLUDE_SYSCALL_H_

#include "types.h"

#include "sys_proc.h"		//0
#include "sys_write.h"		//1

#define SYSCALL_MAX_SIZE 128
#define SYS_INT_NO 48

//无参调用
#define SYS_PROC_NUM 1

//单参调用
#define SYS_WRITE 51

//双参调用
#define SYS_ALERT_WRITE_COLOR 101

extern uint32_t sys_call_table[SYSCALL_MAX_SIZE];

void init_sys_call();
#endif
