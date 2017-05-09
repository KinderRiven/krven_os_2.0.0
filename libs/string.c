#include "string.h"

inline void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len){

	if(len <= 0)
		return;
	
	for(; len != 0; len --){
		*dest++ = *src++;
	}
}

void memset(void *dest,	uint8_t val, uint32_t len){

	if(len <= 0)
		return;
	
	uint8_t *dst = (uint8_t *)dest;

	for(; len != 0; len--){
		*dst++ = val;
	}
}

void bzero(void *dest, uint32_t len){

	memset(dest, 0, len);

}

int strcmp(const char *str1, const char *str2){

	int i, len1, len2;
	
	len1 = strlen(str1);
	len2 = strlen(str2);
	
	for(i = 0; i < len1 || i < len2; i++){
		int tmp = str1[i] - str2[i];
		if(tmp != 0)
			return tmp;
	}
	return 0;

}

int strlen(const char *str){

	int i;
	for(i = 0; str[i] != '\0'; i++);	
	return i + 1;
}


