#ifndef INCLUDE_CONSOLE_H_
#define INCLUDE_CONSOLE_H_

#include "types.h"
#include "common.h"

typedef 
enum color{

	c_black 		= 	 0,
	c_blue			= 	 1,
	c_green 		= 	 2,
	c_cyan  		= 	 3,		//青色
	c_red			=	 4,
	c_magenta		=	 5,		//紫红
	c_brown			=	 6,
	c_light_grey	=	 7,
	c_dark_grey		=	 8,
	c_light_blue	=	 9,
	c_light_green	=	10,
	c_light_cyan	=	11,
	c_light_red		=	12,
	c_light_magenta =	13,
	c_light_brown	=	14,
	c_white			=	15	
	
}c_color_t;

//清屏函数
void console_clear();

//打印一个字符
void console_putc_color(char c, c_color_t bg, c_color_t font);

//打印一个字符串
void console_write(char *cstr);

//打印一个有颜色的字符串
void console_write_color(char *cstr, c_color_t bg, c_color_t font);

#endif
