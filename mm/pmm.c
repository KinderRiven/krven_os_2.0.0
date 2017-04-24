#include "pmm.h"
#include "stdio.h"
#include "multiboot.h"

static uint32_t pmm_stack[PAGE_MAX_SIZE + 1];
static uint32_t pmm_stack_top;

uint32_t phy_page_count;

//可分配内存的开始
uint32_t phy_addr_start;

//可分配内存的结束
uint32_t phy_addr_end;

void show_memory_map(){

	uint32_t mmap_addr = glb_mboot_ptr -> mmap_addr;
	uint32_t mmap_length = glb_mboot_ptr -> mmap_length;

	printc(c_black, c_light_brown, "Memory map:\n");
	
	mmap_entry_t *mmap;
	
	for(mmap = (mmap_entry_t*)mmap_addr; (uint32_t) mmap < mmap_addr + mmap_length; mmap ++){
		printc(c_black, c_light_brown, "base_addr = 0x%X%08X, length = 0x%X%08X, type = 0x%X\n",
			(uint32_t) mmap -> base_addr_high, (uint32_t) mmap -> base_addr_low,
			(uint32_t) mmap -> length_high, (uint32_t) mmap -> length_low,
			(uint32_t) mmap -> type
			);		
	}

}

void show_pmm_status(){
	
	printc(c_black, c_light_red,   "Physical page count:    %d\n", phy_page_count);
	printc(c_black, c_light_red,   "Physical page start:    0x%08X\n", phy_addr_start);
	printc(c_black, c_light_red,   "Physical page end:      0x%08X\n", phy_addr_end);
	printc(c_black, c_light_green, "Kernel in Memory start: 0x%08X\n", kern_start);
	printc(c_black, c_light_green, "Kernel in Memory end:   0x%08X\n", kern_end);
	printc(c_black, c_light_green, "Kernel in Memory used:  %d KB\n", (kern_end - kern_start + 1023) / 1024);
	
}

void init_pmm(){

	mmap_entry_t *mmap_start_addr = (mmap_entry_t *) glb_mboot_ptr -> mmap_addr;
	mmap_entry_t *mmap_end_addr = (mmap_entry_t *) glb_mboot_ptr -> mmap_addr + glb_mboot_ptr -> mmap_length;
	
	mmap_entry_t *mmap_entry;
	phy_page_count = 0;

	for(mmap_entry = mmap_start_addr; mmap_entry < mmap_end_addr; mmap_entry ++){
			
			if(mmap_entry -> type == 1 && mmap_entry -> base_addr_low == 0x100000){
				
				//从这里开始分配
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
					phy_page_count ++;
				}
				phy_addr_end = page_addr;
			}
	}
}

uint32_t pmm_alloc_page(){
	uint32_t page = pmm_stack[pmm_stack_top --];
	return page;
}

void pmm_free_page(uint32_t p){
	pmm_stack[++pmm_stack_top] = p;
}
