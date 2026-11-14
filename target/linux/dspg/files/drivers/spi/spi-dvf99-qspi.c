// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/clk.h>
#include <linux/completion.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/sizes.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi-mem.h>

#define DVF99_QSPI_CFG			0x000
#define DVF99_QSPI_STS			0x008
#define DVF99_QSPI_ISR			0x00c
#define DVF99_QSPI_IER			0x010
#define DVF99_QSPI_ICR			0x014
#define DVF99_QSPI_FRD			0x040
#define DVF99_QSPI_CS_BASE(cs)		(0x100 * ((cs) + 1))
#define DVF99_QSPI_RDTRSTP		0x03c
#define DVF99_QSPI_RDCMD		0x040
#define DVF99_QSPI_MEMADD		0x054
#define DVF99_QSPI_TRSTP		0x300
#define DVF99_QSPI_CMD			0x304
#define DVF99_QSPI_ADD			0x308
#define DVF99_QSPI_CSTIMING		0x408

#define DVF99_QSPI_STS_IDLE		BIT(0)
#define DVF99_QSPI_STS_BUSY		BIT(31)

#define DVF99_QSPI_INT_DONE		BIT(6)

#define DVF99_QSPI_TR_OPCODE		GENMASK(1, 0)
#define DVF99_QSPI_TR_ADDR		GENMASK(3, 2)
#define DVF99_QSPI_TR_DUMMY		GENMASK(5, 4)
#define DVF99_QSPI_TR_DIN		GENMASK(9, 8)
#define DVF99_QSPI_TR_ADDRMODE		GENMASK(11, 10)
#define DVF99_QSPI_TR_DUMMY_SIZE	GENMASK(14, 12)
#define DVF99_QSPI_TR_HAS_OPCODE	BIT(19)
#define DVF99_QSPI_TR_SIZE		GENMASK(25, 20)

#define DVF99_QSPI_CMD_OPCODE		GENMASK(7, 0)
#define DVF99_QSPI_CMD_TYPE		GENMASK(11, 8)
#define DVF99_QSPI_CMD_READ		BIT(13)
#define DVF99_QSPI_CMD_CS		BIT(30)
#define DVF99_QSPI_CMD_START		BIT(31)

#define DVF99_QSPI_CFG_INIT		0x10009
#define DVF99_QSPI_CSTIMING_INIT	0x300
#define DVF99_QSPI_MEMADD_INIT		0xffff0000

#define DVF99_QSPI_TYPE_OTHER		9
#define DVF99_QSPI_FIFO_SIZE		32
#define DVF99_QSPI_MAX_WINDOW_READ	SZ_64K
#define DVF99_QSPI_NUM_CS		2
#define DVF99_QSPI_TIMEOUT_MS		1000
#define DVF99_QSPI_IDLE_TIMEOUT_US	100000

enum dvf99_qspi_lanes {
	DVF99_QSPI_LANES_SINGLE,
	DVF99_QSPI_LANES_DUAL,
};

enum dvf99_qspi_addr_mode {
	DVF99_QSPI_ADDR_NONE,
	DVF99_QSPI_ADDR_3B,
	DVF99_QSPI_ADDR_4B,
};

static const u8 dvf99_qspi_read_only_opcodes[] = {
	0x03, 0x04, 0x05, 0x06, 0x0b, 0x0c, 0x13, 0x15, 0x35, 0x3b, 0x3c,
	0x5a, 0x70, 0x9f, 0xb7, 0xbb, 0xbc, 0xe9,
};

struct dvf99_qspi {
	void __iomem *regs;
	resource_size_t window[DVF99_QSPI_NUM_CS];
	struct clk *clk;
	unsigned long rate;
	struct completion done;
	struct mutex lock;
};

static irqreturn_t dvf99_qspi_irq(int irq, void *data)
{
	struct dvf99_qspi *qspi = data;
	u32 status;

	if (!(readl(qspi->regs + DVF99_QSPI_ISR) & DVF99_QSPI_INT_DONE))
		return IRQ_NONE;

	writel(DVF99_QSPI_INT_DONE, qspi->regs + DVF99_QSPI_ICR);
	readl_poll_timeout_atomic(qspi->regs + DVF99_QSPI_ISR, status,
				  !(status & DVF99_QSPI_INT_DONE), 1, 1000);
	writel(0, qspi->regs + DVF99_QSPI_ICR);

	complete(&qspi->done);

	return IRQ_HANDLED;
}

