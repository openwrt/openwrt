/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 */

#include <linux/mtd/physmap.h>
#include <linux/mtd/nand.h>
#include <linux/platform_device.h>
#include <linux/io.h>

#include <lantiq_soc.h>
#include <lantiq_irq.h>
#include <lantiq_platform.h>

#include "devices.h"

/* nand registers */
#define LTQ_EBU_NAND_WAIT	0xB4
#define LTQ_EBU_NAND_ECC0	0xB8
#define LTQ_EBU_NAND_ECC_AC	0xBC
#define LTQ_EBU_NAND_CON	0xB0
#define LTQ_EBU_ADDSEL1		0x24

/* gpio definitions */
#define PIN_ALE			13
#define PIN_CLE			24
#define PIN_CS1			23
#define PIN_RDY			48  /* NFLASH_READY */
#define PIN_RD			49  /* NFLASH_READ_N */

#define NAND_CMD_ALE		(1 << 2)
#define NAND_CMD_CLE		(1 << 3)
#define NAND_CMD_CS		(1 << 4)
#define NAND_WRITE_CMD_RESET	0xff
#define NAND_WRITE_CMD		(NAND_CMD_CS | NAND_CMD_CLE)
#define NAND_WRITE_ADDR		(NAND_CMD_CS | NAND_CMD_ALE)
#define NAND_WRITE_DATA		(NAND_CMD_CS)
#define NAND_READ_DATA		(NAND_CMD_CS)
#define NAND_WAIT_WR_C		(1 << 3)
#define NAND_WAIT_RD		(0x1)

#define ADDSEL1_MASK(x)		(x << 4)
#define ADDSEL1_REGEN		1
#define BUSCON1_SETUP		(1 << 22)
#define BUSCON1_BCGEN_RES	(0x3 << 12)
#define BUSCON1_WAITWRC2	(2 << 8)
#define BUSCON1_WAITRDC2	(2 << 6)
#define BUSCON1_HOLDC1		(1 << 4)
#define BUSCON1_RECOVC1		(1 << 2)
#define BUSCON1_CMULT4		1
#define NAND_CON_NANDM		1
#define NAND_CON_CSMUX		(1 << 1)
#define NAND_CON_CS_P		(1 << 4)
#define NAND_CON_SE_P		(1 << 5)
#define NAND_CON_WP_P		(1 << 6)
#define NAND_CON_PRE_P		(1 << 7)
#define NAND_CON_IN_CS0		0
#define NAND_CON_OUT_CS0	0
#define NAND_CON_IN_CS1		(1 << 8)
#define NAND_CON_OUT_CS1	(1 << 10)
#define NAND_CON_CE		(1 << 20)

#define NAND_BASE_ADDRESS	(KSEG1 | 0x14000000)

static const char *part_probes[] = { "cmdlinepart", NULL };

static void xway_select_chip(struct mtd_info *mtd, int chip)
{
	switch (chip) {
	case -1:
		ltq_ebu_w32_mask(NAND_CON_CE, 0, LTQ_EBU_NAND_CON);
		ltq_ebu_w32_mask(NAND_CON_NANDM, 0, LTQ_EBU_NAND_CON);
		break;
	case 0:
		ltq_ebu_w32_mask(0, NAND_CON_NANDM, LTQ_EBU_NAND_CON);
		ltq_ebu_w32_mask(0, NAND_CON_CE, LTQ_EBU_NAND_CON);
		/* reset the nand chip */
		while ((ltq_ebu_r32(LTQ_EBU_NAND_WAIT) & NAND_WAIT_WR_C) == 0)
			;
		ltq_w32(NAND_WRITE_CMD_RESET,
			((u32 *) (NAND_BASE_ADDRESS | NAND_WRITE_CMD)));
		break;
	default:
		BUG();
	}
}

static void xway_cmd_ctrl(struct mtd_info *mtd, int data, unsigned int ctrl)
{
	struct nand_chip *this = mtd->priv;

	if (ctrl & NAND_CTRL_CHANGE) {
		if (ctrl & NAND_CLE)
			this->IO_ADDR_W = (void __iomem *)
					(NAND_BASE_ADDRESS | NAND_WRITE_CMD);
		else if (ctrl & NAND_ALE)
			this->IO_ADDR_W = (void __iomem *)
					(NAND_BASE_ADDRESS | NAND_WRITE_ADDR);
	}

	if (data != NAND_CMD_NONE) {
		*(volatile u8*) ((u32) this->IO_ADDR_W) = data;
		while ((ltq_ebu_r32(LTQ_EBU_NAND_WAIT) & NAND_WAIT_WR_C) == 0)
			;
	}
}

