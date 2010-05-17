#ifndef _DANUBE_DEVICES_H__
#define _DANUBE_DEVICES_H__

#include "../common/devices.h"

enum {
	PCI_CLOCK_INT = 0,
	PCI_CLOCK_EXT
};

void __init danube_register_usb(void);
void __init danube_register_ebu_gpio(struct resource *resource, u32 value);
void __init danube_register_ethernet(unsigned char *mac, int mii_mode);
void __init danube_register_pci(int clock, int irq_mask);

#endif