static int dvf99_qspi_wait_idle(struct dvf99_qspi *qspi)
{
	u32 status;

	return readl_poll_timeout(qspi->regs + DVF99_QSPI_STS, status,
				  (status & DVF99_QSPI_STS_IDLE) &&
				  !(status & DVF99_QSPI_STS_BUSY),
				  10, DVF99_QSPI_IDLE_TIMEOUT_US);
}

static enum dvf99_qspi_lanes dvf99_qspi_lanes(u8 buswidth)
{
	return buswidth == 2 ? DVF99_QSPI_LANES_DUAL : DVF99_QSPI_LANES_SINGLE;
}

static enum dvf99_qspi_addr_mode dvf99_qspi_addr_mode(u8 nbytes)
{
	if (nbytes == 4)
		return DVF99_QSPI_ADDR_4B;
	if (nbytes == 3)
		return DVF99_QSPI_ADDR_3B;

	return DVF99_QSPI_ADDR_NONE;
}

static bool dvf99_qspi_opcode_is_read_only(u16 opcode)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(dvf99_qspi_read_only_opcodes); i++)
		if (opcode == dvf99_qspi_read_only_opcodes[i])
			return true;

	return false;
}

static bool dvf99_qspi_op_is_window_read(const struct spi_mem_op *op)
{
	return op->addr.nbytes && op->data.dir == SPI_MEM_DATA_IN;
}

static bool dvf99_qspi_buswidth_ok(u8 buswidth)
{
	return buswidth <= 2;
}

static bool dvf99_qspi_supports_op(struct spi_mem *mem,
				   const struct spi_mem_op *op)
{
	if (op->cmd.nbytes != 1 || op->cmd.dtr || op->addr.dtr ||
	    op->dummy.dtr || op->data.dtr)
		return false;

	if (!dvf99_qspi_opcode_is_read_only(op->cmd.opcode))
		return false;

	if (op->data.dir == SPI_MEM_DATA_OUT)
		return false;

	if (!dvf99_qspi_buswidth_ok(op->cmd.buswidth) ||
	    !dvf99_qspi_buswidth_ok(op->addr.buswidth) ||
	    !dvf99_qspi_buswidth_ok(op->dummy.buswidth) ||
	    !dvf99_qspi_buswidth_ok(op->data.buswidth))
		return false;

	if (op->addr.nbytes && op->addr.nbytes != 3 && op->addr.nbytes != 4)
		return false;

	if (dvf99_qspi_op_is_window_read(op)) {
		if (op->dummy.nbytes > FIELD_MAX(DVF99_QSPI_TR_DUMMY_SIZE))
			return false;
	} else if (op->dummy.nbytes ||
		   op->data.nbytes > DVF99_QSPI_FIFO_SIZE) {
		return false;
	}

	return spi_mem_default_supports_op(mem, op);
}

static int dvf99_qspi_adjust_op_size(struct spi_mem *mem,
				     struct spi_mem_op *op)
{
	if (dvf99_qspi_op_is_window_read(op))
		op->data.nbytes = min_t(unsigned int, op->data.nbytes,
					DVF99_QSPI_MAX_WINDOW_READ);
	else
		op->data.nbytes = min_t(unsigned int, op->data.nbytes,
					DVF99_QSPI_FIFO_SIZE);

	return 0;
}

static int dvf99_qspi_set_rate(struct dvf99_qspi *qspi, unsigned int freq)
{
	int ret;

	if (!freq || freq == qspi->rate)
		return 0;

	ret = clk_set_rate(qspi->clk, freq);
	if (ret)
		return ret;

	qspi->rate = freq;

	return 0;
}

