/*==============================================================================*/
/* rbmipsnand.c                                                                 */
/* This module is derived from the 2.4 driver shipped by Microtik for their     */
/* Routerboard 1xx and 5xx series boards.  It provides support for the built in */
/* NAND flash on the Routerboard 1xx series boards for Linux 2.6.19+.           */
/* Licence: Original Microtik code seems not to have a licence.                 */
/*          Rewritten code all GPL V2.                                          */
/* Copyright(C) 2007 david.goodenough@linkchoose.co.uk (for rewriten code)      */
/*==============================================================================*/

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
#include <asm/mach-adm5120/adm5120_info.h>
#include <asm/mach-adm5120/adm5120_defs.h>

#define SMEM1(x) (*((volatile unsigned char *) (KSEG1ADDR(ADM5120_SRAM1_BASE) + x)))

#define NAND_RW_REG	0x0	//data register
#define NAND_SET_CEn	0x1	//CE# low
#define NAND_CLR_CEn	0x2	//CE# high
#define NAND_CLR_CLE	0x3	//CLE low
#define NAND_SET_CLE	0x4	//CLE high
#define NAND_CLR_ALE	0x5	//ALE low
#define NAND_SET_ALE	0x6	//ALE high
#define NAND_SET_SPn	0x7	//SP# low (use spare area)
#define NAND_CLR_SPn	0x8	//SP# high (do not use spare area)
#define NAND_SET_WPn	0x9	//WP# low
#define NAND_CLR_WPn	0xA	//WP# high
#define NAND_STS_REG	0xB	//Status register

#define MEM32(x) *((volatile unsigned *) (x))

static struct mtd_partition partition_info[] = {
    {
        name: "RouterBoard NAND Boot",
        offset: 0,
        size: 4 * 1024 * 1024
    },
    {
        name: "rootfs",
        offset: MTDPART_OFS_NXTBLK,
        size: MTDPART_SIZ_FULL
    }
};

static struct nand_ecclayout rb_ecclayout = {
        .eccbytes = 6,
        .eccpos = { 8, 9, 10, 13, 14, 15 },
        .oobavail = 9,
        .oobfree = { { 0, 4 }, { 6, 2 }, { 11, 2 }, { 4, 1} }
};

struct adm5120_nand_info {
	struct nand_chip chip;
	struct mtd_info mtd;
	void __iomem *io_base;
#ifdef CONFIG_MTD_PARTITIONS
	int	nr_parts;
	struct mtd_partition *parts;
#endif
};

static int rb100_dev_ready(struct mtd_info *mtd)
{
	return SMEM1(NAND_STS_REG) & 0x80;
}

static void rbmips_hwcontrol100(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
    	struct nand_chip *chip = mtd->priv;
	if (ctrl & NAND_CTRL_CHANGE)
	{
        	SMEM1((( ctrl & NAND_CLE) ? NAND_SET_CLE : NAND_CLR_CLE)) = 0x01;
        	SMEM1((( ctrl & NAND_ALE) ? NAND_SET_ALE : NAND_CLR_ALE)) = 0x01;
        	SMEM1((( ctrl & NAND_NCE) ? NAND_SET_CEn : NAND_CLR_CEn)) = 0x01;
        }
    	if (cmd != NAND_CMD_NONE)
        	writeb( cmd, chip->IO_ADDR_W);
}

/*========================================================================*/
/* We need to use the OLD Yaffs-1 OOB layout, otherwise the RB bootloader */
/* will not be able to find the kernel that we load.  So set the oobinfo  */
/* when creating the partitions.                                          */ 
/*========================================================================*/

static unsigned init_ok = 0;

unsigned get_rbnand_block_size(struct adm5120_nand_info *data)
{
	return init_ok ? data->mtd.writesize : 0;
}

EXPORT_SYMBOL(get_rbnand_block_size);

static int rbmips_probe(struct platform_device *pdev)
{
	struct adm5120_nand_info *data;
	int res = 0;

	/* Allocate memory for the nand_chip structure */
	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (!data) {
		dev_err(&pdev->dev, "Failed to allocate device structure\n");
		return -ENOMEM;

	}
	
	data->io_base = ioremap(pdev->resource[0].start, 0x1000);

	if (data->io_base == NULL) {
		dev_err(&pdev->dev, "ioremap failed\n");
		kfree(data);
		return -EIO;
	}

	MEM32(0xB2000064) = 0x100;
	MEM32(0xB2000008) = 0x1;
	SMEM1(NAND_SET_SPn) = 0x01;
	SMEM1(NAND_CLR_WPn) = 0x01;

	data->chip.priv = &data;
	data->mtd.priv = &data->chip;
	data->mtd.owner = THIS_MODULE;

	data->chip.IO_ADDR_R = (unsigned char *)KSEG1ADDR(ADM5120_SRAM1_BASE);
	data->chip.IO_ADDR_W = data->chip.IO_ADDR_R;
	data->chip.cmd_ctrl = rbmips_hwcontrol100;
	data->chip.dev_ready = rb100_dev_ready;
	data->chip.ecc.mode = NAND_ECC_SOFT;
	data->chip.ecc.layout = &rb_ecclayout;
	data->chip.chip_delay = 25;
	data->chip.options |= NAND_NO_AUTOINCR;

	platform_set_drvdata(pdev, data);

	/* Why do we need to scan 4 times ? */
	if (nand_scan(&data->mtd, 1) && nand_scan(&data->mtd, 1) && nand_scan(&data->mtd, 1)  && nand_scan(&data->mtd, 1)) {
		printk(KERN_INFO "RB1xxx nand device not found\n");
		res = -ENXIO;
		goto out;
	}

	add_mtd_partitions(&data->mtd, partition_info, 2);
	init_ok = 1;

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
	struct adm5120_nand_info *data = platform_get_drvdata(pdev);
	
	nand_release(&data->mtd);
	iounmap(data->io_base);
	kfree(data);
	
	return 0;
}

static struct platform_driver adm5120_nand_driver = {
	.probe 		= rbmips_probe,
	.remove 	= rbmips_remove,
	.driver 	= {
		.name	= "adm5120-nand",
		.owner	= THIS_MODULE,
	},
};

static int __init adm5120_nand_init(void)
{
	int err;
	err = platform_driver_register(&adm5120_nand_driver);
	return err;
}

static void __exit adm5120_nand_exit(void)
{
	platform_driver_unregister(&adm5120_nand_driver);
}

module_init(adm5120_nand_init);
module_exit(adm5120_nand_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("David Goodenough, Florian Fainelli");
MODULE_DESCRIPTION("RouterBOARD 100 NAND driver");

