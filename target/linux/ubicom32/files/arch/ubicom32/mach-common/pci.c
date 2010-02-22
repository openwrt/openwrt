/*
 * arch/ubicom32/mach-common/pci.c
 *	PCI interface management.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>

#include <asm/devtree.h>
#include <asm/ip5000.h>
#include <asm/ubicom32-common.h>

static int debug_pci = 1 ;

/* #define PCI_USE_INTERNAL_LOCK 1 */

#ifdef PCI_USE_INTERNAL_LOCK
#define PCI_LOCK(lock, irqflag)	pci_lock_acquire(irqflag)
#define PCI_UNLOCK(lock, irqflag) pci_lock_release(irqflag)
#elif defined(CONFIG_SMP)
static DEFINE_SPINLOCK(pci_master_lock);
#define PCI_LOCK(lock, irqflag)	spin_lock_irqsave(lock, irqflag)
#define PCI_UNLOCK(lock, irqflag) spin_unlock_irqrestore(lock, irqflag)
#else
#define PCI_LOCK(lock, irqflag)		local_irq_save(irqflag)
#define PCI_UNLOCK(lock, irqflag)	local_irq_restore(irqflag)
#endif

#define PCI_DEV0_IDSEL CONFIG_PCI_DEV0_IDSEL
#define PCI_DEV1_IDSEL CONFIG_PCI_DEV1_IDSEL

/*
 * PCI commands
 */
#define PCI_CMD_INT_ACK		0x00	/* not supported */
#define PCI_CMD_SPECIAL		0x01	/* not supported */
#define PCI_CMD_IO_READ		0x02
#define PCI_CMD_IO_WRITE	0x03
#define PCI_CMD_MEM_READ	0x06
#define PCI_CMD_MEM_WRITE	0x07
#define PCI_CMD_CFG_READ	0x0a
#define PCI_CMD_CFG_WRITE	0x0b
#define PCI_CMD_MEM_READ_MULT	0x0c	/* not supported */
#define PCI_CMD_DUAL_ADDR	0x0d	/* not supported */
#define PCI_CMD_MEM_READ_LINE	0x0e	/* not supported */
#define PCI_CMD_MEM_WRITE_INVAL	0x0f	/* not supported */
/*
 * Status codes, returned by pci_read_u32() and pci_write_u32()
 */
#define PCI_RESP_IN_PROGRESS	0xff  /* request still in queue */
#define PCI_RESP_OK		0
/*
 * The following codes indicate that the request has completed
 */
#define PCI_RESP_NO_DEVSEL		1  /* timeout before target asserted
					    * DEVSEL! */
#define PCI_RESP_LOST_DEVSEL		2  /* had DEVSEL, but went away before
					    * transfer completed! */
#define PCI_RESP_BAD_TRDY		3  /* target asserted TRDY without
					    * DEVSEL! */
#define PCI_RESP_NO_TRDY		4  /* timeout before target asserted
					    * TRDY! */
#define PCI_RESP_BAD_STOP		5  /* target asserted STOP and TRDY
					    * without DEVSEL! */
#define PCI_RESP_TARGET_ABORT		6
#define PCI_RESP_TARGET_RETRY		7
#define	PCI_RESP_TARGET_DISCONNECT	8
#define PCI_RESP_MISMATCH		9  /* data read back doesn't match data
					    * written - debug only, the core PCI
					    * routines never return this */
#define PCI_RESP_DET_SERR		10
#define PCI_RESP_DET_PERR		11
#define PCI_RESP_MALFORMED_REQ		12 /* Could be due to misaligned
					    * requests or invalid address */
#define PCI_RESP_NO_RESOURCE		13 /* Could be memory or other resourse
					    * like queue space */
#define PCI_RESP_ERROR			14 /* All emcompassing error */

/* registers in PCI config space */
#define PCI_DEVICE_VENDOR_ID_REG	0x00
#define PCI_STATUS_COMMAND_REG		0x04
#define PCI_CLASS_REVISION_REG		0x08
#define PCI_BHLC_REG			0x0c  /* BIST, Header type, Latency
					       * timer, Cache line size */
#define PCI_BASE_ADDR_REG		0x10
#define PCI_BASE_REG_COUNT		6
#define CARDBUS_CIS_PTR_REG		0x28
#define PCI_SUB_SYSTEM_ID_REG		0x2c
#define PCI_EXP_ROM_ADDR_REG		0x30
#define PCI_CAP_PTR_REG			0x34
#define PCI_LGPL_REG			0x3C  /* max Latency, min Gnt, interrupt
					       * Pin, interrupt Line */

