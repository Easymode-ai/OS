#include "vmm.h"
#include "pmm.h"


#define OK 1

#define CMM_OFFSET 0x300000000


uint64_t page_offset = 0;

uint64_t * cr3_contents;



uint64_t * read_cr3() {
	uint64_t * value;
	__asm__("mov %%cr3, %%rax" : "=a"(value));
	/* AT&T syntax: __asm__("mov %%cr3, %%rax" : "=a"(value)); */
	return value;
}

void write_cr3(uintptr_t value) {
	asm volatile ("mov %0,%%cr3" : : "r" (value));
}



typedef uint64_t phys_t;
typedef uint64_t virt_t;

typedef uint64_t page_flags_t;




#define IS_ALIGN(addr) ((((uint32_t)(addr)) | 0xFFFFF000) == 0)
#define PAGE_ALIGN(addr) ((((uint32_t)(addr)) & 0xFFFFF000) + 0x1000)


#define P4_INDEX(vaddr) (((uint64_t) (vaddr) >> 39) & 0x1FF)
#define P3_INDEX(vaddr) (((uint64_t) (vaddr) >> 30) & 0x1FF)
#define P2_INDEX(vaddr) (((uint64_t) (vaddr) >> 21) & 0x1FF)
#define P1_INDEX(vaddr) (((uint64_t) (vaddr) >> 12) & 0x1FF)


struct page_table* virtual_new() {

	struct page_table *ret = (struct page_table*) (frame_alloc(1) * PAGE_SIZE);

	memset(ret, 0, PAGE_SIZE);

	struct page_table *p4 = (struct page_table*) (page_offset + (uint64_t) ret);
	struct page_table *new_p4 = (struct page_table*) (page_offset + (uint64_t) ret);
	for (int p4i = 0; p4i < 512; p4i++) {
		struct page_table_entry *p4e = &p4->entry[p4i];
		if (!p4e->present) continue;
		struct page_table_entry *new_p4e = &new_p4->entry[p4i];
		memcpy(new_p4e, p4e, sizeof(struct page_table_entry));
		new_p4e->frame = frame_alloc(1);
		memset((void*) (uint64_t) (page_offset + (new_p4e->frame * PAGE_SIZE)), 0, PAGE_SIZE);

		struct page_table *p3 = (struct page_table*) (page_offset + p4e->frame * PAGE_SIZE);
		struct page_table *new_p3 = (struct page_table*) (page_offset + new_p4e->frame * PAGE_SIZE);
		for (int p3i = 0; p3i < 512; p3i++) {
			struct page_table_entry *p3e = &p3->entry[p3i];
			if (!p3e->present) continue;
			struct page_table_entry *new_p3e = &new_p3->entry[p3i];
			memcpy(new_p3e, p3e, sizeof(struct page_table_entry));
			if (p3e->huge) continue;
			new_p3e->frame = frame_alloc(1);
			memset((void*) (uint64_t) (page_offset + (new_p3e->frame * PAGE_SIZE)), 0, PAGE_SIZE);

			struct page_table *p2 = (struct page_table*) (page_offset + p3e->frame * PAGE_SIZE);
			struct page_table *new_p2 = (struct page_table*) (page_offset + new_p3e->frame * PAGE_SIZE);
			for (int p2i = 0; p2i < 512; p2i++) {
				struct page_table_entry *p2e = &p2->entry[p2i];
				if (!p2e->present) continue;
				struct page_table_entry *new_p2e = &new_p2->entry[p2i];
				memcpy(new_p2e, p2e, sizeof(struct page_table_entry));
				if (p2e->huge) continue;
				new_p2e->frame = frame_alloc(1);
				memset((void*) (uint64_t) (page_offset + (new_p2e->frame * PAGE_SIZE)), 0, PAGE_SIZE);

				struct page_table *p1 = (struct page_table*) (page_offset + p2e->frame * PAGE_SIZE);
				struct page_table *new_p1 = (struct page_table*) (page_offset + new_p2e->frame * PAGE_SIZE);
				for (int p1i = 0; p1i < 512; p1i++) {
					struct page_table_entry *p1e = &p1->entry[p1i];
					if (!p1e->present) continue;
					struct page_table_entry *new_p1e = &new_p1->entry[p1i];
					memcpy(new_p1e, p1e, sizeof(struct page_table_entry));
				}
			}
		}
	}
	return ret;
}

