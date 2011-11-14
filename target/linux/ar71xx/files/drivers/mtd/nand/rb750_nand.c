/*
 *  NAND flash driver for the MikroTik RouterBOARD 750
 *
 *  Copyright (C) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/slab.h>

#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/mach-rb750.h>

#define DRV_NAME	"rb750-nand"
#define DRV_VERSION	"0.1.0"
#define DRV_DESC	"NAND flash driver for the RouterBOARD 750"

#define RB750_NAND_IO0		BIT(RB750_GPIO_NAND_IO0)
#define RB750_NAND_ALE		BIT(RB750_GPIO_NAND_ALE)
#define RB750_NAND_CLE		BIT(RB750_GPIO_NAND_CLE)
#define RB750_NAND_NRE		BIT(RB750_GPIO_NAND_NRE)
#define RB750_NAND_NWE		BIT(RB750_GPIO_NAND_NWE)
#define RB750_NAND_RDY		BIT(RB750_GPIO_NAND_RDY)
#define RB750_NAND_NCE		BIT(RB750_GPIO_NAND_NCE)

#define RB750_NAND_DATA_SHIFT	1
#define RB750_NAND_DATA_BITS	(0xff << RB750_NAND_DATA_SHIFT)
#define RB750_NAND_INPUT_BITS	(RB750_NAND_DATA_BITS | RB750_NAND_RDY)
#define RB750_NAND_OUTPUT_BITS	(RB750_NAND_ALE | RB750_NAND_CLE | \
				 RB750_NAND_NRE | RB750_NAND_NWE | \
				 RB750_NAND_NCE)

struct rb750_nand_info {
	struct nand_chip	chip;
	struct mtd_info		mtd;
};

/*
 * We need to use the OLD Yaffs-1 OOB layout, otherwise the RB bootloader
 * will not be able to find the kernel that we load.
 */
static struct nand_ecclayout rb750_nand_ecclayout = {
	.eccbytes	= 6,
	.eccpos		= { 8, 9, 10, 13, 14, 15 },
	.oobavail	= 9,
	.oobfree	= { { 0, 4 }, { 6, 2 }, { 11, 2 }, { 4, 1 } }
};

static struct mtd_partition rb750_nand_partitions[] = {
	{
		.name	= "booter",
		.offset	= 0,
		.size	= (256 * 1024),
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "kernel",
		.offset	= (256 * 1024),
		.size	= (4 * 1024 * 1024) - (256 * 1024),
	}, {
		.name	= "rootfs",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= MTDPART_SIZ_FULL,
	},
};

static void rb750_nand_write(const u8 *buf, unsigned len)
{
	void __iomem *base = ar71xx_gpio_base;
	u32 out;
	unsigned i;

	/* set data lines to output mode */
	__raw_writel(__raw_readl(base + GPIO_REG_OE) | RB750_NAND_DATA_BITS,
		     base + GPIO_REG_OE);

	out = __raw_readl(base + GPIO_REG_OUT);
	out &= ~(RB750_NAND_DATA_BITS | RB750_NAND_NWE);
	for (i = 0; i != len; i++) {
		u32 data;

		data = buf[i];
		data <<= RB750_NAND_DATA_SHIFT;
		data |= out;
		__raw_writel(data, base + GPIO_REG_OUT);

		__raw_writel(data | RB750_NAND_NWE, base + GPIO_REG_OUT);
		/* flush write */
		__raw_readl(base + GPIO_REG_OUT);
	}

	/* set data lines to input mode */
	__raw_writel(__raw_readl(base + GPIO_REG_OE) & ~RB750_NAND_DATA_BITS,
		     base + GPIO_REG_OE);
	/* flush write */
	__raw_readl(base + GPIO_REG_OE);
}

