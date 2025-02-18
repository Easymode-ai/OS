#pragma once
#include "ahci.h"

HBA_MEM *abar;




int find_cmdslot(HBA_PORT *port)
{
	uint32_t slots = (port->sact | port->ci);
	int cmdslots = (abar -> cap & 0x0f00) >> 8;

	for (int i=0; i<cmdslots; i++)
	{
		if ((slots&1) == 0)
		{
			//e9_printf("[AHCI] command slot %d\n", i);
			return i;
		}
		slots >>= 1;
	}
	e9_printf("[AHCI] Cannot find free command list entry\n");
	return -1;
}

bool read(HBA_PORT *port, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf)
{
	port->is = (uint32_t) -1;		
	int spin = 0; 
	int slot = find_cmdslot(port);
	if (slot == -1)
	return false;

	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);	
	cmdheader->w = 0;		
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;

	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) +
	(cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));
	int i=0;
	
	/* 8K bytes (16 sectors) per PRDT */
	for (i =0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024-1;	/* 8K bytes (this value should always be set to 1 less than the actual value) */
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;	/* 4K words */
		count -= 16;	/* 16 sectors */
	}

	cmdtbl->prdt_entry[i].dba = (uint32_t) buf;

	cmdtbl->prdt_entry[i].dbc = (count<<9)-1;	/* 512 bytes per sector */
	cmdtbl->prdt_entry[i].i = 1;

	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);

	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;
	cmdfis->command = ATA_CMD_READ_DMA_EX;

	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	// LBA mode

	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);

	cmdfis->countl = count & 0xFF;
	cmdfis->counth = (count >> 8) & 0xFF;

	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		e9_printf("[AHCI] Port is hung\n");
		return false;
	}

	port->ci = 1<<slot;

	while (1)
	{

		if ((port->ci & (1<<slot)) == 0) 
		break;
		
		if (port->is & HBA_PxIS_TFES)
		{
			e9_printf("[AHCI] Read disk error\n");
			return false;
		}	
	}

	if (port->is & HBA_PxIS_TFES)
	{
		e9_printf("[AHCI] Read disk error\n");
		return false;
	}
	
	//e9_printf("[AHCI] Read disk without error\n");
	return true;
}


bool write(HBA_PORT *port, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf)
{
	port->is = (uint32_t) -1;		
	int spin = 0; 
	int slot = find_cmdslot(port);
	if (slot == -1)
	return false;

	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER*)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);	
	cmdheader->w = 0;	
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;	

	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*)(cmdheader->ctba);
	memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) +
	(cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));
	int i=0;
	
	for (i =0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024-1;
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024;	
		count -= 16;	
	}
	
	cmdtbl->prdt_entry[i].dba = (uint32_t) buf;

	cmdtbl->prdt_entry[i].dbc = (count<<9)-1;	
	cmdtbl->prdt_entry[i].i = 1;

	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);

	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;	 /* write  */
	cmdfis->command = ATA_CMD_WRITE_DMA_EX; /* write */



	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;	

	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);

	cmdfis->countl = count & 0xFF;
	cmdfis->counth = (count >> 8) & 0xFF;
	
	while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if (spin == 1000000)
	{
		e9_printf("[AHCI] Port is hung\n");
		return false;
	}

	port->ci = 1<<slot;	// Issue command

	// Wait for completion
	while (1)
	{
		// In some longer duration reads, it may be helpful to spin on the DPS bit 
		// in the PxIS port field as well (1 << 5)
		if ((port->ci & (1<<slot)) == 0) 
		break;
		
		if (port->is & HBA_PxIS_TFES)	// Task file error
		{
			e9_printf("[AHCI] Write disk error\n");
			return false;
		}
		
		
	}

	// Check again
	if (port->is & HBA_PxIS_TFES)
	{
		e9_printf("[AHCI] Write disk error\n");
		return false;
	}
	
	e9_printf("[AHCI] Write disk without error\n");
	return true;
}


