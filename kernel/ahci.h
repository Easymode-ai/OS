
#include <stdint.h>
#include <stdbool.h>
#include "vmm.h"
#include "kernel/pci.h"
#include "ahci_struct.h"
#include "fs/echfs.h"

#define	SATA_SIG_ATA	0x00000101	/* SATA drive */
#define	SATA_SIG_ATAPI	0xEB140101	/* SATAPI drive */
#define	SATA_SIG_SEMB	0xC33C0101	/* Enclosure management bridge */
#define	SATA_SIG_PM	0x96690101		/* Port multiplier */

#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define AHCI_DEV_SATAPI 4

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

#define ATA_DEV_BUSY 0x80
#define ATA_DEV_DRQ 0x08
#define HBA_PxCMD_CR            (1 << 15) /* CR - Command list Running */
#define HBA_PxCMD_FR            (1 << 14) /* FR - FIS receive Running */
#define HBA_PxCMD_FRE           (1 <<  4) /* FRE - FIS Receive Enable */
#define HBA_PxCMD_SUD           (1 <<  1) /* SUD - Spin-Up Device */
#define HBA_PxCMD_ST            (1 <<  0) /* ST - Start (command processing) */

HBA_MEM *abar;
struct port_data **pdtable;


struct port_data {
	void *clb;
	void *fb;
	void *ctba[32];
	HBA_PORT *port;
};


int find_cmdslot(HBA_PORT *port);
bool read(HBA_PORT *port, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf);
bool write(HBA_PORT *port, uint32_t startl, uint32_t starth, uint32_t count, uint16_t *buf);


static int check_type(HBA_PORT *port);

typedef struct {
	int inode_num;
	char filename[100];
	int perm;
	int size;
	char type;
	int sector_loc[10];
	int written;
} inode;


struct echfs_identity_table {
	uint8_t  jmp[4];
	char     signature[8];
	uint64_t block_count;
	uint64_t dir_length;
	uint64_t block_size;
	uint32_t reserved;
	struct guid guid;
} __attribute__((packed));

void start_cmd(HBA_PORT *port);
void stop_cmd(HBA_PORT *port);

#define AHCI_KERN_BASE  0x4000000
#define AHCI_PHYS_BASE 0x4000000
#define AHCI_VIRT_BASE 0x4000000

#define PHYS_PAGE(a) (AHCI_PHYS_BASE + (a * 0x1000))
#define VIRT_PAGE(a) (AHCI_VIRT_BASE + (a * 0x1000))

#define SIZE_OF_SECTOR          512

void port_rebase(HBA_PORT *port, int portno, struct port_data *pdata);


void read_sector(struct port_data *pdata,void* read_addr, uint64_t sector_no, uint64_t sec_off, uint64_t size);
//static super_block* s_star = NULL;
int disk_read(int drive, void *buffer, uint64_t loc, uint64_t count);
int disk_read_4096(int drive, void *buffer, uint64_t loc, uint64_t count) ;
static int read_block(struct echfs_file_handle *file, void *buf, uint64_t block, uint64_t offset, uint64_t count);

#define INODE_S 23
#define INODE_E 151

#define SUPER_BLOCK_SECTOR      1
typedef struct tagBITMAPINFOHEADER
{
	uint32_t biSize;  //specifies the number of bytes required by the struct
	uint32_t biWidth;  //specifies width in pixels
	uint32_t biHeight;  //species height in pixels
	uint16_t biPlanes; //specifies the number of color planes, must be 1
	uint16_t biBitCount; //specifies the number of bit per pixel
	uint32_t biCompression;//spcifies the type of compression
	uint32_t biSizeImage;  //size of image in bytes
	uint32_t biXPelsPerMeter;  //number of pixels per meter in x axis
	uint32_t biYPelsPerMeter;  //number of pixels per meter in y axis
	uint32_t biClrUsed;  //number of colors used by th ebitmap
	uint32_t biClrImportant;  //number of colors that are important
} __attribute__ ((packed));

typedef struct tagBITMAPFILEHEADER
{
	uint16_t bfType;  //specifies the file type
	uint32_t bfSize;  //specifies the size in bytes of the bitmap file
	uint16_t bfReserved1;  //reserved; must be 0
	uint16_t bfReserved2;  //reserved; must be 0
	uint32_t bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
}__attribute__ ((packed));

typedef struct rgba
{
	
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
}__attribute__ ((packed));


struct port_data ** probe_port(HBA_MEM *abar_temp);


int ahci_device_init(pci_device* d);