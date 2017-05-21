#ifndef INCLUDE_SCHED_H_
#define INCLUDE_SCHED_H_

#include "proc.h"

extern proc_t* current_proc;

//线性任务调度
void task_line_schedule();

//基于优先级的等待比优先级调度算法
void task_priority_schedule();

//线性进程调度
void proc_line_schedule();

//初始化
void init_schedule();

#endif
