// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2012 DSPG Technologies GmbH
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/bitfield.h>
#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>

#define DVF99_SPI_CFG		0x00
#define DVF99_SPI_RATE		0x04
#define DVF99_SPI_INT_EN	0x08
#define DVF99_SPI_INT_STAT	0x0c
#define DVF99_SPI_INT_CLR	0x10
#define DVF99_SPI_INT_CAUSE	0x14
#define DVF99_SPI_TX_DATA	0x18
#define DVF99_SPI_RX_DATA	0x1c
#define DVF99_SPI_DELAY		0x20

#define DVF99_SPI_CFG_EN	BIT(31)
#define DVF99_SPI_CFG_FLUSH	(BIT(18) | BIT(17))
#define DVF99_SPI_CFG_DIRECT_CS	BIT(7)
#define DVF99_SPI_CFG_SWCS	BIT(6)
#define DVF99_SPI_CFG_MSB_FIRST	BIT(5)
#define DVF99_SPI_CFG_CS1_DIS	BIT(3)
#define DVF99_SPI_CFG_CS0_DIS	BIT(1)
#define DVF99_SPI_CFG_MODE0		BIT(0)

#define DVF99_SPI_INT_RX_WM	BIT(0)
#define DVF99_SPI_INT_RX_TO	BIT(1)
#define DVF99_SPI_INT_RX_UR	BIT(2)
#define DVF99_SPI_INT_TX_WM	BIT(8)
#define DVF99_SPI_INT_TX_OV	BIT(9)
#define DVF99_SPI_INT_DONE	BIT(10)
#define DVF99_SPI_INT_ALL	(DVF99_SPI_INT_RX_WM | DVF99_SPI_INT_RX_TO | \
				 DVF99_SPI_INT_RX_UR | DVF99_SPI_INT_TX_WM | \
				 DVF99_SPI_INT_TX_OV)
#define DVF99_SPI_INT_RX_LEVEL	GENMASK(7, 4)

#define DVF99_SPI_FIFO_SIZE	16
#define DVF99_SPI_NUM_CS	2
#define DVF99_SPI_RATE_MAX	10

struct dvf99_spi {
	struct spi_controller *ctlr;
	void __iomem *base;
	struct clk *clk;
	const u8 *tx_buf;
	u8 *rx_buf;
	unsigned int tx_left;
	unsigned int rx_left;
};

static void dvf99_spi_irq_disable(struct dvf99_spi *hw)
{
	writel(0, hw->base + DVF99_SPI_INT_EN);
	writel(DVF99_SPI_INT_ALL, hw->base + DVF99_SPI_INT_CLR);
}

static void dvf99_spi_tx_fill(struct dvf99_spi *hw)
{
	unsigned int count = 0;
	u8 data = 0;

	while (hw->tx_left && count < DVF99_SPI_FIFO_SIZE &&
	       (readl(hw->base + DVF99_SPI_INT_STAT) & DVF99_SPI_INT_TX_WM)) {
		if (hw->tx_buf)
			data = *hw->tx_buf++;
		writel(data, hw->base + DVF99_SPI_TX_DATA);
		hw->tx_left--;
		count++;
	}

	writel(DVF99_SPI_INT_RX_WM |
	       FIELD_PREP(DVF99_SPI_INT_RX_LEVEL, count - 1),
	       hw->base + DVF99_SPI_INT_EN);
}

static void dvf99_spi_rx_drain(struct dvf99_spi *hw)
{
	u8 data;

	while (hw->rx_left &&
	       (readl(hw->base + DVF99_SPI_INT_STAT) & DVF99_SPI_INT_RX_WM)) {
		data = readl(hw->base + DVF99_SPI_RX_DATA);
		if (hw->rx_buf)
			*hw->rx_buf++ = data;
		hw->rx_left--;
	}
}

static irqreturn_t dvf99_spi_irq(int irq, void *dev_id)
{
	struct dvf99_spi *hw = dev_id;
	u32 cause = readl(hw->base + DVF99_SPI_INT_CAUSE);

	if (!(cause & DVF99_SPI_INT_RX_WM))
		return IRQ_NONE;

	dvf99_spi_irq_disable(hw);
	dvf99_spi_rx_drain(hw);

	if (hw->tx_left) {
		dvf99_spi_tx_fill(hw);
		return IRQ_HANDLED;
	}

	spi_finalize_current_transfer(hw->ctlr);

	return IRQ_HANDLED;
}

static void dvf99_spi_set_cs(struct spi_device *spi, bool level)
{
	struct dvf99_spi *hw = spi_controller_get_devdata(spi->controller);
	u32 cfg = readl(hw->base + DVF99_SPI_CFG);

	cfg |= DVF99_SPI_CFG_DIRECT_CS;
	if (level)
		cfg |= DVF99_SPI_CFG_SWCS;
	else
		cfg &= ~DVF99_SPI_CFG_SWCS;

	writel(cfg, hw->base + DVF99_SPI_CFG);
}

static int dvf99_spi_setup(struct spi_device *spi)
{
	u32 mode = spi->mode & SPI_MODE_3;

	if (mode != SPI_MODE_0 && mode != SPI_MODE_3)
		return -EINVAL;

	return 0;
}

static int dvf99_spi_prepare_message(struct spi_controller *ctlr,
				     struct spi_message *msg)
{
	struct dvf99_spi *hw = spi_controller_get_devdata(ctlr);
	struct spi_device *spi = msg->spi;
	u32 cfg;

