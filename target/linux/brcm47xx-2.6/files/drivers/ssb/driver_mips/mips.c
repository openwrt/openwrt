/*
 * Sonics Silicon Backplane
 * Broadcom MIPS core driver
 *
 * Copyright 2005, Broadcom Corporation
 * Copyright 2006, 2007, Michael Buesch <mb@bu3sch.de>
 * Copyright 2006, 2007, Felix Fietkau <nbd@openwrt.org>
 *
 * Licensed under the GNU/GPL. See COPYING for details.
 */

#include <linux/ssb/ssb.h>

#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/serial_reg.h>
#include <asm/time.h>

#include "../ssb_private.h"

#define mips_read32(mcore, offset) ssb_read32((mcore)->dev, offset)
#define mips_write32(mcore, offset, value) ssb_write32((mcore)->dev, offset, value)
#define extif_read32(extif, offset) ssb_read32((extif)->dev, offset)
#define extif_write32(extif, offset, value) ssb_write32((extif)->dev, offset, value)

static const u32 ipsflag_irq_mask[] = {
	0,
	SSB_IPSFLAG_IRQ1,
	SSB_IPSFLAG_IRQ2,
	SSB_IPSFLAG_IRQ3,
	SSB_IPSFLAG_IRQ4,
};

static const u32 ipsflag_irq_shift[] = {
	0,
	SSB_IPSFLAG_IRQ1_SHIFT,
	SSB_IPSFLAG_IRQ2_SHIFT,
	SSB_IPSFLAG_IRQ3_SHIFT,
	SSB_IPSFLAG_IRQ4_SHIFT,
};

static inline u32 ssb_irqflag(struct ssb_device *dev)
{
	return ssb_read32(dev, SSB_TPSFLAG) & SSB_TPSFLAG_BPFLAG;
}

/* Get the MIPS IRQ assignment for a specified device.
 * If unassigned, 0 is returned.
 */
unsigned int ssb_mips_irq(struct ssb_device *dev)
{
	struct ssb_bus *bus = dev->bus;
	u32 irqflag;
	u32 ipsflag;
	u32 tmp;
	unsigned int irq;

	irqflag = ssb_irqflag(dev);
	ipsflag = ssb_read32(bus->mipscore.dev, SSB_IPSFLAG);
	for (irq = 1; irq <= 4; irq++) {
		tmp = ((ipsflag & ipsflag_irq_mask[irq]) >> ipsflag_irq_shift[irq]);
		if (tmp == irqflag)
			break;
	}
	if (irq	== 5)
		irq = 0;

	return irq;
}

static void clear_irq(struct ssb_bus *bus, unsigned int irq)
{
	struct ssb_device *dev = bus->mipscore.dev;

	/* Clear the IRQ in the MIPScore backplane registers */
	if (irq == 0) {
		ssb_write32(dev, SSB_INTVEC, 0);
	} else {
		ssb_write32(dev, SSB_IPSFLAG,
			    ssb_read32(dev, SSB_IPSFLAG) |
			    ipsflag_irq_mask[irq]);
	}
}

static void set_irq(struct ssb_device *dev, unsigned int irq)
{
	unsigned int oldirq = ssb_mips_irq(dev);
	struct ssb_bus *bus = dev->bus;
	struct ssb_device *mdev = bus->mipscore.dev;
	u32 irqflag = ssb_irqflag(dev);

	dev->irq = irq + 2;

	ssb_dprintk(KERN_INFO PFX
		    "set_irq: core 0x%04x, irq %d => %d\n",
		    dev->id.coreid, oldirq, irq);
	/* clear the old irq */
	if (oldirq == 0)
		ssb_write32(mdev, SSB_INTVEC, (~(1 << irqflag) & ssb_read32(mdev, SSB_INTVEC)));
	else
		clear_irq(bus, oldirq);

	/* assign the new one */
	if (irq == 0)
		ssb_write32(mdev, SSB_INTVEC, ((1 << irqflag) & ssb_read32(mdev, SSB_INTVEC)));

	irqflag <<= ipsflag_irq_shift[irq];
	irqflag |= (ssb_read32(mdev, SSB_IPSFLAG) & ~ipsflag_irq_mask[irq]);
	ssb_write32(mdev, SSB_IPSFLAG, irqflag);
}

static int ssb_extif_serial_init(struct ssb_extif *dev, struct ssb_serial_port *ports)
{
	//TODO if (EXTIF available
#if 0
		extifregs_t *eir = (extifregs_t *) regs;
		sbconfig_t *sb;

		/* Determine external UART register base */
		sb = (sbconfig_t *)((ulong) eir + SBCONFIGOFF);
		base = EXTIF_CFGIF_BASE(sb_base(R_REG(&sb->sbadmatch1)));

		/* Determine IRQ */
		irq = sb_irq(sbh);

		/* Disable GPIO interrupt initially */
		W_REG(&eir->gpiointpolarity, 0);
		W_REG(&eir->gpiointmask, 0);

		/* Search for external UARTs */
		n = 2;
		for (i = 0; i < 2; i++) {
			regs = (void *) REG_MAP(base + (i * 8), 8);
			if (BCMINIT(serial_exists)(regs)) {
				/* Set GPIO 1 to be the external UART IRQ */
				W_REG(&eir->gpiointmask, 2);
				if (add)
					add(regs, irq, 13500000, 0);
			}
		}

		/* Add internal UART if enabled */
		if (R_REG(&eir->corecontrol) & CC_UE)
			if (add)
				add((void *) &eir->uartdata, irq, sb_clock(sbh), 2);

#endif

}


