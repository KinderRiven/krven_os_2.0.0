#include "pmm.h"
#include "stdio.h"
#include "multiboot.h"

//存放着页面
static uint32_t pmm_stack[PAGE_MAX_SIZE + 1];

static uint32_t pmm_stack_top;

uint32_t phy_page_count;

uint32_t phy_page_total;

//可分配内存的开始
uint32_t phy_addr_start;

//可分配内存的结束
uint32_t phy_addr_end;

//获得内核内存占用大小
int get_kernel_mm()
{
	return phy_addr_end - phy_addr_start;
}

//显示内核内存占用情况
void show_memory_map(){

	mmap_entry_t *mmap_addr = (mmap_entry_t *) glb_mboot_ptr -> mmap_addr;
	uint32_t mmap_length = glb_mboot_ptr -> mmap_length;

	printc(c_black, c_light_brown, "Memory map:\n");
	
	mmap_entry_t *mmap;
	
	for(mmap = (mmap_entry_t*)mmap_addr; mmap < mmap_addr + mmap_length; mmap ++){
		printc(c_black, c_light_brown, "base_addr = 0x%X%08X, length = 0x%X%08X, type = 0x%X\n",
			(uint32_t) mmap -> base_addr_high, (uint32_t) mmap -> base_addr_low,
			(uint32_t) mmap -> length_high, (uint32_t) mmap -> length_low,
			(uint32_t) mmap -> type
			);		
	}

}

//现实物理内存使用情况、物理页框分配情况
void show_pmm_status(){

	printc(c_black, c_light_red, "Physical page now/top : %d/%d\n", pmm_stack_top, PAGE_MAX_SIZE);	
	printc(c_black, c_light_red, "Physical page count   : %d/%d\n", phy_page_count, phy_page_total);
	printc(c_black, c_light_red, "Physical page start   : 0x%08X\n", phy_addr_start);
	printc(c_black, c_light_red, "Physical page end     : 0x%08X\n", phy_addr_end);
	printc(c_black, c_light_red, "Kernel in Memory start: 0x%08X\n", kern_start);
	printc(c_black, c_light_red, "Kernel in Memory end  : 0x%08X\n", kern_end);
	printc(c_black, c_light_red, "Kernel in Memory used : %d KB\n", (kern_end - kern_start + 1023) / 1024);
	printc(c_black, c_light_red, "Dynamic Memory size   : %d MB\n", (phy_page_count * PMM_PAGE_SIZE) / (1024 * 1024));	

}

//初始化物理
void init_pmm(){

	mmap_entry_t *mmap_start_addr = (mmap_entry_t *) glb_mboot_ptr -> mmap_addr;
	mmap_entry_t *mmap_end_addr = (mmap_entry_t *) glb_mboot_ptr -> mmap_addr + glb_mboot_ptr -> mmap_length;
	
	mmap_entry_t *mmap_entry;
	phy_page_count = 0;

	for(mmap_entry = mmap_start_addr; mmap_entry < mmap_end_addr; mmap_entry ++){
			
			if(mmap_entry -> type == 1 && mmap_entry -> base_addr_low == 0x100000){
				
				//从这里开始分配
				//可以分配的物理地址的起始地址
				uint32_t page_addr = mmap_entry -> base_addr_low + (uint32_t)(kern_end - kern_start);
				
				//进行对齐
				page_addr = (page_addr + PMM_PAGE_SIZE) & PMM_PAGE_MASK;
				
				//一共多长
				uint32_t length = mmap_entry -> base_addr_low + mmap_entry -> length_low;
				
				//开始分配,最多分配到512MB之前				
				phy_addr_start = page_addr;
				
				while(page_addr < length && page_addr <= PMM_MAX_SIZE){
					pmm_free_page(page_addr);
					page_addr += PMM_PAGE_SIZE;
				}
				phy_addr_end = page_addr;
			}
	}

	//页面总数
	phy_page_total = phy_page_count;	
}

//申请一段连续的物理内存区域
uint32_t alloc_pmm(int size)
{
	uint32_t addr = NULL;

	while(size > 0)
	{
		addr = pmm_stack[pmm_stack_top--];	
		size -= PMM_PAGE_SIZE;
		phy_page_count--;
	}		

	return addr;	
}

//请求一个物理页面
//返回页面的起始地址 
uint32_t pmm_alloc_page()
{
	uint32_t page = pmm_stack[pmm_stack_top--];
	phy_page_count--;	

	return page;
}

//释放一个物理页面
void pmm_free_page(uint32_t p){

	pmm_stack[++pmm_stack_top] = p;
	phy_page_count++;
}
