/*
 *  NAND flash driver for the MikroTik RouterBOARD 91x series
 *
 *  Copyright (C) 2013 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/module.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/slab.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#define DRV_NAME	"rb91x-nand"
#define DRV_DESC	"NAND flash driver for the RouterBOARD 91x series"

#define RB91X_NAND_NRE_ENABLE	BIT(3)
#define RB91X_NAND_RDY		BIT(4)
#define RB91X_LATCH_ENABLE	BIT(11)
#define RB91X_NAND_NRWE		BIT(12)
#define RB91X_NAND_NCE		BIT(13)
#define RB91X_NAND_CLE		BIT(14)
#define RB91X_NAND_ALE		BIT(15)

#define RB91X_NAND_DATA_BITS	(BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) |\
				 BIT(13) | BIT(14) | BIT(15))

#define RB91X_NAND_INPUT_BITS	(RB91X_NAND_DATA_BITS | RB91X_NAND_RDY)
#define RB91X_NAND_OUTPUT_BITS	\
	(RB91X_NAND_DATA_BITS | RB91X_NAND_NRWE)

#define RB91X_NAND_LOW_DATA_MASK	0x1f
#define RB91X_NAND_HIGH_DATA_MASK	0xe0
#define RB91X_NAND_HIGH_DATA_SHIFT	8

struct rb91x_nand_info {
	struct nand_chip	chip;
	struct mtd_info		mtd;
};

static inline struct rb91x_nand_info *mtd_to_rbinfo(struct mtd_info *mtd)
{
	return container_of(mtd, struct rb91x_nand_info, mtd);
}

/*
 * We need to use the OLD Yaffs-1 OOB layout, otherwise the RB bootloader
 * will not be able to find the kernel that we load.
 */
static struct nand_ecclayout rb91x_nand_ecclayout = {
	.eccbytes	= 6,
	.eccpos		= { 8, 9, 10, 13, 14, 15 },
	.oobavail	= 9,
	.oobfree	= { { 0, 4 }, { 6, 2 }, { 11, 2 }, { 4, 1 } }
};