static int check_type(HBA_PORT *port)
{
	uint32_t ssts = port->ssts;

	uint8_t ipm = (ssts >> 8) & 0x0F;
	uint8_t det = ssts & 0x0F;

	if (det != HBA_PORT_DET_PRESENT)	
	return AHCI_DEV_NULL;
	if (ipm != HBA_PORT_IPM_ACTIVE)
	return AHCI_DEV_NULL;

	switch (port->sig)
	{
	case SATA_SIG_ATAPI:
		{
			e9_printf("[AHCI] %x: Device is AHCI_DEV_SATAPI\n", port);
			return AHCI_DEV_SATAPI;
		}
	case SATA_SIG_SEMB:
		{
			e9_printf("[AHCI] %x: Device is AHCI_DEV_SEMB\n", port);
			return AHCI_DEV_SEMB;
		}
	case SATA_SIG_PM:
		{
			e9_printf("[AHCI] %x: Device is AHCI_DEV_PM\n", port);
			return AHCI_DEV_PM;
		}
	default:
		{
			e9_printf("[AHCI] %x: Device is AHCI_DEV_SATA\n", port);
			return AHCI_DEV_SATA;
		}
	}
}

// Start command engine
void start_cmd(HBA_PORT *port)
{
	// Wait until CR (bit15) is cleared
	while (port->cmd & HBA_PxCMD_CR);

	// Set FRE (bit4) and ST (bit0)
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST; 
	
	e9_printf("[AHCI] start_cmd\n");
}
void stop_cmd(HBA_PORT *port)
{
	// Clear ST (bit0)
	port->cmd &= ~HBA_PxCMD_ST;

	// Clear FRE (bit4)
	port->cmd &= ~HBA_PxCMD_FRE;

	// Wait until FR (bit14), CR (bit15) are cleared
	while(1)
	{
		if (port->cmd & HBA_PxCMD_FR)
		continue;
		if (port->cmd & HBA_PxCMD_CR)
		continue;
		break;
	}
	
	e9_printf("[AHCI] stop_cmd\n");

}


void port_rebase(HBA_PORT *port, int portno, struct port_data *pdata)
{
	int i;
	uint64_t clb_addr, fbu_addr, ctb_addr;
	stop_cmd(port);

	port->clb  = (((uint64_t)AHCI_PHYS_BASE & 0xffffffff));
	port->clbu = 0;
	pdata->clb = port->clb;
	
	port->fb   = (((uint64_t)AHCI_PHYS_BASE + (uint64_t) ((32<<10)))& 0xffffffff);
	port->fbu  = ((((uint64_t)AHCI_PHYS_BASE + (uint64_t) ((32<<10)))>>32)& 0xffffffff);
	pdata->fb =     port->fb   ;
	port->serr = 1;   //For each implemented port, clear the PxSERR register, by writing 1 to each mplemented location
	port->is   = 0;
	port->ie   = 1;

	clb_addr = 0;
	clb_addr = ((( clb_addr | port->clbu ) << 32 ) | port->clb );
	clb_addr =  clb_addr + AHCI_KERN_BASE;
	memset((void *)clb_addr, 0, 1024);

	fbu_addr = 0;
	fbu_addr = ((( fbu_addr | port->fbu ) << 32 ) | port->fb );
	fbu_addr = fbu_addr + AHCI_KERN_BASE;
	memset((void*)fbu_addr, 0, 256);

	clb_addr = 0;
	clb_addr = ((( clb_addr | port->clbu ) << 32 ) | port->clb );
	clb_addr = ( clb_addr + AHCI_KERN_BASE);

	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER *) clb_addr;
	for (i = 0; i < 32; i++) {
		cmdheader[i].prdtl = 8; // 8 prdt entries per command table
		// 256 bytes per command table, 64+16+48+16*8
		// Command table offset: 40K + 8K*portno + cmdheader_index*256
		cmdheader[i].ctba  = (((uint64_t)AHCI_PHYS_BASE + (uint64_t) (( 40 << 10 )) + (uint64_t)(( i << 8 ))) & 0xffffffff);
		cmdheader[i].ctbau = ((((uint64_t)AHCI_PHYS_BASE + (uint64_t) (( 40 << 10)) + (uint64_t)(( i << 8 ))) >> 32)& 0xffffffff);

		ctb_addr = 0;
		ctb_addr = ((( ctb_addr | cmdheader[i].ctbau ) << 32 ) | cmdheader[i].ctba );
		ctb_addr =  ctb_addr + AHCI_KERN_BASE;

		memset((void*)ctb_addr, 0, 256);
	}

	start_cmd(port);
	pdata->port = port;
	port->is = 0;   
	port->ie = 0xffffffff;

}
void read_sector(struct port_data *pdata,void* read_addr, uint64_t sector_no, uint64_t sec_off, uint64_t size)
{
	uint64_t vaddr   = VIRT_PAGE(1);
	uint32_t sec_low = sector_no & 0xFFFFFFFF;
	uint32_t sec_hi  = sector_no >> 32;

	// Check for Invalid offset/size
	if (sec_off + size > SIZE_OF_SECTOR)
	return;

	read(pdata, sec_low, sec_hi, 1, size);

	memcpy(read_addr, (void*)vaddr + sec_off, size);
	e9_printf("\nSecR[%d]+%d %d %x\n\n", sec_low, sec_off, size, vaddr);
}
#define BLOCK_SIZE_IN_SECTORS 16
//static super_block* s_star = NULL;

