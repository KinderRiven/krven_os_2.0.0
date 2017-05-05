#ifndef INCLUDE_SCHED_H_
#define INCLUDE_SCHED_H_

#include "proc.h"

extern proc_t* current_proc;

void schedule();
void init_schedule();

#endif
