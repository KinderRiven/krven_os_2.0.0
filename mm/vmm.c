#include "vmm.h"
#include "string.h"
#include "stdio.h"

//4096字节对齐
pgd_t pgd_kern[PGD_SIZE] __attribute__ ((aligned(PAGE_SIZE)));
//4096字节对齐

static pte_t pte_kern[PTE_COUNT][PTE_SIZE] __attribute__ ((aligned(PAGE_SIZE)));


//正式初始化内核
void init_vmm(){
	
	uint32_t kern_pte_first_idx = PGD_INDEX(PAGE_OFFSET);
	uint32_t i, j;

	//PTE_COUNT 128
	for(i = kern_pte_first_idx, j = 0; i < PTE_COUNT + kern_pte_first_idx; i++, j++){
		
		//pte的物理地址	
		pgd_kern[i] = ((uint32_t)pte_kern[j] - PAGE_OFFSET) | PAGE_PRESENT | PAGE_WRITE;
	}
	
	uint32_t *pte = (uint32_t *) pte_kern;
	for( i = 1; i < PTE_COUNT * PTE_SIZE; i++){
		pte[i] = (i << 12) | PAGE_PRESENT | PAGE_WRITE;
	}

	//PGD的物理地址
	uint32_t pgd_kern_phy_addr = (uint32_t)pgd_kern - PAGE_OFFSET;
	switch_pgd(pgd_kern_phy_addr);
}

void switch_pgd(uint32_t pd){
	asm volatile("mov %0, %%cr3" : : "r" (pd));
}

//建立虚拟地址vm到实地址pa的映射
void map(pgd_t *pgd_now, uint32_t va, uint32_t pa, uint32_t flags){

	pgd_t pgd_idx = PGD_INDEX(va);
	pte_t pte_idx = PTE_INDEX(va);
	
	//通过页表目录找到页表地址(这个地址是物理地址)
	pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);

	//如果找不到这个页表
	if(!pte){
		//申请一个页表
		pte = (pte_t *) pmm_alloc_page();
		//将目录项指向页表地址
		pgd_now[pgd_idx] = (uint32_t) pte | PAGE_PRESENT | PAGE_WRITE;
		//转化为线性地址
		pte = (pte_t *) ((uint32_t)pte + PAGE_OFFSET);
		bzero(pte, PAGE_SIZE);		
	} 
	else{
		//将页地址转化为线性地址
		pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
	}
	
		
	pte[pte_idx] = (uint32_t)(pa & PAGE_MASK) | flags;
	asm volatile("invlpg (%0)" : : "a" (va));	

}

void unmap(pgd_t *pgd_now, uint32_t va){

	pgd_t pgd_idx = PGD_INDEX(va);
	pte_t pte_idx = PGD_INDEX(va);

	//通过页表目录找到页表	
	pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);
	
	if(!pte){
		return ;
	}
	
	//转化为线性地址
	pte = (pte_t *)((uint32_t)pte + PAGE_OFFSET);
	
	pte[pte_idx] = 0;
	asm volatile("invlpg (%0)" : : "a" (va));
}

uint32_t get_mapping(pgd_t *pgd_now, uint32_t va, uint32_t *pa){

	uint32_t pgd_idx = PGD_INDEX(va);
	uint32_t pte_idx = PTE_INDEX(va);

	pte_t *pte = (pte_t *)(pgd_now[pgd_idx] & PAGE_MASK);
	//如果页表都不存在，说明不可能存在映射
	if (!pte){
		return 0;
	}	
	pte = (pte_t *)((uint32_t) pte + PAGE_OFFSET);

	//如果页表中的页表项内存不存在
	if(pte[pte_idx] != 0 && pa){
		*pa = pte[pte_idx] & PAGE_MASK;
		return 1;
	}
	return 0;
}

