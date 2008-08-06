/*
 *  NAND flash driver for the MikroTik RouterBoard 4xx series
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This file was based on the driver for Linux 2.6.22 published by
 *  MikroTik for their RouterBoard 4xx series devices.
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
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/gpio.h>

#include <asm/mach-ar71xx/ar71xx.h>

#define DRV_NAME        "rb4xx-nand"
#define DRV_VERSION     "0.1.10"
#define DRV_DESC        "NAND flash driver for RouterBoard 4xx series"

#define USE_FAST_READ	1
#define USE_FAST_WRITE	1
#undef RB4XX_NAND_DEBUG

#ifdef RB4XX_NAND_DEBUG
#define DBG(fmt, arg...)	printk(KERN_DEBUG DRV_NAME ": " fmt, ## arg)
#else
#define DBG(fmt, arg...)	do {} while (0)
#endif

#define RB4XX_NAND_GPIO_RDY	5
#define RB4XX_FLASH_HZ		33333334
#define RB4XX_NAND_HZ		33333334

#define SPI_CTRL_FASTEST	0x40
#define SPI_CTRL_SAFE		0x43	/* 25 MHz for AHB 200 MHz */
#define SBIT_IOC_BASE		SPI_IOC_CS1
#define SBIT_IOC_DO_SHIFT	0
#define SBIT_IOC_DO		(1u << SBIT_IOC_DO_SHIFT)
#define SBIT_IOC_DO2_SHIFT	18
#define SBIT_IOC_DO2		(1u << SBIT_IOC_DO2_SHIFT)

#define CPLD_CMD_WRITE_MULT	0x08	/* send cmd, n x send data, read data */
#define CPLD_CMD_WRITE_CFG	0x09	/* send cmd, n x send cfg */
#define CPLD_CMD_READ_MULT	0x0a	/* send cmd, send idle, n x read data */
#define CPLD_CMD_READ_FAST	0x0b	/* send cmd, 4 x idle, n x read data */

#define CFG_BIT_nCE	0x80
#define CFG_BIT_CLE	0x40
#define CFG_BIT_ALE	0x20
#define CFG_BIT_FAN	0x10
#define CFG_BIT_nLED4	0x08
#define CFG_BIT_nLED3	0x04
#define CFG_BIT_nLED2	0x02
#define CFG_BIT_nLED1	0x01

#define CFG_BIT_nLEDS \
	(CFG_BIT_nLED1 | CFG_BIT_nLED2 | CFG_BIT_nLED3 | CFG_BIT_nLED4)

struct rb4xx_nand_info {
	struct nand_chip	chip;
	struct mtd_info		mtd;
};

/*
 * We need to use the OLD Yaffs-1 OOB layout, otherwise the RB bootloader
 * will not be able to find the kernel that we load.
 */
static struct nand_ecclayout rb4xx_nand_ecclayout = {
	.eccbytes	= 6,
	.eccpos		= { 8, 9, 10, 13, 14, 15 },
	.oobavail	= 9,
	.oobfree	= { { 0, 4 }, { 6, 2 }, { 11, 2 }, { 4, 1 } }
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
		.name	= "rootfs",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= MTDPART_SIZ_FULL,
	},
};

#if USE_FAST_READ
#define SPI_NDATA_BASE	0x00800000
static unsigned spi_ctrl_fread = SPI_CTRL_SAFE;
static unsigned spi_ctrl_flash = SPI_CTRL_SAFE;
extern unsigned mips_hpt_frequency;
#endif

static inline unsigned rb4xx_spi_rreg(unsigned r)
{
	return __raw_readl((void * __iomem)(KSEG1ADDR(AR71XX_SPI_BASE) + r));
}

static inline void rb4xx_spi_wreg(unsigned r, unsigned v)
{
	__raw_writel(v, (void * __iomem)(KSEG1ADDR(AR71XX_SPI_BASE) + r));
}

static inline void do_spi_clk(int bit)
{
	unsigned bval = SBIT_IOC_BASE | (bit & 1);

	rb4xx_spi_wreg(SPI_REG_IOC, bval);
	rb4xx_spi_wreg(SPI_REG_IOC, bval | SPI_IOC_CLK);
}

