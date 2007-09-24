#include <linux/io.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/bootinfo.h>

#define IDT434_REG_BASE ((volatile void *) KSEG1ADDR(0x18000000))

#define GPIOF 0x050000
#define GPIOC 0x050004
#define GPIOD 0x050008

#define GPIO_RDY (1 << 0x08)
#define GPIO_WPX (1 << 0x09)
#define GPIO_ALE (1 << 0x0a)
#define GPIO_CLE (1 << 0x0b)

#define DEV2BASE 0x010020

#define LO_WPX   (1 << 0)
#define LO_ALE   (1 << 1)
#define LO_CLE   (1 << 2)
#define LO_CEX   (1 << 3)
#define LO_FOFF  (1 << 5)
#define LO_SPICS (1 << 6)
#define LO_ULED  (1 << 7)

#define MEM32(x) *((volatile unsigned *) (x))

extern char *board_type;

struct rb500_nand_info {
        struct nand_chip chip;
        struct mtd_info mtd;
        void __iomem *io_base;
#ifdef CONFIG_MTD_PARTITIONS
        int     nr_parts;
        struct mtd_partition *parts;
#endif
	int	init_ok;
	int	flags1;
	int 	flags2;
};

static struct mtd_partition partition_info[] = {
        {
              name:"RouterBoard NAND Boot",
              offset:0,
      size:4 * 1024 * 1024},
        {
              name:"rootfs",
              offset:MTDPART_OFS_NXTBLK,
      	size:MTDPART_SIZ_FULL}
};

extern void changeLatchU5(unsigned char orMask, unsigned char nandMask);

static int rb500_dev_ready(struct mtd_info *mtd)
{
	return MEM32(IDT434_REG_BASE + GPIOD) & GPIO_RDY;
}

/*
 * hardware specific access to control-lines
 *
 * ctrl:
 *     NAND_CLE: bit 2 -> bit 3 
 *     NAND_ALE: bit 3 -> bit 2
 */
static void rbmips_hwcontrol500(struct mtd_info *mtd, int cmd,
				unsigned int ctrl)
{
	struct nand_chip *chip = mtd->priv;
	unsigned char orbits, nandbits;

	if (ctrl & NAND_CTRL_CHANGE) {

		orbits = (ctrl & NAND_CLE) << 1;
		orbits |= (ctrl & NAND_ALE) >> 1;

		nandbits = (~ctrl & NAND_CLE) << 1;
		nandbits |= (~ctrl & NAND_ALE) >> 1;

		changeLatchU5(orbits, nandbits);
	}
	if (cmd != NAND_CMD_NONE)
		writeb(cmd, chip->IO_ADDR_W);

}

unsigned get_rbnand_block_size(struct rb500_nand_info *data)
{
	if (data->init_ok)
		return data->mtd.writesize;
	else
		return 0;
}

EXPORT_SYMBOL(get_rbnand_block_size);

static int rbmips_probe(struct platform_device *pdev)
{
	struct rb500_nand_info *data;
	int res = 0;
	int *b;

	/* Allocate memory for the structure */
	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (!data) {
		dev_err(&pdev->dev, "Failed to allocate device structure\n");
		return -ENOMEM;
	}

	data->io_base = ioremap(pdev->resource[0].start, pdev->resource[0].end - pdev->resource[0].start + 1);

	if (data->io_base == NULL) {
		dev_err(&pdev->dev, "ioremap failed\n");
		kfree(data);
		return -EIO;
        }

	if (!strcmp(board_type, "500r5")) {
		data->flags1 = LO_FOFF | LO_CEX;
		data->flags2 = LO_ULED | LO_ALE | LO_CLE | LO_WPX;
	}
	else {
		data->flags1 = LO_WPX | LO_FOFF | LO_CEX;
		data->flags2 = LO_ULED | LO_ALE | LO_CLE;
	}

	changeLatchU5(data->flags1, data->flags2);

	data->chip.cmd_ctrl = rbmips_hwcontrol500;

	data->chip.dev_ready = rb500_dev_ready;
	data->chip.IO_ADDR_W = (unsigned char *)KSEG1ADDR(MEM32(IDT434_REG_BASE + DEV2BASE));
	data->chip.IO_ADDR_R = data->chip.IO_ADDR_W;

	data->chip.ecc.mode = NAND_ECC_SOFT;
	data->chip.chip_delay = 25;
	data->chip.options |= NAND_NO_AUTOINCR;

        data->chip.priv = &data;
        data->mtd.priv = &data->chip;
        data->mtd.owner = THIS_MODULE;

	b = (int *) KSEG1ADDR(0x18010020);
	printk("dev2base 0x%08x mask 0x%08x c 0x%08x tc 0x%08x\n", b[0],
	       b[1], b[2], b[3]);

	platform_set_drvdata(pdev, data);

	/* Why do we need to scan 4 times ? */
        if (nand_scan(&data->mtd, 1) && nand_scan(&data->mtd, 1) && nand_scan(&data->mtd, 1)  && nand_scan(&data->mtd, 1)) {
                printk(KERN_INFO "RB500 nand device not found\n");
                res = -ENXIO;
                goto out;
        }

	printk(KERN_INFO "RB500 NAND\n");
	add_mtd_partitions(&data->mtd, partition_info, 2);
	data->init_ok = 1;

        res = add_mtd_device(&data->mtd);
        if (!res)
                return res;

        nand_release(&data->mtd);
out:
        platform_set_drvdata(pdev, NULL);
        iounmap(data->io_base);
        kfree(data);
        return res;
}

static int __devexit rbmips_remove(struct platform_device *pdev)
{
        struct rb500_nand_info *data = platform_get_drvdata(pdev);

        nand_release(&data->mtd);
        iounmap(data->io_base);
        kfree(data);

        return 0;
}

static struct platform_driver rb500_nand_driver = {
        .probe          = rbmips_probe,
        .remove         = rbmips_remove,
        .driver         = {
                .name   = "rb500-nand",
                .owner  = THIS_MODULE,
        },
};

static int __init rb500_nand_init(void)
{
        int err;
        err = platform_driver_register(&rb500_nand_driver);
        return err;
}

static void __exit rb500_nand_exit(void)
{
        platform_driver_unregister(&rb500_nand_driver);
}

module_init(rb500_nand_init);
module_exit(rb500_nand_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Goodenough, Felix Fietkau, Florian Fainelli");
MODULE_DESCRIPTION("RouterBOARD 500 NAND driver");