struct pci_master_request {
	volatile u32_t pci_address;	/* must be 4-byte aligned */
	volatile u32_t data;		/* must be 4-byte aligned */
	volatile u8_t cmd;
	volatile u8_t byte_valid;
	volatile u8_t status;
};

struct pci_devnode {
	struct devtree_node dn;
	u32_t pci_idsel_0;
	u32_t pci_idsel_1;
	u32_t pci_cpu_address;
	struct pci_master_request volatile *volatile req;
};

static struct pci_master_request req;	/* globally used for faster master write
					 * (discarding result when possible) */
static struct pci_devnode *pci_node;

#if !defined(CONFIG_DEBUG_PCIMEASURE)
#define PCI_DECLARE_MEASUREMENT
#define PCI_MEASUREMENT_START()
#define PCI_MEASUREMENT_END(idx)
#else
#define PCI_DECLARE_MEASUREMENT \
	int __diff;		\
	unsigned int __tstart;

#define PCI_MEASUREMENT_START() \
	__tstart = UBICOM32_IO_TIMER->sysval;

#define PCI_MEASUREMENT_END(idx) \
	__diff = (int)UBICOM32_IO_TIMER->sysval - (int)__tstart; \
	pci_measurement_update((idx), __diff);

#define PCI_WEIGHT 32

struct pci_measurement {
	volatile unsigned int min;
	volatile unsigned int avg;
	volatile unsigned int max;
};

enum pci_measurement_list {
	PCI_MEASUREMENT_READ32,
	PCI_MEASUREMENT_WRITE32,
	PCI_MEASUREMENT_READ16,
	PCI_MEASUREMENT_WRITE16,
	PCI_MEASUREMENT_READ8,
	PCI_MEASUREMENT_WRITE8,
	PCI_MEASUREMENT_LAST,
};

static const char *pci_measurement_name_list[PCI_MEASUREMENT_LAST] = {
	"READ32",
	"WRITE32",
	"READ16",
	"WRITE16",
	"READ8",
	"WRITE8"
};
static struct pci_measurement pci_measurements[PCI_MEASUREMENT_LAST];

/*
 * pci_measurement_update()
 *	Update an entry in the measurement array for this idx.
 */
static void pci_measurement_update(int idx, int sample)
{
	struct pci_measurement *pm = &pci_measurements[idx];
	if ((pm->min == 0) || (pm->min > sample)) {
		pm->min = sample;
	}
	if (pm->max < sample) {
		pm->max = sample;
	}
	pm->avg = ((pm->avg * (PCI_WEIGHT - 1)) + sample) / PCI_WEIGHT;
}
#endif

#if defined(PCI_USE_INTERNAL_LOCK)
/*
 * pci_lock_release()
 *	Release the PCI lock.
 */
static void pci_lock_release(unsigned long irqflag)
{
	UBICOM32_UNLOCK(PCI_LOCK_BIT);
}

/*
 * pci_lock_acquire()
 *	Acquire the PCI lock, spin if not available.
 */
static void pci_lock_acquire(unsigned long irqflag)
{
	UBICOM32_LOCK(PCI_LOCK_BIT);
}
#endif

/*
 * pci_set_hrt_interrupt()
 */
static inline void pci_set_hrt_interrupt(struct pci_devnode *pci_node)
{
	ubicom32_set_interrupt(pci_node->dn.sendirq);
}

/*
 * pci_read_u32()
 *	Synchronously read 32 bits from PCI space.
 */
u8 pci_read_u32(u8 pci_cmd, u32 address, u32 *data)
{
	u8 status;
	unsigned long irqflag;


	/*
	 * Fill in the request.
	 */
	volatile struct pci_master_request lreq;
	PCI_DECLARE_MEASUREMENT;

	lreq.pci_address = address;
	lreq.cmd = pci_cmd;
	lreq.byte_valid = 0xf;		/* enable all bytes */

	/*
	 * Wait for any previous request to complete and then make this request.
	 */
	PCI_MEASUREMENT_START();
	PCI_LOCK(&pci_master_lock, irqflag);
	while (unlikely(pci_node->req == &req))
		;
	pci_node->req = &lreq;
	pci_set_hrt_interrupt(pci_node);
	PCI_UNLOCK(&pci_master_lock, irqflag);

	/*
	 * Wait for the result to show up.
	 */
	while (unlikely(pci_node->req == &lreq))
		;
	status = lreq.status;
	if (likely(status == PCI_RESP_OK))
		*data = le32_to_cpu(lreq.data);
	else
		*data = 0;
	PCI_MEASUREMENT_END(PCI_MEASUREMENT_READ32);
	return status;
}

/*
 * pci_write_u32()
 *	Asyncrhnously or synchronously write 32 bits to PCI master space.
 */
