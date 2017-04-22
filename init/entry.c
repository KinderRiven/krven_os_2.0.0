#include "types.h"

int g_eax = 1;
int g_ebx = 2;
int g_ecx;

int kernel_entry()
{
	uint8_t color = (0 << 4) | (15 & 0x0F);
	uint8_t *mm = (uint8_t *)0xB8000;
	int i = 0;
	
	for(i = 0; i < 10; i++)
	{
		*mm++ = 'H';
		*mm++ = color;
	}
	return 0;
}
