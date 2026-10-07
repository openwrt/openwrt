// SPDX-License-Identifier: GPL-2.0-only
/*
 * ZTE zx279128s SPI flash controller (SPIFC)
 *
 * Copyright (C) 2026 Navid Ghahremani <ghahramani.navid@gmail.com>
 *
 * The controller runs one flash operation at a time: the driver writes the
 * opcode, the address, a description of the operation's phases and the data
 * length, and starts it. Data passes through a 32-bit FIFO. The command and
 * address phases always use one line; the data phase can use one, two or
 * four. The register layout comes from the vendor boot loader's table of
 * flash operations.
 */

#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/minmax.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi-mem.h>
#include <linux/string.h>

#define SPIFC_TRIGGER		0x04
#define  SPIFC_TRIGGER_START	BIT(0)
#define SPIFC_ACK		0x08
#define  SPIFC_ACK_DONE		BIT(0)	/* written while _BUSY is clear */
#define  SPIFC_ACK_BUSY		BIT(1)
#define SPIFC_XFER_CFG		0x0c
#define  SPIFC_XFER_CFG_BITS	0x1c440	/* set before every operation */
#define SPIFC_FMT0		0x10	/* the phases of the operation */
#define  SPIFC_FMT0_DATA_OUT	BIT(0)
#define  SPIFC_FMT0_DATA_IN	BIT(1)
#define  SPIFC_FMT0_DUMMY	BIT(2)
#define  SPIFC_FMT0_ADDR	BIT(4)
#define SPIFC_FMT1		0x14	/* and their sizes */
#define  SPIFC_FMT1_DATA_LINES	GENMASK(2, 0)
#define   SPIFC_LINES_1		0
#define   SPIFC_LINES_2		4
#define   SPIFC_LINES_4		5
#define  SPIFC_FMT1_ADDR_BYTES	GENMASK(6, 5)	/* bytes - 1 */
#define  SPIFC_FMT1_DUMMY_BYTES	GENMASK(15, 12)
#define SPIFC_LEN		0x18	/* data bytes - 1 */
#define SPIFC_ADDR		0x1c
#define SPIFC_OPCODE		0x20
#define SPIFC_TIMING		0x24
#define  SPIFC_TIMING_SAMPLE	GENMASK(17, 16)
#define SPIFC_STATUS		0x2c
#define  SPIFC_STATUS_DONE	BIT(0)
#define SPIFC_BURST		0x30
#define  SPIFC_BURST_VAL	0x3f
#define SPIFC_FIFO_STATUS	0x34
#define  SPIFC_FIFO_RX_AVAIL	GENMASK(12, 8)
#define  SPIFC_FIFO_TX_FREE	GENMASK(20, 16)
#define SPIFC_FIFO_DATA		0x38

#define SPIFC_MAX_DATA		4096
#define SPIFC_TIMEOUT_US	20000

struct zx_spifc {
	void __iomem *base;
	struct clk *wclk;
	unsigned long speed_hz;
};

static void zx_spifc_ack(struct zx_spifc *spifc)
{
	u32 val = readl(spifc->base + SPIFC_ACK);

	if (!(val & SPIFC_ACK_BUSY))
		writel(val | SPIFC_ACK_DONE, spifc->base + SPIFC_ACK);
}

static void zx_spifc_start(struct zx_spifc *spifc)
{
	writel(readl(spifc->base + SPIFC_TRIGGER) | SPIFC_TRIGGER_START,
	       spifc->base + SPIFC_TRIGGER);
}

static int zx_spifc_wait_fifo(struct zx_spifc *spifc, u32 mask)
{
	u32 val;

	return readl_poll_timeout_atomic(spifc->base + SPIFC_FIFO_STATUS, val,
					 val & mask, 1, SPIFC_TIMEOUT_US);
}

static int zx_spifc_read_fifo(struct zx_spifc *spifc, u8 *buf,
			      unsigned int len)
{
	unsigned int i;
	u32 word;
	int ret;

	for (i = 0; i < len; i += 4) {
		ret = zx_spifc_wait_fifo(spifc, SPIFC_FIFO_RX_AVAIL);
		if (ret)
			return ret;
		word = readl(spifc->base + SPIFC_FIFO_DATA);
		memcpy(buf + i, &word, min(4U, len - i));
	}

	return 0;
}

