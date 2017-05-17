#include "hd.h"
#include "const.h"
#include "string.h"
#include "proc.h"
#include "debug.h"
#include "common.h"
#include "stdio.h"
#include "sys.h"
#include "msg.h"
#include "idt.h"

//硬盘的pid号
pid_t hd_pid;

//分区表
hd_part_t hd_part_table[MAX_HD_PART];
int hd_part_num = 0;

//打印磁盘信息
static void hd_print_info(uint16_t *hdinfo);

//hd缓冲区
static uint8_t hdbuf[SECTOR_SIZE * 2]; 

//向寄存器发送数据
static void hd_cmd_out(hd_cmd_t *cmd);

//等待一个设备中断
static void wait_interrupt();

static void init_hd();

static void hd_read(msg_t *msg);

static void hd_write(msg_t *msg);

//根据设备的次设备号获得驱动号
static void hd_open(msg_t *msg);

static void hd_info(msg_t *msg);

//传入驱动器号执行命令
static void hd_identify(int drive);

//获得分区表信息
static void hd_get_part_table(int drive, int base_lba, int first_extra_lba);

static void hd_get_part(int drive, int sector_no);

static void print_part(hd_part_t *hd_part);

//扇区16进制内容输出
static void print_hdbuf_hex(int start, int size);

//打印一段内存区域

static void print_hdbuf_hex(int start, int size)
{
	uint8_t *src = hdbuf + start;
	int i;

	//打印一个扇区
	for(i = 0; i < size; i++)
	{
		printf("%02x ", *src++);
		
		if((i + 1) % 16 == 0) 
			printf("\n");
	}

}

//硬盘中断发生的时候进行的处理
void hd_handler()
{
	//printk("Hard disk interrupt happened!\n");
	//解除一个硬盘中断
	msg_send_interrupt(hd_pid, HD_INTERRUPT);	

	//解除阻塞
	inb(0x1F7);
}

//初始化硬盘驱动程序
static void init_hd()
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
		
		//根据message的type执行不同的操作指令
		switch(msg.type)
		{
			//设备开启
			//用于获得硬盘信息和分区信息
			case MSG_DEV_OPEN:
				hd_open(&msg);
				break;
		
			//硬盘读写
			case MSG_DEV_READ:
				hd_read(&msg);
				break;
			case MSG_DEV_WRITE:
				hd_write(&msg);
				break;			
			//获得某一个分区信息
			case MSG_DEV_INFO:
				hd_info(&msg);
				break;
			default:
				printf("Hard disk task caused an error [0x%X]\n", msg.type);
				break;
		}
	}
}

static void hd_info(msg_t *msg)
{
	int i = 0;

	for(i = 0; i < hd_part_num; i++)
	{
		//找到对应的硬盘type
		if(hd_part_table[i].type == msg -> device)
		{
			memcpy(msg -> buf, (uint8_t *) &hd_part_table[i], sizeof(hd_part_table[i]));
			return;
		}
	}
			
}

static void hd_read(msg_t *msg)
{
	hd_cmd_t cmd;

	uint32_t sector_no = msg -> pos;
	int drive = msg -> device;	

	//cmd.data
	cmd.feature  = 0;
	cmd.count 	 = (msg -> cnt + SECTOR_SIZE - 1) / SECTOR_SIZE;
	cmd.lba_low  = sector_no & 0xFF;
	cmd.lba_mid  = (sector_no >> 8) & 0xFF;
	cmd.lba_high = (sector_no >> 16) & 0xFF;
	
	//lba模式 
	//lba = 1 (LBA mod)  
	//drv = 0 
	//lba_highest 
	cmd.device = MAKE_DEVICE_REG(1, drive, (sector_no >> 24) & 0xF);

	//Commnad
	cmd.command = ATA_READ;

	//进行硬盘操作
	hd_cmd_out(&cmd);	

	//等待硬盘数据准备
	wait_interrupt();

	port_read(REG_DATA, hdbuf, msg -> cnt);
	memcpy(msg -> buf, hdbuf, msg -> cnt);

	//发送一个read完成的的消息	
	msg_t finish_msg;
	send_message(hd_pid, msg -> pid, &finish_msg); 

}

