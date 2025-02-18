#pragma once

#include <stdint.h>
#include <stdbool.h>
static uint8_t * drivebuffer = 0x000000000020D000;//malloc(0x00384036);
//#include "../library/e9print.h"
//#include "pmm.h"
#define DIV_ROUNDUP(a, b) (((a) + ((b) - 1)) / (b))

struct guid {
    uint32_t a;
    uint16_t b;
    uint16_t c;
    uint8_t  d[8];
} __attribute__((packed));


struct file_handle {
    bool       is_memfile;
    void      *fd;
    int      (*read)(void *fd, void *buf, uint64_t loc, uint64_t count);
    uint64_t   size;
};
//#include <lib/part.h>
//#include <lib/blib.h>
struct part {
    int drive;
    int partition;
    int sector_size;
    uint64_t first_sect;
    uint64_t sect_count;
    bool guid_valid;
    struct guid guid;
    bool part_guid_valid;
    struct guid part_guid;
};

struct echfs_dir_entry {
    uint64_t parent_id;
    uint8_t type;
    char name[201];
    uint64_t atime;
    uint64_t mtime;
    uint16_t perms;
    uint16_t owner;
    uint16_t group;
    uint64_t ctime;
    uint64_t payload;
    uint64_t size;
}__attribute__((packed));

struct echfs_file_handle {
    int disk;
    struct part part;
    uint64_t block_size;
    uint64_t block_count;
    uint64_t dir_length;
    uint64_t alloc_table_size;
    uint64_t alloc_table_offset;
    uint64_t dir_offset;
    uint8_t *alloc_map;
    struct echfs_dir_entry dir_entry;
};

int echfs_check_signature(struct part *part);
bool echfs_get_guid(struct guid *guid, struct part *part);

int echfs_open(struct echfs_file_handle *ret, struct part *part, const char *filename);
int echfs_read(struct echfs_file_handle *file, void *buf, uint64_t loc, uint64_t count);

struct gpt_table_header {
    // the head
    char     signature[8];
    uint32_t revision;
    uint32_t header_size;
    uint32_t crc32;
    uint32_t _reserved0;

    // the partitioning info
    uint64_t my_lba;
    uint64_t alternate_lba;
    uint64_t first_usable_lba;
    uint64_t last_usable_lba;

    // the guid
    struct guid disk_guid;

    // entries related
    uint64_t partition_entry_lba;
    uint32_t number_of_partition_entries;
    uint32_t size_of_partition_entry;
    uint32_t partition_entry_array_crc32;
} __attribute__((packed));

struct gpt_entry {
    struct guid partition_type_guid;

    struct guid unique_partition_guid;

    uint64_t starting_lba;
    uint64_t ending_lba;

    uint64_t attributes;

    uint16_t partition_name[36];
} __attribute__((packed));

struct mbr_entry {
	uint8_t status;
	uint8_t chs_first_sect[3];
	uint8_t type;
	uint8_t chs_last_sect[3];
	uint32_t first_sect;
	uint32_t sect_count;
} __attribute__((packed));

int echfs_ls(const char *path);
