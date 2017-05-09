#ifndef INCLUDE_MSG_H_
#define INCLUDE_MSG_H_

#include "types.h"

#define MESSAGE_SIZE 1024

typedef
struct msg_t{
	
	char message[MESSAGE_SIZE];	
	int	int_no;
	
}msg_t;


#endif
