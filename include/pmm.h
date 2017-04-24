#ifndef INCLUDE_PM_H_
#define INCLUDE_PM_H_

#include "types.h"

extern uint8_t kern_start[];
extern uint8_t kern_end[];

extern uint32_t phy_page_count;

//最多管理512MBN内存
#define PMM_MAX_SIZE 0x20000000

//物理块大小为4096KB
#define PMM_PAGE_SIZE 4096

//最多有几个页面
#define PAGE_MAX_SIZE (PMM_MAX_SIZE / PMM_PAGE_SIZE)

//内核栈大小
#define STACK_SIZE 8192

//内存对齐
#define PMM_PAGE_MASK 0xFFFFF000
void show_memory_map();

void show_pmm_status();

void init_pmm();

uint32_t pmm_alloc_page();

void pmm_free_page(uint32_t p);

#endif
