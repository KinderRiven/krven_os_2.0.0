#ifndef INCLUDE_DEBUG_H_
#define INCLUDE_DEBUG_H_

#include "types.h"

void sleep(uint32_t time);

void debug_user_task();
void debug_sys_task();

void debug_sys_call();

void debug_send_task();
void debug_recv_task();
void debug_send_task2();

void debug_print_right(uint8_t byte);
void debug_print_left(uint8_t byte);

void debug_hd_task();
#endif