u8 pci_write_u32(u8 pci_cmd, u32 address, u32 data)
{
	unsigned long irqflag;
	PCI_DECLARE_MEASUREMENT;

	/*
	 * Wait for any previous write or pending read to complete.
	 *
	 * We use a global data block because once we write the request
	 * we do not wait for it to complete before exiting.
	 */
	PCI_MEASUREMENT_START();
	PCI_LOCK(&pci_master_lock, irqflag);
	while (unlikely(pci_node->req == &req))
		;
	req.pci_address = address;
	req.data = cpu_to_le32(data);
	req.cmd = pci_cmd;
	req.byte_valid = 0xf;		/* enable all bytes */
	pci_node->req = &req;
	pci_set_hrt_interrupt(pci_node);
	PCI_UNLOCK(&pci_master_lock, irqflag);
	PCI_MEASUREMENT_END(PCI_MEASUREMENT_WRITE32);
	return PCI_RESP_OK;
}

/*
 * pci_read_u16()
 *	Synchronously read 16 bits from PCI space.
 */
u8 pci_read_u16(u8 pci_cmd, u32 address, u16 *data)
{
	u8 status;
	unsigned long irqflag;

	/*
	 * Fill in the request.
	 */
	volatile struct pci_master_request lreq;
	PCI_DECLARE_MEASUREMENT;

	lreq.pci_address = address & ~2;
	lreq.cmd = pci_cmd;
	lreq.byte_valid = (address & 2) ? 0xc : 0x3;

	/*
	 * Wait for any previous request to complete and then make this request.
	 */
	PCI_MEASUREMENT_START();
	PCI_LOCK(&pci_master_lock, irqflag);
	while (unlikely(pci_node->req == &req))
		;
	pci_node->req = &lreq;
	pci_set_hrt_interrupt(pci_node);
	PCI_UNLOCK(&pci_master_lock, irqflag);

	/*
	 * Wait for the result to show up.
	 */
	while (unlikely(pci_node->req == &lreq))
		;
	status = lreq.status;
	if (likely(status == PCI_RESP_OK)) {
		lreq.data = le32_to_cpu(lreq.data);
		*data = (u16)((address & 2) ? (lreq.data >> 16) : lreq.data);
	} else
		*data = 0;
	PCI_MEASUREMENT_END(PCI_MEASUREMENT_READ16);
	return status;
}

/*
 * pci_write_u16()
 *	Asyncrhnously or synchronously write 16 bits to PCI master space.
 */
u8 pci_write_u16(u8 pci_cmd, u32 address, u16 data)
{
	unsigned long irqflag;
	PCI_DECLARE_MEASUREMENT;

	/*
	 * Wait for any previous write or pending read to complete.
	 *
	 * We use a global data block because once we write the request
	 * we do not wait for it to complete before exiting.
	 */
	PCI_MEASUREMENT_START();
	PCI_LOCK(&pci_master_lock, irqflag);
	while (unlikely(pci_node->req == &req))
		;
	req.pci_address = address & ~2;
	req.data = (u32)data;
	req.data = cpu_to_le32((address & 2) ? (req.data << 16) : req.data);
	req.cmd = pci_cmd;
	req.byte_valid = (address & 2) ? 0xc : 0x3;
	pci_node->req = &req;
	pci_set_hrt_interrupt(pci_node);
	PCI_UNLOCK(&pci_master_lock, irqflag);
	PCI_MEASUREMENT_END(PCI_MEASUREMENT_WRITE16);
	return PCI_RESP_OK;
}

/*
 * pci_read_u8()
 *	Synchronously read 8 bits from PCI space.
 */
u8 pci_read_u8(u8 pci_cmd, u32 address, u8 *data)
{
	u8 status;
	unsigned long irqflag;

	/*
	 * Fill in the request.
	 */
	volatile struct pci_master_request lreq;
	PCI_DECLARE_MEASUREMENT;

	lreq.pci_address = address & ~3;
	lreq.cmd = pci_cmd;
	lreq.byte_valid = 1 << (address & 0x3);

	/*
	 * Wait for any previous request to complete and then make this request.
	 */
	PCI_MEASUREMENT_START();
	PCI_LOCK(&pci_master_lock, irqflag);
	while (unlikely(pci_node->req == &req))
		;
	pci_node->req = &lreq;
	pci_set_hrt_interrupt(pci_node);
	PCI_UNLOCK(&pci_master_lock, irqflag);

	/*
	 * Wait for the result to show up.
	 */
	while (unlikely(pci_node->req == &lreq))
		;
	status = lreq.status;
	if (likely(status == PCI_RESP_OK)) {
		*data = (u8)(lreq.data >> (24 - ((address & 0x3) << 3)));
	} else
		*data = 0;
	PCI_MEASUREMENT_END(PCI_MEASUREMENT_READ8);
	return status;
}

