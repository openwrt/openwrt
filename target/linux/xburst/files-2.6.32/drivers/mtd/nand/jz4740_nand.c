/*
 *  Copyright (C) 2009, Lars-Peter Clausen <lars@metafoo.de>
 *  	JZ4720/JZ4740 SoC NAND controller driver
 *
 *  This program is free software; you can redistribute	 it and/or modify it
 *  under  the terms of	 the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the	License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/ioport.h>
#include <linux/platform_device.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>

#include <linux/mtd/jz4740_nand.h>
#include <linux/gpio.h>

#define JZ_REG_NAND_CTRL	0x50
#define JZ_REG_NAND_ECC_CTRL	0x100
#define JZ_REG_NAND_DATA	0x104
#define JZ_REG_NAND_PAR0	0x108
#define JZ_REG_NAND_PAR1	0x10C
#define JZ_REG_NAND_PAR2	0x110
#define JZ_REG_NAND_IRQ_STAT	0x114
#define JZ_REG_NAND_IRQ_CTRL	0x118
#define JZ_REG_NAND_ERR(x)	(0x11C + (x << 2))

#define JZ_NAND_ECC_CTRL_PAR_READY	BIT(4)
#define JZ_NAND_ECC_CTRL_ENCODING	BIT(3)
#define JZ_NAND_ECC_CTRL_RS		BIT(2)
#define JZ_NAND_ECC_CTRL_RESET		BIT(1)
#define JZ_NAND_ECC_CTRL_ENABLE		BIT(0)

#define JZ_NAND_STATUS_ERR_COUNT	(BIT(31) | BIT(30) | BIT(29))
#define JZ_NAND_STATUS_PAD_FINISH	BIT(4)
#define JZ_NAND_STATUS_DEC_FINISH	BIT(3)
#define JZ_NAND_STATUS_ENC_FINISH	BIT(2)
#define JZ_NAND_STATUS_UNCOR_ERROR	BIT(1)
#define JZ_NAND_STATUS_ERROR		BIT(0)

#define JZ_NAND_CTRL_ENABLE_CHIP(x) BIT(x << 1)
#define JZ_NAND_CTRL_ASSERT_CHIP(x) BIT((x << 1) + 1)

#define JZ_NAND_DATA_ADDR ((void __iomem *)0xB8000000)
#define JZ_NAND_CMD_ADDR (JZ_NAND_DATA_ADDR + 0x8000)
#define JZ_NAND_ADDR_ADDR (JZ_NAND_DATA_ADDR + 0x10000)

struct jz_nand {
	struct mtd_info mtd;
	struct nand_chip chip;
	void __iomem *base;
	struct resource *mem;

	struct jz_nand_platform_data *pdata;
	bool is_reading;
};

static inline struct jz_nand *mtd_to_jz_nand(struct mtd_info *mtd)
{
	return container_of(mtd, struct jz_nand, mtd);
}

static void jz_nand_cmd_ctrl(struct mtd_info *mtd, int dat, unsigned int ctrl)
{
	struct jz_nand *nand = mtd_to_jz_nand(mtd);
	struct nand_chip *chip = mtd->priv;
	uint32_t reg;

	if (ctrl & NAND_CTRL_CHANGE) {
		BUG_ON((ctrl & NAND_ALE) && (ctrl & NAND_CLE));
		if (ctrl & NAND_ALE)
			chip->IO_ADDR_W = JZ_NAND_ADDR_ADDR;
		else if (ctrl & NAND_CLE)
			chip->IO_ADDR_W = JZ_NAND_CMD_ADDR;
		else
			chip->IO_ADDR_W = JZ_NAND_DATA_ADDR;

		reg = readl(nand->base + JZ_REG_NAND_CTRL);
		if ( ctrl & NAND_NCE )
			reg |= JZ_NAND_CTRL_ASSERT_CHIP(0);
		else
			reg &= ~JZ_NAND_CTRL_ASSERT_CHIP(0);
		writel(reg, nand->base + JZ_REG_NAND_CTRL);
	}
	if (dat != NAND_CMD_NONE)
		writeb(dat, chip->IO_ADDR_W);
}

static int jz_nand_dev_ready(struct mtd_info *mtd)
{
	struct jz_nand *nand = mtd_to_jz_nand(mtd);
	return gpio_get_value_cansleep(nand->pdata->busy_gpio);
}

static void jz_nand_hwctl(struct mtd_info *mtd, int mode)
{
	struct jz_nand *nand = mtd_to_jz_nand(mtd);
	uint32_t reg;


	writel(0, nand->base + JZ_REG_NAND_IRQ_STAT);
	reg = readl(nand->base + JZ_REG_NAND_ECC_CTRL);

	reg |= JZ_NAND_ECC_CTRL_RESET;
	reg |= JZ_NAND_ECC_CTRL_ENABLE;
	reg |= JZ_NAND_ECC_CTRL_RS;

	switch(mode) {
	case NAND_ECC_READ:
		reg &= ~JZ_NAND_ECC_CTRL_ENCODING;
		nand->is_reading = true;
		break;
	case NAND_ECC_WRITE:
		reg |= JZ_NAND_ECC_CTRL_ENCODING;
		nand->is_reading = false;
		break;
	default:
		break;
	}

	writel(reg, nand->base + JZ_REG_NAND_ECC_CTRL);
}


static int jz_nand_calculate_ecc_rs(struct mtd_info* mtd, const uint8_t* dat,
					uint8_t *ecc_code)
{
	struct jz_nand *nand = mtd_to_jz_nand(mtd);
	uint32_t reg, status;
	int i;
	static uint8_t all_ff_ecc[] = {0xcd, 0x9d, 0x90, 0x58, 0xf4, 0x8b, 0xff, 0xb7, 0x6f};

	if (nand->is_reading)
		return 0;

	do {
		status = readl(nand->base + JZ_REG_NAND_IRQ_STAT);
	} while(!(status & JZ_NAND_STATUS_ENC_FINISH));

	reg = readl(nand->base + JZ_REG_NAND_ECC_CTRL);
	reg &= ~JZ_NAND_ECC_CTRL_ENABLE;
	writel(reg, nand->base + JZ_REG_NAND_ECC_CTRL);

	for (i = 0; i < 9; ++i) {
		ecc_code[i] = readb(nand->base + JZ_REG_NAND_PAR0 + i);
	}

	/* If the written data is completly 0xff, we also want to write 0xff as
	 * ecc, otherwise we will get in trouble when doing subpage writes. */
	if (memcmp(ecc_code, all_ff_ecc, 9) == 0) {
		memset(ecc_code, 0xff, 9);
	}

	return 0;
}

