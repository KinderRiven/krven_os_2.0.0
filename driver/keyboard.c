#include "keyboard.h"
#include "idt.h"
#include "stdio.h"
#include "common.h"
#include "console.h"
#include "tty.h"

#define KEY_MAP_SIZE 256

char tab_blank[] = "    ";

//键盘的扫描码映射
static keymap_t keymap[KEY_MAP_SIZE];

//键盘的扫描码映射数量
static uint8_t keymap_size = 0;

//键盘输入缓冲
static keyboard_buffer_t keyboard_buffer;

//建立键盘的扫描码映射数量
static void set_keyboard_map(int8_t ascii, uint8_t make_1, uint8_t make_2, uint8_t break_1, uint8_t break_2);

//是否打开大小写锁定
uint8_t caps_lock = 0;

//是否按住了shift
uint8_t shift = 0;

void keyboard_handler(){
	
	uint8_t byte = inb(0x60);
	//printf("%0x\n", byte);
	
	if(keyboard_buffer.count < BUFFER_SIZE){
	
		//加入到缓冲区
		*keyboard_buffer.tail++ = byte;
		keyboard_buffer.count++;
		
		//到达缓冲区的尾部就移动到开头
		if(keyboard_buffer.tail == keyboard_buffer.buf + BUFFER_SIZE){
			keyboard_buffer.tail = keyboard_buffer.buf;
		}
	}	
}



void keyboard_buffer_read(){

	int i;

	if(keyboard_buffer.count > 0){

		uint8_t byte1 = *keyboard_buffer.head++;
		uint8_t byte2 = 0;
		keyboard_buffer.count --;
		
		//双字节功能键
		if(byte1 == 0xE0)
		{
			byte2 = *keyboard_buffer.head++;
			keyboard_buffer.count --;
			return;
		}
		//功能键
		switch(byte1){
			
			//大小写锁定		
			case 0x3A :
				caps_lock = (caps_lock == 0) ? 1 : 0;
				return;
			//shift
			case 0x2A :
				case 0x36 :
				shift = 1;
				return;
			case 0xAA:
			case 0xB6:
				shift = 0;
				return;
			//退格
			case 0x0E:
				//console_write("\b \b");
				tty_putc(current_tty, '\b');
				return;
			//Tab
			case 0x0F:
				//console_write(tab_blank);
				tty_write(current_tty, tab_blank);	
				return;
			default:
				break;
		}
	
		//字符打印
		for(i = 0; i < keymap_size; i++)
		{
			if(keymap[i].make_1 == byte1 && keymap[i].make_2 == byte2)
			{
				int8_t ascii = keymap[i].ascii;
				//大小写
				if(ascii >= 'A' && ascii <= 'Z' && caps_lock == 0)
					ascii = ascii - 'A' + 'a';

				if(shift){
					switch(ascii)
					{
						case '1':
							ascii = '!'; 
							break;
						case '2':
							ascii = '@';
							break;
						case '3':
							ascii = '#';
							break;
						case '4':
							ascii = '$';
							break;
						case '5':
							ascii = '^';
							break;
						case '6':
							ascii = '^'; 
							break;
						case '7':
							ascii = '&';
							break;
						case '8':
							ascii = '*';
							break;
						case '9':
							ascii = '(';
							break;
						case '0':
							ascii = ')';
							break;
						case '-':
							ascii = '_'; 
							break;
						case '=':
							ascii = '+';
							break;
						case '[':
							ascii = '{';
							break;
						case ']':
							ascii = '}';
							break;
						case ';':
							ascii = ':';
							break;
						case '\'':
							ascii = '"'; 
							break;
						case ',':
							ascii = '<';
							break;
						case '.':
							ascii = '>';
							break;
						case '/':
							ascii = '?';
							break;
						case '`':
							ascii = '~';
							break;
						case '\\':
							ascii = '|'; 
							break;
						default:
							break;	
					}
				}
				//console_putc(ascii);
				tty_putc(current_tty, ascii);
			}	
		}
	}
	
	return;
}


void keyboard_buffer_start(){

	while(1){	
		keyboard_buffer_read();		
	}

}

