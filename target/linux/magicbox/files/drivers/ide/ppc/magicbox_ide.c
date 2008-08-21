/*
 * Driver for MagicBox 2.0 onboard CompactFlash adapter.
 *
 * Written by Wojtek Kaniewski <wojtekka@toxygen.net>
 * Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *
 * GNU General Public License.
 */

#include <linux/version.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/blkdev.h>
#include <linux/hdreg.h>
#include <linux/ide.h>
#include <linux/delay.h>
#include <linux/platform_device.h>

#define UIC0_PR		0xc4
#define UIC0_TR		0xc5
#define MAGICBOX_CF_IRQ	25

static u8 magicbox_ide_inb(unsigned long port)
{
	return (u8) (readw((void __iomem *) port) >> 8) & 0xff;
}

static void magicbox_ide_outb(u8 value, unsigned long port)
{
	writew(value << 8, (void __iomem *) port);
}

static void magicbox_ide_outbsync(ide_drive_t *drive, u8 value,
				  unsigned long port)
{
	writew(value << 8, (void __iomem *) port);
}

static inline void magicbox_ide_insw(unsigned long port, void *addr, u32 count)
{
	u16 *ptr;

	for (ptr = addr; count--; ptr++)
		*ptr = readw((void __iomem *) port);
}

static inline void magicbox_ide_insl(unsigned long port, void *addr, u32 count)
{
	u32 *ptr;

	for (ptr = addr; count--; ptr++)
		*ptr = readl((void __iomem *) port);
}

static inline void magicbox_ide_outsw(unsigned long port, void *addr,
					u32 count)
{
	u16 *ptr;

	for (ptr = addr; count--; ptr++)
		writew(*ptr, (void __iomem *) port);
}

static inline void magicbox_ide_outsl(unsigned long port, void *addr,
					u32 count)
{
	u32 *ptr;

	for (ptr = addr; count--; ptr++)
		writel(*ptr, (void __iomem *) port);
}

static void magicbox_ide_tf_load(ide_drive_t *drive, ide_task_t *task)
{
	struct ide_io_ports *io_ports = &drive->hwif->io_ports;
	struct ide_taskfile *tf = &task->tf;
	u8 HIHI = (task->tf_flags & IDE_TFLAG_LBA48) ? 0xE0 : 0xEF;

	if (task->tf_flags & IDE_TFLAG_FLAGGED)
		HIHI = 0xFF;

	ide_set_irq(drive, 1);

	if (task->tf_flags & IDE_TFLAG_OUT_DATA)
		writel((tf->hob_data << 8) | tf->data,
			(void __iomem *) io_ports->data_addr);

	if (task->tf_flags & IDE_TFLAG_OUT_HOB_FEATURE)
		magicbox_ide_outb(tf->hob_feature, io_ports->feature_addr);
	if (task->tf_flags & IDE_TFLAG_OUT_HOB_NSECT)
		magicbox_ide_outb(tf->hob_nsect, io_ports->nsect_addr);
	if (task->tf_flags & IDE_TFLAG_OUT_HOB_LBAL)
		magicbox_ide_outb(tf->hob_lbal, io_ports->lbal_addr);
	if (task->tf_flags & IDE_TFLAG_OUT_HOB_LBAM)
		magicbox_ide_outb(tf->hob_lbam, io_ports->lbam_addr);
	if (task->tf_flags & IDE_TFLAG_OUT_HOB_LBAH)
		magicbox_ide_outb(tf->hob_lbah, io_ports->lbah_addr);

	if (task->tf_flags & IDE_TFLAG_OUT_FEATURE)
		magicbox_ide_outb(tf->feature, io_ports->feature_addr);
	if (task->tf_flags & IDE_TFLAG_OUT_NSECT)
		magicbox_ide_outb(tf->nsect, io_ports->nsect_addr);
	if (task->tf_flags & IDE_TFLAG_OUT_LBAL)
		magicbox_ide_outb(tf->lbal, io_ports->lbal_addr);
	if (task->tf_flags & IDE_TFLAG_OUT_LBAM)
		magicbox_ide_outb(tf->lbam, io_ports->lbam_addr);
	if (task->tf_flags & IDE_TFLAG_OUT_LBAH)
		magicbox_ide_outb(tf->lbah, io_ports->lbah_addr);

	if (task->tf_flags & IDE_TFLAG_OUT_DEVICE)
		magicbox_ide_outb((tf->device & HIHI) | drive->select.all,
			     io_ports->device_addr);
}

