#include "mm.h"
#include "init.h"
#include "types.h"
#include "gdt.h"
#include "heap.h"
#include "timer.h"
#include "console.h"
#include "stdio.h"
#include "pmm.h"
#include "vmm.h"
#include "multiboot.h"
#include "idt.h"
#include "keyboard.h"
#include "tty.h"
#include "sys.h"
#include "proc.h"
#include "shell.h"
#include "tss.h"
#include "fs.h"
#include "kernel.h"
#include "debug.h"
#include "sched.h"
#include "task.h"
#include "hd.h"

void kern_init();
multiboot_t *glb_mboot_ptr;


//临时页目录和页表
__attribute__((section(".init.data"))) pgd_t *pgd_tmp  = (pgd_t *) 0x1000;
__attribute__((section(".init.data"))) pgd_t *pte_low  = (pgd_t *) 0x2000;
__attribute__((section(".init.data"))) pgd_t *pte_high = (pgd_t *) 0x3000;

__attribute__((section(".init.text")))
void kern_entry(){

	pgd_tmp[0] = (pgd_t) pte_low | PDE_PRESENT | PDE_WRITE;
	pgd_tmp[PGD_INDEX(PAGE_OFFSET)] = (pgd_t) pte_high | PDE_PRESENT | PDE_WRITE;
	
	int i = 0;
	
	for( i = 0; i < 1024; i++){
		pte_low[i] = (i << 12) | PTE_PRESENT | PTE_WRITE;
	}

	for( i = 0; i < 1024; i++){
		pte_high[i] = (i << 12) | PTE_PRESENT | PTE_WRITE;
	}

	//开启临时分页	
	asm volatile("mov %0, %%cr3" : : "r" (pgd_tmp));

	//打开分页操作	
	uint32_t cr0;
	asm volatile("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" : : "r" (cr0));

	//切换到内核栈
	kernel_stack_top = ((uint32_t)kernel_stack + KERNEL_STACK_SIZE) & 0xFFFFFFF0;
	asm volatile("mov %0, %%esp\n\t"
					"xor %%ebp, %%ebp" : : "r"(kernel_stack_top));
	
	glb_mboot_ptr = mboot_ptr_tmp + PAGE_OFFSET;
	
	kern_init();
}


void kern_init()
{
	
	console_clear();	
	//printc(c_black, c_light_red, "Welcome to K'OS!\n");	
	
	//初始化全局描述符号
	init_gdt();
	//printc(c_black, c_light_red, "Init gdt finished!\n");

	
	//物理内存初始化
	init_pmm();
	//printc(c_black, c_light_red, "Init pmm finished!\n");
	//show_pmm_status();	
	//show_memory_map();

	//虚拟内存初始化
	init_vmm();
	//printc(c_black, c_light_brown, "Init vmm finished!\n");	

	//中断初始化
	init_idt();
	//printc(c_black, c_light_brown, "Init idt finished!\n");
	
	//内存堆初始化
	init_heap();
	//test_heap();
	//while(1);
	
	//打开中断	
	//printc(c_black, c_red, "Interrupt is enable!\n");	

	//初始化时钟中断
	init_timer(100);

	//初始化键盘中断
	init_keyboard();

	//初始化TSS进程表
	init_tss();
	init_shell();

	//初始化系统调用
	init_sys_call();	

	//清屏
	console_clear();

	//初始化进程表
	init_proc_table();
	
	//初始化任务列表
	init_task_table();
	
	//新建程序
	//new_task_proc((uint32_t) tty_start);
	//new_task_proc((uint32_t) user_task_a);
	//new_task_proc((uint32_t) sys_task_a);
	//new_task_proc((uint32_t) keyboard_buffer_start);	
	
	//新建任务
	//tty进程
	add_new_task(SYS_TASK,  (uint32_t) tty_start, "TTY");
	
	//键盘读取任务
	add_new_task(SYS_TASK, (uint32_t) keyboard_buffer_start, "KEYBOARD");	

	//init进程
	add_new_task(USER_TASK, (uint32_t ) init_task, "INIT");
	
	//系统硬盘驱动进程
	add_new_task(SYS_TASK, (uint32_t) hd_task, "HD");
	
	//虚拟文件系统进程
	add_new_task(SYS_TASK, (uint32_t) fs_task, "FS");

	//内存管系统进程
	add_new_task(SYS_TASK, (uint32_t) mm_task, "MM");
	
	//用户进程A
	add_new_task(SYS_TASK, (uint32_t) debug_sys_task, "SYS");
	
	//系统进程B
	add_new_task(USER_TASK, (uint32_t) debug_user_task, "USER");	
	
	//系统进程send1
	//add_new_task(SYS_TASK, (uint32_t) debug_send_task);	
	
	//系统进程send2
	//add_new_task(SYS_TASK, (uint32_t) debug_send_task2);
	
	//系统进程Reveice
	//add_new_task(SYS_TASK, (uint32_t) debug_recv_task);

	//系统调用
	//add_new_task(SYS_TASK, (uint32_t) debug_sys_call);	
	
	//初始化进程
	task_line_schedule();
	init_schedule();
	
	while(1){
		asm volatile("hlt");
	}
}

