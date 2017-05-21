#include "proc.h"
#include "sys_proc.h"
#include "sched.h"
#include "stdio.h"
#include "hd.h"
#include "task.h"

// 进程结束主要做2个事
// 1.释放进程块的内存
// 2.挂起TASK表为STOP

void sys_kill(pid_t pid)
{
	//释放进程块
	procs[pid].take_up = 0;

	//挂起task进程
	printk("PID : %d TID : %d\n", pid, procs[pid].tid);

	//while(1);
	task_table[procs[pid].tid].status = TASK_STOP;	
}

void sys_exit()
{
	
}

////////////////////////////////////////////////////////////////////////////

void sys_get_proc_num()
{
	printk("This is a system call!\n");
}

///////////////////////////////////////////////////////////////////////////

void sys_get_proc_pid(pid_t *pid)
{
	*pid = current_proc -> pid;	
}

///////////////////////////////////////////////////////////////////////////

void sys_send_message(pid_t recv_from, pid_t send_to, msg_t *msg)
{
	proc_t *proc_from = &procs[recv_from];
	msg_send(proc_from, send_to, msg);

	//if(send_to == hd_pid)
	//{
	//	printk("Send to HD\n");
	//}
	
	//重新进行调度
	if(proc_from -> msg_block == 1){
		proc_line_schedule();	
	}
}

///////////////////////////////////////////////////////////////////////////

void sys_recv_message(pid_t recv_from, pid_t send_to, msg_t *msg)
{
	proc_t *proc_to = &procs[send_to];
	msg_receive(proc_to, recv_from, msg);

	//if(send_to == hd_pid)
	//{
	//	printk("HD recv!\n");
	//}

	//重新进行调度
	if(proc_to -> msg_block == 1){
		proc_line_schedule();
	}
}

/////////////////////////////////////////////////////////////////////////////

void sys_send_interrupt(pid_t pid, int interrupt_id)
{

	proc_t *proc = &procs[pid];	
	
	msg_send_interrupt(pid, interrupt_id);
	
	if(proc -> msg_block == 1){
		proc_line_schedule();
	}
}

/////////////////////////////////////////////////////////////////////////////

void sys_recv_interrupt(pid_t pid, int interrupt_id)
{

	proc_t *proc = &procs[pid];	
	
	msg_recv_interrupt(pid, interrupt_id);

	if(proc -> msg_block == 1){
		proc_line_schedule();
	}
}