static void magicbox_ide_tf_read(ide_drive_t *drive, ide_task_t *task)
{
	struct ide_io_ports *io_ports = &drive->hwif->io_ports;
	struct ide_taskfile *tf = &task->tf;

	if (task->tf_flags & IDE_TFLAG_IN_DATA) {
		u16 data = (u16) readl((void __iomem *) io_ports->data_addr);

		tf->data = data & 0xff;
		tf->hob_data = (data >> 8) & 0xff;
	}

	/* be sure we're looking at the low order bits */
	magicbox_ide_outb(drive->ctl & ~0x80, io_ports->ctl_addr);

	if (task->tf_flags & IDE_TFLAG_IN_NSECT)
		tf->nsect  = magicbox_ide_inb(io_ports->nsect_addr);
	if (task->tf_flags & IDE_TFLAG_IN_LBAL)
		tf->lbal   = magicbox_ide_inb(io_ports->lbal_addr);
	if (task->tf_flags & IDE_TFLAG_IN_LBAM)
		tf->lbam   = magicbox_ide_inb(io_ports->lbam_addr);
	if (task->tf_flags & IDE_TFLAG_IN_LBAH)
		tf->lbah   = magicbox_ide_inb(io_ports->lbah_addr);
	if (task->tf_flags & IDE_TFLAG_IN_DEVICE)
		tf->device = magicbox_ide_inb(io_ports->device_addr);

	if (task->tf_flags & IDE_TFLAG_LBA48) {
		magicbox_ide_outb(drive->ctl | 0x80, io_ports->ctl_addr);

		if (task->tf_flags & IDE_TFLAG_IN_HOB_FEATURE)
			tf->hob_feature = magicbox_ide_inb(io_ports->feature_addr);
		if (task->tf_flags & IDE_TFLAG_IN_HOB_NSECT)
			tf->hob_nsect   = magicbox_ide_inb(io_ports->nsect_addr);
		if (task->tf_flags & IDE_TFLAG_IN_HOB_LBAL)
			tf->hob_lbal    = magicbox_ide_inb(io_ports->lbal_addr);
		if (task->tf_flags & IDE_TFLAG_IN_HOB_LBAM)
			tf->hob_lbam    = magicbox_ide_inb(io_ports->lbam_addr);
		if (task->tf_flags & IDE_TFLAG_IN_HOB_LBAH)
			tf->hob_lbah    = magicbox_ide_inb(io_ports->lbah_addr);
	}
}

static void magicbox_ide_input_data(ide_drive_t *drive, struct request *rq,
			   void *buf, unsigned int len)
{
	unsigned long port = drive->hwif->io_ports.data_addr;

	len++;

	if (drive->io_32bit) {
		magicbox_ide_insl(port, buf, len / 4);

		if ((len & 3) >= 2)
			magicbox_ide_insw(port, (u8 *)buf + (len & ~3), 1);
	} else
		magicbox_ide_insw(port, buf, len / 2);
}

static void magicbox_ide_output_data(ide_drive_t *drive,  struct request *rq,
			    void *buf, unsigned int len)
{
	unsigned long port = drive->hwif->io_ports.data_addr;

	len++;

	if (drive->io_32bit) {
		magicbox_ide_outsl(port, buf, len / 4);

		if ((len & 3) >= 2)
			magicbox_ide_outsw(port, (u8 *)buf + (len & ~3), 1);
	} else
		magicbox_ide_outsw(port, buf, len / 2);
}