static int rb750_nand_read_verify(u8 *read_buf, unsigned len,
				  const u8 *verify_buf)
{
	void __iomem *base = ar71xx_gpio_base;
	unsigned i;

	for (i = 0; i < len; i++) {
		u8 data;

		/* activate RE line */
		__raw_writel(RB750_NAND_NRE, base + GPIO_REG_CLEAR);
		/* flush write */
		__raw_readl(base + GPIO_REG_CLEAR);

		/* read input lines */
		data = __raw_readl(base + GPIO_REG_IN) >> RB750_NAND_DATA_SHIFT;

		/* deactivate RE line */
		__raw_writel(RB750_NAND_NRE, base + GPIO_REG_SET);

		if (read_buf)
			read_buf[i] = data;
		else if (verify_buf && verify_buf[i] != data)
			return -EFAULT;
	}

	return 0;
}

static void rb750_nand_select_chip(struct mtd_info *mtd, int chip)
{
	void __iomem *base = ar71xx_gpio_base;
	u32 func;

	func = __raw_readl(base + GPIO_REG_FUNC);
	if (chip >= 0) {
		/* disable latch */
		rb750_latch_change(RB750_LVC573_LE, 0);

		/* disable alternate functions */
		ar71xx_gpio_function_setup(AR724X_GPIO_FUNC_JTAG_DISABLE,
					   AR724X_GPIO_FUNC_SPI_EN);

		/* set input mode for data lines */
		__raw_writel(__raw_readl(base + GPIO_REG_OE) &
			     ~RB750_NAND_INPUT_BITS,
			     base + GPIO_REG_OE);

		/* deactivate RE and WE lines */
		__raw_writel(RB750_NAND_NRE | RB750_NAND_NWE,
			     base + GPIO_REG_SET);
		/* flush write */
		(void) __raw_readl(base + GPIO_REG_SET);

		/* activate CE line */
		__raw_writel(RB750_NAND_NCE, base + GPIO_REG_CLEAR);
	} else {
		/* deactivate CE line */
		__raw_writel(RB750_NAND_NCE, base + GPIO_REG_SET);
		/* flush write */
		(void) __raw_readl(base + GPIO_REG_SET);

		__raw_writel(__raw_readl(base + GPIO_REG_OE) |
			     RB750_NAND_IO0 | RB750_NAND_RDY,
			     base + GPIO_REG_OE);

		/* restore alternate functions */
		ar71xx_gpio_function_setup(AR724X_GPIO_FUNC_SPI_EN,
					   AR724X_GPIO_FUNC_JTAG_DISABLE);

		/* enable latch */
		rb750_latch_change(0, RB750_LVC573_LE);
	}
}

static int rb750_nand_dev_ready(struct mtd_info *mtd)
{
	void __iomem *base = ar71xx_gpio_base;

	return !!(__raw_readl(base + GPIO_REG_IN) & RB750_NAND_RDY);
}

static void rb750_nand_cmd_ctrl(struct mtd_info *mtd, int cmd,
				unsigned int ctrl)
{
	if (ctrl & NAND_CTRL_CHANGE) {
		void __iomem *base = ar71xx_gpio_base;
		u32 t;

		t = __raw_readl(base + GPIO_REG_OUT);

		t &= ~(RB750_NAND_CLE | RB750_NAND_ALE);
		t |= (ctrl & NAND_CLE) ? RB750_NAND_CLE : 0;
		t |= (ctrl & NAND_ALE) ? RB750_NAND_ALE : 0;

		__raw_writel(t, base + GPIO_REG_OUT);
		/* flush write */
		__raw_readl(base + GPIO_REG_OUT);
	}

	if (cmd != NAND_CMD_NONE) {
		u8 t = cmd;
		rb750_nand_write(&t, 1);
	}
}

static u8 rb750_nand_read_byte(struct mtd_info *mtd)
{
	u8 data = 0;
	rb750_nand_read_verify(&data, 1, NULL);
	return data;
}

static void rb750_nand_read_buf(struct mtd_info *mtd, u8 *buf, int len)
{
	rb750_nand_read_verify(buf, len, NULL);
}

static void rb750_nand_write_buf(struct mtd_info *mtd, const u8 *buf, int len)
{
	rb750_nand_write(buf, len);
}