/* Preload the FIFO, start the operation, then keep the FIFO filled */
static int zx_spifc_write_fifo(struct zx_spifc *spifc, const u8 *buf,
			       unsigned int len)
{
	unsigned int i = 0;
	bool started = false;
	u32 word;
	int ret;

	while (i < len) {
		if (!(readl(spifc->base + SPIFC_FIFO_STATUS) &
		      SPIFC_FIFO_TX_FREE)) {
			if (!started) {
				zx_spifc_start(spifc);
				started = true;
			}
			ret = zx_spifc_wait_fifo(spifc, SPIFC_FIFO_TX_FREE);
			if (ret)
				return ret;
		}
		word = 0;
		memcpy(&word, buf + i, min(4U, len - i));
		writel(word, spifc->base + SPIFC_FIFO_DATA);
		i += 4;
	}

	if (!started)
		zx_spifc_start(spifc);

	return 0;
}

static u32 zx_spifc_lines(u8 buswidth)
{
	switch (buswidth) {
	case 4:
		return SPIFC_LINES_4;
	case 2:
		return SPIFC_LINES_2;
	default:
		return SPIFC_LINES_1;
	}
}

/* The CRPM divider must never round above the flash's frequency limit. */
static int zx_spifc_set_speed(struct zx_spifc *spifc, unsigned int max_hz)
{
	unsigned long rate;
	long rounded;
	int ret;

	if (!max_hz)
		return -EINVAL;
	if (spifc->speed_hz == max_hz)
		return 0;

	rounded = clk_round_rate(spifc->wclk, max_hz);
	if (rounded < 0)
		return rounded;
	if (!rounded || rounded > max_hz)
		return -EINVAL;

	ret = clk_set_rate(spifc->wclk, rounded);
	if (ret)
		return ret;
	rate = clk_get_rate(spifc->wclk);
	if (!rate || rate > max_hz)
		return -EINVAL;
	spifc->speed_hz = rate;

	return 0;
}

static int zx_spifc_setup(struct spi_device *spi)
{
	struct zx_spifc *spifc = spi_controller_get_devdata(spi->controller);

	return zx_spifc_set_speed(spifc, spi->max_speed_hz);
}

static int zx_spifc_exec_op(struct spi_mem *mem, const struct spi_mem_op *op)
{
	struct zx_spifc *spifc = spi_controller_get_devdata(mem->spi->controller);
	void __iomem *base = spifc->base;
	u32 fmt0 = 0, fmt1 = 0, val;
	int ret;

	ret = zx_spifc_set_speed(spifc, op->max_freq ?: mem->spi->max_speed_hz);
	if (ret)
		return ret;

	if (op->addr.nbytes) {
		fmt0 |= SPIFC_FMT0_ADDR;
		fmt1 |= FIELD_PREP(SPIFC_FMT1_ADDR_BYTES, op->addr.nbytes - 1);
	}
	if (op->dummy.nbytes) {
		fmt0 |= SPIFC_FMT0_DUMMY;
		fmt1 |= FIELD_PREP(SPIFC_FMT1_DUMMY_BYTES, op->dummy.nbytes);
	}
	if (op->data.nbytes) {
		fmt0 |= op->data.dir == SPI_MEM_DATA_IN ? SPIFC_FMT0_DATA_IN :
							  SPIFC_FMT0_DATA_OUT;
		fmt1 |= FIELD_PREP(SPIFC_FMT1_DATA_LINES,
				   zx_spifc_lines(op->data.buswidth));
	}

	zx_spifc_ack(spifc);
	writel(op->cmd.opcode, base + SPIFC_OPCODE);
	writel(fmt0, base + SPIFC_FMT0);
	writel(fmt1, base + SPIFC_FMT1);
	writel(op->data.nbytes ? op->data.nbytes - 1 : 0, base + SPIFC_LEN);
	writel(op->addr.nbytes ? op->addr.val : 0, base + SPIFC_ADDR);
	writel(readl(base + SPIFC_XFER_CFG) | SPIFC_XFER_CFG_BITS,
	       base + SPIFC_XFER_CFG);
	writel(SPIFC_BURST_VAL, base + SPIFC_BURST);

	if (op->data.nbytes && op->data.dir == SPI_MEM_DATA_OUT) {
		ret = zx_spifc_write_fifo(spifc, op->data.buf.out,
					  op->data.nbytes);
	} else {
		zx_spifc_start(spifc);
		ret = 0;
		if (op->data.nbytes)
			ret = zx_spifc_read_fifo(spifc, op->data.buf.in,
						 op->data.nbytes);
	}

	if (!ret)
		ret = readl_poll_timeout_atomic(base + SPIFC_STATUS, val,
						val & SPIFC_STATUS_DONE, 1,
						SPIFC_TIMEOUT_US);
	zx_spifc_ack(spifc);

	return ret;
}

