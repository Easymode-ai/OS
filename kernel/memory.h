
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

	for(int i = 0; i < MAXMEMORY; i++)
	{
		first_page_table[i] = (i * 0x1000) | 3; // attributes: supervisor level, read/write, present.
	}

	
	page_directory[0] = ((unsigned int)first_page_table) | 3;
	loadpage(page_directory);
	enablePaging();
	
	
	current = &first_page_table[0];
	struct memoryblock * block = (struct memoryblock *)first_page_table[0];
	offset = &first_page_table[0];
	block->address = current;
	block->size= 1;
	lastblock = block;
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
	
	
	/*while(size > 0)
	{
		offset+1;
		
		struct memoryblock * newblock = (struct memoryblock *)current;
		
		newblock->address = current;
		newblock->size = 1;
		
	
			
		last = newblock;
		size -= 1;	
		
	}*/
	
	
	
	
	//if (last != NULL)
	//lastblock = last;
	
	
	return c;
}

static void free(char *arg)
{
	_free(arg);
	arg[0]=NULL;
}