static int rb750_nand_verify_buf(struct mtd_info *mtd, const u8 *buf, int len)
{
	return rb750_nand_read_verify(NULL, len, buf);
}

static void __init rb750_nand_gpio_init(void)
{
	void __iomem *base = ar71xx_gpio_base;
	u32 out;

	out = __raw_readl(base + GPIO_REG_OUT);

	/* setup output levels */
	__raw_writel(RB750_NAND_NCE | RB750_NAND_NRE | RB750_NAND_NWE,
		     base + GPIO_REG_SET);

	__raw_writel(RB750_NAND_ALE | RB750_NAND_CLE,
		     base + GPIO_REG_CLEAR);

	/* setup input lines */
	__raw_writel(__raw_readl(base + GPIO_REG_OE) & ~(RB750_NAND_INPUT_BITS),
		     base + GPIO_REG_OE);

	/* setup output lines */
	__raw_writel(__raw_readl(base + GPIO_REG_OE) | RB750_NAND_OUTPUT_BITS,
		     base + GPIO_REG_OE);

	rb750_latch_change(~out & RB750_NAND_IO0, out & RB750_NAND_IO0);
}

static int __init rb750_nand_probe(struct platform_device *pdev)
{
	struct rb750_nand_info	*info;
	int ret;

	printk(KERN_INFO DRV_DESC " version " DRV_VERSION "\n");

	rb750_nand_gpio_init();

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->chip.priv	= &info;
	info->mtd.priv	= &info->chip;
	info->mtd.owner	= THIS_MODULE;

	info->chip.select_chip	= rb750_nand_select_chip;
	info->chip.cmd_ctrl	= rb750_nand_cmd_ctrl;
	info->chip.dev_ready	= rb750_nand_dev_ready;
	info->chip.read_byte	= rb750_nand_read_byte;
	info->chip.write_buf	= rb750_nand_write_buf;
	info->chip.read_buf	= rb750_nand_read_buf;
	info->chip.verify_buf	= rb750_nand_verify_buf;

	info->chip.chip_delay	= 25;
	info->chip.ecc.mode	= NAND_ECC_SOFT;
	info->chip.options	|= NAND_NO_AUTOINCR;

	platform_set_drvdata(pdev, info);

	ret = nand_scan_ident(&info->mtd, 1, NULL);
	if (ret) {
		ret = -ENXIO;
		goto err_free_info;
	}

	if (info->mtd.writesize == 512)
		info->chip.ecc.layout = &rb750_nand_ecclayout;

	ret = nand_scan_tail(&info->mtd);
	if (ret) {
		return -ENXIO;
		goto err_set_drvdata;
	}

#ifdef CONFIG_MTD_PARTITIONS
	ret = add_mtd_partitions(&info->mtd, rb750_nand_partitions,
				 ARRAY_SIZE(rb750_nand_partitions));
#else
	ret = add_mtd_device(&info->mtd);
#endif
	if (ret)
		goto err_release_nand;

	return 0;

err_release_nand:
	nand_release(&info->mtd);
err_set_drvdata:
	platform_set_drvdata(pdev, NULL);
err_free_info:
	kfree(info);
	return ret;
}

static int __devexit rb750_nand_remove(struct platform_device *pdev)
{
	struct rb750_nand_info *info = platform_get_drvdata(pdev);

	nand_release(&info->mtd);
	platform_set_drvdata(pdev, NULL);
	kfree(info);

	return 0;
}

static struct platform_driver rb750_nand_driver = {
	.probe	= rb750_nand_probe,
	.remove	= __devexit_p(rb750_nand_remove),
	.driver	= {
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init rb750_nand_init(void)
{
	return platform_driver_register(&rb750_nand_driver);
}

static void __exit rb750_nand_exit(void)
{
	platform_driver_unregister(&rb750_nand_driver);
}

module_init(rb750_nand_init);
module_exit(rb750_nand_exit);

MODULE_DESCRIPTION(DRV_DESC);
MODULE_VERSION(DRV_VERSION);
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_LICENSE("GPL v2");
