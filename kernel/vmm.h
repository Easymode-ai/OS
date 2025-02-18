#pragma once
#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE_2MIB 0x200000

struct page_table_entry {
	uint64_t present : 1;
	uint64_t writable : 1;
	uint64_t user_accessible : 1;
	uint64_t write_through_caching : 1;
	uint64_t disable_cache : 1;
	uint64_t accessed : 1;
	uint64_t dirty : 1;
	uint64_t huge : 1;
	uint64_t global : 1;
	uint64_t unused : 3;
	uint64_t frame : 40;
	uint64_t unused1 : 11;
	uint64_t no_execute : 1;
} __attribute__ ((packed));

struct page_table {
	struct page_table_entry entry[512];
};

struct page_table *kernel_p4;

void print_page(struct page_table_entry *pte, uint64_t vaddr);
void virtual_map(struct page_table *p4, uint64_t vaddress, uint64_t paddress, int page_count, int user, int cache);
void initvmm(uint64_t total_memory);
