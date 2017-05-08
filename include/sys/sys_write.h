#ifndef INCLUDE_SYS_WRITE_H_
#define INCLUDE_sYS_WRITE_H_

#include "types.h"
#include "console.h"

void write(char *cstr);
void sys_write(char *cstr);

void alert_write_color(c_color_t bg, c_color_t font);
void sys_alert_write_color(c_color_t bg, c_color_t font);

void write_color(char *cstr, c_color_t bg, c_color_t font);
void sys_write_color(char *cstr, c_color_t bg, c_color_t font);

void write_clear();
void sys_write_clear();

#endif
