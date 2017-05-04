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
#include "sched.h"
#include "task.h"
#include "keyboard.h"
#include "tty.h"
#include "proc.h"
#include "tss.h"

void kern_init();

multiboot_t *glb_mboot_ptr;

//内核栈
char kern_stack[STACK_SIZE];

uint32_t kern_stack_top;

//临时页目录和页表
__attribute__((section(".init.data"))) pgd_t *pgd_tmp  = (pgd_t *) 0x1000;
__attribute__((section(".init.data"))) pgd_t *pte_low  = (pgd_t *) 0x2000;
__attribute__((section(".init.data"))) pgd_t *pte_high = (pgd_t *) 0x3000;

__attribute__((section(".init.text")))
void kern_entry(){

	pgd_tmp[0] = (pgd_t) pte_low | PAGE_PRESENT | PAGE_WRITE;
	pgd_tmp[PGD_INDEX(PAGE_OFFSET)] = (pgd_t) pte_high | PAGE_PRESENT | PAGE_WRITE;
	
	int i = 0;
	
	for( i = 0; i < 1024; i++){
		pte_low[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
	}

	for( i = 0; i < 1024; i++){
		pte_high[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
	}

	//开启临时分页	
	asm volatile("mov %0, %%cr3" : : "r" (pgd_tmp));
	
	uint32_t cr0;
	asm volatile("mov %%cr0, %0" : "=r" (cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0" : : "r" (cr0));

	//切换到内核栈
	kern_stack_top = ((uint32_t)kern_stack + STACK_SIZE) & 0xFFFFFFF0;
	asm volatile("mov %0, %%esp\n\t"
					"xor %%ebp, %%ebp" : : "r"(kern_stack_top));
	
	glb_mboot_ptr = mboot_ptr_tmp + PAGE_OFFSET;
	kern_init();
}

int thread_proc_b(void *arg){
	
	while(1){
		printc(c_black, c_red, "B");
	}
}

int thread_proc_a(void *arg){
	
	while(1){
		printc(c_black, c_blue, "A");
	}

}	

void kern_init()
{
	console_clear();
	printc(c_black, c_light_red, "Welcome to K'OS!\n");	
	
	//初始化全局描述符号
	init_gdt();
	printc(c_black, c_light_red, "Init gdt finished!\n");

	
	//物理内存初始化
	init_pmm();
	printc(c_black, c_light_red, "Init pmm finished!\n");
	show_pmm_status();	
	show_memory_map();

	//虚拟内存初始化
	init_vmm();
	printc(c_black, c_light_brown, "Init vmm finished!\n");	

	init_tss();
	init_proc(0);

	//中断初始化
	init_idt();
	printc(c_black, c_light_brown, "Init idt finished!\n");
	
	//内存堆初始化
	init_heap();
	//test_heap();

	//调度进程初始化
	init_sched();	
	
	//键盘进程打开
	init_keyboard();	

	//打开中断	
	enable_intr();
	printc(c_black, c_red, "Interrupt is enable!\n");	

	kernel_thread(keyboard_buffer_handler, NULL);
	
	/**
	 * kernel_thread(thread_proc_b, NULL);	
	 * kernel_thread(thread_proc_a, NULL);
	 * kernel_thread(thread_proc_c, NULL);	
	 **/
	
	//init_tss();
	//init_proc(0);


	init_timer(200);
	console_clear();

	//中断窗口
	init_tty();
	kernel_thread(tty_thread, NULL);
	
	while(1){
		asm volatile("hlt");
	}
}

