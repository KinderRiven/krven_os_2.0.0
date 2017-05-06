#ifndef INCLUDE_SYSCALL_H_
#define INCLUDE_SYSCALL_H_

#include "sys_proc.h"

#define SYSCALL_MAX_SIZE 128
#define SYS_CALL_PRINT	0	//测试系统调用 

typedef void(*sys_call_t)();
extern sys_call_t sys_call_table[SYSCALL_MAX_SIZE];

#endif
