#ifndef INCLUDE_VMM_H_
#define INCLUDE_VMM_H_

#include "types.h"
#include "pmm.h"

#define PAGE_SIZE  4096			//页面大小为4096个字节

#define PAGE_OFFSET 0xC0000000	//内存偏移量

#define PAGE_MASK	0xFFFFF000	//页表项掩码 

#define PDE_PRESENT 	(1 << 0)	//存在位
#define PDE_WRITE		(1 << 1)	//读写位
#define PDE_USER 		(1 << 2)	//用户位
#define PDE_PWT			(1 << 3)	//Write-through
#define PDE_PCD			(1 << 4)	//Cache disabled
#define PDE_Access		(1 << 5)	//Accessed
#define PDE_Zero		(1 << 6)	//Reserved
#define PDE_PS			(1 << 7)	//Page size
#define PDE_G			(1 << 8)	//Global page

#define PTE_PRESENT 	(1 << 0)	//存在位
#define PTE_WRITE		(1 << 1)	//读写位
#define PTE_USER 		(1 << 2)	//用户位
#define PTE_PWT			(1 << 3)	//Write-through
#define PTE_PCD			(1 << 4)	//Cache disabled
#define PTE_Access		(1 << 5)	//Accessed
#define PTE_Dir			(1 << 6)	//Dirty
#define PTE_PAT			(1 << 7)	//always 0
#define PTE_G			(1 << 8)	//Global page


#define PGD_INDEX(x) (((x) >> 22) & 0x3FF)

#define PTE_INDEX(x) (((x) >> 12) & 0x3FF)

#define OFFSET_INDEX(x) ((x) & 0xFFF)

typedef uint32_t pgd_t;

typedef uint32_t pte_t;

#define PGD_SIZE (PAGE_SIZE / sizeof(pgd_t))	// 4096 字节 / 4 字节

#define PTE_SIZE (PAGE_SIZE / sizeof(pte_t))	// 4096 字节 / 4 字节

#define PTE_COUNT 128


extern uint32_t pgd_kern[PGD_SIZE];

void init_vmm();

void switch_pgd();

void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags);

void unmap(pgd_t *pgd_now, uint32_t va);

uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa);

#endif
