#ifndef INCLUDE_VMM_H_
#define INCLUDE_VMM_H_

#include "types.h"
#include "pmm.h"

#define PAGE_SIZE  4096			//页面大小为4096个字节

#define PAGE_OFFSET 0xC0000000	//内存偏移量


#define PAGE_PRESENT 	0x1		//存在位
#define PAGE_WRITE		0x2		//读写位
#define PAGE_USER 		0x4		//用户位


#define PAGE_MASK 0xFFFFF000

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
