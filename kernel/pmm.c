#include "pmm.h"

uint64_t placement_address = 0;

uint64_t frame_bitmap_size;
uint8_t *frame_bitmap;


int frame_bitmap_check(uint64_t index, uint64_t length) {
	for (uint64_t i = 0; i < length; i++)
	if (frame_bitmap[(index + i) / 8] & (1 << (7 - ((index + i) % 8))))
	return 1;
	return 0;
}

void frame_bitmap_set(uint64_t index, uint64_t length) {
	for (uint64_t i = 0; i < length; i++)
	frame_bitmap[(index + i) / 8 ] |= (1 << (7 - ((index + i) % 8)));

	//e9_printf("Bitmap Framebit set: %x len: %d\n", index, length);
}

void frame_bitmap_unset(uint64_t index, uint64_t length) {
	for (uint64_t i = 0; i < length; i++)
	frame_bitmap[(index + i) / 8] &= ~(1 << (7 - ((index + i) % 8)));
}

uint64_t frame_alloc(uint64_t length) {

	
	for (uint64_t i = 0; i < frame_count; i++) {
		
		
		if (!frame_bitmap_check(i, length)) {
				frame_bitmap_set(i, length  );
				
				//frame_bitmap_set(i, (length + PAGE_SIZE - 1) / PAGE_SIZE);

			return i;
		}
	}


	e9_printf("frame_alloc() failed. ran out of memory?\n");
	return -1;
}
extern end();


void initpmm(uint64_t total_memory)
{

	uint64_t kernelend = end;
	
	
	uint64_t kernelstart = 0xffffffff80000000 ;


	frame_count = ((total_memory) + PAGE_SIZE - 1) / PAGE_SIZE;
	frame_bitmap_size = (frame_count + 7) / 8;
	frame_bitmap = 0x200000;
	
	memset(frame_bitmap, 0, frame_bitmap_size);
	frame_bitmap_set(0, (0x200000   + frame_bitmap_size + PAGE_SIZE - 1) / PAGE_SIZE);
	
	placement_address =  0x0000000000000000;// //kernelend + bitmap_size;
}

void * malloc(uint64_t size)
{
	uint64_t tmp = placement_address- 0xffffffff80000000;
	
	placement_address += size;
	e9_printf("malloc : %x address\n ", tmp);
	return tmp;
}


void * calloc(size_t num, size_t size)
{
	uint64_t tmp = malloc(num * size);
	memset(tmp,0, (num * size));
	return tmp;
}

void free(uintptr_t ptr)
{
	
}