/*
 * pci_write_u8()
 *	Asyncrhnously or synchronously write 8 bits to PCI master space.
 */
u8 pci_write_u8(u8 pci_cmd, u32 address, u8 data)
{
	unsigned long irqflag;
	PCI_DECLARE_MEASUREMENT;

	/*
	 * Wait for any previous write or pending read to complete.
	 *
	 * We use a global data block because once we write the request
	 * we do not wait for it to complete before exiting.
	 */
	PCI_MEASUREMENT_START();
	PCI_LOCK(&pci_master_lock, irqflag);
	while (unlikely(pci_node->req == &req))
		;
	req.pci_address = address & ~3;
	req.data = ((u32)data << (24 - ((address & 0x3) << 3)));
	req.cmd = pci_cmd;
	req.byte_valid = 1 << (address & 0x3);
	pci_node->req = &req;
	pci_set_hrt_interrupt(pci_node);
	PCI_UNLOCK(&pci_master_lock, irqflag);
	PCI_MEASUREMENT_END(PCI_MEASUREMENT_WRITE8);
	return PCI_RESP_OK;
}

unsigned int ubi32_pci_read_u32(const volatile void __iomem *addr)
{
	unsigned int data;
	pci_read_u32(PCI_CMD_MEM_READ, (u32)addr, &data);
	return data;
}
EXPORT_SYMBOL(ubi32_pci_read_u32);

unsigned short ubi32_pci_read_u16(const volatile void __iomem *addr)
{
	unsigned short data;
	pci_read_u16(PCI_CMD_MEM_READ, (u32)addr, &data);
	return data;
}
EXPORT_SYMBOL(ubi32_pci_read_u16);

unsigned char  ubi32_pci_read_u8(const volatile void __iomem *addr)
{
	unsigned char  data;
	pci_read_u8(PCI_CMD_MEM_READ, (u32)addr, &data);
	return data;
}
EXPORT_SYMBOL(ubi32_pci_read_u8);

void ubi32_pci_write_u32(unsigned int val, const volatile void __iomem *addr)
{
	pci_write_u32(PCI_CMD_MEM_WRITE, (u32)addr, val);
}
EXPORT_SYMBOL(ubi32_pci_write_u32);

void ubi32_pci_write_u16(unsigned short val, const volatile void __iomem *addr)
{
	pci_write_u16(PCI_CMD_MEM_WRITE, (u32)addr, val);
}
EXPORT_SYMBOL(ubi32_pci_write_u16);

void ubi32_pci_write_u8(unsigned char val, const void volatile __iomem *addr)
{
	pci_write_u8(PCI_CMD_MEM_WRITE, (u32)addr, val);
}
EXPORT_SYMBOL(ubi32_pci_write_u8);

#if defined(CONFIG_DEBUG_PCIMEASURE)
static unsigned int pci_cycles_to_nano(unsigned int cycles, unsigned int frequency)
{
	unsigned int nano = ((cycles * 1000) / (frequency / 1000000));
	return nano;
}

/*
 * pci_measurement_show()
 *	Print out the min, avg, max values for each PCI transaction type.
 *
 * By request, the max value is reset after each dump.
 */
static int pci_measurement_show(struct seq_file *p, void *v)
{
	unsigned int min, avg, max;
	unsigned int freq = processor_frequency();
	int trans = *((loff_t *) v);

	if (trans == 0) {
		seq_puts(p, "min\tavg\tmax\t(nano-seconds)\n");
	}

	if (trans >= PCI_MEASUREMENT_LAST) {
		return 0;
	}

	min = pci_cycles_to_nano(pci_measurements[trans].min, freq);
	avg = pci_cycles_to_nano(pci_measurements[trans].avg, freq);
	max = pci_cycles_to_nano(pci_measurements[trans].max, freq);
	pci_measurements[trans].max = 0;
	seq_printf(p, "%u\t%u\t%u\t%s\n", min, avg, max, pci_measurement_name_list[trans]);
	return 0;
}

static void *pci_measurement_start(struct seq_file *f, loff_t *pos)
{
	return (*pos < PCI_MEASUREMENT_LAST) ? pos : NULL;
}

static void *pci_measurement_next(struct seq_file *f, void *v, loff_t *pos)
{
	(*pos)++;
	if (*pos >= PCI_MEASUREMENT_LAST)
		return NULL;
	return pos;
}

static void pci_measurement_stop(struct seq_file *f, void *v)
{
	/* Nothing to do */
}

static const struct seq_operations pci_measurement_seq_ops = {
	.start = pci_measurement_start,
	.next  = pci_measurement_next,
	.stop  = pci_measurement_stop,
	.show  = pci_measurement_show,
};