static void do_spi_byte(uint8_t byte)
{
	do_spi_clk(byte >> 7);
	do_spi_clk(byte >> 6);
	do_spi_clk(byte >> 5);
	do_spi_clk(byte >> 4);
	do_spi_clk(byte >> 3);
	do_spi_clk(byte >> 2);
	do_spi_clk(byte >> 1);
	do_spi_clk(byte);

	DBG("spi_byte sent 0x%02x got 0x%x\n",
					byte, rb4xx_spi_rreg(SPI_REG_RDS));
}

#if USE_FAST_WRITE
static inline void do_spi_clk_fast(int bit1, int bit2)
{
	unsigned bval = (SBIT_IOC_BASE |
			((bit1 << SBIT_IOC_DO_SHIFT) & SBIT_IOC_DO) |
			((bit2 << SBIT_IOC_DO2_SHIFT) & SBIT_IOC_DO2));

	rb4xx_spi_wreg(SPI_REG_IOC, bval);
	rb4xx_spi_wreg(SPI_REG_IOC, bval | SPI_IOC_CLK);
}

static inline void do_spi_byte_fast(uint8_t byte)
{
	do_spi_clk_fast(byte >> 7, byte >> 6);
	do_spi_clk_fast(byte >> 5, byte >> 4);
	do_spi_clk_fast(byte >> 3, byte >> 2);
	do_spi_clk_fast(byte >> 1, byte >> 0);

	DBG("spi_byte_fast sent 0x%02x got 0x%x\n",
					byte, rb4xx_spi_rreg(SPI_REG_RDS));
}
#else
static inline void do_spi_byte_fast(uint8_t byte)
{
	do_spi_byte(byte);
}
#endif /* USE_FAST_WRITE */

static int do_spi_cmd(unsigned cmd, unsigned sendCnt, const uint8_t *sendData,
		unsigned recvCnt, uint8_t *recvData,
		const uint8_t *verifyData, int fastWrite)
{
	unsigned i;

	DBG("SPI cmd 0x%x send %u recv %u\n", cmd, sendCnt, recvCnt);

	rb4xx_spi_wreg(SPI_REG_FS, SPI_FS_GPIO);
	rb4xx_spi_wreg(SPI_REG_CTRL, SPI_CTRL_FASTEST);

	do_spi_byte(cmd);
#if 0
	if (cmd == CPLD_CMD_READ_FAST) {
		do_spi_byte(0x80);
		do_spi_byte(0);
		do_spi_byte(0);
	}
#endif
	for (i = 0; i < sendCnt; ++i) {
		if (fastWrite)
			do_spi_byte_fast(sendData[i]);
		else
			do_spi_byte(sendData[i]);
	}

	for (i = 0; i < recvCnt; ++i) {
		if (fastWrite)
			do_spi_byte_fast(0);
		else
			do_spi_byte(0);

		if (recvData) {
			recvData[i] = rb4xx_spi_rreg(SPI_REG_RDS) & 0xff;
		} else if (verifyData) {
			if (verifyData[i] != (rb4xx_spi_rreg(SPI_REG_RDS)
							 & 0xff))
				break;
		}
	}

	rb4xx_spi_wreg(SPI_REG_IOC, SBIT_IOC_BASE | SPI_IOC_CS0);
	rb4xx_spi_wreg(SPI_REG_CTRL, spi_ctrl_flash);
	rb4xx_spi_wreg(SPI_REG_FS, 0);

	return i == recvCnt;
}

static int got_write = 1;

static void rb4xx_nand_write_data(const uint8_t *byte, unsigned cnt)
{
	do_spi_cmd(CPLD_CMD_WRITE_MULT, cnt, byte, 1, NULL, NULL, 1);
	got_write = 1;
}

static void rb4xx_nand_write_byte(uint8_t byte)
{
	rb4xx_nand_write_data(&byte, 1);
}

