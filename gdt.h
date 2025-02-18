#pragma once

#include "kernel/tss.h"

typedef struct
{
	uint32_t limit_low 	: 16;
	uint32_t base_low  	: 24;
	uint8_t  type		: 4;
	uint8_t  system		: 1;
	uint8_t	 dpl		: 2;
	uint8_t	 present	: 1;
	uint16_t limit_high	: 4;
	uint8_t  avilable	: 1;
	uint8_t  longmode	: 1;
	uint8_t	 size		: 1;
	uint8_t  gran		: 1;
	uint32_t base_high	: 8;
} __attribute__((packed)) gdt_entry_t;

typedef struct
{
	uint16_t limit;
	uint64_t base;
} gdt_ptr_t;


typedef struct
{
	uint32_t limit_low	: 16;
	uint32_t base_low	: 24;
	uint8_t	 type		:  4;
	uint8_t  _0_1		:  1;
	uint8_t  dpl		:  2;
	uint8_t  present	:  1;
	uint8_t  limit_high	:  4;
	uint8_t  avilable	:  1;
	uint8_t  _0_2		:  2;
	uint8_t  gran		:  1;
	uint64_t base_high	: 40;
	uint8_t  _reserved1 :  8;
	uint8_t	 _0_3		:  5;
	uint32_t _reserved2 : 19;
} __attribute__((packed)) tss64_seg_t;

typedef struct
{
	uint32_t _reserved;
	uint32_t rsp0_low;
	uint32_t rsp0_high;
	uint32_t _unused_in_our_kernel[23];
} tss64_t;


gdt_entry_t gdt[32];

volatile struct {
    uint16_t size;
    gdt_entry_t* offset;
} __attribute__((packed))
gdtr_ptr;


void gdt_set_entry
(uint32_t id, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
	gdt[id] = 
		(gdt_entry_t)
		{
			.limit_low	= limit & 0xFFFF,
			.base_low	= base  & 0xFFFFFF,
			.type		= access & 0xF,
			.system     = 1,	// Always one for Non system descriptors
			.dpl		= (access >> 5) & 3,
			.present	= (access >> 7) & 1,
			.limit_high = (limit >> 16) & 0xF,
			.avilable	= 1,
			.longmode	= (flags >> 1) & 1,
			.size		= ((flags >> 1) & 1)?0:((flags >> 2) & 1),
			.gran		= (flags >> 3) & 1,
			.base_high  = base >> 24,
		};
}

tss64_t tss64;
#define TSS64_AVL 0x9

void setup_tss(void *_tss64)
{
	uint64_t limit = sizeof(tss64);
	uint64_t base  = (uint64_t)&tss64;
	
	tss64_seg_t *tss64_seg = (tss64_seg_t*)_tss64;
	*tss64_seg = 
		(tss64_seg_t)
		{
			.limit_low 	= limit & 0xFFFF,
			.base_low	= base  & 0xFFFFFF,
			.type		= TSS64_AVL,
			._0_1		= 0,
			.dpl		= 3,
			.present	= 1,
			.limit_high = (limit >> 16) & 0xF,
			.avilable	= 1,
			._0_2		= 0,
			.gran		= 0,
			.base_high	= base >> 24,
			._0_3		= 0,
		};
}

#define ACC_TYPE_CS 0xA
#define ACC_TYPE_DS 0x2
#define ACC_DPL0	0
#define ACC_DPL3	0x60
#define ACC_PRESENT 0x80

#define FLAGS_L		0x2
#define FLAGS_32	0x4
#define FLAGS_gran	0x8

void setup_gdt()
{
	
	gdtr_ptr.size = sizeof(gdt) - 1;
    gdtr_ptr.offset = gdt;

	gdt_set_entry(0, 0, 0, 0, 0);	// Null decriptor
	gdt_set_entry(1, 0, -1, ACC_TYPE_CS | ACC_DPL0 | ACC_PRESENT, FLAGS_L); // Kernel CS
	gdt_set_entry(2, 0, -1, ACC_TYPE_DS | ACC_DPL0 | ACC_PRESENT, FLAGS_L); // Kernel DS
	gdt_set_entry(3, 0, -1, ACC_TYPE_CS | ACC_DPL3 | ACC_PRESENT, FLAGS_L);	// User CS
	gdt_set_entry(4, 0, -1, ACC_TYPE_DS | ACC_DPL3 | ACC_PRESENT, FLAGS_L); // User DS
	setup_tss(&gdt[5]);

	gdt_flush();
	load_tss();
	
}