static int pci_measurement_open(struct inode *inode, struct file *filp)
{
	return seq_open(filp, &pci_measurement_seq_ops);
}

static const struct file_operations pci_measurement_fops = {
	.open		= pci_measurement_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= seq_release,
};

static int __init pci_measurement_init(void)
{
	proc_create("pci_measurements", 0, NULL, &pci_measurement_fops);
	return 0;
}
module_init(pci_measurement_init);
#endif

static int ubi32_pci_read_config(struct pci_bus *bus, unsigned int devfn,
				 int where, int size, u32 *value)
{
	u8 cmd;
	u32 addr;
	u8  data8;
	u16 data16;

	u8 slot = PCI_SLOT(devfn);
	u8 fn = PCI_FUNC(devfn);

	if (slot > 1) {
		return PCIBIOS_DEVICE_NOT_FOUND;
	} else if (slot == 0) {
		addr = PCI_DEV0_IDSEL + where;
	} else {
		addr = PCI_DEV1_IDSEL + where;
	}

	addr += (fn << 8);

	cmd = PCI_CMD_CFG_READ;
	if (size == 1) {
		pci_read_u8(cmd, addr, &data8);
		*value = (u32)data8;
	} else if (size == 2) {
		pci_read_u16(cmd, addr, &data16);
		*value = (u32)data16;
	} else {
		pci_read_u32(cmd, addr, value);
	}

	return PCIBIOS_SUCCESSFUL;
}

static int ubi32_pci_write_config(struct pci_bus *bus, unsigned int devfn,
				  int where, int size, u32 value)
{
	u8 cmd;
	u32 addr;
	u8 slot = PCI_SLOT(devfn);
	u8 fn = PCI_FUNC(devfn);

	if (slot > 1) {
		return PCIBIOS_DEVICE_NOT_FOUND;
	} else if (slot == 0) {
		addr = PCI_DEV0_IDSEL + where;
	} else {
		addr = PCI_DEV1_IDSEL + where;
	}

	addr += (fn << 8);

	cmd = PCI_CMD_CFG_WRITE;
	if (size == 1) {
		pci_write_u8(cmd, addr, (u8)value);
	} else if (size == 2) {
		pci_write_u16(cmd, addr, (u16)value);
	} else {
		pci_write_u32(cmd, addr, value);
	}

	return PCIBIOS_SUCCESSFUL;
}

int pci_set_dma_max_seg_size(struct pci_dev *dev, unsigned int size)
{
	return -EIO;
}
EXPORT_SYMBOL(pci_set_dma_max_seg_size);

int pci_set_dma_seg_boundary(struct pci_dev *dev, unsigned long mask)
{
	return -EIO;
}
EXPORT_SYMBOL(pci_set_dma_seg_boundary);

void __iomem *pci_iomap(struct pci_dev *dev, int bar, unsigned long maxlen)
{
	resource_size_t start = pci_resource_start(dev, bar);
	resource_size_t len   = pci_resource_len(dev, bar);
	unsigned long flags = pci_resource_flags(dev, bar);

	if (!len || !start) {
		return NULL;
	}

	if (maxlen && len > maxlen) {
		len = maxlen;
	}

	if (flags & IORESOURCE_IO) {
		return ioport_map(start, len);
	}

	if (flags & IORESOURCE_MEM) {
		if (flags & IORESOURCE_CACHEABLE) {
			return ioremap(start, len);
		}
		return ioremap_nocache(start, len);
	}
	return NULL;
}
EXPORT_SYMBOL(pci_iomap);

void pci_iounmap(struct pci_dev *dev, void __iomem *addr)
{
	if ((unsigned long)addr >= VMALLOC_START &&
	    (unsigned long)addr < VMALLOC_END) {
		iounmap(addr);
	}
}
EXPORT_SYMBOL(pci_iounmap);

/*
 *  From arch/arm/kernel/bios32.c
 *
 *  PCI bios-type initialisation for PCI machines
 *
 *  Bits taken from various places.
 */
static void __init pcibios_init_hw(struct hw_pci *hw)
{
	struct pci_sys_data *sys = NULL;
	int ret;
	int nr, busnr;

	for (nr = busnr = 0; nr < hw->nr_controllers; nr++) {
		sys = kzalloc(sizeof(struct pci_sys_data), GFP_KERNEL);
		if (!sys)
			panic("PCI: unable to allocate sys data!");

		sys->hw      = hw;
		sys->busnr   = busnr;
		sys->map_irq = hw->map_irq;
		sys->resource[0] = &ioport_resource;
		sys->resource[1] = &iomem_resource;

		ret = hw->setup(nr, sys);

		if (ret > 0) {
			sys->bus = hw->scan(nr, sys);

			if (!sys->bus)
				panic("PCI: unable to scan bus!");

			busnr = sys->bus->subordinate + 1;

			list_add(&sys->node, &hw->buses);
		} else {
			kfree(sys);
			if (ret < 0)
				break;
		}
	}
}