static struct mtd_partition rb91x_nand_partitions[] = {
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

static void rb91x_change_gpo(u32 clear, u32 set)
{
	void __iomem *base = ath79_gpio_base;
	static unsigned on = 0xE002800;
	static unsigned off = 0x0000C008;
	static unsigned oe = 0;
	static DEFINE_SPINLOCK(lock);
	unsigned long flags;

	spin_lock_irqsave(&lock, flags);

	on = (on | set) & ~clear;
	off = (off | clear) & ~set;

	if (!oe)
		oe = __raw_readl(base + AR71XX_GPIO_REG_OE);

	if (on & RB91X_LATCH_ENABLE) {
		u32 t;

		t = oe & __raw_readl(base + AR71XX_GPIO_REG_OE);
		t &= ~(on | off);
		__raw_writel(t, base + AR71XX_GPIO_REG_OE);
		__raw_writel(off, base + AR71XX_GPIO_REG_CLEAR);
		__raw_writel(on, base + AR71XX_GPIO_REG_SET);
	} else if (clear & RB91X_LATCH_ENABLE) {
		oe = __raw_readl(base + AR71XX_GPIO_REG_OE);
		__raw_writel(RB91X_LATCH_ENABLE,
			     base + AR71XX_GPIO_REG_CLEAR);
		/* flush write */
		__raw_readl(base + AR71XX_GPIO_REG_CLEAR);
	}

	spin_unlock_irqrestore(&lock, flags);
}

static inline void rb91x_latch_enable(void)
{
	rb91x_change_gpo(RB91X_LATCH_ENABLE, 0);
}

static inline void rb91x_latch_disable(void)
{
	rb91x_change_gpo(0, RB91X_LATCH_ENABLE);
}

static void rb91x_nand_write(const u8 *buf, unsigned len)
{
	void __iomem *base = ath79_gpio_base;
	u32 oe_reg;
	u32 out_reg;
	u32 out;
	unsigned i;

	rb91x_latch_enable();

	oe_reg = __raw_readl(base + AR71XX_GPIO_REG_OE);
	out_reg = __raw_readl(base + AR71XX_GPIO_REG_OUT);

	/* set data lines to output mode */
	__raw_writel(oe_reg & ~(RB91X_NAND_DATA_BITS | RB91X_NAND_NRWE),
		     base + AR71XX_GPIO_REG_OE);

	out = out_reg & ~(RB91X_NAND_DATA_BITS | RB91X_NAND_NRWE);
	for (i = 0; i != len; i++) {
		u32 data;

		data = (buf[i] & RB91X_NAND_HIGH_DATA_MASK) <<
			RB91X_NAND_HIGH_DATA_SHIFT;
		data |= buf[i] & RB91X_NAND_LOW_DATA_MASK;
		data |= out;
		__raw_writel(data, base + AR71XX_GPIO_REG_OUT);

		/* deactivate WE line */
		data |= RB91X_NAND_NRWE;
		__raw_writel(data, base + AR71XX_GPIO_REG_OUT);
		/* flush write */
		__raw_readl(base + AR71XX_GPIO_REG_OUT);
	}

	/* restore  registers */
	__raw_writel(oe_reg, base + AR71XX_GPIO_REG_OE);
	__raw_writel(out_reg, base + AR71XX_GPIO_REG_OUT);
	/* flush write */
	__raw_readl(base + AR71XX_GPIO_REG_OUT);

	rb91x_latch_disable();
}

static void rb91x_nand_read(u8 *read_buf, unsigned len)
{
	void __iomem *base = ath79_gpio_base;
	u32 oe_reg;
	u32 out_reg;
	unsigned i;

	/* save registers */
	oe_reg = __raw_readl(base + AR71XX_GPIO_REG_OE);

	/* select nRE mode */
	rb91x_change_gpo(0, RB91X_NAND_NRE_ENABLE);

	/* enable latch */
	rb91x_latch_enable();

	out_reg = __raw_readl(base + AR71XX_GPIO_REG_OUT);

	/* set data lines to input mode */
	__raw_writel(oe_reg | RB91X_NAND_DATA_BITS,
		     base + AR71XX_GPIO_REG_OE);

	for (i = 0; i < len; i++) {
		u32 in;
		u8 data;

		/* activate RE line */
		__raw_writel(RB91X_NAND_NRWE, base + AR71XX_GPIO_REG_CLEAR);
		/* flush write */
		__raw_readl(base + AR71XX_GPIO_REG_CLEAR);

		/* read input lines */
		in = __raw_readl(base + AR71XX_GPIO_REG_IN);

		/* deactivate RE line */
		__raw_writel(RB91X_NAND_NRWE, base + AR71XX_GPIO_REG_SET);

		data = (in & RB91X_NAND_LOW_DATA_MASK);
		data |= (in >> RB91X_NAND_HIGH_DATA_SHIFT) &
			RB91X_NAND_HIGH_DATA_MASK;

		read_buf[i] = data;
	}

	/* restore  registers */
	__raw_writel(oe_reg, base + AR71XX_GPIO_REG_OE);
	__raw_writel(out_reg, base + AR71XX_GPIO_REG_OUT);
	/* flush write */
	__raw_readl(base + AR71XX_GPIO_REG_OUT);

	/* disable latch */
	rb91x_latch_disable();

	/* deselect nRE mode */
	rb91x_change_gpo(RB91X_NAND_NRE_ENABLE, 0);
}

static int rb91x_nand_dev_ready(struct mtd_info *mtd)
{
	void __iomem *base = ath79_gpio_base;

	return !!(__raw_readl(base + AR71XX_GPIO_REG_IN) & RB91X_NAND_RDY);
}

static void rb91x_nand_cmd_ctrl(struct mtd_info *mtd, int cmd,
				unsigned int ctrl)
{
	if (ctrl & NAND_CTRL_CHANGE) {
		u32 on = 0;
		u32 off;

		if (!(ctrl & NAND_NCE))
			on |= RB91X_NAND_NCE;

		if (ctrl & NAND_CLE)
			on |= RB91X_NAND_CLE;

		if (ctrl & NAND_ALE)
			on |= RB91X_NAND_ALE;

		off = on ^ (RB91X_NAND_ALE | RB91X_NAND_NCE | RB91X_NAND_CLE);
		rb91x_change_gpo(off, on);
	}

	if (cmd != NAND_CMD_NONE) {
		u8 t = cmd;

		rb91x_nand_write(&t, 1);
	}
}

static u8 rb91x_nand_read_byte(struct mtd_info *mtd)
{
	u8 data = 0xff;

	rb91x_nand_read(&data, 1);

	return data;
}

static void rb91x_nand_read_buf(struct mtd_info *mtd, u8 *buf, int len)
{
	rb91x_nand_read(buf, len);
}

static void rb91x_nand_write_buf(struct mtd_info *mtd, const u8 *buf, int len)
{
	rb91x_nand_write(buf, len);
}

static int rb91x_nand_probe(struct platform_device *pdev)
{
	struct rb91x_nand_info	*info;
	int ret;

	pr_info(DRV_DESC "\n");

	info = devm_kzalloc(&pdev->dev, sizeof(*info), GFP_KERNEL);
	if (!info)
		return -ENOMEM;

	info->chip.priv	= &info;
	info->mtd.priv	= &info->chip;
	info->mtd.owner	= THIS_MODULE;

	info->chip.cmd_ctrl	= rb91x_nand_cmd_ctrl;
	info->chip.dev_ready	= rb91x_nand_dev_ready;
	info->chip.read_byte	= rb91x_nand_read_byte;
	info->chip.write_buf	= rb91x_nand_write_buf;
	info->chip.read_buf	= rb91x_nand_read_buf;

	info->chip.chip_delay	= 25;
	info->chip.ecc.mode	= NAND_ECC_SOFT;

	platform_set_drvdata(pdev, info);

	ret = nand_scan_ident(&info->mtd, 1, NULL);
	if (ret)
		return ret;

	if (info->mtd.writesize == 512)
		info->chip.ecc.layout = &rb91x_nand_ecclayout;

	ret = nand_scan_tail(&info->mtd);
	if (ret)
		return ret;

	ret = mtd_device_register(&info->mtd, rb91x_nand_partitions,
				 ARRAY_SIZE(rb91x_nand_partitions));
	if (ret)
		goto err_release_nand;

	return 0;

err_release_nand:
	nand_release(&info->mtd);
	return ret;
}

static int rb91x_nand_remove(struct platform_device *pdev)
{
	struct rb91x_nand_info *info = platform_get_drvdata(pdev);

	nand_release(&info->mtd);

	return 0;
}

static struct platform_driver rb91x_nand_driver = {
	.probe	= rb91x_nand_probe,
	.remove	= rb91x_nand_remove,
	.driver	= {
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
	},
};

module_platform_driver(rb91x_nand_driver);

MODULE_DESCRIPTION(DRV_DESC);
MODULE_VERSION(DRV_VERSION);
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_LICENSE("GPL v2");
