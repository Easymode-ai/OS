#pragma once

#define FIXED_HIGHER_HALF_OFFSET_64 ((uint64_t)0xffffffff80000000)


#define PT_LOAD     0x00000001
#define PT_INTERP   0x00000003
#define PT_PHDR     0x00000006

#define ABI_SYSV    0x00
#define ARCH_X86_64 0x3e
#define ARCH_X86_32 0x03
#define BITS_LE     0x01
#define ET_DYN      0x0003
#define SHT_RELA    0x00000004
#define R_X86_64_RELATIVE 0x00000008

/* Indices into identification array */
#define EI_CLASS    4
#define EI_DATA     5
#define EI_VERSION  6
#define EI_OSABI    7

	
struct elf64_hdr {
    uint8_t  ident[16];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint64_t entry;
    uint64_t phoff;
    uint64_t shoff;
    uint32_t flags;
    uint16_t hdr_size;
    uint16_t phdr_size;
    uint16_t ph_num;
    uint16_t shdr_size;
    uint16_t sh_num;
    uint16_t shstrndx;
};
struct elf64_phdr {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
};

struct elf64_shdr {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
};

struct elf64_rela {
    uint64_t r_addr;
    uint32_t r_info;
    uint32_t r_symbol;
    uint64_t r_addend;
};

int  fread(struct file_handle * fd, void * buffer, uint64_t pos, uint64_t size )
{
	if (fd->is_memfile) {
	memcpy(buffer,fd->fd+pos, size); 
	}
	else {
		struct echfs_file_handle * fdr = (struct echfs_file_handle *)fd->fd;
		//e9_printf("hello %x\n", fdr->alloc_map);
		memcpy(buffer,fdr->alloc_map +pos , size); 
	}
}
int elf_bits(struct file_handle *fd) {
    struct elf64_hdr hdr;
    fread(fd, &hdr, 0, 20);

    if (strncmp((char *)hdr.ident, "\177ELF", 4)) {
        e9_print("elf: Not a valid ELF file.\n");
        return -1;
    }

    switch (hdr.machine) {
        case ARCH_X86_64:
            return 64;
        case ARCH_X86_32:
            return 32;
        default:
            return -1;
    }
}

