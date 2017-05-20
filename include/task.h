#ifndef INCLUDE_TASK_H_
#define INCLUDE_TASK_H_
#include "types.h"

#define MAX_TASK_SIZE 1024
#define MAX_TASK_NAME 24

typedef 
enum task_status
{
	TASK_STOP = 0,
	TASK_RUNNING = 1,
	TASK_WAITING = 2

} task_status;

typedef 
enum task_level
{
	KERNEL_TASK = 0,
	SYS_TASK = 1,		
	USER_TASK = 3

} task_level;

typedef
struct task_t
{

	pid_t pid;					//进程号
	tid_t tid;					//任务号
	
	task_status status;			//运行权限
	task_level level;			//任务等级
	
	uint32_t entry;				//进程入口指针	
	char name[MAX_TASK_NAME];	//任务名称

} task_t;

//任务表
extern task_t task_table[MAX_TASK_SIZE];

//任务总数
extern uint32_t task_size;

//初始化任务表
void init_task_table();

//添加一个新的任务,返回任务的tid号
tid_t add_new_task(task_level level, uint32_t entry, char *name);

void show_task_table();
#endif
