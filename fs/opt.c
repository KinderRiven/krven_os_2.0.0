#include "debug.h"
#include "fs.h"
#include "const.h"
#include "string.h"
#include "stdio.h"
#include "hd.h"
#include "msg.h"
#include "sys.h"

static uint8_t opt_buf[FS_BUF_SIZE];

//获得目录列表
void get_dir_entries(msg_t *msg)
{
	inode_t inode = find_inode(msg -> inode);
	
	int start_sector = inode.start_sect;
	int sector_num = inode.sect_num;
	int i;
	uint8_t *ptr = msg -> buf; 

	for(i = start_sector; i < start_sector + sector_num; i++)
	{
		fs_read(i, (void *)opt_buf, sizeof(opt_buf));
		
		//512
		memcpy(ptr, opt_buf, sizeof(opt_buf));	
		ptr += sizeof(opt_buf);	
	}

	msg_t msg_finish;
	send_message(fs_pid, msg -> pid, &msg_finish);

}