/*
 * Swizzle the device pin each time we cross a bridge.
 * This might update pin and returns the slot number.
 */
static u8 __devinit pcibios_swizzle(struct pci_dev *dev, u8 *pin)
{
	struct pci_sys_data *sys = dev->sysdata;
	int slot = 0, oldpin = *pin;

	if (sys->swizzle)
		slot = sys->swizzle(dev, pin);

	if (debug_pci)
		printk("PCI: %s swizzling pin %d => pin %d slot %d\n",
			pci_name(dev), oldpin, *pin, slot);
	return slot;
}

/*
 * Map a slot/pin to an IRQ.
 */
static int pcibios_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
	struct pci_sys_data *sys = dev->sysdata;
	int irq = -1;

	if (sys->map_irq)
		irq = sys->map_irq(dev, slot, pin);

	if (debug_pci)
		printk("PCI: %s mapping slot %d pin %d => irq %d\n",
			pci_name(dev), slot, pin, irq);

	return irq;
}

void __init pci_common_init(struct hw_pci *hw)
{
	struct pci_sys_data *sys;

	INIT_LIST_HEAD(&hw->buses);

	if (hw->preinit)
		hw->preinit();
	pcibios_init_hw(hw);
	if (hw->postinit)
		hw->postinit();

	pci_fixup_irqs(pcibios_swizzle, pcibios_map_irq);
	list_for_each_entry(sys, &hw->buses, node) {
		struct pci_bus *bus = sys->bus;
		/*
		 * Size the bridge windows.
		 */
		pci_bus_size_bridges(bus);
		/*
		 * Assign resources.
		 */
		pci_bus_assign_resources(bus);

		/*
		 * Tell drivers about devices found.
		 */
		pci_bus_add_devices(bus);
	}
}

char * __init pcibios_setup(char *str)
{
	if (!strcmp(str, "debug")) {
		debug_pci = 1;
		return NULL;
	}
	return str;
}

/*
 * From arch/i386/kernel/pci-i386.c:
 *
 * We need to avoid collisions with `mirrored' VGA ports
 * and other strange ISA hardware, so we always want the
 * addresses to be allocated in the 0x000-0x0ff region
 * modulo 0x400.
 *
 * Why? Because some silly external IO cards only decode
 * the low 10 bits of the IO address. The 0x00-0xff region
 * is reserved for motherboard devices that decode all 16
 * bits, so it's ok to allocate at, say, 0x2800-0x28ff,
 * but we want to try to avoid allocating at 0x2900-0x2bff
 * which might be mirrored at 0x0100-0x03ff..
 */
void pcibios_align_resource(void *data, struct resource *res,
			    resource_size_t size, resource_size_t align)
{
	resource_size_t start = res->start;

	if (res->flags & IORESOURCE_IO && start & 0x300)
		start = (start + 0x3ff) & ~0x3ff;

	res->start = (start + align - 1) & ~(align - 1);
}


void __devinit pcibios_update_irq(struct pci_dev *dev, int irq)
{
	if (debug_pci)
		printk("PCI: Assigning IRQ %02d to %s\n", irq, pci_name(dev));
	pci_write_config_byte(dev, PCI_INTERRUPT_LINE, irq);
}

/*
 * If the bus contains any of these devices, then we must not turn on
 * parity checking of any kind.  Currently this is CyberPro 20x0 only.
 */
static inline int pdev_bad_for_parity(struct pci_dev *dev)
{
	return (dev->vendor == PCI_VENDOR_ID_INTERG &&
		(dev->device == PCI_DEVICE_ID_INTERG_2000 ||
		 dev->device == PCI_DEVICE_ID_INTERG_2010)) ||
		(dev->vendor == PCI_VENDOR_ID_ITE &&
		 dev->device == PCI_DEVICE_ID_ITE_8152);

}

/*
 * Adjust the device resources from bus-centric to Linux-centric.
 */
static void __devinit
pdev_fixup_device_resources(struct pci_sys_data *root, struct pci_dev *dev)
{
	resource_size_t offset;
	int i;

	for (i = 0; i < PCI_NUM_RESOURCES; i++) {
		if (dev->resource[i].start == 0)
			continue;
		if (dev->resource[i].flags & IORESOURCE_MEM)
			offset = root->mem_offset;
		else
			offset = root->io_offset;

		dev->resource[i].start += offset;
		dev->resource[i].end   += offset;
	}
}

