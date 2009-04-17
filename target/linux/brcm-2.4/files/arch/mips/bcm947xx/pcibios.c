/*
 * Low-Level PCI and SB support for BCM47xx (Linux support code)
 *
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */

#include <linux/config.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/paccess.h>

#include <typedefs.h>
#include <osl.h>
#include <sbconfig.h>
#include <sbutils.h>
#include <hndpci.h>
#include <pcicfg.h>
#include <bcmdevs.h>
#include <bcmnvram.h>

/* Global SB handle */
extern sb_t *bcm947xx_sbh;
extern spinlock_t bcm947xx_sbh_lock;

/* Convenience */
#define sbh bcm947xx_sbh
#define sbh_lock bcm947xx_sbh_lock

static int
sbpci_read_config_byte(struct pci_dev *dev, int where, u8 *value)
{
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&sbh_lock, flags);
	ret = sbpci_read_config(sbh, dev->bus->number, PCI_SLOT(dev->devfn),
		PCI_FUNC(dev->devfn), where, value, sizeof(*value));
	spin_unlock_irqrestore(&sbh_lock, flags);
	return ret ? PCIBIOS_DEVICE_NOT_FOUND : PCIBIOS_SUCCESSFUL;
}

static int
sbpci_read_config_word(struct pci_dev *dev, int where, u16 *value)
{
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&sbh_lock, flags);
	ret = sbpci_read_config(sbh, dev->bus->number, PCI_SLOT(dev->devfn),
		PCI_FUNC(dev->devfn), where, value, sizeof(*value));
	spin_unlock_irqrestore(&sbh_lock, flags);
	return ret ? PCIBIOS_DEVICE_NOT_FOUND : PCIBIOS_SUCCESSFUL;
}

static int
sbpci_read_config_dword(struct pci_dev *dev, int where, u32 *value)
{
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&sbh_lock, flags);
	ret = sbpci_read_config(sbh, dev->bus->number, PCI_SLOT(dev->devfn),
		PCI_FUNC(dev->devfn), where, value, sizeof(*value));
	spin_unlock_irqrestore(&sbh_lock, flags);
	return ret ? PCIBIOS_DEVICE_NOT_FOUND : PCIBIOS_SUCCESSFUL;
}

static int
sbpci_write_config_byte(struct pci_dev *dev, int where, u8 value)
{
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&sbh_lock, flags);
	ret = sbpci_write_config(sbh, dev->bus->number, PCI_SLOT(dev->devfn),
		PCI_FUNC(dev->devfn), where, &value, sizeof(value));
	spin_unlock_irqrestore(&sbh_lock, flags);
	return ret ? PCIBIOS_DEVICE_NOT_FOUND : PCIBIOS_SUCCESSFUL;
}

static int
sbpci_write_config_word(struct pci_dev *dev, int where, u16 value)
{
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&sbh_lock, flags);
	ret = sbpci_write_config(sbh, dev->bus->number, PCI_SLOT(dev->devfn),
		PCI_FUNC(dev->devfn), where, &value, sizeof(value));
	spin_unlock_irqrestore(&sbh_lock, flags);
	return ret ? PCIBIOS_DEVICE_NOT_FOUND : PCIBIOS_SUCCESSFUL;
}

static int
sbpci_write_config_dword(struct pci_dev *dev, int where, u32 value)
{
	unsigned long flags;
	int ret;

	spin_lock_irqsave(&sbh_lock, flags);
	ret = sbpci_write_config(sbh, dev->bus->number, PCI_SLOT(dev->devfn),
		PCI_FUNC(dev->devfn), where, &value, sizeof(value));
	spin_unlock_irqrestore(&sbh_lock, flags);
	return ret ? PCIBIOS_DEVICE_NOT_FOUND : PCIBIOS_SUCCESSFUL;
}

static struct pci_ops pcibios_ops = {
	sbpci_read_config_byte,
	sbpci_read_config_word,
	sbpci_read_config_dword,
	sbpci_write_config_byte,
	sbpci_write_config_word,
	sbpci_write_config_dword
};


void __init
pcibios_init(void)
{
	ulong flags;

	if (!(sbh = sb_kattach(SB_OSH)))
		panic("sb_kattach failed");
	spin_lock_init(&sbh_lock);

	spin_lock_irqsave(&sbh_lock, flags);
	sbpci_init(sbh);
	spin_unlock_irqrestore(&sbh_lock, flags);

	set_io_port_base((unsigned long) ioremap_nocache(SB_PCI_MEM, 0x04000000));
	mdelay(300); /* workaround for atheros cards */

	/* Scan the SB bus */
	pci_scan_bus(0, &pcibios_ops, NULL);

}

