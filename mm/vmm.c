#include "vmm.h"
#include "string.h"

pgd_t pgd_kern[PGD_SIZE] __attribute__ ((aligned(PAGE_SIZE)));
static pte_t pte_kern[PTE_COUNT][PTE_SIZE] __attribute__ ((aligned(PAGE_SIZE)));


//正式初始化内核
void init_vmm(){
	
	uint32_t kern_pte_first_idx = PGD_INDEX(PAGE_OFFSET);
	
	uint32_t i, j;
	for(i = kern_pte_first_idx, j = 0; i < PTE_COUNT + kern_pte_first_idx; i++, j++){
		//物理地址	
		pgd_kern[i] = ((uint32_t)pte_kern[j] - PAGE_OFFSET) | PAGE_PRESENT | PAGE_WRITE;
	}
	
	uint32_t *pte = (uint32_t *) pte_kern;
	for( i = 1; i < PTE_COUNT * PTE_SIZE; i++){
		pte[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
	}

	//地址
	uint32_t pgd_kern_phy_addr = (uint32_t)pgd_kern - PAGE_OFFSET;
	switch_pgd(pgd_kern_phy_addr);
}

void switch_pgd(uint32_t pd){
	asm volatile("mov %0, %%cr3" : : "r" (pd));
}