int disk_read(int drive, void *buffer, uint64_t loc, uint64_t count) {
 
	uint64_t BLOCK_SIZE = 512;
	uint64_t progress = 0;

	while (progress < count) 
	{
		uint64_t block = (loc +progress)/BLOCK_SIZE;
		
		memset(drivebuffer,0, BLOCK_SIZE);
		
		//e9_printf("%x drivebuffer\n", drivebuffer);
		int ret = read(pdtable[0]->port, block, 0, 1, drivebuffer);	
		
		//if (!ret) return ret;
		
		//for(int x =0; x < BLOCK_SIZE; x++)
		//	e9_printf("%02X ", drivebuffer[x]);
		
	
		uint64_t chunk = count - progress;
        uint64_t offset = (loc + progress) % BLOCK_SIZE;
        if (chunk > BLOCK_SIZE - offset)
            chunk = BLOCK_SIZE - offset;
		
		memcpy(buffer + progress, &drivebuffer[offset], chunk);
		progress += chunk;
	}

    return 0;
}

int disk_read_4096(int drive, void *buffer, uint64_t loc, uint64_t count) {
 
	uint64_t BLOCK_SIZE = 4096;
	uint64_t progress = 0;

	while (progress < count) 
	{
		uint64_t block = (loc)/512;
		
		memset(drivebuffer,0, BLOCK_SIZE);
		int ret = read(pdtable[0]->port, block, 0, 8, drivebuffer);	
		
		//if (!ret) return ret;
	
		/*uint64_t chunk = count - progress;
        uint64_t offset = (loc + progress) % BLOCK_SIZE;
        if (chunk > BLOCK_SIZE - offset)
            chunk = BLOCK_SIZE - offset;*/
		
		memcpy(buffer, drivebuffer, BLOCK_SIZE);
		progress += BLOCK_SIZE;
	}

    return 0;
}

static int read_block(struct echfs_file_handle *file, void *buf, uint64_t block, uint64_t offset, uint64_t count) {

	//return part_read(&file->part, buf, (file->alloc_map[block] * file->block_size) + offset, count);
	return 0;
}




struct port_data ** probe_port(HBA_MEM *abar_temp)
{
	
