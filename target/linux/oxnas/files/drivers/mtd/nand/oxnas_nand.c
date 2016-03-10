/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  based on xway_nand.c
 *  Copyright © 2012 John Crispin <blogic@openwrt.org>
 *  and oxnas_nand.c "NAND glue for Oxnas platforms"
 *  written by Ma Haijun <mahaijuns@gmail.com>
 */

#include <linux/mtd/nand.h>
#include <linux/of_gpio.h>
#include <linux/of_platform.h>
#include <linux/clk.h>
#include <linux/reset.h>

/* nand commands */
#define NAND_CMD_ALE		BIT(18)
#define NAND_CMD_CLE		BIT(19)
#define NAND_CMD_CS		0
#define NAND_CMD_RESET		0xff
#define NAND_CMD		(NAND_CMD_CS | NAND_CMD_CLE)
#define NAND_ADDR		(NAND_CMD_CS | NAND_CMD_ALE)
#define NAND_DATA		(NAND_CMD_CS)

static void oxnas_cmd_ctrl(struct mtd_info *mtd, int cmd, unsigned int ctrl)
{
	struct nand_chip *this = mtd->priv;
	unsigned long nandaddr = (unsigned long) this->IO_ADDR_W;

	if (ctrl & NAND_CTRL_CHANGE) {
		nandaddr &= ~(NAND_CMD | NAND_ADDR);
		if (ctrl & NAND_CLE)
			nandaddr |= NAND_CMD;
		else if (ctrl & NAND_ALE)
			nandaddr |= NAND_ADDR;
		this->IO_ADDR_W = (void __iomem *) nandaddr;
	}

	if (cmd != NAND_CMD_NONE)
		writeb(cmd, (void __iomem *) nandaddr);
}

static int oxnas_nand_probe(struct platform_device *pdev)
{
	/* enable clock and release static block reset */
	struct clk *clk = of_clk_get(pdev->dev.of_node, 0);

	if (IS_ERR(clk))
		return PTR_ERR(clk);

	clk_prepare_enable(clk);
	device_reset(&pdev->dev);

	return 0;
}

static struct platform_nand_data oxnas_nand_data = {
	.chip = {
		.nr_chips		= 1,
		.chip_delay		= 30,
	},
	.ctrl = {
		.probe		= oxnas_nand_probe,
		.cmd_ctrl	= oxnas_cmd_ctrl,
	}
};

/*
 * Try to find the node inside the DT. If it is available attach out
 * platform_nand_data
 */
static int __init oxnas_register_nand(void)
{
	struct device_node *node;
	struct platform_device *pdev;

	node = of_find_compatible_node(NULL, NULL, "plxtech,nand-nas782x");
	if (!node)
		return -ENOENT;
	pdev = of_find_device_by_node(node);
	if (!pdev)
		return -EINVAL;
	pdev->dev.platform_data = &oxnas_nand_data;
	of_node_put(node);
	return 0;
}

subsys_initcall(oxnas_register_nand);
