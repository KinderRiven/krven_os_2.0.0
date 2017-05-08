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

void init_timer(uint32_t frequency){

	register_interrupt_handler(IRQ0,clock_handler);
	
	//uint32_t divisor = 1193180 / frequency;
	
	//outb(0x43, 0x36);
	
	//uint8_t low = (uint8_t)(divisor & 0xFF);
	//uint8_t high = (uint8_t)((divisor >> 8) & 0xFF);

	//outb(0x40, low);
	//outb(0x40, high);
}