static int xway_dev_ready(struct mtd_info *mtd)
{
	return ltq_ebu_r32(LTQ_EBU_NAND_WAIT) & NAND_WAIT_RD;
}

void nand_write(unsigned int addr, unsigned int val)
{
	ltq_w32(val, ((u32 *) (NAND_BASE_ADDRESS | addr)));
	while ((ltq_ebu_r32(LTQ_EBU_NAND_WAIT) & NAND_WAIT_WR_C) == 0)
		;
}

unsigned char xway_read_byte(struct mtd_info *mtd)
{
	return ltq_r8((void __iomem *)(NAND_BASE_ADDRESS | (NAND_READ_DATA)));
}

static void xway_read_buf(struct mtd_info *mtd, uint8_t *buf, int len)
{
	int i;

	for (i = 0; i < len; i++)
	{
		unsigned char res8 =  ltq_r8((void __iomem *)(NAND_BASE_ADDRESS | (NAND_READ_DATA)));
		buf[i] = res8;
	}
}

static void xway_write_buf(struct mtd_info *mtd, const uint8_t *buf, int len)
{
	int i;

	for (i = 0; i < len; i++)
	{
		ltq_w8(buf[i], ((u32*)(NAND_BASE_ADDRESS | (NAND_WRITE_DATA))));
		while((ltq_ebu_r32(LTQ_EBU_NAND_WAIT) & NAND_WAIT_WR_C) == 0);
	}
}

int xway_probe(struct platform_device *pdev)
{
	/* might need this later ?
	ltq_gpio_request(PIN_CS1, 2, 1, "NAND_CS1");
	*/
	ltq_gpio_request(&pdev->dev, PIN_CLE, 2, 1, "NAND_CLE");
	ltq_gpio_request(&pdev->dev, PIN_ALE, 2, 1, "NAND_ALE");
	if (ltq_is_ar9() || ltq_is_vr9()) {
		ltq_gpio_request(&pdev->dev, PIN_RDY, 2, 0, "NAND_BSY");
		ltq_gpio_request(&pdev->dev, PIN_RD, 2, 1, "NAND_RD");
	}

	ltq_ebu_w32((NAND_BASE_ADDRESS & 0x1fffff00)
		| ADDSEL1_MASK(3) | ADDSEL1_REGEN, LTQ_EBU_ADDSEL1);

	ltq_ebu_w32(BUSCON1_SETUP | BUSCON1_BCGEN_RES | BUSCON1_WAITWRC2
		| BUSCON1_WAITRDC2 | BUSCON1_HOLDC1 | BUSCON1_RECOVC1
		| BUSCON1_CMULT4, LTQ_EBU_BUSCON1);

	ltq_ebu_w32(NAND_CON_NANDM | NAND_CON_CSMUX | NAND_CON_CS_P
		| NAND_CON_SE_P | NAND_CON_WP_P | NAND_CON_PRE_P
		| NAND_CON_IN_CS0 | NAND_CON_OUT_CS0, LTQ_EBU_NAND_CON);

	ltq_w32(NAND_WRITE_CMD_RESET,
		((u32 *) (NAND_BASE_ADDRESS | NAND_WRITE_CMD)));
	while ((ltq_ebu_r32(LTQ_EBU_NAND_WAIT) & NAND_WAIT_WR_C) == 0)
		;

	return 0;
}

static struct platform_nand_data falcon_flash_nand_data = {
	.chip = {
		.nr_chips		= 1,
		.chip_delay		= 30,
		.part_probe_types	= part_probes,
	},
	.ctrl = {
		.probe		= xway_probe,
		.cmd_ctrl	= xway_cmd_ctrl,
		.dev_ready	= xway_dev_ready,
		.select_chip	= xway_select_chip,
		.read_byte	= xway_read_byte,
		.read_buf	= xway_read_buf,
		.write_buf	= xway_write_buf,
	}
};

static struct resource ltq_nand_res =
	MEM_RES("nand", 0x14000000, 0x7ffffff);

static struct platform_device ltq_flash_nand = {
	.name		= "gen_nand",
	.id		= -1,
	.num_resources	= 1,
	.resource	= &ltq_nand_res,
	.dev		= {
		.platform_data = &falcon_flash_nand_data,
	},
};

void __init xway_register_nand(struct mtd_partition *parts, int count)
{
	falcon_flash_nand_data.chip.partitions = parts;
	falcon_flash_nand_data.chip.nr_partitions = count;
	platform_device_register(&ltq_flash_nand);
}
