#pragma once
#include <stdint.h>
#include <stddef.h>
#define PAGE_SIZE 4096
int frame_bitmap_check(uint64_t index, uint64_t length);
void frame_bitmap_set(uint64_t index, uint64_t length);
void frame_bitmap_unset(uint64_t index, uint64_t length);
uint64_t frame_alloc(uint64_t length);
void * malloc(uint64_t size);
void * calloc(size_t num, size_t size);
void free(uintptr_t ptr);
void initpmm(uint64_t total_memory);
static uint64_t frame_count = 0;
