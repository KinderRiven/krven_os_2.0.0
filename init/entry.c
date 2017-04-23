#include "types.h"
#include "gdt.h"

int kernel_entry()
{
	gdt_init();

	uint8_t *input = (uint8_t *)0xB8000;
	uint8_t color = (0 << 4) | (15 & 0x0F);
	int i = 0;
	for(i = 0; i < 10; i++)
	{
		*input++ = 'H';
		*input++ = color;
	}
	
	return 0;
}

