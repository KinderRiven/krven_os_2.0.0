#include "hd.h"
#include "common.h"
#include "stdio.h"
#include "sys.h"
#include "msg.h"
#include "idt.h"

pid_t hd_pid;

//向寄存器发送数据
static void hd_cmd_out(struct hd_cmd *cmd);

//等待一个设备中断
static void wait_interrupt();

//硬盘中断发生的时候进行的处理
void hd_handler()
{
	
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
		recv_message(ANY, hd_pid, &msg);
		switch(msg.type)
		{
			case MSG_DEV_OPEN:
				printf("Hard disk task start to running!\n");
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

	hd_cmd_out(&cmd);	
	wait_interrupt();	

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

}
