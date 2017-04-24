#ifndef INCLUDE_STDIO_H_
#define INCLUDE_STDIO_H_

#include "types.h"
#include "console.h"

void printf(const char *format, ...);
void printc(c_color_t back, c_color_t fore, const char *format, ...);

void printk(const char *format, ...);
void printk_color(c_color_t back, c_color_t fore, const char *format, ...);

#endif