/*#define printkd printk*/
#define printkd(...)

static void correct_data(uint8_t *dat, int index, int mask)
{
	int offset = index & 0x7;
	uint16_t data;
	printkd("correct: ");

	index += (index >> 3);

	data = dat[index];
	data |= dat[index+1] << 8;

	printkd("0x%x -> ", data);

	mask ^= (data >> offset) & 0x1ff;
	data &= ~(0x1ff << offset);
	data |= (mask << offset);

	printkd("0x%x\n", data);

	dat[index] = data & 0xff;
	dat[index+1] = (data >> 8) & 0xff;
}

static int jz_nand_correct_ecc_rs(struct mtd_info* mtd, uint8_t *dat,
				  uint8_t *read_ecc, uint8_t *calc_ecc)
{
	struct jz_nand *nand = mtd_to_jz_nand(mtd);
	int i, error_count, index;
	uint32_t reg, status, error;
	uint32_t t;

	t = read_ecc[0];

	if (t == 0xff) {
		for (i = 1; i < 9; ++i)
			t &= read_ecc[i];

		t &= dat[0];
		t &= dat[nand->chip.ecc.size / 2];
		t &= dat[nand->chip.ecc.size - 1];

		if (t == 0xff) {
			for (i = 1; i < nand->chip.ecc.size - 1; ++i)
				t &= dat[i];
			if (t == 0xff)
				return 0;
		}
	}

	for(i = 0; i < 9; ++i)
		writeb(read_ecc[i], nand->base + JZ_REG_NAND_PAR0 + i);

	reg = readl(nand->base + JZ_REG_NAND_ECC_CTRL);
	reg |= JZ_NAND_ECC_CTRL_PAR_READY;
	writel(reg, nand->base + JZ_REG_NAND_ECC_CTRL);

	do {
		status = readl(nand->base + JZ_REG_NAND_IRQ_STAT);
	} while (!(status & JZ_NAND_STATUS_DEC_FINISH));

	reg = readl(nand->base + JZ_REG_NAND_ECC_CTRL);
	reg &= ~JZ_NAND_ECC_CTRL_ENABLE;
	writel(reg, nand->base + JZ_REG_NAND_ECC_CTRL);

	if (status & JZ_NAND_STATUS_ERROR) {
		if (status & JZ_NAND_STATUS_UNCOR_ERROR) {
			printkd("uncorrectable ecc:");
			for(i = 0; i < 9; ++i)
				printkd(" 0x%x", read_ecc[i]);
			printkd("\n");
			printkd("uncorrectable data:");
			for(i = 0; i < 32; ++i)
				printkd(" 0x%x", dat[i]);
			printkd("\n");
			return -1;
		}

		error_count = (status & JZ_NAND_STATUS_ERR_COUNT) >> 29;

		printkd("error_count: %d %x\n", error_count, status);

		for(i = 0; i < error_count; ++i) {
			error = readl(nand->base + JZ_REG_NAND_ERR(i));
			index = ((error >> 16) & 0x1ff) - 1;
			if (index >= 0 && index < 512) {
				correct_data(dat, index, error & 0x1ff);
			}
		}

		return error_count;
	}

	return 0;
}



