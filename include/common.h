#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include "types.h"

//向指定端口读一个byte
void outb(uint16_t	port, uint8_t value);

//向指定端口写一个字节
uint8_t inb(uint16_t port);

//向指定端口写一个字
uint16_t inw(uint16_t port);

//从指定端口读长度为n字节的内容
void port_read(uint16_t port, void *buf, int n);

//向指定端口写长度为n字节的内容
void port_write(uint16_t port, void *buf, int n);

#endif
