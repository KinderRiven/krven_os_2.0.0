#include "fs.h"
#include "hd.h"
#include "debug.h"
#include "string.h"

static uint8_t fs_wtbuf[FS_BUF_SIZE];
static int wt_lock = 0;

//写操作
void write_to_hd(int pos)
{

	msg_t msg;

	msg.pid = fs_pid;

	//消息类型
	msg.type = MSG_DEV_WRITE;

	//设别号	
	msg.device = 0;
	
	//缓冲区
	msg.buf = fs_wtbuf;
	
	//位置
	msg.pos = pos;
	
	//缓冲区大小512字节
	msg.cnt = FS_BUF_SIZE;		

	send_message(fs_pid, hd_pid, &msg);

	//这里应该等待一下
	sleep(50);
}

void fs_write(int sector_no, void *buf, int buf_size)
{
	//printf("%d %d\n", sector_no, wt_lock);
	while(wt_lock);
	
	wt_lock++;

	memcpy((uint8_t *)fs_wtbuf, (uint8_t *) buf, buf_size);
	write_to_hd(fs_root + sector_no);	
	
	//printf("%d %d\n", sector_no, wt_lock);
	wt_lock--;
}