#if USE_FAST_READ
static uint8_t *rb4xx_nand_read_getaddr(unsigned cnt)
{
	static unsigned nboffset = 0x100000;
	unsigned addr;

	if (got_write) {
		nboffset = (nboffset + 31) & ~31;
		if (nboffset >= 0x100000)	/* 1MB */
			nboffset = 0;

		got_write = 0;
		rb4xx_spi_wreg(SPI_REG_FS, SPI_FS_GPIO);
		rb4xx_spi_wreg(SPI_REG_CTRL, spi_ctrl_fread);
		rb4xx_spi_wreg(SPI_REG_FS, 0);
	}

	addr = KSEG1ADDR(AR71XX_SPI_BASE + SPI_NDATA_BASE) + nboffset;
	DBG("rb4xx_nand_read_getaddr 0x%x cnt 0x%x\n", addr, cnt);

	nboffset += cnt;
	return (uint8_t *)addr;
}

static void rb4xx_nand_read_data(uint8_t *buf, unsigned cnt)
{
	unsigned size32 = cnt & ~31;
	unsigned remain = cnt & 31;

	if (size32) {
		uint8_t *addr = rb4xx_nand_read_getaddr(size32);
		memcpy(buf, (void *)addr, size32);
	}

	if (remain) {
		do_spi_cmd(CPLD_CMD_READ_MULT, 1, buf, remain,
			   buf + size32, NULL, 0);
	}
}

static int rb4xx_nand_verify_data(const uint8_t *buf, unsigned cnt)
{
	unsigned size32 = cnt & ~31;
	unsigned remain = cnt & 31;

	if (size32) {
		uint8_t *addr = rb4xx_nand_read_getaddr(size32);
		if (memcmp(buf, (void *)addr, size32) != 0)
			return 0;
	}

	if (remain) {
		return do_spi_cmd(CPLD_CMD_READ_MULT, 1, buf, remain,
				  NULL, buf + size32, 0);
	}
	return 1;
}
#else /* USE_FAST_READ */
static void rb4xx_nand_read_data(uint8_t *buf, unsigned cnt)
{
	do_spi_cmd(CPLD_CMD_READ_MULT, 1, buf, cnt, buf, NULL, 0);
}

static int rb4xx_nand_verify_data(const uint8_t *buf, unsigned cnt)
{
	return do_spi_cmd(CPLD_CMD_READ_MULT, 1, buf, cnt, NULL, buf, 0);
}
#endif /* USE_FAST_READ */

static void rb4xx_nand_write_cfg(uint8_t byte)
{
	do_spi_cmd(CPLD_CMD_WRITE_CFG, 1, &byte, 0, NULL, NULL, 0);
	got_write = 1;
}

static int rb4xx_nand_dev_ready(struct mtd_info *mtd)
{
	return gpio_get_value(RB4XX_NAND_GPIO_RDY);
}

static void rb4xx_nand_cmd_ctrl(struct mtd_info *mtd, int cmd,
				unsigned int ctrl)
{
	if (ctrl & NAND_CTRL_CHANGE) {
		uint8_t cfg = CFG_BIT_nLEDS;

		cfg |= (ctrl & NAND_CLE) ? CFG_BIT_CLE : 0;
		cfg |= (ctrl & NAND_ALE) ? CFG_BIT_ALE : 0;
		cfg |= (ctrl & NAND_NCE) ? 0 : CFG_BIT_nCE;

		rb4xx_nand_write_cfg(cfg);
	}

	if (cmd != NAND_CMD_NONE)
		rb4xx_nand_write_byte(cmd);
}

static uint8_t rb4xx_nand_read_byte(struct mtd_info *mtd)
{
	uint8_t byte = 0;

	rb4xx_nand_read_data(&byte, 1);
	return byte;
}

static void rb4xx_nand_write_buf(struct mtd_info *mtd, const uint8_t *buf,
				int len)
{
	rb4xx_nand_write_data(buf, len);
}

static void rb4xx_nand_read_buf(struct mtd_info *mtd, uint8_t *buf,
				int len)
{
	rb4xx_nand_read_data(buf, len);
}

static int rb4xx_nand_verify_buf(struct mtd_info *mtd, const uint8_t *buf,
				int len)
{
	if (!rb4xx_nand_verify_data(buf, len))
		return -EFAULT;

	return 0;
}

