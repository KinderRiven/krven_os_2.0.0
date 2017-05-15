#include "idt.h"
#include "string.h"
#include "stdio.h"
#include "const.h"
#include "sys.h"

idt_entry_t idt_entries[256];

idt_ptr_t idt_ptr;

interrupt_handler_t interrupt_handlers[256];

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);

extern void idt_flush(uint32_t);

void isr_handler(proc_regs_t *regs){

	
	printk_color(c_black, c_red, "\ngs: 0x%x ,fs: 0x%x ,es: 0x%x ,ds: 0x%x\n", regs -> gs, regs -> fs, regs -> es, regs -> ds);	
	printk_color(c_black, c_red, "int_no: %d , err_code: %d\n", regs -> int_no, regs -> err_code);
	
	printk_color(c_black, c_red, "eip: 0x%x ,cs: 0x%x ,eflags: 0x%x ,esp: 0x%x ,ss: 0x%x\n", 
			regs -> eip, regs -> cs, regs -> eflags, regs -> esp, regs -> ss);
	printk_color(c_black, c_red, "retaddr : %d\n", regs -> retaddr);
	
	while(1);
}

void irq_handler(proc_regs_t *regs){

	//发送重设信号
	
	//从片
	if(regs -> int_no >= 40){
		outb(0xA0, 0x20);
	}
	
	//主片
	outb(0x20, 0x20);
	
	if(interrupt_handlers[regs -> int_no]){
		interrupt_handlers[regs -> int_no](regs);
	}
}

void init_idt(){

	// 初始化芯片	
	// 重新映射IRQ表
	// 主片端口 0x20 0x21
	// 从片端口 0xA0 0xA1
	
	// 初始化主片、从片
	// 0001 0001
	outb(0x20, 0x11);
	outb(0xA0, 0x11);

	// 主片IRQ从0x20（32）号中断开始
	outb(0x21, 0x20);
	
	// 从片IRQ从0x28（40）号中断开始
	outb(0xA1, 0x28);

	// 设置主片IR2引脚连接从片
	outb(0x21, 0x04);
	
	// 告诉从片输出引脚和主片IR2号相连
	outb(0xA1, 0x02);
	
	// 设置从片和主片按照8086方式工作
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	
	// 设置从片主片允许中断
	outb(0x21, 0x0);
	outb(0xA1, 0x0);
	
	bzero((uint8_t *)&interrupt_handlers, sizeof(interrupt_handler_t) * 256);
	
	idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
	idt_ptr.base  = (uint32_t)&idt_entries;
	
	bzero((uint8_t *)&idt_entries, sizeof(idt_entry_t) * 256);

	// 0-32:  用于 CPU 的中断处理
	idt_set_gate( 0, (uint32_t)isr0,  KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate( 1, (uint32_t)isr1,  KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate( 2, (uint32_t)isr2,  KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate( 3, (uint32_t)isr3,  KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate( 4, (uint32_t)isr4,  KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate( 5, (uint32_t)isr5,  KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate( 6, (uint32_t)isr6,  KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate( 7, (uint32_t)isr7,  KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate( 8, (uint32_t)isr8,  KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate( 9, (uint32_t)isr9,  KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(10, (uint32_t)isr10, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(11, (uint32_t)isr11, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(12, (uint32_t)isr12, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(13, (uint32_t)isr13, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(14, (uint32_t)isr14, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(15, (uint32_t)isr15, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(16, (uint32_t)isr16, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(17, (uint32_t)isr17, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(18, (uint32_t)isr18, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(19, (uint32_t)isr19, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(20, (uint32_t)isr20, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(21, (uint32_t)isr21, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(22, (uint32_t)isr22, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(23, (uint32_t)isr23, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(24, (uint32_t)isr24, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(25, (uint32_t)isr25, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(26, (uint32_t)isr26, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(27, (uint32_t)isr27, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(28, (uint32_t)isr28, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(29, (uint32_t)isr29, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(30, (uint32_t)isr30, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(31, (uint32_t)isr31, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);

	idt_set_gate(32, (uint32_t)irq0, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(33, (uint32_t)irq1, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(34, (uint32_t)irq2, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(35, (uint32_t)irq3, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(36, (uint32_t)irq4, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(37, (uint32_t)irq5, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(38, (uint32_t)irq6, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(39, (uint32_t)irq7, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(40, (uint32_t)irq8, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(41, (uint32_t)irq9, KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(42, (uint32_t)irq10,KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(43, (uint32_t)irq11,KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(44, (uint32_t)irq12,KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(45, (uint32_t)irq13,KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(46, (uint32_t)irq14,KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);
	idt_set_gate(47, (uint32_t)irq15,KERNEL_CS_INDEX, TYPE_386IGate | DPL_0);

	//系统调用
	idt_set_gate(48, (uint32_t)sys_call, KERNEL_CS_INDEX, TYPE_386IGate | DPL_1);
	
	// 255 将来用于实现系统调用
	idt_set_gate(255, (uint32_t)isr255, KERNEL_CS_INDEX, TYPE_386IGate | DPL_1);


	// 更新设置中断描述符表
	idt_flush((uint32_t)&idt_ptr);
	
}

void register_interrupt_handler(uint8_t n, interrupt_handler_t h){

	interrupt_handlers[n] = h;

}

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags){

	idt_entries[num].base_lo = base & 0xFFFF;
	idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

	idt_entries[num].sel = sel;
	idt_entries[num].always0 = 0;

	idt_entries[num].flags = flags;
}

//开启中断
void enable_intr(){
	asm volatile("sti");
}