char * __init
pcibios_setup(char *str)
{
	if (!strncmp(str, "ban=", 4)) {
		sbpci_ban(simple_strtoul(str + 4, NULL, 0));
		return NULL;
	}

	return (str);
}

static u32 pci_iobase = 0x100;
static u32 pci_membase = SB_PCI_DMA;
static u32 pcmcia_membase = 0x40004000;

void __init
pcibios_fixup_bus(struct pci_bus *b)
{
	struct list_head *ln;
	struct pci_dev *d;
	struct resource *res;
	int pos, size;
	u32 *base;
	u8 irq;

	printk("PCI: Fixing up bus %d\n", b->number);

	/* Fix up SB */
	if (b->number == 0) {
		for (ln = b->devices.next; ln != &b->devices; ln = ln->next) {
			d = pci_dev_b(ln);
			/* Fix up interrupt lines */
			pci_read_config_byte(d, PCI_INTERRUPT_LINE, &irq);
			d->irq = irq + 2;
			pci_write_config_byte(d, PCI_INTERRUPT_LINE, d->irq);
		}
	}

	/* Fix up external PCI */
	else {
		for (ln = b->devices.next; ln != &b->devices; ln = ln->next) {
			d = pci_dev_b(ln);
			/* Fix up resource bases */
			for (pos = 0; pos < 6; pos++) {
				res = &d->resource[pos];
				base = (res->flags & IORESOURCE_IO) ? &pci_iobase : ((b->number == 2) ? &pcmcia_membase : &pci_membase);
				if (res->end) {
					size = res->end - res->start + 1;
					if (*base & (size - 1))
						*base = (*base + size) & ~(size - 1);
					res->start = *base;
					res->end = res->start + size - 1;
					*base += size;
					pci_write_config_dword(d,
						PCI_BASE_ADDRESS_0 + (pos << 2), res->start);
				}
				/* Fix up PCI bridge BAR0 only */
				if (b->number == 1 && PCI_SLOT(d->devfn) == 0)
					break;
			}
			/* Fix up interrupt lines */
			if (pci_find_device(VENDOR_BROADCOM, SB_PCI, NULL))
				d->irq = (pci_find_device(VENDOR_BROADCOM, SB_PCI, NULL))->irq;
			pci_write_config_byte(d, PCI_INTERRUPT_LINE, d->irq);
		}
	}
}

unsigned int
pcibios_assign_all_busses(void)
{
	return 1;
}

void
pcibios_align_resource(void *data, struct resource *res,
	unsigned long size, unsigned long align)
{
}

int
pcibios_enable_resources(struct pci_dev *dev)
{
	u16 cmd, old_cmd;
	int idx;
	struct resource *r;

	/* External PCI only */
	if (dev->bus->number == 0)
		return 0;

	pci_read_config_word(dev, PCI_COMMAND, &cmd);
	old_cmd = cmd;
	for (idx = 0; idx < 6; idx++) {
		r = &dev->resource[idx];
		if (r->flags & IORESOURCE_IO)
			cmd |= PCI_COMMAND_IO;
		if (r->flags & IORESOURCE_MEM)
			cmd |= PCI_COMMAND_MEMORY;
	}
	if (dev->resource[PCI_ROM_RESOURCE].start)
		cmd |= PCI_COMMAND_MEMORY;
	if (cmd != old_cmd) {
		printk("PCI: Enabling device %s (%04x -> %04x)\n", dev->slot_name, old_cmd, cmd);
		pci_write_config_word(dev, PCI_COMMAND, cmd);
	}
	return 0;
}

