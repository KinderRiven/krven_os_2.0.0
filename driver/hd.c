#include "hd.h"
#include "const.h"
#include "proc.h"
#include "debug.h"
#include "common.h"
#include "stdio.h"
#include "sys.h"
#include "msg.h"
#include "idt.h"

pid_t hd_pid;

//打印磁盘信息
static void hd_print_info(uint16_t *hdinfo);

//hd缓冲区
static uint8_t hdbuf[SECTOR_SIZE * 2]; 

//向寄存器发送数据
static void hd_cmd_out(struct hd_cmd *cmd);

//等待一个设备中断
static void wait_interrupt();

//硬盘中断发生的时候进行的处理
void hd_handler()
{
	//解除一个硬盘中断
	msg_send_interrupt(hd_pid, HD_INTERRUPT);	
}

//初始化硬盘驱动程序
void init_hd()
{
	register_interrupt_handler(IRQ14, hd_handler);	
}

void hd_task()
{

	msg_t msg;
	
	//初始化硬盘驱动程序
	init_hd();

	//获得硬盘驱动程序pid
	get_proc_pid(&hd_pid);	

	while(1)
	{
		//开始接收硬盘请求
		recv_message(ANY, hd_pid, &msg);
		
		//根据message的type执行不同的
		switch(msg.type)
		{
			case MSG_DEV_OPEN:
				//printf("Hard disk task start to running!");
				hd_identify(msg.device);
				break;
			default:
				printf("Hard disk task caused an error [0x%X]\n", msg.type);
				break;
		}
	}
}

void hd_identify(uint8_t drive)
{
	struct hd_cmd cmd;
	
	//lba模式 选择主设备
	cmd.device = MAKE_DEVICE_REG(0, drive, 0);

	cmd.command = ATA_IDENTIFY;

	//进行硬盘操作
	hd_cmd_out(&cmd);	

	//等待一个硬盘读取结束中断	
	wait_interrupt();

	//读取内容
	port_read(REG_DATA, hdbuf, SECTOR_SIZE);
	
	//打印磁盘信息
	hd_print_info((uint16_t *)hdbuf);
}


//打印硬盘信息
static void hd_print_info(uint16_t *hdinfo)
{
	int i, k;

	char s[64];

	struct iden_info_ascii {
		int idx;
		int len;
		char * desc;
	} iinfo[] = {{10, 20, "HD SN"}, /* Serial number in ASCII */
		     {27, 40, "HD Model"} /* Model number in ASCII */ };

	for (k = 0; k < sizeof(iinfo)/sizeof(iinfo[0]); k++) {
		char * p = (char*)&hdinfo[iinfo[k].idx];
		for (i = 0; i < iinfo[k].len/2; i++) {
			s[i*2+1] = *p++;
			s[i*2] = *p++;
		}
		s[i*2] = 0;
		printf("\n[%s", iinfo[k].desc);
		printf("] : %s", s);
	}

	
	int capabilities = hdinfo[49];
	printf("\n[LBA supported] : %s",
	       (capabilities & 0x0200) ? "Yes" : "No");
	

	int cmd_set_supported = hdinfo[83];
	printf("\n[LBA48 supported] : %s",
	       (cmd_set_supported & 0x0400) ? "Yes" : "No");

	int sectors = ((int)hdinfo[61] << 16) + hdinfo[60];
	printf("\n[HD size] : %dMB", sectors * 512 / 1000000);	

	printf("\n");
}

static void hd_cmd_out(struct hd_cmd * cmd)
{
	outb(REG_DEV_CTRL, 0);

	outb(REG_FEATURE, cmd -> feature);		
	outb(REG_COUNT, cmd -> count);
	outb(REG_LAB_LOW, cmd -> lba_low);
	outb(REG_LAB_MID, cmd -> lba_mid);
	outb(REG_LAB_HIGH, cmd -> lba_high);	
	outb(REG_DEVICE, cmd -> device);
	
	outb(REG_CMD, cmd -> command);
}

static void wait_interrupt()
{
	//接收一个来自磁盘的中断
	recv_interrupt(hd_pid,	HD_INTERRUPT);
}