static void hd_write(msg_t *msg)
{
	hd_cmd_t cmd;
	
	uint32_t sector_no = msg -> pos;
	int drive = msg -> device;	

	//cmd.data
	cmd.feature  = 0;
	cmd.count 	 = (msg -> cnt + SECTOR_SIZE - 1) / SECTOR_SIZE;
	cmd.lba_low  = sector_no & 0xFF;
	cmd.lba_mid  = (sector_no >> 8) & 0xFF;
	cmd.lba_high = (sector_no >> 16) & 0xFF;
	
	//lba模式 
	//lba = 1 (LBA mod)  
	//drv = 0 
	//lba_highest 
	cmd.device = MAKE_DEVICE_REG(1, drive, (sector_no >> 24) & 0xF);

	//Commnad
	cmd.command = ATA_WRITE;

	//进行硬盘操作
	hd_cmd_out(&cmd);	

	//printf("%x %d\n", msg -> buf, msg -> cnt);

	port_write(REG_DATA, msg -> buf, msg -> cnt);

	//写完发生一个中断
	wait_interrupt();

	//发送一个结束消息
	msg_t finish_msg;
	send_message(hd_pid, msg -> pid, &finish_msg); 
}

//根据设别的次设别号获取驱动器号
static void hd_open(msg_t *msg)
{
	//只有一块硬盘
	//获取硬盘信息
	if(msg -> debug){
		hd_identify(0);	
	}

	//获得分区信息
	hd_get_part_table(0, 0, 0);

	//需要打印调试信息的话
	if(msg -> debug){
		hd_print_part();
	}	

}

void hd_print_part()
{
	int i;
	for(i = 0; i < hd_part_num; i++)
	{
		print_part(&hd_part_table[i]);
	}
}

static void print_part(hd_part_t *hd_part)
{
	printf("[Status] : 0x%02x ", hd_part -> status); 
	printf("[Type] : 0x%02x ", hd_part -> type);
	printf("[LBA] : %7d ", hd_part -> start_lba);	
	printf("[Size] : %7d\n", hd_part -> sector_num);
}

//DFS遍历
//启动号 + 基地址
//第一个分区是相对于当前的lba
//第二个分区是相对于第一个拓展分区
static void hd_get_part_table(int drive, int base_lba, int first_extra_lba)
{

	int i;
	
	hd_get_part(drive, base_lba);
	
	uint8_t *src = hdbuf + PART_TABLE_LOCAL;

	for(i = 0; i < 4; i++){

		memcpy((uint8_t *)&hd_part_table[hd_part_num], src, sizeof(hd_part_table[0]));

		if(hd_part_table[hd_part_num].type == PART_TYPE_EXTRA){
		
			hd_part_table[hd_part_num].start_lba += first_extra_lba;
			src += 16;
			
			//hd_print_part(&hd_part_table[hd_part_num - 1]);

			//这里的start_lba是偏移量
			if(first_extra_lba == 0){
				
				hd_part_num++;
				hd_get_part_table(drive, hd_part_table[hd_part_num - 1].start_lba, hd_part_table[hd_part_num - 1].start_lba);
			
			}
			else{ 
				hd_get_part_table(drive, hd_part_table[hd_part_num].start_lba, first_extra_lba);	
			}
		}
		//输出信息
		else if(hd_part_table[hd_part_num].type != PART_TYPE_EMPTY){
			
			hd_part_table[hd_part_num].start_lba += base_lba;
			hd_part_num++;
			src += 16;
		
		}
	}
}

static void hd_get_part(int drive, int sector_no)
{
	hd_cmd_t cmd;
	
	//cmd.data
	cmd.feature  = 0;
	cmd.count 	 = 1;
	cmd.lba_low  = sector_no & 0xFF;
	cmd.lba_mid  = (sector_no >> 8) & 0xFF;
	cmd.lba_high = (sector_no >> 16) & 0xFF;
	
	//lba模式 
	//lba = 1 (LBA mod)  
	//drv = 0 
	//lba_highest 
	cmd.device = MAKE_DEVICE_REG(1, drive, (sector_no >> 24) & 0xF);

	//Commnad
	cmd.command = ATA_READ;

	//进行硬盘操作
	hd_cmd_out(&cmd);	

	wait_interrupt();
	
	port_read(REG_DATA, hdbuf, SECTOR_SIZE);

}

//传入驱动器号,打印硬盘信息
static void hd_identify(int drive)
{
	hd_cmd_t cmd;
	
	//cmd.data
	//cmd.feature
	//cmd.count
	//cmd.lba_low
	//cmd.lba_mid
	//cmd.lba_high	
	
	//lba模式 选择主设备
	cmd.device = MAKE_DEVICE_REG(0, drive, 0);

	//Commnad
	cmd.command = ATA_IDENTIFY;

	//进行硬盘操作
	hd_cmd_out(&cmd);	

	//等待一个硬盘读取结束中断	
	wait_interrupt();

	//读取内容,一次读一个扇区大小512字节
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

static void hd_cmd_out(hd_cmd_t * cmd)
{
	//Control Block Registers
	outb(REG_DEV_CTRL, 0);

	//Command Block Registers
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