int elf64_load_section(struct file_handle *fd, void *buffer, const char *name, size_t limit, uint64_t slide) {
    struct elf64_hdr hdr;
    fread(fd, &hdr, 0, sizeof(struct elf64_hdr));

    if (strncmp((char *)hdr.ident, "\177ELF", 4)) {
        e9_print("elf: Not a valid ELF file.\n");
        return 1;
    }

    if (hdr.ident[EI_DATA] != BITS_LE) {
        e9_print("elf: Not a Little-endian ELF file.\n");
        return 1;
    }

    if (hdr.machine != ARCH_X86_64) {
        e9_print("elf: Not an x86_64 ELF file.\n");
        return 1;
    }
	
	e9_print("Booting elf\n");

    struct elf64_shdr shstrtab;
    fread(fd, &shstrtab, hdr.shoff + hdr.shstrndx * sizeof(struct elf64_shdr),
            sizeof(struct elf64_shdr));

    char *names = malloc(shstrtab.sh_size);
    fread(fd, names, shstrtab.sh_offset, shstrtab.sh_size);

    for (uint16_t i = 0; i < hdr.sh_num; i++) {
        struct elf64_shdr section;
        fread(fd, &section, hdr.shoff + i * sizeof(struct elf64_shdr),
                   sizeof(struct elf64_shdr));

        if (!strcmp(&names[section.sh_name], name)) {
            if (section.sh_size > limit)
                return 3;
            if (section.sh_size < limit)
                return 4;
            fread(fd, buffer, section.sh_offset, section.sh_size);
            return elf64_apply_relocations(fd, &hdr, buffer, section.sh_addr, section.sh_size, slide);
        }
    }

    return 2;
}
 int elf64_apply_relocations(struct file_handle *fd, struct elf64_hdr *hdr, void *buffer, uint64_t vaddr, size_t size, uint64_t slide) {
    // Find RELA sections
    for (uint16_t i = 0; i < hdr->sh_num; i++) {
        struct elf64_shdr section;
        fread(fd, &section, hdr->shoff + i * sizeof(struct elf64_shdr),
                    sizeof(struct elf64_shdr));

        if (section.sh_type != SHT_RELA)
            continue;

        if (section.sh_entsize != sizeof(struct elf64_rela)) {
            e9_printf("elf: Unknown sh_entsize for RELA section!\n");
            return 1;
        }

        // This is a RELA header, get and apply all relocations
        for (uint64_t offset = 0; offset < section.sh_size; offset += section.sh_entsize) {
            struct elf64_rela relocation;
            fread(fd, &relocation, section.sh_offset + offset, sizeof(relocation));

            switch (relocation.r_info) {
                case R_X86_64_RELATIVE:
                    // Relocation is before buffer
                    if (relocation.r_addr < vaddr)
                        continue;

                    // Relocation is after buffer
                    if (vaddr + size < relocation.r_addr + 8)
                        continue;

                    // It's inside it, calculate where it is
                    uint64_t *ptr = (uint64_t *)((uint8_t *)buffer - vaddr + relocation.r_addr);

                    // Write the relocated value
                    *ptr = slide + relocation.r_addend;
                    break;

                default:
                    e9_printf("elf: Unknown RELA type: %X\n", relocation.r_info);
                    return 1;
            }
        }
    }

    return 0;
}
int elf64_load(struct file_handle *fd, uint64_t *entry_point, uint64_t *top, uint64_t slide, uint32_t alloc_type) {
    struct elf64_hdr hdr;
    fread(fd, &hdr, 0, sizeof(struct elf64_hdr));

    if (strncmp((char *)hdr.ident, "\177ELF", 4)) {
        e9_print("Not a valid ELF file.\n");
        return -1;
    }

    if (hdr.ident[EI_DATA] != BITS_LE) {
        e9_print("Not a Little-endian ELF file.\n");
        return -1;
    }

    if (hdr.machine != ARCH_X86_64) {
        e9_print("Not an x86_64 ELF file.\n");
        return -1;
    }

	e9_printf("[ELF] signature is ok \n");
    *top = 0;
	//e9_printf("bob %d \n", hdr.ph_num);
    for (uint16_t i = 0; i < hdr.ph_num; i++) {
        struct elf64_phdr phdr;
        fread(fd, &phdr, hdr.phoff + i * sizeof(struct elf64_phdr),
                   sizeof(struct elf64_phdr));

		//e9_printf("bob %x ? %x\n", phdr.p_type, PT_LOAD);
		
        if (phdr.p_type != PT_LOAD)
            continue;

        uint64_t load_vaddr = phdr.p_vaddr;

        if (load_vaddr & ((uint64_t)1 << 63))
            load_vaddr -= FIXED_HIGHER_HALF_OFFSET_64;

        load_vaddr += slide;

        uint64_t this_top = load_vaddr + phdr.p_memsz;

        if (this_top > *top)
            *top = this_top;
	
		struct echfs_file_handle * fdr = (struct echfs_file_handle *)fd->fd;
		//virtual_map((uint64_t)kernel_p4,load_vaddr, load_vaddr,(uint64_t)phdr.p_filesz/PAGE_SIZE, 1,1);
	
		memcpy( load_vaddr ,(void *)((uint32_t)fdr->alloc_map + phdr.p_offset), (uint32_t)phdr.p_filesz );
     //   fread(fd, 0x10000000 + phdr.p_offset , phdr.p_offset, phdr.p_filesz);
		
		//e9_printf("%x\n", 0x400000);
        size_t to_zero = (size_t)(phdr.p_memsz - phdr.p_filesz);

        if (to_zero) {
            void *ptr = (void *)(uint32_t)(load_vaddr + phdr.p_filesz);
			memset(ptr, 0, to_zero);
        }

        if (elf64_apply_relocations(fd, &hdr, (void *)(uint32_t)load_vaddr, phdr.p_vaddr, phdr.p_memsz, slide))
		{
			e9_printf("\n[ELF] relocations applied\n");
            return -1;
		}
		
		e9_printf("\n[ELF] vaddr: %x load vaddr: %x memsize: %x\n", phdr.p_vaddr, load_vaddr, phdr.p_memsz);
    }

    *entry_point = (hdr.entry ) ;//+ 0x7000000;

    return 0;
}

uint64_t the_entry_point;