static int dvf99_qspi_exec_cmd(struct dvf99_qspi *qspi, unsigned int cs,
			       const struct spi_mem_op *op)
{
	u8 *buf = op->data.buf.in;
	unsigned int i;
	u32 trstp, cmd;

	trstp = FIELD_PREP(DVF99_QSPI_TR_OPCODE,
			   dvf99_qspi_lanes(op->cmd.buswidth)) |
		FIELD_PREP(DVF99_QSPI_TR_ADDR,
			   dvf99_qspi_lanes(op->addr.buswidth)) |
		FIELD_PREP(DVF99_QSPI_TR_DIN,
			   dvf99_qspi_lanes(op->data.buswidth)) |
		FIELD_PREP(DVF99_QSPI_TR_ADDRMODE,
			   dvf99_qspi_addr_mode(op->addr.nbytes)) |
		FIELD_PREP(DVF99_QSPI_TR_SIZE, op->data.nbytes) |
		DVF99_QSPI_TR_HAS_OPCODE;

	cmd = FIELD_PREP(DVF99_QSPI_CMD_OPCODE, op->cmd.opcode) |
	      FIELD_PREP(DVF99_QSPI_CMD_TYPE, DVF99_QSPI_TYPE_OTHER) |
	      (cs ? DVF99_QSPI_CMD_CS : 0) | DVF99_QSPI_CMD_START;
	if (op->data.nbytes)
		cmd |= DVF99_QSPI_CMD_READ;

	if (op->addr.nbytes)
		writel(op->addr.val, qspi->regs + DVF99_QSPI_ADD);

	reinit_completion(&qspi->done);
	writel(trstp, qspi->regs + DVF99_QSPI_TRSTP);
	writel(cmd, qspi->regs + DVF99_QSPI_CMD);

	if (!wait_for_completion_timeout(&qspi->done,
					 msecs_to_jiffies(DVF99_QSPI_TIMEOUT_MS)))
		return -ETIMEDOUT;

	for (i = 0; i < op->data.nbytes; i++)
		buf[i] = readb(qspi->regs + DVF99_QSPI_FRD +
			       DVF99_QSPI_FIFO_SIZE - 1 - i);

	return 0;
}

static int dvf99_qspi_exec_window_read(struct dvf99_qspi *qspi,
				       unsigned int cs,
				       const struct spi_mem_op *op)
{
	void __iomem *base = qspi->regs + DVF99_QSPI_CS_BASE(cs);
	void __iomem *window;
	u32 trstp;

	trstp = FIELD_PREP(DVF99_QSPI_TR_OPCODE,
			   dvf99_qspi_lanes(op->cmd.buswidth)) |
		FIELD_PREP(DVF99_QSPI_TR_ADDR,
			   dvf99_qspi_lanes(op->addr.buswidth)) |
		FIELD_PREP(DVF99_QSPI_TR_DUMMY,
			   dvf99_qspi_lanes(op->dummy.buswidth)) |
		FIELD_PREP(DVF99_QSPI_TR_DIN,
			   dvf99_qspi_lanes(op->data.buswidth)) |
		FIELD_PREP(DVF99_QSPI_TR_ADDRMODE,
			   dvf99_qspi_addr_mode(op->addr.nbytes)) |
		FIELD_PREP(DVF99_QSPI_TR_DUMMY_SIZE, op->dummy.nbytes) |
		DVF99_QSPI_TR_HAS_OPCODE;

	writel(trstp, base + DVF99_QSPI_RDTRSTP);
	writel(op->cmd.opcode, base + DVF99_QSPI_RDCMD);

	window = ioremap(qspi->window[cs] + op->addr.val, op->data.nbytes);
	if (!window)
		return -ENOMEM;

	memcpy_fromio(op->data.buf.in, window, op->data.nbytes);
	iounmap(window);

	return 0;
}

static int dvf99_qspi_exec_op(struct spi_mem *mem, const struct spi_mem_op *op)
{
	struct dvf99_qspi *qspi = spi_controller_get_devdata(mem->spi->controller);
	unsigned int cs = spi_get_chipselect(mem->spi, 0);
	int ret;

	if (!dvf99_qspi_supports_op(mem, op))
		return -EOPNOTSUPP;

	mutex_lock(&qspi->lock);

	ret = dvf99_qspi_set_rate(qspi, op->max_freq);
	if (ret)
		goto out;

	ret = dvf99_qspi_wait_idle(qspi);
	if (ret)
		goto out;

	if (dvf99_qspi_op_is_window_read(op))
		ret = dvf99_qspi_exec_window_read(qspi, cs, op);
	else
		ret = dvf99_qspi_exec_cmd(qspi, cs, op);

out:
	mutex_unlock(&qspi->lock);

	return ret;
}

