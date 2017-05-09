#ifndef INCLUDE_MSG_H_
#define INCLUDE_MSG_H_

#include "types.h"

#define MESSAGE_SIZE 1024

typedef
struct msg_t{
	
	uint8_t message[MESSAGE_SIZE];	
	int	int_no;
	
}msg_t;


#endif
