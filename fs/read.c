#include "debug.h"
#include "fs.h"
#include "hd.h"
#include "string.h"

static uint8_t fs_rdbuf[FS_BUF_SIZE];
static int rd_lock = 0;

//读操作
void read_from_hd(int pos)
{
	msg_t msg;

	msg.pid = fs_pid;

	//消息类型
	msg.type = MSG_DEV_READ;
	
	msg.device = 0;
	
	//缓冲区
	msg.buf = fs_rdbuf;
	
	//位置
	msg.pos = pos;
	
	//缓冲区大小512字节
	msg.cnt = FS_BUF_SIZE;		

	//拷贝内容到缓冲区
	send_message(fs_pid, hd_pid, &msg);
	
	sleep(50);
}

void fs_read(int sector_no, void *buf, int buf_size)
{
	//临界资源枷锁
	while(rd_lock);
	
	rd_lock++;

	read_from_hd(sector_no + fs_root);
	memcpy((uint8_t *)buf, (uint8_t *) fs_rdbuf, buf_size);
	
	rd_lock--;
}
