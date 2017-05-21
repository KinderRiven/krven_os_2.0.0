#include "const.h"
#include "fs.h"
#include "debug.h"
#include "string.h"
#include "stdio.h"
#include "hd.h"
#include "sys.h"
#include "proc.h"

void sleep(uint32_t time)
{
	int i, j, k;
	for(i = 0; i < 1000; i++)
		for(j = 0; j < 1000; j++)
			for(k = 0; k < time; k++);
}


//#1
void debug_user_task()
{
	uint8_t byte = 'A';
	
	while(1){
		debug_print_right(byte);	
		sleep(100);
	}
}

//#2
void debug_sys_task()
{

	uint16_t byte = 'B';
	while(1)
	{
		debug_print_right(byte);
		sleep(100);
	}
}

//#3
void debug_sys_call()
{

	char s1[50] = "123456abc";
	char s2[50];

	memcpy((uint8_t *) s2, (uint8_t *) s1, sizeof(s1));
	printf("%s", s1);
	printf(" %s", s2);
	printf("\n");
	
	while(1){
		sleep(100);
	}
}


//#4
void debug_send_task()
{
	char byte = 'C';
	
	pid_t pid;
	get_proc_pid(&pid);
	
	msg_t msg;
	msg.type = 'A';
	
	sleep(100);
	
	while(1){
		send_message(pid, 6, &msg);
		debug_print_right(byte);
		sleep(100);
		msg.type++;
	};
}

//#5
void debug_send_task2()
{
	char byte = 'D';
	
	msg_t msg;
	msg.type = '0';

	pid_t pid;
	get_proc_pid(&pid);

	while(1)
	{
		send_message(pid, 6, &msg);
		debug_print_right(byte);
		sleep(100);
		msg.type++;
	}
}

//#6
void debug_recv_task()
{
	char byte = 'E';
	
	pid_t pid;
	get_proc_pid(&pid);	
	
	msg_t msg;
	
	while(1){
		recv_message(ANY, pid, &msg);
		debug_print_right(byte);
		debug_print_left(msg.type);
		sleep(100);
	};
}


//#7
void debug_hd_task()
{
	
}