void usermodetest()
{
	//syscall(0,0,0,0);
	//syscall(0,0,0,0); 
	
	
	/*asm volatile ("out %0, %1" : : "a"('H'), "Nd"(0xe9));
	asm volatile ("out %0, %1" : : "a"('E'), "Nd"(0xe9));
	asm volatile ("out %0, %1" : : "a"('L'), "Nd"(0xe9));
	asm volatile ("out %0, %1" : : "a"('L'), "Nd"(0xe9));
	asm volatile ("out %0, %1" : : "a"('O'), "Nd"(0xe9));
	asm volatile ("out %0, %1" : : "a"('\n'), "Nd"(0xe9));
	*/
	//debug_log();
	 //asm volatile ("out %0, %1" : : "a"(0x67), "Nd"(0xe9));
	//e9_printf("hi\n");
	int ret;
	int errno;
		
	asm volatile ("syscall"                  
                  : "=a"(ret), "=d"(errno)   
                  : "a"(0)                 
                  : "rcx", "r11", "memory"); 
	

			
				
	asm("jmp .");		  
	while(1)
		asm volatile ("hlt");
/*	write_port(0xe9, 0x61);
	
	uint8_t * r = 0x0;
	r = 0x61;
	
	e9_printf("bob\n");
	asm("jmp .");
	e9_printf("Hello: %x\n", r);
//	asm("jmp .");


/*
	uint8_t * r = 0x0;
	r = 0x61;
	//e9_printf("U%x\n", r);
	uint8_t * q = 0x0;
	q= 0x61;
	asm("jmp .");
	


	
	
	
	e9_printf("USER MODE\n");
	void (*ptr)() = (uint64_t)the_entry_point;	
	ptr();
	
	asm("jmp .");*/
}


void Process(char const *  filename)
{
	ClearScreen();
	e9_printf("Loading process\n");
	  uint16_t hint = 0;

		
		disk_read(0, &hint, 444, sizeof(uint16_t));
		
		e9_printf("hint value %x\n", hint);
		     if (hint && hint != 0x5a5a)
			 {
				 e9_printf("invalid table\n");
				 
				 	struct gpt_table_header header = {0};
					disk_read(0, &header, (void *)(4096 * 1), sizeof(header));
					if (strncmp(header.signature, "EFI PART", 8))
					e9_printf("invalid table\n");
						else
					e9_printf("Valid table\n");
				
					if (header.revision != 0x00010000)
					e9_printf("end of table\n");
			 }else
			 {
				 e9_printf("valid table\n");
			 
			 
	struct echfs_file_handle * fd = malloc(sizeof(struct echfs_file_handle));

	struct part extended_part;
	
	e9_printf("[ELF] Loading file\n");
	int rr = echfs_open(fd, &extended_part,filename);
	
	e9_printf("Opened\n");
	if (rr)
	e9_printf("[AHCI] Could not find file: %s\n", filename);
	else{

		struct file_handle *ret = malloc(sizeof(struct file_handle));
		
		ret->fd   = (void *)fd;
		ret->read = (void *)echfs_read;
		ret->size = fd->dir_entry.size;
		
		e9_printf("File size %d %x\n", ret->size, ret->size);
		
		
		uint64_t entry_point   = 0;
		uint64_t top_used_addr = 0;	
		uint64_t slide =0x0;
		
		//int retr = 	elf64_load(ret, &entry_point, &top_used_addr, slide, 10);

		//print_table(kernel_p4, PAGE_SIZE);
	//	virtual_map((uint64_t)kernel_p4, entry_point, 0x10000000 ,(uint64_t)ret->size/PAGE_SIZE, 1,1);
		//virtual_map((uint64_t)kernel_p4, entry_point, 0x13000000 ,(uint64_t)ret->size/PAGE_SIZE, 1,1);
		
		//e9_printf("[ELF] Top used address in ELF: %x\n", top_used_addr);
		e9_printf("[ELF] Entry Point in ELF: %x\n", entry_point);
	
		//e9_printf("Load tss\n");
		
		//virtual_map((uint64_t)kernel_p4, 0x30000000, 0x30000000 ,(uint64_t)0x10000000/PAGE_SIZE, 1,1);
		//e9_printf("%x addr\n",kernel_p4);
		
		//virtual_map((uint64_t)kernel_p4, 0x1435000000,0x190000000, (uint64_t)7, 0,0);
		//write_cr3((uint64_t)kernel_p4);
		//	print_table(kernel_p4,PAGE_SIZE);

	//	load_tss();
		
		//init_usergdt();
		e9_printf("user gdt\n");
		the_entry_point = entry_point;
		usermode();
	asm ("jmp .");
		/*e9_printf("Done\n");
		//ClearScreen();
		*/
		e9_printf("user mode\n");
		usermodetest();
		e9_printf("user mode \n");
		asm("jmp .");
		
		
		free(ret);
	}
}
	}

