#include "kernel.h"

//内核栈
char kernel_stack[KERNEL_STACK_SIZE];

//内核栈顶
uint32_t kernel_stack_top;