int
pcibios_enable_device(struct pci_dev *dev, int mask)
{
	ulong flags;
	uint coreidx;
	void *regs;

	/* External PCI device enable */
	if (dev->bus->number != 0)
		return pcibios_enable_resources(dev);

	/* These cores come out of reset enabled */
	if (dev->device == SB_MIPS ||
	    dev->device == SB_MIPS33 ||
	    dev->device == SB_EXTIF ||
	    dev->device == SB_CC)
		return 0;

	spin_lock_irqsave(&sbh_lock, flags);
	coreidx = sb_coreidx(sbh);
	regs = sb_setcoreidx(sbh, PCI_SLOT(dev->devfn));
	if (!regs)
		return PCIBIOS_DEVICE_NOT_FOUND;

	/* 
	 * The USB core requires a special bit to be set during core
	 * reset to enable host (OHCI) mode. Resetting the SB core in
	 * pcibios_enable_device() is a hack for compatibility with
	 * vanilla usb-ohci so that it does not have to know about
	 * SB. A driver that wants to use the USB core in device mode
	 * should know about SB and should reset the bit back to 0
	 * after calling pcibios_enable_device().
	 */
	if (sb_coreid(sbh) == SB_USB) {
		printk(KERN_INFO "SB USB 1.1 init\n");
		sb_core_disable(sbh, sb_coreflags(sbh, 0, 0));
		sb_core_reset(sbh, 1 << 29, 0);
	}
	/*
	 * USB 2.0 special considerations:
	 *
	 * 1. Since the core supports both OHCI and EHCI functions, it must
	 *    only be reset once.
	 *
	 * 2. In addition to the standard SB reset sequence, the Host Control
	 *    Register must be programmed to bring the USB core and various
	 *    phy components out of reset.
	 */
	else if (sb_coreid(sbh) == SB_USB20H) {
		
		uint corerev = sb_corerev(sbh);

		printk(KERN_INFO "SB USB20H init\n");
		printk(KERN_INFO "SB COREREV: %d\n", corerev);
		
		if (!sb_iscoreup(sbh)) {
						
			printk(KERN_INFO "SB USB20H resetting\n");

			sb_core_reset(sbh, 0, 0);
			writel(0x7FF, (ulong)regs + 0x200);
			udelay(1);
		}
		/* PRxxxx: War for 5354 failures. */
		if (corerev == 1 || corerev == 2) {
			uint32 tmp;

			/* Change Flush control reg */
			tmp = readl((uintptr)regs + 0x400);
			tmp &= ~8;
			writel(tmp, (uintptr)regs + 0x400);
			tmp = readl((uintptr)regs + 0x400);
			printk(KERN_INFO "USB20H fcr: 0x%x\n", tmp);

			/* Change Shim control reg */
			tmp = readl((uintptr)regs + 0x304);
			tmp &= ~0x100;
			writel(tmp, (uintptr)regs + 0x304);
			tmp = readl((uintptr)regs + 0x304);
			printk(KERN_INFO "USB20H shim cr: 0x%x\n", tmp);
		}

	} else
		sb_core_reset(sbh, 0, 0);

	sb_setcoreidx(sbh, coreidx);
	spin_unlock_irqrestore(&sbh_lock, flags);

	return 0;
}

void
pcibios_update_resource(struct pci_dev *dev, struct resource *root,
	struct resource *res, int resource)
{
	unsigned long where, size;
	u32 reg;

	/* External PCI only */
	if (dev->bus->number == 0)
		return;

	where = PCI_BASE_ADDRESS_0 + (resource * 4);
	size = res->end - res->start;
	pci_read_config_dword(dev, where, &reg);
	
	if (dev->bus->number == 1)
		reg = (reg & size) | (((u32)(res->start - root->start)) & ~size);
	else
		reg = res->start;

	pci_write_config_dword(dev, where, reg);
}

static void __init
quirk_sbpci_bridge(struct pci_dev *dev)
{
	if (dev->bus->number != 1 || PCI_SLOT(dev->devfn) != 0)
		return;

	printk("PCI: Fixing up bridge\n");

	/* Enable PCI bridge bus mastering and memory space */
	pci_set_master(dev);
	pcibios_enable_resources(dev);

	/* Enable PCI bridge BAR1 prefetch and burst */
	pci_write_config_dword(dev, PCI_BAR1_CONTROL, 3);
}	

struct pci_fixup pcibios_fixups[] = {
	{ PCI_FIXUP_HEADER, PCI_ANY_ID, PCI_ANY_ID, quirk_sbpci_bridge },
	{ 0 }
};

/*
 *  If we set up a device for bus mastering, we need to check the latency
 *  timer as certain crappy BIOSes forget to set it properly.
 */
unsigned int pcibios_max_latency = 255;

void pcibios_set_master(struct pci_dev *dev)
{
	u8 lat;
	pci_read_config_byte(dev, PCI_LATENCY_TIMER, &lat);
	if (lat < 16)
		lat = (64 <= pcibios_max_latency) ? 64 : pcibios_max_latency;
	else if (lat > pcibios_max_latency)
		lat = pcibios_max_latency;
	else
		return;
	printk(KERN_DEBUG "PCI: Setting latency timer of device %s to %d\n", dev->slot_name, lat);
	pci_write_config_byte(dev, PCI_LATENCY_TIMER, lat);
}

