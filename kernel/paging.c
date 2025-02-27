/**
 * paging.c	- Defines the interface for and structures relating tp paging.
 * @vesion $Id$
 */

#include "paging.h"
#include "heap.h"



// A bitset of frames - used or free.
u32int *frames;
u32int nframes;

// Defined in kheap.c
extern u32int placement_address;
extern heap_t *kheap;

// Macros used in the bitset algorithms.
#define INDEX_FROM_BIT(a)	(a/(8*4))
#define OFFSET_FROM_BIT(a)	(a%(8*4))

// Static function to set a bit in the frames bitset
static void set_frame(u32int frame_addr)
{
	u32int frame	= frame_addr/0x1000;
	u32int idx	= INDEX_FROM_BIT(frame);
	u32int off	= OFFSET_FROM_BIT(frame);
	frames[idx]	|= (0x1 << off);
}

// Static function to clear a bit in the frames bitset
static void clear_frame(u32int frame_addr)
{
	u32int frame	= frame_addr/0x1000;
	u32int idx	= INDEX_FROM_BIT(frame);
	u32int off	= OFFSET_FROM_BIT(frame);
	frames[idx]	&= ~(0x1 << off);
}

// Static function to test if a bit is set.
static u32int test_frame(u32int frame_addr)
{ 
	u32int frame    = frame_addr/0x1000;
	u32int idx      = INDEX_FROM_BIT(frame);
	u32int off      = OFFSET_FROM_BIT(frame);
	return (frames[idx] & (0x1 << off));
}

// Static function to find the first free frame.
static u32int first_frame()
{
	u32int i, j;
	for (i = 0; i < INDEX_FROM_BIT(nframes); i++) {
		if (frames[i] != 0xFFFFFFFF) { // nothing free, exit early
			// at least one bit is free here.
			for (j = 0; j < 32; j++) {
				u32int toTest = 0x1 << j;
				if ( !(frames[i]&toTest) ) {
					return i * 4 * 8 + j;
				}
			}
		}
	}
}

// Function to allocate a frame
void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
	if (page->frame != 0) {
		return;
	} else {
		u32int idx = first_frame();
		if (idx == (u32int)-1) {
			// PANIC! no free frames!!
		}
		set_frame(idx*0x1000);
		page->present	= 1;
		page->rw	= (is_writeable) ? 1 : 0;
		page->user	= (is_kernel) ? 0 : 1;
		page->frame	= idx;
		
	}
}
void force_frame(page_t *page, int is_kernel, int is_writeable)
{
	if (page->frame != 0) {
		return;
	} else {
		u32int idx = first_frame();
		if (idx == (u32int)-1) {
			// PANIC! no free frames!!
		}
		set_frame(idx*0x1000);
		page->present	= 1;
		page->rw	= (is_writeable) ? 1 : 0;
		page->user	= (is_kernel) ? 0 : 1;
		page->frame	= idx >> 12;
		
	}
}

// Function to deallocate a frame
void free_frame(page_t *page)
{
	u32int frame;
	if ( !(frame = page->frame) ) {
		return;
	} else {
		clear_frame(frame);
		page->frame = 0x0;
	}
}

void initialise_paging(struct stivale_struct * info)
{
	// The size of physical memory. For we moment we
	// assume it is 16Mb big.
	u32int mem_end_page = 0x4000000;

	nframes	= mem_end_page / 0x1000;
	frames	= (u32int*)kmalloc(INDEX_FROM_BIT(nframes));
	memset(frames, 0, INDEX_FROM_BIT(nframes));

	// Let's make a page directory.
	kernel_directory	= (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
	memset(kernel_directory, 0, sizeof(page_directory_t));
	current_directory	= kernel_directory;

	// Map some pages in the kernel heap area.
	// Here we call get_page but not alloc_frame. This causes page_table_t's
	// to be created where necessary.  We can't allocate frames yet bacause they
	// need to be identity mapped first below, and yet we can't increase
	// placement_address between identity mapping end enabling the heap!
	
	
	int i = 0;
	for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000)
		get_page(i, 1, kernel_directory);


	// We need to identify map (phys addr = virt addr) from
	// 0x0 to the end of used memory, so we can access this
	// transparently, as if paging wasn't enabled.
	// NOTE that we use a while loop here deliberately.
	// inside the loop body we actually change placement_address
	// by calling kmalloc(). A while loop causes this to be
	// computed on-the-fly rather than once at the start.
	i = 0;
	
	//e9_printf("%x,", placement_address);
	//e9_printf("%d,", placement_address);
	placement_address+= 0x1000;
	
while (i < placement_address + 0x1000) {
		// Kernel code is readable but not writeable from userspace.
		alloc_frame( get_page(i, 1, kernel_directory), 0, 0 );
		i += 0x1000;
	}
	




	// Now allocate
// Now allocate those pages we mapped earlier.
	//for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000)
	//	alloc_frame( get_page(i, 1, kernel_directory), 0, 0 );

		//e9_printf("%x video address start:",info->framebuffer_addr);
	
	uint64_t start = info->framebuffer_addr;
	uint64_t end = info->framebuffer_addr + (info->framebuffer_width * info->framebuffer_height * (info->framebuffer_bpp/8));
	uint64_t x = 0x3000000;
	for (uint32_t i = start; i < end; i++) {
		page_t * page = get_page(x, 1, kernel_directory);
		alloc_frame( page, 0, 1 );
		page->frame = i >> 12;
		x+= 1;
	}
	
	//videoptr = KHEAP_START + KHEAP_INITIAL_SIZE;
	
	
	/*uint64_t blah = 0xFF123123;
	page_t * page = get_page(blah, 1, kernel_directory);
	alloc_frame( page, 0, 1 );
	page->frame = blah >> 12;
	*/
	// Now, enable paging!
	switch_page_directory(kernel_directory);


	// Initialise the kernel heap.
	////kheap = create_heap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE, 0xCFFFF000, 0, 0);
	
	
//	c[0] = 'x';
	
	//ae9_printf('%c', c);
	///alloc_frame( get_page(&blah, 1, kernel_directory), 0, 1 );
	//9_printf("%x", blah);
		//blah = 'c';

}
void switch_page_directory(page_directory_t *dir)
{
      current_directory = dir;
	 current_directory = dir;
	asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical));
	u32int cr0;
	asm volatile("mov %%cr0, %0": "=r"(cr0));
	cr0 |= 0x80000000;	// Enable paging!
	asm volatile("mov %0, %%cr0":: "r"(cr0));

	
}
page_t *get_page(u32int address, int make, page_directory_t *dir)
{
	// Turn the address into an index.
	address /= 0x1000;
	// Find the page table containing this address.
	u32int table_idx = address / 1024;
	if (dir->tables[table_idx]) { // If this table is already assigned
		
		return &dir->tables[table_idx]->pages[address%1024];
	
	} else if (make) {
		u32int tmp;
		dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
		dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
			
		return &dir->tables[table_idx]->pages[address%1024];
	} else {
		return 0;
	}
}
