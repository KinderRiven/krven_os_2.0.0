#include "keyboard.h"
#include "idt.h"
#include "stdio.h"
#include "common.h"

void keyboard_callback(pt_regs *regs){
	
	//printc(c_black, c_red, "Keyboard Interrupt!\n");
	uint8_t byte = inb(0x60);

	printf("%d\n", byte);	

}

void init_keyboard(){
	
	register_interrupt_handler(IRQ1, keyboard_callback);

}
