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
	uint8_t byte = 'A';
	while(1){
		debug_print_right(byte);	
		sleep(100);
	}
}

void debug_sys_task()
{

	uint16_t byte = 'B';
	while(1){
		debug_print_right(byte);
		sleep(100);
	}
}

void debug_sys_call()
{
	
	while(1){
		printf("S");
		sleep(100);
	}
}

void debug_send_task()
{
	char byte = 'C';
	
	msg_t msg;
	msg.outb = 'A';
	
	sleep(100);
	
	while(1){
		send_message(4, 5, &msg);
		debug_print_right(byte);
		sleep(100);
		msg.outb++;
	};
}

void debug_recv_task()
{
	char byte = 'D';
	
	msg_t msg;
	recv_message(4, 5, &msg);
	
	while(1){
		recv_message(4, 5, &msg);
		debug_print_left(msg.outb);
		debug_print_right(byte);
		sleep(100);
	};
}
