#include "number.h"
#include "string.h"

uint32_t cstr_to_uint32(char *cstr)
{

	int i, l = strlen(cstr);
	uint32_t ret = 0;

	for(i = 0; i < l; i++)
	{
		ret = ret * 10 + (cstr[i] - '0');
	}
	
	return ret;
}
