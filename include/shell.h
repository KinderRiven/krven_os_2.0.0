#ifndef INCLUDE_SHELL_H_
#define INCLUDE_SHELL_H_

#include "types.h"
#include "tty.h"

#define SHELL_BUF_SIZE 2048
#define SHELL_HANDLER_SIZE 128
#define SHELL_PARAM_SIZE 128

void shell_parse_command(uint8_t *args, tty_t *tty);

typedef void (*shell_handler_t)(void *args, tty_t *tty);

//shell table

//杀死进程
void shell_kill_proc(void *args, tty_t *tty);

//现实内存中的进程表
void shell_show_proc(void *args, tty_t *tty);

//fork测试程序
void shell_fork(void *args, tty_t *tty);

//分配一个内存
void shell_pmm_alloc(void *args, tty_t *tty);

void shell_show_ldt(void *args, tty_t *tty);

//显示内核任务表
void shell_show_task(void *args, tty_t *tty);

//显示当前文件夹内容
void shell_ls(void *args, tty_t *tty);

//进入文件夹
void shell_cd(void *args, tty_t *tty);

//清屏
void shell_clear(void *args, tty_t *tty);

//建立文件夹
void shell_mkdir(void *args, tty_t *tty);

//建立文件
void shell_mkfile(void *args, tty_t *tty);

//现实内存使用情况
void shell_pmm_used(void *args, tty_t *tty);

void init_shell();

#endif
