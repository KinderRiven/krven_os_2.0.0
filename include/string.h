#ifndef INCLUDE_STRING_H_
#define INCLUDE_STRING_H_

#include "types.h"

void memcpy(void *dest, const void *src, uint32_t len);

void memset(void *dest,	uint8_t val, uint32_t len);

void bzero(void *dest, uint32_t len);

int strcmp(const char *str1, const char *str2);

int strlen(const char *str);

#endif