	cfg = readl(hw->base + DVF99_SPI_CFG);
	cfg &= DVF99_SPI_CFG_EN | DVF99_SPI_CFG_SWCS;
	cfg |= DVF99_SPI_CFG_DIRECT_CS | DVF99_SPI_CFG_MSB_FIRST;
	if (!(spi->mode & SPI_MODE_3))
		cfg |= DVF99_SPI_CFG_MODE0;
	if (spi_get_chipselect(spi, 0))
		cfg |= DVF99_SPI_CFG_CS0_DIS;
	else
		cfg |= DVF99_SPI_CFG_CS1_DIS;

	writel(cfg | DVF99_SPI_CFG_FLUSH, hw->base + DVF99_SPI_CFG);
	writel(cfg, hw->base + DVF99_SPI_CFG);

	return 0;
}

static unsigned int dvf99_spi_rate_code(unsigned long parent, u32 hz)
{
	unsigned int div = max_t(unsigned long, DIV_ROUND_UP(parent, hz), 2);
	unsigned int code = 0;

	while (div > 2 && code < DVF99_SPI_RATE_MAX) {
		code++;
		div = DIV_ROUND_UP(div, 2);
	}

	return code;
}

static int dvf99_spi_transfer_one(struct spi_controller *ctlr,
				  struct spi_device *spi,
				  struct spi_transfer *xfer)
{
	struct dvf99_spi *hw = spi_controller_get_devdata(ctlr);
	unsigned long parent = clk_get_rate(hw->clk);
	unsigned int code = dvf99_spi_rate_code(parent, xfer->speed_hz);
	u32 stat;
	int ret;

	writel(code, hw->base + DVF99_SPI_RATE);
	xfer->effective_speed_hz = parent >> (code + 1);

	ret = readl_poll_timeout(hw->base + DVF99_SPI_INT_STAT, stat,
				 stat & DVF99_SPI_INT_DONE, 1, 10000);
	if (ret)
		return ret;

	writel(readl(hw->base + DVF99_SPI_CFG) | DVF99_SPI_CFG_EN,
	       hw->base + DVF99_SPI_CFG);

	hw->tx_buf = xfer->tx_buf;
	hw->rx_buf = xfer->rx_buf;
	hw->tx_left = xfer->len;
	hw->rx_left = xfer->len;

	dvf99_spi_irq_disable(hw);
	dvf99_spi_tx_fill(hw);

	return 1;
}

static void dvf99_spi_handle_err(struct spi_controller *ctlr,
				 struct spi_message *msg)
{
	struct dvf99_spi *hw = spi_controller_get_devdata(ctlr);
	u32 cfg = readl(hw->base + DVF99_SPI_CFG);

	dvf99_spi_irq_disable(hw);
	writel(cfg | DVF99_SPI_CFG_FLUSH, hw->base + DVF99_SPI_CFG);
	writel(cfg, hw->base + DVF99_SPI_CFG);
}

static int dvf99_spi_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct spi_controller *ctlr;
	struct dvf99_spi *hw;
	unsigned long rate;
	int irq, ret;

	ctlr = devm_spi_alloc_host(dev, sizeof(*hw));
	if (!ctlr)
		return -ENOMEM;

	hw = spi_controller_get_devdata(ctlr);
	hw->ctlr = ctlr;

	hw->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(hw->base))
		return PTR_ERR(hw->base);

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	hw->clk = devm_clk_get_enabled(dev, NULL);
	if (IS_ERR(hw->clk))
		return dev_err_probe(dev, PTR_ERR(hw->clk), "no clock\n");

	rate = clk_get_rate(hw->clk);
	if (!rate)
		return dev_err_probe(dev, -EINVAL, "clock rate is zero\n");

	dvf99_spi_irq_disable(hw);
	writel(0, hw->base + DVF99_SPI_DELAY);

	ret = devm_request_irq(dev, irq, dvf99_spi_irq, 0, dev_name(dev), hw);
	if (ret)
		return ret;

	ctlr->dev.of_node = dev->of_node;
	ctlr->bus_num = -1;
	ctlr->num_chipselect = DVF99_SPI_NUM_CS;
	ctlr->use_gpio_descriptors = true;
	ctlr->mode_bits = SPI_MODE_3 | SPI_CS_HIGH;
	ctlr->setup = dvf99_spi_setup;
	ctlr->bits_per_word_mask = SPI_BPW_MASK(8);
	ctlr->max_speed_hz = rate / 2;
	ctlr->min_speed_hz = DIV_ROUND_UP(rate, 2 << DVF99_SPI_RATE_MAX);
	ctlr->set_cs = dvf99_spi_set_cs;
	ctlr->prepare_message = dvf99_spi_prepare_message;
	ctlr->transfer_one = dvf99_spi_transfer_one;
	ctlr->handle_err = dvf99_spi_handle_err;

	return devm_spi_register_controller(dev, ctlr);
}

static const struct of_device_id dvf99_spi_of_match[] = {
	{ .compatible = "dspg,dvf99-spi" },
	{ }
};
MODULE_DEVICE_TABLE(of, dvf99_spi_of_match);

static struct platform_driver dvf99_spi_driver = {
	.probe = dvf99_spi_probe,
	.driver = {
		.name = "dvf99-spi",
		.of_match_table = dvf99_spi_of_match,
	},
};
module_platform_driver(dvf99_spi_driver);

MODULE_DESCRIPTION("DSPG DVF99 SPI controller driver");
MODULE_LICENSE("GPL");
