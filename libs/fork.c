#include "msg.h"
#include "sys.h"
#include "proc.h"
#include "mm.h"

int fork()
{
	pid_t pid;
	get_proc_pid(&pid);	
	
	msg_t msg;
	msg.type = MSG_FORK;
	msg.pid = pid;

	send_message(pid, mm_pid, &msg);			

	return 0;
}

