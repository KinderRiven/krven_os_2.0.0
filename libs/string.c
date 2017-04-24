#include "string.h"

void memcpy(void *dest, const void *src, uint32_t len){

	if(len <= 0)
		return;

	uint8_t *_dest = (uint8_t *)dest;
	uint8_t *_src  = (uint8_t *)src;
	
	for(; len != 0; len --){
		*_dest++ = *_src++;
	}
}

void memset(void *dest,	uint8_t val, uint32_t len){

	if(len <= 0)
		return;
	
	uint8_t *_dest = (uint8_t *)dest;

	for(; len != 0; len--){
		*_dest++ = val;
	}
}

void bzero(void *dest, uint32_t len){

	memset(dest, 0, len);

}

int strcmp(const char *str1, const char *str2){

	int _i, _len1, _len2;
	
	_len1 = strlen(str1);
	_len2 = strlen(str2);
	
	for(_i = 0; _i < _len1 || _i < _len2; _i++){
		int _tmp = str1[_i] - str2[_i];
		if(_tmp != 0)
			return _tmp;
	}
	return 0;

}

int strlen(const char *str){

	int _i;
	for(_i = 0; str[_i] != '\0'; _i++);	
	return _i + 1;
}