#ifdef CONFIG_MTD_CMDLINE_PARTS
static const char *part_probes[] = {"cmdline", NULL};
#endif

static int __devinit jz_nand_probe(struct platform_device *pdev)
{
	int ret;
	struct jz_nand *nand;
	struct nand_chip *chip;
	struct mtd_info *mtd;
	struct jz_nand_platform_data *pdata = pdev->dev.platform_data;
#ifdef CONFIG_MTD_PARTITIONS
	struct mtd_partition *partition_info;
	int num_partitions = 0;
#endif

	nand = kzalloc(sizeof(*nand), GFP_KERNEL);
	if (!nand) {
		dev_err(&pdev->dev, "Failed to allocate device structure.\n");
		return -ENOMEM;
	}

	nand->mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!nand->mem) {
		dev_err(&pdev->dev, "Failed to get platform mmio memory\n");
		ret = -ENOENT;
		goto err_free;
	}

	nand->mem = request_mem_region(nand->mem->start, resource_size(nand->mem),
					pdev->name);

	if (!nand->mem) {
		dev_err(&pdev->dev, "Failed to request mmio memory region\n");
		ret = -EBUSY;
		goto err_free;
	}

	nand->base = ioremap(nand->mem->start, resource_size(nand->mem));

	if (!nand->base) {
		dev_err(&pdev->dev, "Faild to ioremap mmio memory region\n");
		ret = -EBUSY;
		goto err_release_mem;
	}

	if (pdata && gpio_is_valid(pdata->busy_gpio)) {
		ret = gpio_request(pdata->busy_gpio, "jz nand busy line");
		if (ret) {
			dev_err(&pdev->dev, "Failed to request busy gpio %d: %d\n",
					pdata->busy_gpio, ret);
			goto err_iounmap;
		}
	}

	mtd		= &nand->mtd;
	chip		= &nand->chip;
	mtd->priv	= chip;
	mtd->owner	= THIS_MODULE;
	mtd->name	= "jz4740-nand";

	chip->ecc.hwctl		= jz_nand_hwctl;

	chip->ecc.calculate	= jz_nand_calculate_ecc_rs;
	chip->ecc.correct	= jz_nand_correct_ecc_rs;
	chip->ecc.mode		= NAND_ECC_HW_OOB_FIRST;
	chip->ecc.size		= 512;
	chip->ecc.bytes		= 9;
	if (pdata)
		chip->ecc.layout = pdata->ecc_layout;

	chip->chip_delay = 50;
	chip->cmd_ctrl = jz_nand_cmd_ctrl;

	if (pdata && gpio_is_valid(pdata->busy_gpio))
		chip->dev_ready = jz_nand_dev_ready;

	chip->IO_ADDR_R = JZ_NAND_DATA_ADDR;
	chip->IO_ADDR_W = JZ_NAND_DATA_ADDR;

	nand->pdata = pdata;
	platform_set_drvdata(pdev, nand);

	ret = nand_scan_ident(mtd, 1);
	if (ret) {
		dev_err(&pdev->dev,  "Failed to scan nand\n");
		goto err_gpio_free;
	}

	if (pdata && pdata->ident_callback) {
		pdata->ident_callback(pdev, chip, &pdata->partitions, &pdata->num_partitions);
	}

	ret = nand_scan_tail(mtd);
	if (ret) {
		dev_err(&pdev->dev,  "Failed to scan nand\n");
		goto err_gpio_free;
	}

