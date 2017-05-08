#ifndef INCLUDE_SCHED_H_
#define INCLUDE_SCHED_H_

#include "proc.h"

extern proc_t* current_proc;

//任务调度
void task_schedule();

//进程调度
void proc_schedule();

//初始化
void init_schedule();

#endif
