#include "pci.h"




struct __pci_driver;


pci_device **pci_devices = 0;
uint32_t devs = 0;

pci_driver **pci_drivers = 0;
uint32_t drivs = 0;


void add_pci_device(pci_device *pdev)
{
	pci_devices[devs] = pdev;
	devs ++;
	return;
}

uint32_t pci_read_dword(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset)
{
	uint64_t address;
	uint64_t lbus = (uint64_t)bus;
	uint64_t lslot = (uint64_t)slot;
	uint64_t lfunc = (uint64_t)func;
	uint16_t tmp = 0;
	address = (uint64_t)((lbus << 16) | (lslot << 11) |
	(lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

	write_portl (0xCF8, address);

	if (offset == 0x24)
	{  
		return read_portl(0xCFC);
	}
	else{
		uint32_t ret = read_portl(0xCFC);
		tmp = (uint16_t)((ret >> ((offset & 2) * 8)) & 0xffff);
	}
	return (tmp);
}

uint16_t pci_read_word(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset)
{
	uint64_t address;
	uint64_t lbus = (uint64_t)bus;
	uint64_t lslot = (uint64_t)slot;
	uint64_t lfunc = (uint64_t)func;
	uint16_t tmp = 0;

	address = (uint64_t)((lbus << 16) | (lslot << 11) |
	(lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

	write_portl (0xCF8, address);

	if (offset == 0x24)
	{  
		return read_portl(0xCFC);
	}
	else{
		uint32_t ret = read_portl(0xCFC); 
		tmp = (uint16_t)((ret >> ((offset & 2) * 8)) & 0xffff);
	}
	return (tmp);
}

uint16_t getVendorID(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = pci_read_word(bus,device,function,0);
	return r0;
}

uint16_t getDeviceID(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = pci_read_word(bus,device,function,2);
	return r0;
}

uint16_t getClassId(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = pci_read_word(bus,device,function,0xA);
	return (r0 & ~0x00FF) >> 8;
}

uint16_t getSubClassId(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = pci_read_word(bus,device,function,0xA);
	return (r0 & ~0xFF00);
}

uint16_t getHeaderType(uint16_t bus, uint16_t device, uint16_t function)
{
	uint32_t r0 = pci_read_word(bus,device,function,0X0E);
	return r0;
}

pci_desc_t * GetDescription(uint16_t vendor, uint16_t device, uint32_t bus, uint32_t slot)
{
	pci_desc_t * desc = malloc(sizeof(pci_desc_t));
	strcpy(desc->name, "Unknown device");

	if (vendor == 0x8086 && device == 0x2918)
	strcpy(desc->name, "82801IB (ICH9) LPC Interface Controller\0");

	if (vendor == 0x8086 && device == 0x2922)
	strcpy(desc->name, "82801IR/IO/IH (ICH9R/DO/DH) 6 port SATA Controller [AHCI mode]\0");


	if (vendor == 0x8086 && device == 0x2930)
	strcpy(desc->name, "82801I (ICH9 Family) SMBus Controller\0");

	if (vendor == 0x8086 && device == 0x10d3)
	strcpy(desc->name, "82574L Gigabit Network Connection\0");

	if (vendor == 0x8086 && device == 0x29c0)
	strcpy(desc->name, "82G33/G31/P35/P31 Express DRAM Controller\0");


	return desc;
}


void checkFunction(uint8_t bus, uint8_t device, uint8_t function) {
	uint8_t baseClass;
	uint8_t subClass;
	uint8_t secondaryBus;

	baseClass = getClassId(bus, device, function);
	subClass = getSubClassId(bus, device, function);
	if( (baseClass == 0x06) && (subClass == 0x04) ) {
		bus++;
		CheckBus(bus);
	}
	
	uint16_t vendor = getVendorID(bus, device, function);

			if(vendor == 0xFFFF)
			return;
			
			
			
			

			uint16_t deviceid = getDeviceID(bus, device, function);
			uint16_t classid = getClassId(bus, device, function);
			uint16_t subclassid = getSubClassId(bus, device, function);

			fontcolor(0x00FF00);
			e9_printf("[%d][PCI] Vendor: {%04x} Device: {%04x} Func: {%02x} Class: {%02x} Subclass {%02x},  Bus {%d}\n",GetTick(), vendor, deviceid, function, classid, subclassid, bus );
			fontcolor(0xFFFFFF);			

			pci_desc_t * name = GetDescription(vendor,deviceid, bus, deviceid);
			e9_printf("[%d][PCI] Name: %s\n",GetTick(), name->name);

			pci_device *pdev = (pci_device *)malloc(sizeof(pci_device));
			
			
			pdev->vendor = vendor;
			pdev->device = deviceid;
			pdev->func = function;
			pdev->driver = 0;
			pdev->bus = bus;
			pdev->slot = device;
		
			add_pci_device(pdev);		

			if(classid == 0x01 && subclassid == 0x06){
				satadrive = pdev;
				//ahci_device_init(pdev);
			}
}


void CheckDevice(uint32_t bus, uint32_t device)
{
	
	uint16_t function = 0;
	

	uint16_t vendor= getVendorID(bus, device, function);
	if(vendor == 0xFFFF) return;        // Device doesn't exist
	
	checkFunction(bus, device, function);
	uint16_t headerType = getHeaderType(bus, device, function);

	if( (headerType & 0x80) != 0) {
		
		for(function = 1; function < 8; function++)
		{
			checkFunction(bus, device,function);
		}
	}

}

void CheckBus(uint32_t bus)
{
	e9_printf("Checking bus\n");
	for(uint32_t device = 0; device < 32; device++)
	{
		CheckDevice(bus, device);
	}
}

void pci_probe()
{
	
	 uint8_t function;
     uint8_t bus;
 
     uint16_t headerType = getHeaderType(0, 0, 0);
     if( (headerType & 0x80) == 0) {
         /* Single PCI host controller */
         CheckBus(0);
	 
     } else {
         /* Multiple PCI host controllers */
         for(function = 0; function < 8; function++) {
             if(getVendorID(0, 0, function) != 0xFFFF) break;
             bus = function;
             CheckBus(bus);
			
         }
     }
	e9_printf("[%d][PCI] Finished PCI Probe\n",GetTick());
}

void pci_init()
{
	fontcolor(0xFFFFFF);
	e9_printf("[%d][PCI] Listing PCI:\n", GetTick());
	devs = drivs = 0;
	pci_devices = (pci_device **)malloc(32 * sizeof(pci_device));

	pci_drivers = (pci_driver **)malloc(32 * sizeof(pci_driver));

	pci_probe();
	fontcolor(0xFFFFFF);
	e9_printf("[%d][PCI] DONE \n", GetTick());
}

void pci_register_driver(pci_driver *driv)
{
	pci_drivers[drivs] = driv;
	drivs ++;
	return;
}
