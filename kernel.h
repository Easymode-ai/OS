#pragma once
#include <stivale2.h>


void stivale2_main(struct stivale2_struct *info);

struct stivale2_header_tag_smp smp_request = {
	.tag = {
		.identifier = STIVALE2_HEADER_TAG_SMP_ID, 
		.next       = 0
	},
	.flags =1
};

struct stivale2_header_tag_framebuffer framebuffer_request = {
	.tag = {
		.identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID, 
		.next       = &smp_request,
	},
	.framebuffer_width = 1280,
		.framebuffer_height = 720,
		.framebuffer_bpp = 32
};

static uint8_t stack[4096] = {0};

__attribute__((section(".stivale2hdr"), used))
struct stivale2_header header2 = {
	.entry_point = (uint64_t)stivale2_main,
	.stack       = (uintptr_t)stack + sizeof(stack),
	.flags       = 1,
	.tags        = (uint64_t)&framebuffer_request
};