static void ssb_mips_serial_init(struct ssb_mipscore *mcore)
{
	struct ssb_bus *bus = mcore->dev->bus;

	if (bus->extif.dev)
		mcore->nr_serial_ports = ssb_extif_serial_init(&bus->extif, mcore->serial_ports);
	else if (bus->chipco.dev)
		mcore->nr_serial_ports = ssb_chipco_serial_init(&bus->chipco, mcore->serial_ports);
	else
		mcore->nr_serial_ports = 0;
}

static void ssb_mips_flash_detect(struct ssb_mipscore *mcore)
{
	struct ssb_bus *bus = mcore->dev->bus;

	mcore->flash_buswidth = 2;
	if (bus->chipco.dev) {
		mcore->flash_window = 0x1c000000;
		mcore->flash_window_size = 0x02000000;
		if ((ssb_read32(bus->chipco.dev, SSB_CHIPCO_FLASH_CFG)
				& SSB_CHIPCO_CFG_DS16) == 0)
			mcore->flash_buswidth = 1;
	} else {
		mcore->flash_window = 0x1fc00000;
		mcore->flash_window_size = 0x00400000;
	}
}

static void ssb_extif_timing_init(struct ssb_extif *extif, u32 ns)
{
	u32 tmp;

	/* Initialize extif so we can get to the LEDs and external UART */
	extif_write32(extif, SSB_EXTIF_PROG_CFG, SSB_EXTCFG_EN);
	
	/* Set timing for the flash */
	tmp  = ceildiv(10, ns) << SSB_PROG_WCNT_3_SHIFT;
	tmp |= ceildiv(40, ns) << SSB_PROG_WCNT_1_SHIFT;
	tmp |= ceildiv(120, ns);
	extif_write32(extif, SSB_EXTIF_PROG_WAITCNT, tmp);

	/* Set programmable interface timing for external uart */
	tmp  = ceildiv(10, ns) << SSB_PROG_WCNT_3_SHIFT;
	tmp |= ceildiv(20, ns) << SSB_PROG_WCNT_2_SHIFT;
	tmp |= ceildiv(100, ns) << SSB_PROG_WCNT_1_SHIFT;
	tmp |= ceildiv(120, ns);
	extif_write32(extif, SSB_EXTIF_PROG_WAITCNT, tmp);
}

static inline void ssb_extif_get_clockcontrol(struct ssb_extif *extif,
				 u32 *pll_type, u32 *n, u32 *m)
{
	*pll_type = SSB_PLLTYPE_1;
	*n = extif_read32(extif, SSB_EXTIF_CLOCK_N);
	*m = extif_read32(extif, SSB_EXTIF_CLOCK_SB);
}

u32 ssb_cpu_clock(struct ssb_mipscore *mcore)
{
	struct ssb_bus *bus = mcore->dev->bus;
	u32 pll_type, n, m, rate = 0;

	if (bus->extif.dev) {
		ssb_extif_get_clockcontrol(&bus->extif, &pll_type, &n, &m);
	} else if (bus->chipco.dev) {
		ssb_chipco_get_clockcpu(&bus->chipco, bus->chip_id, &rate,
			&pll_type, &n, &m);
	} else
		return 0;

	if (rate == 0)
		rate = ssb_calc_clock_rate(pll_type, n, m);

	if (pll_type == SSB_PLLTYPE_6)
		rate *= 2;

	return rate;
}

void ssb_mipscore_init(struct ssb_mipscore *mcore)
{
	struct ssb_bus *bus = mcore->dev->bus;
	struct ssb_device *dev;
	unsigned long hz, ns;
	unsigned int irq, i;

	if (!mcore->dev)
		return; /* We don't have a MIPS core */

	ssb_dprintk(KERN_INFO PFX "Initializing MIPS core...\n");

	hz = ssb_clockspeed(bus);
	if (!hz)
		hz = 100000000;
	ns = 1000000000 / hz;

	if (bus->extif.dev)
		ssb_extif_timing_init(&bus->extif, ns);
	else if (bus->chipco.dev)
		ssb_chipco_timing_init(&bus->chipco, ns);

	/* Assign IRQs to all cores on the bus, start with irq line 2, because serial usually takes 1 */
	for (irq = 2, i = 0; i < bus->nr_devices; i++) {
		dev = &(bus->devices[i]);
		dev->irq = ssb_mips_irq(dev) + 2;
		switch(dev->id.coreid) {
			case SSB_DEV_USB11_HOST:
				/* shouldn't need a separate irq line for non-4710, most of them have a proper
				 * external usb controller on the pci */
				if ((bus->chip_id == 0x4710) && (irq <= 4)) {
					set_irq(dev, irq++);
					break;
				}
			case SSB_DEV_PCI:
			case SSB_DEV_ETHERNET:
			case SSB_DEV_80211:
			case SSB_DEV_USB20_HOST:
				/* These devices get their own IRQ line if available, the rest goes on IRQ0 */
				if (irq <= 4) {
					set_irq(dev, irq++);
					break;
				}
		}
	}

	ssb_mips_serial_init(mcore);
	ssb_mips_flash_detect(mcore);
}

EXPORT_SYMBOL(ssb_mips_irq);
