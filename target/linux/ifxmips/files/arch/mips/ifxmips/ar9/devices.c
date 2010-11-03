#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/mtd/physmap.h>
#include <linux/kernel.h>
#include <linux/reboot.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/etherdevice.h>
#include <linux/reboot.h>
#include <linux/time.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/leds.h>

#include <asm/bootinfo.h>
#include <asm/irq.h>

#include <ifxmips.h>
#include <ifxmips_irq.h>

/* pci */
extern int ifxmips_pci_external_clock;
extern int ifxmips_pci_req_mask;

void __init
ar9_register_pci(int clock, int irq_mask)
{
	ifxmips_pci_external_clock = clock;
	if(irq_mask)
		ifxmips_pci_req_mask = irq_mask;
}