void print_table(struct page_table * p4r, size_t psize) {


	struct page_table *p4 = (struct page_table*) (page_offset + (uint64_t) p4r);

	for (int p4i = 0; p4i < 512; p4i++) {


		struct page_table_entry *p4e = &p4->entry[p4i];
		
		if (!p4e->present) continue;

		print_page(p4e, p4e->frame);

		struct page_table *p3 = (struct page_table*) (page_offset + p4e->frame * psize);


		for (int p3i = 0; p3i < 512; p3i++) {

			struct page_table_entry *p3e = &p3->entry[p3i];
			if (!p3e->present) continue;

			e9_printf("p3: ");
			print_page(p3e, p3e->frame);


			struct page_table *p2 = (struct page_table*) (page_offset + p3e->frame * psize);

			for (int p2i = 0; p2i < 512; p2i++) {
				struct page_table_entry *p2e = &p2->entry[p2i];

				if (!p2e->present) continue;

				struct page_table *p1 = (struct page_table*) (page_offset + p2e->frame * psize);

				for (int p1i = 0; p1i < 512; p1i++) {
					struct page_table_entry *p1e = &p1->entry[p1i];
					if (!p1e->present) continue;
				
				}
			}
		}
	}
}


void virtual_map(struct page_table *p4, uint64_t vaddress, uint64_t paddress, int page_count, int user, int cache) {

	
	for (int page = 0; page < page_count; page++) {

		struct page_table_entry *p4e = &((struct page_table*)((uint64_t) p4 + page_offset))->entry[P4_INDEX(vaddress)];

		if (!p4e->present)
		{
			p4e->present = 1;
			p4e->writable = 1;
			p4e->frame  = frame_alloc(1);
			p4e->user_accessible =user;
			p4e->disable_cache = cache;

			memset((void*) (uint64_t) (page_offset + (p4e->frame * PAGE_SIZE)), 0, PAGE_SIZE);			
		}

		if (user) p4e->user_accessible = 1;
		struct page_table *p3  = (struct page_table*) (uint64_t)(page_offset + (p4e->frame * PAGE_SIZE));
		struct page_table_entry *p3e = &p3->entry[P3_INDEX(vaddress)];

		if (!p3e->present)
		{
			p3e->present = 1;
			p3e->frame  = frame_alloc(1);
			p3e->writable = 1;
			p3e->user_accessible =user;
			p3e->disable_cache = cache;

			memset((void*) (uint64_t) (page_offset + (p3e->frame * PAGE_SIZE)), 0, PAGE_SIZE);	
		}

		if (user) p3e->user_accessible = 1;
		struct page_table *p2 = (struct page_table*) (uint64_t)(page_offset + (p3e->frame * PAGE_SIZE));
		struct page_table_entry *p2e = &p2->entry[P2_INDEX(vaddress)];

		if (!p2e->present)
		{
			p2e->present = 1;
			p2e->frame  = frame_alloc(1);
			p2e->writable = 1;
			p2e->user_accessible =user;
			p2e->disable_cache = cache;

			memset((void*) (uint64_t) (page_offset + (p2e->frame * PAGE_SIZE)), 0, PAGE_SIZE);
		}

		if (user) p2e->user_accessible = 1;
		struct page_table *p1 = (struct page_table*) (uint64_t)	(page_offset + (p2e->frame * PAGE_SIZE));
		struct page_table_entry *p1e = &p1->entry[P1_INDEX(vaddress)];

		if (!p1e->present)
		{
			p1e->present = 1;
			p1e->frame  = paddress / PAGE_SIZE;
			p1e->writable = 1;
			p1e->user_accessible =user;
			p1e->disable_cache = cache;

		}
	
		vaddress += PAGE_SIZE;
		paddress += PAGE_SIZE;
	}
}


void print_page(struct page_table_entry *pte, uint64_t vaddr) {
	e9_printf("%x %x %x present:%d noexecute:%d global:%d huge:%d dirty:%d access:%d dcache:%d wcache:%d useraccess:%d writable:%d huge:%d\n",
			pte,
			vaddr,
			pte->frame,
			pte->present, 
			pte->no_execute,
			pte->global,
			pte->huge,
			pte->dirty,
			pte->accessed,
			pte->disable_cache,
			pte->write_through_caching,
			pte->user_accessible,
			pte->writable,
			pte->huge);
}
void virtual_map_2mib(struct page_table *p4, uint64_t vaddr, uint64_t paddr, int page_count, int user) {
	for (int page = 0; page < page_count; page++) {
		struct page_table_entry *p4e = &p4->entry[P4_INDEX(vaddr)];
		if (!p4e->present) {
			p4e->present = 1;
			p4e->writable = 1;
			p4e->user_accessible = user;
			p4e->frame = frame_alloc(1);
			memset((void*) (uint64_t) (p4e->frame * PAGE_SIZE), 0, PAGE_SIZE);
		}
		struct page_table *p3 = (struct page_table*) (uint64_t) (p4e->frame * PAGE_SIZE);
		struct page_table_entry *p3e = &p3->entry[P3_INDEX(vaddr)];
		if (!p3e->present) {
			p3e->present = 1;
			p3e->writable = 1;
			p3e->user_accessible = user;
			p3e->frame = frame_alloc(1);
			memset((void*) (uint64_t) (p3e->frame * PAGE_SIZE), 0, PAGE_SIZE);
		}
		struct page_table *p2 = (struct page_table*) (uint64_t) (p3e->frame * PAGE_SIZE);
		struct page_table_entry *p2e = &p2->entry[P2_INDEX(vaddr)];
		if (!p2e->present) {
			p2e->present = 1;
			p2e->writable = 1;
			p2e->user_accessible = user;
			//p2e->huge = 1;
			p2e->frame = paddr / PAGE_SIZE;
		}
		vaddr += PAGE_SIZE_2MIB;
		paddr += PAGE_SIZE_2MIB;
	}
}
extern end();