	// Search disk in implemented ports
	uint32_t pi = abar_temp->pi;
	int i = 0;
	pdtable = malloc(32*sizeof(void *));

	
	while (i<32)
	{
		
		if (pi & 1)
		{
			
			
			int dt = check_type(&abar_temp->ports[i]);

			
			if (dt == AHCI_DEV_SATA)
			{
				e9_printf("[%d][AHCI] SATA drive found at port %d\n",GetTick(), i);
				abar = abar_temp;
				e9_printf("[%d][AHCI] rebase %d\n",GetTick(), i);
				
				pdtable[i] = malloc(sizeof(struct port_data));

				port_rebase(abar->ports, i, pdtable[i]);
				e9_printf("[%d][ACHI] port rebased\n",GetTick());
				
				
				
				//	asm("jmp .");
				
				//	screen[imageIdx] = tempRGB;
				
				
				
			}
			
			else if (dt == AHCI_DEV_SATAPI)
			{
				e9_printf("[AHCI] SATAPI drive found on port %d\n", i);
			}
			else if (dt == AHCI_DEV_SEMB)
			{
				e9_printf("[AHCI] SEMB drive found on  port %d\n", i);
			}
			else if (dt == AHCI_DEV_PM)
			{
				e9_printf("[AHCI] PM drive found on port %d\n", i);
			}
			else
			{
				fontcolor(0xAAAAAA);
				e9_printf("[%d][AHCI] No drive found on port %d\n",GetTick(), i);
				fontcolor(0xAAAAAA);
			}
		}

		pi >>= 1;
		i ++;
	}
	return pdtable;
}