static void __devinit
pbus_assign_bus_resources(struct pci_bus *bus, struct pci_sys_data *root)
{
	struct pci_dev *dev = bus->self;
	int i;

	if (!dev) {
		/*
		 * Assign root bus resources.
		 */
		for (i = 0; i < 3; i++)
			bus->resource[i] = root->resource[i];
	}
}

/*
 * pcibios_fixup_bus - Called after each bus is probed,
 * but before its children are examined.
 */
void pcibios_fixup_bus(struct pci_bus *bus)
{
	struct pci_sys_data *root = bus->sysdata;
	struct pci_dev *dev;
	u16 features = PCI_COMMAND_SERR | PCI_COMMAND_PARITY |
		PCI_COMMAND_FAST_BACK;

	pbus_assign_bus_resources(bus, root);

	/*
	 * Walk the devices on this bus, working out what we can
	 * and can't support.
	 */
	list_for_each_entry(dev, &bus->devices, bus_list) {
		u16 status;

		pdev_fixup_device_resources(root, dev);

		pci_read_config_word(dev, PCI_STATUS, &status);

		/*
		 * If any device on this bus does not support fast back
		 * to back transfers, then the bus as a whole is not able
		 * to support them.  Having fast back to back transfers
		 * on saves us one PCI cycle per transaction.
		 */
		if (!(status & PCI_STATUS_FAST_BACK))
			features &= ~PCI_COMMAND_FAST_BACK;

		if (pdev_bad_for_parity(dev))
			features &= ~(PCI_COMMAND_SERR | PCI_COMMAND_PARITY);

		switch (dev->class >> 8) {
		case PCI_CLASS_BRIDGE_PCI:
			pci_read_config_word(dev, PCI_BRIDGE_CONTROL, &status);
			status |= PCI_BRIDGE_CTL_PARITY |
				PCI_BRIDGE_CTL_MASTER_ABORT;
			status &= ~(PCI_BRIDGE_CTL_BUS_RESET |
				    PCI_BRIDGE_CTL_FAST_BACK);
			pci_write_config_word(dev, PCI_BRIDGE_CONTROL, status);
			break;

		case PCI_CLASS_BRIDGE_CARDBUS:
			pci_read_config_word(dev, PCI_CB_BRIDGE_CONTROL,
					     &status);
			status |= PCI_CB_BRIDGE_CTL_PARITY |
				PCI_CB_BRIDGE_CTL_MASTER_ABORT;
			pci_write_config_word(dev, PCI_CB_BRIDGE_CONTROL,
					      status);
			break;
		}
	}

	/*
	 * Now walk the devices again, this time setting them up.
	 */
	list_for_each_entry(dev, &bus->devices, bus_list) {
		u16 cmd;

		pci_read_config_word(dev, PCI_COMMAND, &cmd);
		cmd |= features;
		pci_write_config_word(dev, PCI_COMMAND, cmd);

		pci_write_config_byte(dev, PCI_CACHE_LINE_SIZE,
				      L1_CACHE_BYTES >> 2);
	}

	/*
	 * Propagate the flags to the PCI bridge.
	 */
	if (bus->self && bus->self->hdr_type == PCI_HEADER_TYPE_BRIDGE) {
		if (features & PCI_COMMAND_FAST_BACK)
			bus->bridge_ctl |= PCI_BRIDGE_CTL_FAST_BACK;
		if (features & PCI_COMMAND_PARITY)
			bus->bridge_ctl |= PCI_BRIDGE_CTL_PARITY;
	}

	/*
	 * Report what we did for this bus
	 */
	printk(KERN_INFO "PCI: bus%d: Fast back to back transfers %sabled\n",
		bus->number, (features & PCI_COMMAND_FAST_BACK) ? "en" : "dis");
}
/*
 * Convert from Linux-centric to bus-centric addresses for bridge devices.
 */
void
pcibios_resource_to_bus(struct pci_dev *dev, struct pci_bus_region *region,
			 struct resource *res)
{
	struct pci_sys_data *root = dev->sysdata;
	unsigned long offset = 0;

	if (res->flags & IORESOURCE_IO)
		offset = root->io_offset;
	if (res->flags & IORESOURCE_MEM)
		offset = root->mem_offset;

	region->start = res->start - offset;
	region->end   = res->end - offset;
}

void __devinit
pcibios_bus_to_resource(struct pci_dev *dev, struct resource *res,
			struct pci_bus_region *region)
{
	struct pci_sys_data *root = dev->sysdata;
	unsigned long offset = 0;

	if (res->flags & IORESOURCE_IO)
		offset = root->io_offset;
	if (res->flags & IORESOURCE_MEM)
		offset = root->mem_offset;

	res->start = region->start + offset;
	res->end   = region->end + offset;
}

