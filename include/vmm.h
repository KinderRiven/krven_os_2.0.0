#ifndef INCLUDE_VMM_H_
#define INCLUDE_VMM_H_

#include "types.h"
#include "pmm.h"

#define PAGE_SIZE  4096

#define PAGE_OFFSET 0xC0000000

//是否有效
#define PAGE_PRESENT 0x1

//R/W
#define PAGE_WRITE	0x2

//U/S
#define PAGE_USER 0x4

#define PAGE_MASK 0xFFFFF000

//从页目录获得目录项
#define PGD_INDEX(x) (((x) >> 22) & 0x3FF)

//从页表中获取页表项
#define PTE_INDEX(x) (((x) >> 12) & 0x3FF)

//从页中获取数据
#define OFFSET_INDEX(x) ((x) & 0xFFF)

typedef uint32_t pgd_t;

typedef uint32_t pte_t;

#define PGD_SIZE (PAGE_SIZE / sizeof(pgd_t))

#define PTE_SIZE (PAGE_SIZE / sizeof(pte_t))

#define PTE_COUNT 128

void init_vmm();

void switch_pgd();

#endif
