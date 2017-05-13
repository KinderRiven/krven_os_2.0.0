#include "timer.h"
#include "idt.h"
#include "stdio.h"
#include "common.h"
#include "sched.h"

void clock_handler(){

	//任务调度	
	//task_schedule();
	
	//进程调度 
	proc_schedule();

}

// 第一位 0 计数器使用二进制 1使用BCD码
// 第二到四位（模式）
// 五、六位（读写锁）
// 七、八位（计数器位数选择）
void init_timer(uint32_t frequency){

	register_interrupt_handler(IRQ0,clock_handler);
	
	uint32_t divisor = 1193180 / frequency;
	
	//通过43h端口写8253模式控制存储器	
	// {0 0 1 1} {0 1 1 0}
	outb(0x43, 0x36);
	
	//修改时钟频率
	uint8_t low = (uint8_t)(divisor & 0xFF);
	uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);
	outb(0x40, low);
	outb(0x40, high);

}