int ahci_device_init(pci_device* d)
{
	e9_printf("[%d][AHCI] Retrieve Bar5 Address\n", GetTick());
	uint16_t * bar5 = pci_read_dword(d->bus, d->slot, d->func, 0x24);
	fontcolor(0xAAAAAA);
	e9_printf("[%d][AHCI] Got Bar[5] Address: %x\n", GetTick(), bar5);
	e9_printf("[%d][AHCI] Probing Bar[5]\n",GetTick());
	struct port_data ** pdtable = probe_port(bar5);	
	fontcolor(0xFFFFFF);
	
	
	drivebuffer = 0x10000000;
	
	uint8_t * bitmapbufferRAM = (uint8_t *)malloc(4096);
	
	virtual_map((uint64_t)kernel_p4, (uint64_t)drivebuffer, (uint64_t)bitmapbufferRAM, (uint64_t) 4096/512, 1, 1);
	//write_cr3((uint64_t)kernel_p4);
//	asm("jmp .");
	//uint8_t * bitmapbufferRAM = (uint8_t *)malloc(0x09384036);
	
	//uint8_t * drivebuffer = 0x30000000;//(uint8_t *)frame_alloc(0x09384036);
	
	//e9_printf("Drive buffer %x\n", drivebuffer);
	//kernel_p4 = read_cr3();
//	e9_printf("CR3 is %x\n", kernel_p4);
	//e9_printf("boo\n");
//	virtual_map((uint64_t)kernel_p4, (uint64_t)0x30000000,(uint64_t)bitmapbufferRAM,(uint64_t)0x09384036/512, 1, 0);
	
	//e9_printf("bob\n");
	///int res=1;
	//int y =0;
	
	//uint64_t raddress = 0x0; /*  read address 0x0 - disk-0x1000 */
	//uint64_t block = raddress/512;
	//char buff[16] =  {};
	
	//int multiplier = 8;
	//int blocksize = 512;
	
	/*
					read 4096 blocks from SATA disk, there is a bug where it ends prematurely at the end of drive by 512 bytes.
				*/
					
			//	res = read(&abar->ports[i], block, 0, 1, common_buf);

			//	for(int x= 0; x < 10; x++)
			//		common_buf[x] = 0x61;
					
			//	res = write(&abar->ports[i], 0, 0, 1, common_buf);

			/*	e9_printf("\n wrote 0x61 to disk * 10\n");			
				
				while (res != 0) 
				{
					y=0;
					e9_printf("[AHCI] Read SATA device [Port %d][%x] @ %x\n", pdtable[0]->port, block *(blocksize));	
					
					memset(drivebuffer,0, 4096);
				
					res = read(pdtable[0]->port, block, 0, multiplier, drivebuffer);
					
					if (res != 0)
					{		
						for(uint64_t x = 0; x < (512 * multiplier) ; x++)
						{
							if (y == 0)
							{
								e9_printf("%08X: ", (block * (blocksize)) + x);	
								memset(buff,0, 10);
							}
							
							buff[y] = drivebuffer[x];	
							e9_printf("%02X ", (uint8_t)drivebuffer[x]);		
							y++;
							
							if (y > 15) 
							{
								for (int q =0 ; q < 16; q++)
								{
									if ((int)buff[q] > (int)0x33 )
									e9_printf("%c",buff[q]);
									else
									e9_printf(".");
								}
								
								
								y=0; 
								
								e9_printf("\n");
							
							}	
						}
					}	
					block += multiplier;
					if (block > 1) 	break;
				}
*/
	
	/* read bitmap from sata disk */
	
	uint32_t bmp = 0x00330000;
/*	uint32_t ret = 0;
	
	block= bmp/512; /* block position / 512 byte blocks 

	uint64_t ticks = GetTicks();
	e9_printf("[%d] Read SATA disk %d @ %x\n",GetTick(),pdtable[0]->port, bmp);
	/* copy length of bitmap file from disk to memory 4096 (8*512) bytes at a time 
	while (ret < 0x00384036) 
	{
		read(pdtable[0]->port, block, 0, 8, drivebuffer);	
		memcpy(bitmapbufferRAM + ret, drivebuffer, 512*8);
		ret += 512*8;
		block += 8;
	}
	
	
	e9_printf("[%d] Read %d bytes disk @ %x\n",GetTick(),ret);
	uint64_t ticksnow = GetTicks();


	uint64_t tickstaken = ticksnow - ticks;
	
	
	struct  tagBITMAPFILEHEADER * bitmapdata = malloc(sizeof( struct tagBITMAPFILEHEADER));
	memcpy(bitmapdata, bitmapbufferRAM, sizeof(struct tagBITMAPFILEHEADER));
	
	struct  tagBITMAPINFOHEADER * bitmapheader = malloc(sizeof( struct tagBITMAPINFOHEADER));
	memcpy(bitmapheader, bitmapbufferRAM + sizeof(struct tagBITMAPFILEHEADER), sizeof(struct tagBITMAPINFOHEADER));

	int offset=bitmapdata->bfOffBits-3;
	
	e9_printf("%d \n", bitmapheader->biHeight);
	e9_printf("%d \n", bitmapheader->biWidth);
	for(int y =bitmapheader->biHeight; y > 0; y--)
	{	
		for(int x = 0; x < bitmapheader->biWidth; x++)
		{				
			uint32_t where =  x + (y * videoheader->framebuffer_pitch/4)- videoheader->framebuffer_pitch/4;
			
			int b = (int)bitmapbufferRAM[offset];
			int g = (int)bitmapbufferRAM[offset+1];
			int r = (int)bitmapbufferRAM[offset+2];
			int a = (int)bitmapbufferRAM[offset+3];
			
			uint32_t color = ((r & 0xff) << 24) + ((g & 0xff) << 16) + ((b & 0xff) << 8)
			+ (a & 0xff);
			
			uint32_t * screen = videoheader->framebuffer_addr;	
			screen[where] = color;
			
			offset += 4;
		}
	}
	
	ResetPosition();
	
	e9_printf("Read %d bytes from SATA to Memory @ %x, blit pixels to Framebuffer @ %x\n", ret,bitmapbufferRAM, videoheader->framebuffer_addr); 
	fontcolor(0xffffff);
	
	e9_printf("Finished loading OS\n");
	e9_printf("[PIT] Tick: %d\n", GetTick());*/
}

