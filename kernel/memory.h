#pragma once


typedef struct page
{
	uint32_t present	: 1;	// Page present in memory
	uint32_t rw	: 1;	// Read-only if clear, readwrite if set
	uint32_t user	: 1;	// Suprevisor level only if clear
	uint32_t accessed	: 1;	// Has the page been accessed since last refresh?
	uint32_t dirty	: 1;	// Has the page been written to since last refresh?
	uint32_t unused	: 7;	// Amalgamation of unused and reserved bits
	uint32_t frame	: 20;	// Frame address (shifted right 12 bits)
} page_t;

struct memoryblock {
	size_t size;
	uint32_t address;
	struct memoryblock *next;
	struct memoryblock *prev;
};

static struct memoryblock * memoryblocks;
static uint32_t current;
static struct memoryblock * lastblock;

static uint32_t offset = 0;

enum SIZE {
	BYTE = 1,
	KBYTE = 1024,
	MBYTE = 1024 * 1024
};

static void initMemory()
{
	//set each entry to not present

	for(int i = 0; i < 1024; i++)
	{
    // This sets the following flags to the pages:
    //   Supervisor: Only kernel-mode can access them
    //   Write Enabled: It can be both read from and written to
    //   Not Present: The page table is not present
    page_directory[i] = 0x00000002;
	}

	for(int i = 0; i < 1024; i++)
	{
		first_page_table[i] = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present.
	}
	
	page_directory[0] = ((unsigned int)first_page_table) | 3;
	
	loadpage(page_directory);
	enablePaging();
	
	/*
	current = &first_page_table[0];
	struct memoryblock * block = (struct memoryblock *)first_page_table[0];
	offset = &first_page_table[0];
	block->address = current;
	block->size= 1;
	lastblock = block;*/
}

static uint32_t mallocr(int optional)
{
	return &first_page_table[optional];
}

static uint32_t malloc(size_t size)
{
	struct memoryblock * currentblock = lastblock;
	struct memoryblock * last = NULL;
	
	uint32_t c = offset;
	
	offset += (size);
	
	return c;
}

static void free(char *arg)
{
	/* let's do this */
}


int idx =0;
page_t *GetPage(unsigned Address)
{
	int is_writeable=1;
	int is_kernel = 0;
	
	page_t * page = Address;
	page->present	= 1;
	page->rw	= (is_writeable) ? 1 : 0;
	page->user	= (is_kernel) ? 0 : 1;
	page->frame	= idx++;
}
