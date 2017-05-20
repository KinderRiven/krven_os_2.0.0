#ifndef INCLUDE_SYS_CALL_PROC_H_
#define INCLUDE_SYS_CALL_PROC_H_

#include "types.h"
#include "proc.h"

//#1
//获得系统中进程数的系统调用
void sys_get_proc_num();
void get_proc_num();

//#4
void sys_exit();
void exit();

//#52
//获得进程的系统调用号
void sys_get_proc_pid(pid_t *pid);
void get_proc_pid(pid_t *pid);

void sys_kill(pid_t pid);
void kill(pid_t pid);

//#102 发送中断
void send_interrupt(pid_t pid, int interrupt_id);
void sys_send_interrupt(pid_t pid, int interrupt_id);

//#103 接收中断
void recv_interrupt(pid_t pid, int interrupt_id);
void sys_recv_interrupt(pid_t pid, int interrupt_id);

//进行消息传递的系统调用
//#152 消息发送
void sys_send_message(pid_t recv_from, pid_t send_to, msg_t *msg);
void send_message(pid_t recv_from, pid_t send_to, msg_t *msg);

//#153 消息接收
void sys_recv_message(pid_t recv_from, pid_t send_to, msg_t *msg);
void recv_message(pid_t recv_from, pid_t send_to, msg_t *msg);

#endif