#ifdef CONFIG_HOTPLUG
EXPORT_SYMBOL(pcibios_fixup_bus);
EXPORT_SYMBOL(pcibios_resource_to_bus);
EXPORT_SYMBOL(pcibios_bus_to_resource);
#endif

/**
 * pcibios_enable_device - Enable I/O and memory.
 * @dev: PCI device to be enabled
 */
int pcibios_enable_device(struct pci_dev *dev, int mask)
{
	u16 cmd, old_cmd;
	int idx;
	struct resource *r;

	pci_read_config_word(dev, PCI_COMMAND, &cmd);
	old_cmd = cmd;
	for (idx = 0; idx < 6; idx++) {
		/* Only set up the requested stuff */
		if (!(mask & (1 << idx)))
			continue;

		r = dev->resource + idx;
		if (!r->start && r->end) {
			printk(KERN_ERR "PCI: Device %s not available because"
			       " of resource collisions\n", pci_name(dev));
			return -EINVAL;
		}
		if (r->flags & IORESOURCE_IO)
			cmd |= PCI_COMMAND_IO;
		if (r->flags & IORESOURCE_MEM)
			cmd |= PCI_COMMAND_MEMORY;
	}

	/*
	 * Bridges (eg, cardbus bridges) need to be fully enabled
	 */
	if ((dev->class >> 16) == PCI_BASE_CLASS_BRIDGE)
		cmd |= PCI_COMMAND_IO | PCI_COMMAND_MEMORY;

	if (cmd != old_cmd) {
		printk("PCI: enabling device %s (%04x -> %04x)\n",
		       pci_name(dev), old_cmd, cmd);
		pci_write_config_word(dev, PCI_COMMAND, cmd);
	}
	return 0;
}


struct pci_ops ubi32_pci_ops = {
	.read   = ubi32_pci_read_config,
	.write  = ubi32_pci_write_config,
};

static struct pci_bus *ubi32_pci_scan_bus(int nr, struct pci_sys_data *sys)
{
	return pci_scan_bus(sys->busnr, &ubi32_pci_ops, sys);
}

#define UBI32_PCI_MEM_BASE PCI_DEV_REG_BASE
#define UBI32_PCI_MEM_LEN  0x80000000

#define UBI32_PCI_IO_BASE 0x0
#define UBI32_PCI_IO_END  0x0

static struct resource ubi32_pci_mem = {
	.name	= "PCI memory space",
	.start	= UBI32_PCI_MEM_BASE,
	.end	= UBI32_PCI_MEM_BASE + UBI32_PCI_MEM_LEN - 1,
	.flags	= IORESOURCE_MEM,
};

static struct resource ubi32_pci_io = {
	.name	= "PCI IO space",
	.start	= UBI32_PCI_IO_BASE,
	.end	= UBI32_PCI_IO_END,
	.flags	= IORESOURCE_IO,
};

static int __init ubi32_pci_setup(int nr, struct pci_sys_data *sys)
{
	if (nr > 0)
		return 0;

	request_resource(&iomem_resource, &ubi32_pci_mem);
	request_resource(&ioport_resource, &ubi32_pci_io);

	sys->resource[0] = &ubi32_pci_io;
	sys->resource[1] = &ubi32_pci_mem;
	sys->resource[2] = NULL;

	return 1;
}

static void __init ubi32_pci_preinit(void)
{
}

static int __init ubi32_pci_map_irq(struct pci_dev *dev, u8 slot, u8 pin)
{
	return pci_node->dn.recvirq;
}

struct hw_pci ubi32_pci __initdata = {
	.nr_controllers	= 1,
	.preinit	= ubi32_pci_preinit,
	.setup		= ubi32_pci_setup,
	.scan		= ubi32_pci_scan_bus,
	.map_irq	= ubi32_pci_map_irq,
};

static int __init ubi32_pci_init(void)
{
	pci_node = (struct pci_devnode *)devtree_find_node("pci");
	if (pci_node == NULL) {
		printk(KERN_WARNING "PCI init failed\n");
		return -ENOSYS;
	}
	pci_common_init(&ubi32_pci);
	return 0;
}

subsys_initcall(ubi32_pci_init);

/*
 * workaround for dual PCI card interrupt
 */
#define PCI_COMMON_INT_BIT (1 << 19)
void ubi32_pci_int_wr(void)
{
	volatile unsigned int pci_int_line;
	pci_int_line = UBICOM32_IO_PORT(RB)->gpio_in;
	if (!(pci_int_line & PCI_COMMON_INT_BIT))
	{
		ubicom32_set_interrupt(pci_node->dn.recvirq);
	}
}
EXPORT_SYMBOL(ubi32_pci_int_wr);