static const struct spi_controller_mem_ops dvf99_qspi_mem_ops = {
	.adjust_op_size = dvf99_qspi_adjust_op_size,
	.supports_op = dvf99_qspi_supports_op,
	.exec_op = dvf99_qspi_exec_op,
};

static int dvf99_qspi_window_get(struct platform_device *pdev,
				 struct dvf99_qspi *qspi, unsigned int cs)
{
	struct resource *res;

	res = platform_get_resource(pdev, IORESOURCE_MEM, cs + 1);
	if (!res)
		return dev_err_probe(&pdev->dev, -EINVAL,
				     "missing window for CS%u\n", cs);

	if (!devm_request_mem_region(&pdev->dev, res->start,
				     resource_size(res), dev_name(&pdev->dev)))
		return -EBUSY;

	qspi->window[cs] = res->start;

	return 0;
}

static int dvf99_qspi_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct spi_controller *ctlr;
	struct reset_control *rst;
	struct dvf99_qspi *qspi;
	unsigned int cs;
	int irq, ret;

	ctlr = devm_spi_alloc_host(dev, sizeof(*qspi));
	if (!ctlr)
		return -ENOMEM;

	qspi = spi_controller_get_devdata(ctlr);
	init_completion(&qspi->done);

	ret = devm_mutex_init(dev, &qspi->lock);
	if (ret)
		return ret;

	qspi->regs = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(qspi->regs))
		return PTR_ERR(qspi->regs);

	for (cs = 0; cs < DVF99_QSPI_NUM_CS; cs++) {
		ret = dvf99_qspi_window_get(pdev, qspi, cs);
		if (ret)
			return ret;
	}

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	qspi->clk = devm_clk_get_enabled(dev, NULL);
	if (IS_ERR(qspi->clk))
		return dev_err_probe(dev, PTR_ERR(qspi->clk),
				     "failed to get clock\n");

	rst = devm_reset_control_get_optional_exclusive_deasserted(dev, NULL);
	if (IS_ERR(rst))
		return dev_err_probe(dev, PTR_ERR(rst), "failed to get reset\n");

	writel(DVF99_QSPI_CSTIMING_INIT, qspi->regs + DVF99_QSPI_CSTIMING);
	writel(DVF99_QSPI_CFG_INIT, qspi->regs + DVF99_QSPI_CFG);
	for (cs = 0; cs < DVF99_QSPI_NUM_CS; cs++)
		writel(DVF99_QSPI_MEMADD_INIT,
		       qspi->regs + DVF99_QSPI_CS_BASE(cs) + DVF99_QSPI_MEMADD);
	writel(DVF99_QSPI_INT_DONE, qspi->regs + DVF99_QSPI_IER);

	ret = devm_request_irq(dev, irq, dvf99_qspi_irq, 0, dev_name(dev), qspi);
	if (ret)
		return ret;

	ctlr->dev.of_node = dev->of_node;
	ctlr->bus_num = -1;
	ctlr->num_chipselect = DVF99_QSPI_NUM_CS;
	ctlr->mode_bits = SPI_CPOL | SPI_CPHA | SPI_RX_DUAL;
	ctlr->mem_ops = &dvf99_qspi_mem_ops;

	return devm_spi_register_controller(dev, ctlr);
}

static const struct of_device_id dvf99_qspi_of_match[] = {
	{ .compatible = "dspg,dvf99-qspi" },
	{ }
};
MODULE_DEVICE_TABLE(of, dvf99_qspi_of_match);

static struct platform_driver dvf99_qspi_driver = {
	.probe = dvf99_qspi_probe,
	.driver = {
		.name = "dvf99-qspi",
		.of_match_table = dvf99_qspi_of_match,
	},
};
module_platform_driver(dvf99_qspi_driver);

MODULE_AUTHOR("John Crispin <john@phrozen.org>");
MODULE_DESCRIPTION("DSPG DVF99 QSPI controller driver");
MODULE_LICENSE("GPL");
