#include <stdint.h>
#include <stdbool.h>
#include "echfs.h"


struct echfs_identity_table {
    uint8_t  jmp[4];
    char     signature[8];
    uint64_t block_count;
    uint64_t dir_length;
    uint64_t block_size;
    uint32_t reserved;
    struct guid guid;
} __attribute__((packed));

#define ROOT_DIR_ID  (~((uint64_t)0))
#define END_OF_CHAIN (~((uint64_t)0))
#define FILE_TYPE    0
#define DIR_TYPE     1

static int read_block(struct echfs_file_handle *file, void *buf, uint64_t block, uint64_t offset, uint64_t count) {
   

   return disk_read(&file->part, buf, (file->alloc_map[block] * file->block_size) + offset, count);
}

int echfs_read(struct echfs_file_handle *file, void *buf, uint64_t loc, uint64_t count) {
    for (uint64_t progress = 0; progress < count;) {
        uint64_t block = (loc + progress) / file->block_size;

        uint64_t chunk = count - progress;
        uint64_t offset = (loc + progress) % file->block_size;
        if (chunk > file->block_size - offset)
            chunk = file->block_size - offset;

        read_block(file, buf + progress, block, offset, chunk);
        progress += chunk;
    }

    return 0;
}

int echfs_check_signature(struct part *part) {
    struct echfs_identity_table id_table;
	e9_printf("check sig\n");
    disk_read(part, &id_table, (void *)0x00100000, sizeof(struct echfs_identity_table));

    if (strncmp(id_table.signature, "_ECH_FS_", 8)) {
        return 0;
    }

    return 1;
}

bool echfs_get_guid(struct guid *guid, struct part *part) {
    struct echfs_identity_table id_table;
   disk_read(part, &id_table, (void *)0, sizeof(struct echfs_identity_table));

    if (strncmp(id_table.signature, "_ECH_FS_", 8)) {
        return false;
    }

    *guid = id_table.guid;

    return true;
}

int echfs_ls(const char *path) {
  
	
	e9_printf("bbo\n");
	const char *fullpath = path;
	

	struct echfs_identity_table id_table;
    disk_read(0, (void *)&id_table, (void *)0x00100000, sizeof(struct echfs_identity_table));
	
	
	uint64_t dir_length = id_table.dir_length * id_table.block_size;;

  
    uint64_t dir_offset         = (16 * id_table.block_size) + (DIV_ROUNDUP(id_table.block_count * sizeof(uint64_t), id_table.block_size) *id_table.block_size);
	
    if (strncmp(id_table.signature, "_ECH_FS_", 8)) {
        e9_printf("[ECHFS] signature invalid\n");
		
        return -1;
    }
	 
	uint64_t wanted_parent = ROOT_DIR_ID;
    bool     last_elem     = false;
	
forward:;
	char wanted_name[128];
	
    for (; *path == '/'; path++);
    for (int i = 0; ; i++, path++) {
        if (*path == '\0' || *path == '/') {
            if (*path == '\0')
                last_elem = true;
            wanted_name[i] = '\0';
            path++;
            break;
        }
        wanted_name[i] = *path;
    }
	
	uint64_t curoffset = 0;
	
	struct echfs_dir_entry result;
	    for (uint64_t i = 0; i < dir_length; i += sizeof(struct echfs_dir_entry)) {
		
		
		curoffset = dir_offset + 0x100000 + (i);//+ ret->dir_offset);// - (2 * sizeof(struct echfs_dir_entry));

        disk_read(0, &result, curoffset, sizeof(struct echfs_dir_entry));

		if (!result.parent_id) {
            break;
        }
		
		if (!strlen(wanted_name))
			if (result.parent_id == wanted_parent)		
			if (strcmp("",result.name))
			if (result.type == DIR_TYPE)
			{
			fontcolor(0xFF0000);
			e9_printf("* %s/%s/\n", fullpath, result.name);	
			}
			else
			{
			fontcolor(0x00FF00);
			e9_printf("* %s/%s\n", fullpath, result.name);	
			}
	
		
	
		if (strlen(wanted_name))
		if (!strcmp(wanted_name,result.name) &&
            result.type == DIR_TYPE) {
				
				wanted_parent = result.payload;
                goto forward;
			}

	
		
		
    }
	fontcolor(0xFFFFFF);
	return 1;
}