static bool zx_spifc_supports_op(struct spi_mem *mem,
				 const struct spi_mem_op *op)
{
	if (!spi_mem_default_supports_op(mem, op))
		return false;

	/* Command and address are always on one line */
	if (op->cmd.nbytes != 1 || op->cmd.buswidth != 1)
		return false;
	if (op->addr.nbytes > 3 || (op->addr.nbytes && op->addr.buswidth != 1))
		return false;
	if (op->dummy.nbytes > 15 ||
	    (op->dummy.nbytes && op->dummy.buswidth != 1))
		return false;

	/* The vendor code programs on one or four lines, never two */
	if (op->data.nbytes && op->data.dir == SPI_MEM_DATA_OUT &&
	    op->data.buswidth == 2)
		return false;

	return true;
}

static int zx_spifc_adjust_op_size(struct spi_mem *mem, struct spi_mem_op *op)
{
	op->data.nbytes = min(op->data.nbytes, SPIFC_MAX_DATA);

	return 0;
}

static const struct spi_controller_mem_caps zx_spifc_mem_caps = {
	.per_op_freq = true,
};

static const struct spi_controller_mem_ops zx_spifc_mem_ops = {
	.adjust_op_size = zx_spifc_adjust_op_size,
	.supports_op = zx_spifc_supports_op,
	.exec_op = zx_spifc_exec_op,
};

static int zx_spifc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct spi_controller *ctlr;
	struct zx_spifc *spifc;
	struct clk *clk;
	long min_hz;
	u32 val;

	ctlr = devm_spi_alloc_host(dev, sizeof(*spifc));
	if (!ctlr)
		return -ENOMEM;
	spifc = spi_controller_get_devdata(ctlr);

	spifc->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(spifc->base))
		return PTR_ERR(spifc->base);

	clk = devm_clk_get_enabled(dev, "pclk");
	if (IS_ERR(clk))
		return dev_err_probe(dev, PTR_ERR(clk),
				     "failed to get the register clock\n");
	spifc->wclk = devm_clk_get_enabled(dev, "wclk");
	if (IS_ERR(spifc->wclk))
		return dev_err_probe(dev, PTR_ERR(spifc->wclk),
				     "failed to get the work clock\n");

	min_hz = clk_round_rate(spifc->wclk, 1);
	if (min_hz <= 0)
		return dev_err_probe(dev, min_hz ?: -EINVAL,
				     "invalid minimum work clock rate\n");

	/* The data sampling point, as the boot loader sets it */
	val = readl(spifc->base + SPIFC_TIMING);
	val &= ~SPIFC_TIMING_SAMPLE;
	val |= FIELD_PREP(SPIFC_TIMING_SAMPLE, 1);
	writel(val, spifc->base + SPIFC_TIMING);

	ctlr->mode_bits = SPI_RX_DUAL | SPI_RX_QUAD | SPI_TX_QUAD;
	ctlr->mem_ops = &zx_spifc_mem_ops;
	ctlr->mem_caps = &zx_spifc_mem_caps;
	ctlr->setup = zx_spifc_setup;
	ctlr->min_speed_hz = min_hz;
	/* Faster rates have not been validated on this controller. */
	ctlr->max_speed_hz = 50000000;
	ctlr->num_chipselect = 1;
	ctlr->dev.of_node = dev->of_node;

	return devm_spi_register_controller(dev, ctlr);
}

static const struct of_device_id zx_spifc_of_match[] = {
	{ .compatible = "zte,zx279128s-spifc" },
	{ }
};
MODULE_DEVICE_TABLE(of, zx_spifc_of_match);

static struct platform_driver zx_spifc_driver = {
	.probe = zx_spifc_probe,
	.driver = {
		.name = "zx279128s-spifc",
		.of_match_table = zx_spifc_of_match,
	},
};
module_platform_driver(zx_spifc_driver);

MODULE_AUTHOR("Navid Ghahremani <ghahramani.navid@gmail.com>");
MODULE_DESCRIPTION("ZTE zx279128s SPI flash controller driver");
MODULE_LICENSE("GPL");