void init_keyboard(){
	
	//中断函数注册
	register_interrupt_handler(IRQ1, keyboard_handler);

	//缓冲区初始化	
	keyboard_buffer.count = 0;
	keyboard_buffer.head = keyboard_buffer.buf;
	keyboard_buffer.tail = keyboard_buffer.buf;	

	//扫描表初始化
	//字母
	set_keyboard_map('A', 0x1E, 0, 0x9E, 0);
	set_keyboard_map('B', 0x30, 0, 0xB0, 0);
	set_keyboard_map('C', 0x2E, 0, 0xAE, 0);
	set_keyboard_map('D', 0x20, 0, 0xA0, 0);
	set_keyboard_map('E', 0x12, 0, 0x92, 0);
	set_keyboard_map('F', 0x21, 0, 0xA1, 0);
	set_keyboard_map('G', 0x22, 0, 0xA2, 0);
	set_keyboard_map('H', 0x23, 0, 0xA3, 0);
	set_keyboard_map('I', 0x17, 0, 0x97, 0);
	set_keyboard_map('J', 0x24, 0, 0xA4, 0);
	set_keyboard_map('K', 0X25, 0, 0XA5, 0);
	set_keyboard_map('L', 0x26, 0, 0xA6, 0);
	set_keyboard_map('M', 0x32, 0, 0xB2, 0);
	set_keyboard_map('N', 0x31, 0, 0xB1, 0);
	set_keyboard_map('O', 0x18, 0, 0x98, 0);
	set_keyboard_map('P', 0x19, 0, 0x99, 0);
	set_keyboard_map('Q', 0x10, 0, 0x19, 0);
	set_keyboard_map('R', 0x13, 0, 0x93, 0);
	set_keyboard_map('S', 0x1F, 0, 0x9F, 0);
	set_keyboard_map('T', 0x14, 0, 0x94, 0);
	set_keyboard_map('U', 0x16, 0, 0x96, 0);
	set_keyboard_map('V', 0x2F, 0, 0xAF, 0);
	set_keyboard_map('W', 0x11, 0, 0x91, 0);
	set_keyboard_map('X', 0x2D, 0, 0xAD, 0);
	set_keyboard_map('Y', 0x15, 0, 0x95, 0);
	set_keyboard_map('Z', 0x2C, 0, 0xAC, 0);
	
	//数字	
	set_keyboard_map('0', 0x0B, 0, 0x8B, 0); 	
	set_keyboard_map('1', 0x02, 0, 0x82, 0); 	
	set_keyboard_map('2', 0x03, 0, 0x83, 0); 	
	set_keyboard_map('3', 0x04, 0, 0x84, 0); 	
	set_keyboard_map('4', 0x05, 0, 0x85, 0); 	
	set_keyboard_map('5', 0x06, 0, 0x86, 0); 	
	set_keyboard_map('6', 0x07, 0, 0x87, 0); 	
	set_keyboard_map('7', 0x08, 0, 0x88, 0); 	
	set_keyboard_map('8', 0x09, 0, 0x89, 0); 	
	set_keyboard_map('9', 0x0A, 0, 0x8A, 0); 	

	//符号
	set_keyboard_map('-', 0x0C, 0, 0x8C, 0);
	set_keyboard_map('=', 0x0D, 0, 0x8D, 0);
	set_keyboard_map('/', 0x35, 0, 0xB5, 0);
	set_keyboard_map(',', 0x33, 0, 0xB3, 0);
	set_keyboard_map('.', 0x34, 0, 0xB4, 0);
	set_keyboard_map(';', 0x27, 0, 0xA7, 0);
	set_keyboard_map('\'', 0x28, 0, 0xA8, 0);
	set_keyboard_map('[', 0x1A, 0, 0x9A, 0);
	set_keyboard_map(']', 0x1B, 0, 0x9B, 0);
	set_keyboard_map('`', 0x29, 0, 0x89, 0);
	set_keyboard_map('\\', 0x2B, 0, 0xAB, 0);
		
	//退格符
	set_keyboard_map('\b',0x0E, 0, 0x8E, 0);
	//tab
	set_keyboard_map('\t',0x0F, 0, 0x8F, 0);
	//空格
	set_keyboard_map(' ', 0x39, 0, 0xB9, 0);	
	//caps_lock
	set_keyboard_map(' ', 0x3A, 0, 0xBA, 0);
	//L-shift
	set_keyboard_map(' ', 0x2A, 0, 0xAA, 0);
	//R-shift
	set_keyboard_map(' ', 0x36, 0, 0xB6, 0);
	//回车
	set_keyboard_map('\n', 0x1C, 0, 0x9C, 0);
	
	//F1 F2 F3
	set_keyboard_map(-1, 0x3B, 0, 0xBB, 0);
	set_keyboard_map(-2, 0x3C, 0, 0xBC, 0);
	set_keyboard_map(-3, 0x3D, 0, 0xBD, 0);
}

static void set_keyboard_map(int8_t ascii, uint8_t make_1, uint8_t make_2, uint8_t break_1, uint8_t break_2){

	keymap[keymap_size].ascii = ascii;
	keymap[keymap_size].make_1 = make_1;
	keymap[keymap_size].make_2 = make_2;
	keymap[keymap_size].break_1 = break_1;
	keymap[keymap_size].break_2 = break_2;
	keymap_size ++;

}