int echfs_open(struct echfs_file_handle *ret, struct part *part, const char *path) {
		
    const char *fullpath = path;

    ret->disk = part->drive;
    ret->part = *part;

    struct echfs_identity_table id_table;
    disk_read(0, (void *)&id_table, (void *)0x00100000, sizeof(struct echfs_identity_table));
	
    if (strncmp(id_table.signature, "_ECH_FS_", 8)) {
        e9_printf("[ECHFS] signature invalid\n");
		
        return -1;
    }

	e9_printf("ret block count %x\n", id_table.block_count);
	


    ret->block_size         = id_table.block_size;
    ret->block_count        = id_table.block_count;
    ret->dir_length         = id_table.dir_length * ret->block_size;
    ret->alloc_table_size   = DIV_ROUNDUP(ret->block_count * sizeof(uint64_t), ret->block_size) * ret->block_size;
    ret->alloc_table_offset = 16 * ret->block_size;
    ret->dir_offset         = ret->alloc_table_offset + ret->alloc_table_size;

    uint64_t wanted_parent = ROOT_DIR_ID;
    bool     last_elem     = false;
	
	e9_printf("ret block size %x\n", ret->block_size );
	e9_printf("ret block count %x\n", ret->block_count);
	e9_printf("ret dir_length  %x\n", ret->dir_length);
	e9_printf("ret alloc_table_size %x\n", ret->alloc_table_size);
	e9_printf("ret dir_offset %x\n", ret->dir_offset);
	
next:;

    char wanted_name[128];
	
    for (; *path == '/'; path++);
    for (int i = 0; ; i++, path++) {
        if (*path == '\0' || *path == '/') {
            if (*path == '\0')
                last_elem = true;
            wanted_name[i] = '\0';
            path++;
            break;
        }
        wanted_name[i] = *path;
    }
	
	e9_printf("[ECHFS] Searching table for: %s\n", wanted_name);

	uint64_t * curoffset = 0x0;
	
	e9_printf("Dir len %d\n", ret->dir_length);
    for (uint64_t i = 0; i < ret->dir_length; i += sizeof(struct echfs_dir_entry)) {
		
		
		curoffset = ret->dir_offset + 0x100000 + (i);//+ ret->dir_offset);// - (2 * sizeof(struct echfs_dir_entry));

        disk_read(0, &ret->dir_entry, curoffset, sizeof(struct echfs_dir_entry));

		if (!ret->dir_entry.parent_id) {
            break;
        }
		
	
	/*	e9_printf("\n[.ECHFS] Dir offset %x\n", curoffset);
		e9_printf("[ECHFS] parent_id %x\n", ret->dir_entry.parent_id);
		e9_printf("[ECHFS] type %d\n", ret->dir_entry.type);	
		e9_printf("[ECHFS] name %s\n", ret->dir_entry.name);	
		
		for(int x =0; x < 200; x++)
			e9_printf("%02X ", ret->dir_entry.name[x]);
		e9_printf("[ECHFS] atime %d\n", ret->dir_entry.atime);	
		e9_printf("[ECHFS] mtime %d\n", ret->dir_entry.mtime);	
		e9_printf("[ECHFS] size %d\n", ret->dir_entry.size);	
		e9_printf("[ECHFS] payload %x\n", ret->dir_entry.payload);
 */
	
	
		if (!strcmp(wanted_name, ret->dir_entry.name) &&
			ret->dir_entry.parent_id == wanted_parent &&
            ret->dir_entry.type == (last_elem ? FILE_TYPE : DIR_TYPE)) {
            if (last_elem) {
		
                goto found;
            } else {
			
                wanted_parent = ret->dir_entry.payload;
                goto next;
            }
        }
			
		/*		if (!strcmp(wanted_name, ret->dir_entry.name) && ret->dir_entry.type == FILE_TYPE )
			{
				goto found;
			}*/
    }

    e9_printf("[ECHFS] file %s not found\n", fullpath);
    return -1;

found:;
	
	e9_printf("[ECHFS] Reading file: %s\n", ret->dir_entry.name);
  
    uint64_t file_block_count = ret->dir_entry.size / 4096;
	ret->block_size = 4096;
	
	e9_printf("Size is %d\n",ret->dir_entry.size);
    ret->alloc_map = (uint8_t *)malloc(ret->dir_entry.size);
	
	uint8_t * charbuffer =malloc(ret->dir_entry.size);
	e9_printf("[ELF] file block count %d \n", file_block_count);
	uint64_t totalr =0;
    for (uint64_t i = 0; i < file_block_count; i++) {

	
		
		uint64_t address =   0x100000 + (ret->dir_entry.payload  * 512 ) + (i * 4096) ;
		
		//e9_printf("address %x\n", address);
		
			totalr+= 4096;
			disk_read_4096(0, 
            &charbuffer[i * 4096],
            (uint64_t)address,
             4096);
		
    }
	
	ret->alloc_map = charbuffer;
	
	e9_printf("Read\n");
	
	//for(int x =0; x < 100; x++)
	
    return 0;
}