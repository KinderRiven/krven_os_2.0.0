#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_

#include "types.h"

void outb(uint16_t	port, uint8_t value);

uint8_t inb(uint16_t port);

uint16_t inw(uint16_t port);

#endif
