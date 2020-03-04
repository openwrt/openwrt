/*
 *  NAND flash driver for the MikroTik RouterBoard 4xx series
 *
 *  Copyright (C) 2008-2011 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This file was based on the driver for Linux 2.6.22 published by
 *  MikroTik for their RouterBoard 4xx series devices.
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/mtd/rawnand.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/gpio.h>
#include <linux/slab.h>
#include <linux/of.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/rb4xx_cpld.h>

#define DRV_NAME        "rb4xx-nand"
#define DRV_VERSION     "0.2.1"
#define DRV_DESC        "Mikrotik RB4xx NAND flash driver"

#define RB4XX_NAND_GPIO_READY	5
#define RB4XX_NAND_GPIO_ALE	37
#define RB4XX_NAND_GPIO_CLE	38
#define RB4XX_NAND_GPIO_NCE	39

struct rb4xx_nand_info {
	struct nand_chip	chip;
};

static inline struct rb4xx_nand_info *mtd_to_rbinfo(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd_to_nand(mtd);

	return container_of(chip, struct rb4xx_nand_info, chip);
}

static struct mtd_info *rbinfo_to_mtd(struct rb4xx_nand_info *nfc)
{
	return nand_to_mtd(&nfc->chip);
}

static int rb4xx_ooblayout_ecc(struct mtd_info *mtd, int section,
			       struct mtd_oob_region *oobregion)
{
	switch (section) {
	case 0:
		oobregion->offset = 8;
		oobregion->length = 3;
		return 0;
	case 1:
		oobregion->offset = 13;
		oobregion->length = 3;
		return 0;
	default:
		return -ERANGE;
	}
}

static int rb4xx_ooblayout_free(struct mtd_info *mtd, int section,
				struct mtd_oob_region *oobregion)
{
	switch (section) {
	case 0:
		oobregion->offset = 0;
		oobregion->length = 4;
		return 0;
	case 1:
		oobregion->offset = 4;
		oobregion->length = 1;
		return 0;
	case 2:
		oobregion->offset = 6;
		oobregion->length = 2;
		return 0;
	case 3:
		oobregion->offset = 11;
		oobregion->length = 2;
		return 0;
	default:
		return -ERANGE;
	}
}

static const struct mtd_ooblayout_ops rb4xx_nand_ecclayout_ops = {
	.ecc = rb4xx_ooblayout_ecc,
	.free = rb4xx_ooblayout_free,
};

static struct mtd_partition rb4xx_nand_partitions[] = {
	{
		.name	= "booter",
		.offset	= 0,
		.size	= (256 * 1024),
		.mask_flags = MTD_WRITEABLE,
	},
	{
		.name	= "kernel",
		.offset	= (256 * 1024),
		.size	= (4 * 1024 * 1024) - (256 * 1024),
	},
	{
		.name	= "ubi",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= MTDPART_SIZ_FULL,
	},
};

static int rb4xx_nand_dev_ready(struct mtd_info *mtd)
{
	return gpio_get_value_cansleep(RB4XX_NAND_GPIO_READY);
}

static void rb4xx_nand_write_cmd(unsigned char cmd)
{
	unsigned char data = cmd;
	int err;

	err = rb4xx_cpld_write(&data, 1);
	if (err)
		pr_err("rb4xx_nand: write cmd failed, err=%d\n", err);
}

static void rb4xx_nand_cmd_ctrl(struct mtd_info *mtd, int cmd,
				unsigned int ctrl)
{
	if (ctrl & NAND_CTRL_CHANGE) {
		gpio_set_value_cansleep(RB4XX_NAND_GPIO_CLE,
					(ctrl & NAND_CLE) ? 1 : 0);
		gpio_set_value_cansleep(RB4XX_NAND_GPIO_ALE,
					(ctrl & NAND_ALE) ? 1 : 0);
		gpio_set_value_cansleep(RB4XX_NAND_GPIO_NCE,
					(ctrl & NAND_NCE) ? 0 : 1);
	}

	if (cmd != NAND_CMD_NONE)
		rb4xx_nand_write_cmd(cmd);
}

static unsigned char rb4xx_nand_read_byte(struct mtd_info *mtd)
{
	unsigned char data = 0;
	int err;

	err = rb4xx_cpld_read(&data, 1);
	if (err) {
		pr_err("rb4xx_nand: read data failed, err=%d\n", err);
		data = 0xff;
	}

	return data;
}

static void rb4xx_nand_write_buf(struct mtd_info *mtd, const unsigned char *buf,
				 int len)
{
	int err;

	err = rb4xx_cpld_write(buf, len);
	if (err)
		pr_err("rb4xx_nand: write buf failed, err=%d\n", err);
}

static void rb4xx_nand_read_buf(struct mtd_info *mtd, unsigned char *buf,
				int len)
{
	int err;

	err = rb4xx_cpld_read(buf, len);
	if (err)
		pr_err("rb4xx_nand: read buf failed, err=%d\n", err);
}

static int rb4xx_nand_probe(struct platform_device *pdev)
{
	struct rb4xx_nand_info	*info;
	struct mtd_info *mtd;
	int ret;

	printk(KERN_INFO DRV_DESC " version " DRV_VERSION "\n");

	ret = gpio_request(RB4XX_NAND_GPIO_READY, "NAND RDY");
	if (ret) {
		dev_err(&pdev->dev, "unable to request gpio %d\n",
			RB4XX_NAND_GPIO_READY);
		goto err;
	}

	ret = gpio_direction_input(RB4XX_NAND_GPIO_READY);
	if (ret) {
		dev_err(&pdev->dev, "unable to set input mode on gpio %d\n",
			RB4XX_NAND_GPIO_READY);
		goto err_free_gpio_ready;
	}

	ret = gpio_request(RB4XX_NAND_GPIO_ALE, "NAND ALE");
	if (ret) {
		dev_err(&pdev->dev, "unable to request gpio %d\n",
			RB4XX_NAND_GPIO_ALE);
		goto err_free_gpio_ready;
	}

	ret = gpio_direction_output(RB4XX_NAND_GPIO_ALE, 0);
	if (ret) {
		dev_err(&pdev->dev, "unable to set output mode on gpio %d\n",
			RB4XX_NAND_GPIO_ALE);
		goto err_free_gpio_ale;
	}

	ret = gpio_request(RB4XX_NAND_GPIO_CLE, "NAND CLE");
	if (ret) {
		dev_err(&pdev->dev, "unable to request gpio %d\n",
			RB4XX_NAND_GPIO_CLE);
		goto err_free_gpio_ale;
	}

	ret = gpio_direction_output(RB4XX_NAND_GPIO_CLE, 0);
	if (ret) {
		dev_err(&pdev->dev, "unable to set output mode on gpio %d\n",
			RB4XX_NAND_GPIO_CLE);
		goto err_free_gpio_cle;
	}

	ret = gpio_request(RB4XX_NAND_GPIO_NCE, "NAND NCE");
	if (ret) {
		dev_err(&pdev->dev, "unable to request gpio %d\n",
			RB4XX_NAND_GPIO_NCE);
		goto err_free_gpio_cle;
	}

	ret = gpio_direction_output(RB4XX_NAND_GPIO_NCE, 1);
	if (ret) {
		dev_err(&pdev->dev, "unable to set output mode on gpio %d\n",
			RB4XX_NAND_GPIO_ALE);
		goto err_free_gpio_nce;
	}

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info) {
		dev_err(&pdev->dev, "rb4xx-nand: no memory for private data\n");
		ret = -ENOMEM;
		goto err_free_gpio_nce;
	}

	info->chip.priv	= &info;
	mtd = rbinfo_to_mtd(info);

	mtd->owner	= THIS_MODULE;

	info->chip.cmd_ctrl	= rb4xx_nand_cmd_ctrl;
	info->chip.dev_ready	= rb4xx_nand_dev_ready;
	info->chip.read_byte	= rb4xx_nand_read_byte;
	info->chip.write_buf	= rb4xx_nand_write_buf;
	info->chip.read_buf	= rb4xx_nand_read_buf;

	info->chip.chip_delay	= 25;
	info->chip.ecc.mode	= NAND_ECC_SOFT;
	info->chip.ecc.algo = NAND_ECC_HAMMING;
	info->chip.options = NAND_NO_SUBPAGE_WRITE;

	platform_set_drvdata(pdev, info);

	if (mtd->writesize == 512)
		mtd_set_ooblayout(mtd, &rb4xx_nand_ecclayout_ops);

	ret = nand_scan(mtd, 1);

	if (ret) {
		return -ENXIO;
		goto err_set_drvdata;
	}

	mtd_device_register(mtd, rb4xx_nand_partitions,
				ARRAY_SIZE(rb4xx_nand_partitions));
	if (ret)
		goto err_release_nand;

	return 0;

err_release_nand:
	nand_release(mtd);
err_set_drvdata:
	platform_set_drvdata(pdev, NULL);
err_free_info:
	kfree(info);
err_free_gpio_nce:
	gpio_free(RB4XX_NAND_GPIO_NCE);
err_free_gpio_cle:
	gpio_free(RB4XX_NAND_GPIO_CLE);
err_free_gpio_ale:
	gpio_free(RB4XX_NAND_GPIO_ALE);
err_free_gpio_ready:
	gpio_free(RB4XX_NAND_GPIO_READY);
err:
	return ret;
}

static int rb4xx_nand_remove(struct platform_device *pdev)
{
	struct rb4xx_nand_info *info = platform_get_drvdata(pdev);

	nand_release(rbinfo_to_mtd(info));
	platform_set_drvdata(pdev, NULL);
	kfree(info);
	gpio_free(RB4XX_NAND_GPIO_NCE);
	gpio_free(RB4XX_NAND_GPIO_CLE);
	gpio_free(RB4XX_NAND_GPIO_ALE);
	gpio_free(RB4XX_NAND_GPIO_READY);

	return 0;
}

static const struct of_device_id rb4xx_nand_dt_match[] = {
		{ .compatible = "mikrotik,rb4xx-nand" },
		{ },
};
MODULE_DEVICE_TABLE(of, rb4xx_nand_dt_match);

static struct platform_driver rb4xx_nand_driver = {
	.probe	= rb4xx_nand_probe,
	.remove	= rb4xx_nand_remove,
	.driver	= {
		.name			= DRV_NAME,
		.owner			= THIS_MODULE,
		.of_match_table = of_match_ptr(rb4xx_nand_dt_match)
	},
};

module_platform_driver(rb4xx_nand_driver);

MODULE_DESCRIPTION(DRV_DESC);
MODULE_VERSION(DRV_VERSION);
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_AUTHOR("Imre Kaloz <kaloz@openwrt.org>");
MODULE_LICENSE("GPL v2");
