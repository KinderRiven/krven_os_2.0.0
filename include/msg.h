#ifndef INCLUDE_MSG_H_
#define INCLUDE_MSG_H_

#include "types.h"

#define MESSAGE_SIZE 128

#define MSG_INTERRUPT	0xFF
#define MSG_DEV_OPEN	0x01

typedef
struct msg_t{
	
	uint8_t type;
	uint8_t device;

}msg_t;


#endif
