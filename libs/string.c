#include "string.h"

inline void memcpy(uint8_t *dest, const uint8_t *src, uint32_t len){

	for(; len != 0; len --){
		*dest++ = *src++;
	}
}

inline void memset(void *dest,	uint8_t val, uint32_t len){

	uint8_t *dst = (uint8_t *)dest;

	for(; len != 0; len--){
		*dst++ = val;
	}
}

inline void bzero(void *dest, uint32_t len){

	memset(dest, 0, len);

}

inline int strcmp(const char *str1, const char *str2){

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

inline int strlen(const char *str){

	int i;
	for(i = 0; str[i] != '\0'; i++);	
	return i;
}

inline void strcpy(char *str1, const char *str2)
{
	int l = strlen(str2);
	int i;

	for(i = 0; i <= l; i++)
	{
		str1[i] = str2[i];
	}
}

inline void strcat(char *str1, const char *str2)
{
	int l1 = strlen(str1);
	int l2 = strlen(str2);
	int i, j;

	for(i = l1, j = 0; j < l2; i++, j++)
	{
		str1[i] = str2[j];
	}
	str1[i] = '\0';
}
