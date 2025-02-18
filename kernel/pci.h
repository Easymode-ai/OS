#pragma once
#include <stdint.h>
#include <stddef.h>

typedef struct {

	char name[512];

} pci_desc_t;

pci_desc_t * GetDescription(uint16_t vendor, uint16_t device, uint32_t bus, uint32_t slot);


typedef struct {
	uint16_t vendor;
	uint16_t device;
	uint16_t func;
	uint16_t bus;
	uint16_t slot;
	struct __pci_driver *driver;
} pci_device;

typedef struct {
	uint32_t vendor;
	uint32_t device;
	uint32_t func;

} pci_device_id;

typedef struct __pci_driver {
	pci_device_id *table;
	char *name;
	uint8_t (*init_one)(pci_device *);
	uint8_t (*init_driver)(void);
	uint8_t (*exit_driver)(void);
} pci_driver;

struct pci_device *satadrive;

void add_pci_device(pci_device *pdev);
uint32_t pci_read_dword(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset);
uint16_t pci_read_word(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset);
uint16_t getVendorID(uint16_t bus, uint16_t device, uint16_t function);
uint16_t getDeviceID(uint16_t bus, uint16_t device, uint16_t function);
uint16_t getClassId(uint16_t bus, uint16_t device, uint16_t function);
uint16_t getSubClassId(uint16_t bus, uint16_t device, uint16_t function);
uint16_t getHeaderType(uint16_t bus, uint16_t device, uint16_t function);
void checkFunction(uint8_t bus, uint8_t device, uint8_t function);
void CheckDevice(uint32_t bus, uint32_t device);
void CheckBus(uint32_t bus);