void initvmm(uint64_t total_memory)
{
	uint64_t kernelsize = end-(0xffffffff80000000 + 0x200000);
	uint64_t kernelend = end;
	uint64_t kernelstart = 0xffffffff80000000;

	e9_printf("kernelend %x\n", kernelend);
	page_offset = 0;
	
	kernel_p4 = read_cr3();

	//kernel_p4 =frame_alloc(1) * PAGE_SIZE;
	kernel_p4 = (struct page_table*) (frame_alloc(1) * PAGE_SIZE);
	memset(kernel_p4,0, PAGE_SIZE);
	e9_printf("Page Table Address: %x\n", kernel_p4);  
	uint64_t * memoryaddr = malloc(total_memory);
	e9_printf("memoryaddr Address: %x\n", memoryaddr);  

	//virtual_map((uint64_t)kernel_p4, (uint64_t)0x00000000FEBD5000, 0x00000000FEBD5000, (uint64_t) 0x100000/PAGE_SIZE, 1,1);

	virtual_map((uint64_t)kernel_p4, (uint64_t)0x0, (uint64_t)0x0, (uint64_t) total_memory*8/PAGE_SIZE, 1,1);
	//virtual_map((uint64_t)kernel_p4, (uint64_t)0x0000000800000000, (uint64_t)0x0, (uint64_t) total_memory*4/PAGE_SIZE, 1,1);
	
	
///	virtual_map((uint64_t)kernel_p4, (uint64_t)0x0, (uint64_t)0x0, (uint64_t) PAGE_SIZE, 1,1);
	//virtual_map((uint64_t)kernel_p4, (uint64_t)0x00000003FFFFFB50, 0, (uint64_t) 0x10000000/PAGE_SIZE, 1,1);
	
	//	virtual_map((uint64_t)kernel_p4, (uint64_t)0x0000000008000000, 0, (uint64_t) 0x100000/PAGE_SIZE, 1,1);
	/*
	virtual_map((uint64_t)kernel_p4, (uint64_t)0x0000000000100000, 0, (uint64_t) 0x100000/PAGE_SIZE, 1,1);
	virtual_map((uint64_t)kernel_p4, (uint64_t)0x0000000020000000, 0, (uint64_t) 0x100000/PAGE_SIZE, 1,1);

	virtual_map((uint64_t)kernel_p4, (uint64_t)0xFFFFFFFFECC62000, 0, (uint64_t) 0x2000000/PAGE_SIZE, 1,1);
	virtual_map((uint64_t)kernel_p4, (uint64_t)0x0000000000200000, 0, (uint64_t) 0x100000/PAGE_SIZE, 1,1);
	virtual_map((uint64_t)kernel_p4, (uint64_t)0xFFFFFFFFE005A046, 0, (uint64_t) 0x100000/PAGE_SIZE, 1,1);
	virtual_map((uint64_t)kernel_p4, (uint64_t)0x0000000041600000, 0, (uint64_t) 0x100000/PAGE_SIZE, 1,1);*/

	virtual_map((uint64_t)kernel_p4, (uint64_t)0x7FC0000000, 0x100000, 0x100000/PAGE_SIZE, 1,1);
	virtual_map((uint64_t)kernel_p4, (uint64_t)0xffffffff80000000, 0, (uint64_t) (kernelsize+0x10000+total_memory)/PAGE_SIZE, 1,1);
	virtual_map((uint64_t)kernel_p4, (uint64_t)0xffff800000000000, 0, (uint64_t) 0x8000000/PAGE_SIZE, 1,1);
	
	e9_printf("GO\n");
	

	write_cr3((uint64_t)kernel_p4);

	e9_printf("We didn't crash\n");
	/* stop here we didn't crash!!! */

}





