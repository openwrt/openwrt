#ifndef _DANUBE_DEVICES_H__
#define _DANUBE_DEVICES_H__

#include "../common/devices.h"

enum {
	PCI_CLOCK_INT = 0,
	PCI_CLOCK_EXT
};

void __init ar9_register_usb(void);
void __init ar9_register_ebu_gpio(struct resource *resource, u32 value);
void __init ar9_register_ethernet(unsigned char *mac);
void __init ar9_register_pci(int clock, int irq_mask);

#endif
