#include "debug.h"
#include "string.h"
#include "stdio.h"
#include "sys.h"
#include "proc.h"

static void sleep(uint32_t time);

void sleep(uint32_t time)
{
	int i, j, k;
	for(i = 0; i < 1000; i++)
		for(j = 0; j < 1000; j++)
			for(k = 0; k < time; k++);
}

void debug_user_task()
{
	while(1){}
}

void debug_sys_task()
{

	while(1){}
}

void debug_send_task()
{
	while(1){};
}

void debug_recv_task()
{
	while(1){};
}
