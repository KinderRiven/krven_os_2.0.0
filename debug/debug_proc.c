#include "debug.h"
#include "stdio.h"
#include "sys.h"

void user_task_a()
{
	int i, j;

	while(1)
	{
		//write("A");
		//get_proc_num();
		for(i = 0; i < 10000; i++)
			for(j = 0; j < 1000; j++);
	}
}

void sys_task_a()
{
	int i, j;

	while(1)
	{
		//printf("B");
		for(i = 0; i < 10000; i++)
			for(j = 0; j < 1000; j++);
	}
}
