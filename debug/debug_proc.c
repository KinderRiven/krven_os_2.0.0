#include "debug.h"
#include "string.h"
#include "stdio.h"
#include "sys.h"
#include "proc.h"

void user_task_a()
{
	int i, j;
	int clock = 0;
	msg_t msg;

	while(1)
	{
		//printf("[A %d]", clock);
		
		printf("A");
		
		if(clock == 1){
			recv_message(2, 3, &msg);
		}
		
		for(i = 0; i < 10000; i++)
			for(j = 0; j < 1000; j++);

		if(clock == 1){
			printf("Has received :%d\n", msg.int_no);
		}	
	
		clock++;	
	}
}

void sys_task_a()
{
	int i, j;
	int clock = 0;
	msg_t msg;

	while(1)
	{
		printf("B");
		//printf("[B %d]", clock);	
	
		if(clock == 5){
			printf("I will send! %d\n", clock);
			msg.int_no = 0x10;
			send_message(2, 3, &msg);
		}

		for(i = 0; i < 10000; i++)
			for(j = 0; j < 1000; j++);
		
		clock++;
	}
}