static unsigned get_spi_ctrl(unsigned hz_max, const char *name)
{
	unsigned div;

	div = (ar71xx_ahb_freq - 1) / (2 * hz_max);
	/*
	 * CPU has a bug at (div == 0) - first bit read is random
	 */
	if (div == 0)
		++div;

	if (name) {
		unsigned ahb_khz = (ar71xx_ahb_freq + 500) / 1000;
		unsigned div_real = 2 * (div + 1);
		printk(KERN_INFO "%s SPI clock %u kHz (AHB %u kHz / %u)\n",
		       name,
		       ahb_khz / div_real,
		       ahb_khz, div_real);
	}

	return SPI_CTRL_FASTEST + div;
}

static int __init rb4xx_nand_probe(struct platform_device *pdev)
{
	struct rb4xx_nand_info	*info;
	int ret;

	printk(KERN_INFO DRV_DESC " version " DRV_VERSION "\n");

	ret = gpio_request(RB4XX_NAND_GPIO_RDY, "NAND RDY");
	if (ret) {
		printk(KERN_ERR "rb4xx-nand: gpio request failed\n");
		return ret;
	}

	ret = gpio_direction_input(RB4XX_NAND_GPIO_RDY);
	if (ret) {
		printk(KERN_ERR "rb4xx-nand: unable to set input mode "
					"on gpio%d\n", RB4XX_NAND_GPIO_RDY);
		goto err_free_gpio;
	}

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info) {
		printk(KERN_ERR "rb4xx-nand: no memory for private data\n");
		ret = -ENOMEM;
		goto err_free_gpio;
	}

#if USE_FAST_READ
	spi_ctrl_fread = get_spi_ctrl(RB4XX_NAND_HZ, "NAND");
#endif
	spi_ctrl_flash = get_spi_ctrl(RB4XX_FLASH_HZ, "FLASH");

	rb4xx_nand_write_cfg(CFG_BIT_nLEDS | CFG_BIT_nCE);

	info->chip.priv	= &info;
	info->mtd.priv	= &info->chip;
	info->mtd.owner	= THIS_MODULE;

	info->chip.cmd_ctrl	= rb4xx_nand_cmd_ctrl;
	info->chip.dev_ready	= rb4xx_nand_dev_ready;
	info->chip.read_byte	= rb4xx_nand_read_byte;
	info->chip.write_buf	= rb4xx_nand_write_buf;
	info->chip.read_buf	= rb4xx_nand_read_buf;
	info->chip.verify_buf	= rb4xx_nand_verify_buf;

	info->chip.chip_delay	= 25;
	info->chip.ecc.mode	= NAND_ECC_SOFT;
	info->chip.options	|= NAND_NO_AUTOINCR;

	platform_set_drvdata(pdev, info);

	ret = nand_scan_ident(&info->mtd, 1);
	if (ret) {
		ret = -ENXIO;
		goto err_free_info;
	}

	if (info->mtd.writesize == 512)
		info->chip.ecc.layout = &rb4xx_nand_ecclayout;

	ret = nand_scan_tail(&info->mtd);
	if (ret) {
		return -ENXIO;
		goto err_set_drvdata;
	}

#ifdef CONFIG_MTD_PARTITIONS
	ret = add_mtd_partitions(&info->mtd, rb4xx_nand_partitions,
				ARRAY_SIZE(rb4xx_nand_partitions));
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
err_free_gpio:
	gpio_free(RB4XX_NAND_GPIO_RDY);
	return ret;
}

static int __devexit rb4xx_nand_remove(struct platform_device *pdev)
{
	struct rb4xx_nand_info *info = platform_get_drvdata(pdev);

	nand_release(&info->mtd);
	platform_set_drvdata(pdev, NULL);
	kfree(info);

	return 0;
}

static struct platform_driver rb4xx_nand_driver = {
	.probe	= rb4xx_nand_probe,
	.remove	= __devexit_p(rb4xx_nand_remove),
	.driver	= {
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init rb4xx_nand_init(void)
{
	return platform_driver_register(&rb4xx_nand_driver);
}

static void __exit rb4xx_nand_exit(void)
{
	platform_driver_unregister(&rb4xx_nand_driver);
}

module_init(rb4xx_nand_init);
module_exit(rb4xx_nand_exit);

MODULE_DESCRIPTION(DRV_DESC);
MODULE_VERSION(DRV_VERSION);
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_AUTHOR("Imre Kaloz <kaloz@openwrt.org>");
MODULE_LICENSE("GPL v2");