#ifdef CONFIG_MTD_PARTITIONS
#ifdef CONFIG_MTD_CMDLINE_PARTS
	num_partitions = parse_mtd_partitions(mtd, part_probes,
						&partition_info, 0);
#endif
	if (num_partitions <= 0 && pdata) {
		num_partitions = pdata->num_partitions;
		partition_info = pdata->partitions;
	}

	if (num_partitions > 0)
		ret = add_mtd_partitions(mtd, partition_info, num_partitions);
	else
#endif
	ret = add_mtd_device(mtd);

	if (ret) {
		dev_err(&pdev->dev, "Failed to add mtd device\n");
		goto err_nand_release;
	}

	dev_info(&pdev->dev, "Successfully registered JZ4740 NAND driver\n");

	return 0;
err_nand_release:
	nand_release(&nand->mtd);
err_gpio_free:
	platform_set_drvdata(pdev, NULL);
	gpio_free(pdata->busy_gpio);
err_iounmap:
	iounmap(nand->base);
err_release_mem:
	release_mem_region(nand->mem->start, resource_size(nand->mem));
err_free:
	kfree(nand);
	return ret;
}

static void __devexit jz_nand_remove(struct platform_device *pdev)
{
	struct jz_nand *nand = platform_get_drvdata(pdev);

	nand_release(&nand->mtd);

	iounmap(nand->base);

	release_mem_region(nand->mem->start, resource_size(nand->mem));

	platform_set_drvdata(pdev, NULL);
	kfree(nand);
}

struct platform_driver jz_nand_driver = {
	.probe = jz_nand_probe,
	.remove = __devexit_p(jz_nand_probe),
	.driver = {
		.name = "jz4740-nand",
		.owner = THIS_MODULE,
	},
};

static int __init jz_nand_init(void)
{
	return platform_driver_register(&jz_nand_driver);
}
module_init(jz_nand_init);

static void __exit jz_nand_exit(void)
{
	platform_driver_unregister(&jz_nand_driver);
}
module_exit(jz_nand_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lars-Peter Clausen <lars@metafoo.de>");
MODULE_DESCRIPTION("NAND controller driver for JZ4720/JZ4740 SoC");
MODULE_ALIAS("platform:jz4740-nand");
MODULE_ALIAS("platform:jz4720-nand");
