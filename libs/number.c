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

int32_t cstr_to_int32(char *cstr)
{
	int i = 0;
	int flag = 1;
	int l = strlen(cstr);	
	int32_t ret = 0;

	if(cstr[i] == '-')
	{
		i++;
		flag = -1;
	}
	
	for(; i < l; i++)
	{
		ret = ret * 10 + (cstr[i] - '0');
	}
	
	return flag * ret;
}