static void __init magicbox_ide_setup_hw(hw_regs_t *hw, u16 __iomem *base,
					u16 __iomem *ctrl, int irq)
{
	unsigned long port = (unsigned long) base;
	int i;

	memset(hw, 0, sizeof(*hw));
	for (i = 0; i <= 7; i++)
		hw->io_ports_array[i] = port + i * 2;

	/*
	 * the IDE control register is at ATA address 6,
	 * with CS1 active instead of CS0
	 */
	hw->io_ports.ctl_addr = (unsigned long)ctrl + (6 * 2);

	hw->irq = irq;

	hw->chipset = ide_generic;
	hw->ack_intr = NULL;
}

static int __init magibox_ide_probe(void)
{
	hw_regs_t hw;
	ide_hwif_t *hwif;
	u16 __iomem *base;
	u16 __iomem *ctrl;
	u8 idx[4] = { 0xff, 0xff, 0xff, 0xff };
	int err;

	/* Remap physical address space */
	base = ioremap_nocache(0xff100000, 4096);
	if (base == NULL) {
		err = -EBUSY;
		goto err_out;
	}

	ctrl = ioremap_nocache(0xff200000, 4096);
	if (ctrl == NULL) {
		err = -EBUSY;
		goto err_unmap_base;
	}

	magicbox_ide_setup_hw(&hw, base, ctrl, MAGICBOX_CF_IRQ);

	hwif = ide_find_port();
	if (!hwif) {
		err = -ENODEV;
		goto err_unmap_ctrl;
	}

	ide_init_port_data(hwif, hwif->index);
	ide_init_port_hw(hwif, &hw);

	hwif->host_flags = IDE_HFLAG_MMIO;

	hwif->tf_load = magicbox_ide_tf_load;
	hwif->tf_read = magicbox_ide_tf_read;

	hwif->input_data  = magicbox_ide_input_data;
	hwif->output_data = magicbox_ide_output_data;

	hwif->drives[0].unmask = 1;
	hwif->OUTB = magicbox_ide_outb;
	hwif->OUTBSYNC = magicbox_ide_outbsync;
	hwif->INB = magicbox_ide_inb;

	printk(KERN_INFO "ide%d: Magicbox CF interface\n", hwif->index);

	idx[0] = hwif->index;

	ide_device_add(idx, NULL);

	return 0;

err_unmap_ctrl:
	iounmap(ctrl);
err_unmap_base:
	iounmap(base);
err_out:
	return err;
}

static int __init magicbox_ide_init(void)
{
	/* Turn on PerWE instead of PCIsomething */
	mtdcr(DCRN_CPC0_PCI_BASE,
			mfdcr(DCRN_CPC0_PCI_BASE) | (0x80000000L >> 27));

	/* PerCS1 (CF's CS0): base 0xff100000, 16-bit, rw */
	mtdcr(DCRN_EBC_BASE, 1);
	mtdcr(DCRN_EBC_BASE + 1, 0xff11a000);
	mtdcr(DCRN_EBC_BASE, 0x11);
	mtdcr(DCRN_EBC_BASE + 1, 0x080bd800);

	/* PerCS2 (CF's CS1): base 0xff200000, 16-bit, rw */
	mtdcr(DCRN_EBC_BASE, 2);
	mtdcr(DCRN_EBC_BASE + 1, 0xff21a000);
	mtdcr(DCRN_EBC_BASE, 0x12);
	mtdcr(DCRN_EBC_BASE + 1, 0x080bd800);

	/* Set interrupt to low-to-high-edge-triggered */
	mtdcr(UIC0_TR, mfdcr(UIC0_TR) & ~(0x80000000L >> MAGICBOX_CF_IRQ));
	mtdcr(UIC0_PR, mfdcr(UIC0_PR) | (0x80000000L >> MAGICBOX_CF_IRQ));

	return magibox_ide_probe();
}

module_init(magicbox_ide_init);

MODULE_LICENSE("GPL");
