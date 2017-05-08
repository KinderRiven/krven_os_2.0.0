#include "console.h"
#include "sys_write.h"

void sys_write(char *cstr)
{
	console_write(cstr);
}

void sys_alert_write_color(c_color_t bg, c_color_t font)
{
	alert_write_color(bg, font);
}
