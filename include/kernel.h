#ifndef INCLUDE_KERNEL_H_
#define INCLUDE_KERNEL_H_

#include "types.h"
#define KERNEL_STACK_SIZE 4096

//内核栈
extern char kernel_stack[KERNEL_STACK_SIZE];
extern uint32_t kernel_stack_top;

#endif
