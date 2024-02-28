// SPDX-License-Identifier: GPL-2.0+

#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/sh_clk.h>
#include <linux/err.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/sizes.h>

#include <linux/spi/spi-mem.h>
#include <linux/spi/spi.h>

/*
 * siflower SSP fifo level
 */
#define SF_SSP_FIFO_LEVEL		0x100
/*
 * siflower SSP register
 */
#define SSP_CR0				0x000
#define SSP_CR1				0x004
#define SSP_DR				0x008
#define SSP_SR				0x00C
#define SSP_CPSR			0x010
#define SSP_IMSC			0x014
#define SSP_RIS				0x018
#define SSP_MIS				0x01C
#define SSP_ICR				0x020
#define SSP_DMACR			0x024
#define SSP_FIFO_LEVEL		0x028
#define SSP_EXSPI_CMD0		0x02C
#define SSP_EXSPI_CMD1		0x030
#define SSP_EXSPI_CMD2		0x034
/* SSP Control Register 0  - SSP_CR0 */
#define SSP_CR0_EXSPI_FRAME (0x3 << 4)
#define SSP_CR0_SPO			(0x1 << 6)
#define SSP_CR0_SPH			(0x1 << 7)
#define SSP_CR0_BIT_MODE(x) ((x)-1)
#define SSP_SCR_MIN			(0x00)
#define SSP_SCR_MAX			(0xFF)
#define SSP_SCR_SHFT		8
#define DFLT_CLKRATE		2
/* SSP Control Register 1  - SSP_CR1 */
#define SSP_CR1_MASK_SSE	(0x1 << 1)
#define SSP_CPSR_MIN		(0x02)
#define SSP_CPSR_MAX		(0xFE)
#define DFLT_PRESCALE		(0x40)
/* SSP Status Register - SSP_SR */
#define SSP_SR_MASK_TFE		(0x1 << 0) /* Transmit FIFO empty */
#define SSP_SR_MASK_TNF		(0x1 << 1) /* Transmit FIFO not full */
#define SSP_SR_MASK_RNE		(0x1 << 2) /* Receive FIFO not empty */
#define SSP_SR_MASK_RFF		(0x1 << 3) /* Receive FIFO full */
#define SSP_SR_MASK_BSY		(0x1 << 4) /* Busy Flag */

/* SSP FIFO Threshold Register - SSP_FIFO_LEVEL */
#define SSP_FIFO_LEVEL_RX	GENMASK(14, 8) /* Receive FIFO watermark */
#define SSP_FIFO_LEVEL_TX	GENMASK(6, 0) /* Transmit FIFO watermark */
#define DFLT_THRESH_RX		32
#define DFLT_THRESH_TX		32

/* SSP Raw Interrupt Status Register - SSP_RIS */
#define SSP_RIS_MASK_RORRIS	(0x1 << 0) /* Receive Overrun */
#define SSP_RIS_MASK_RTRIS	(0x1 << 1) /* Receive Timeout */
#define SSP_RIS_MASK_RXRIS	(0x1 << 2) /* Receive FIFO Raw Interrupt status */
#define SSP_RIS_MASK_TXRIS	(0x1 << 3) /* Transmit FIFO Raw Interrupt status */

/* EXSPI command register 0 SSP_EXSPI_CMD0 */
#define EXSPI_CMD0_CMD_COUNT	BIT(0)		/* cmd byte, must be set at last */
#define EXSPI_CMD0_ADDR_COUNT	GENMASK(2, 1)	/* addr bytes */
#define EXSPI_CMD0_EHC_COUNT	BIT(3)		/* Set 1 for 4-byte address mode */
#define EXSPI_CMD0_TX_COUNT	GENMASK(14, 4)	/* TX data bytes */
#define EXSPI_CMD0_VALID	BIT(15)		/* Set 1 to make the cmd to be run */

/* EXSPI command register 1 SSP_EXSPI_CMD1 */
#define EXSPI_CMD1_DUMMY_COUNT	GENMASK(3, 0)	/* dummy bytes */
#define EXSPI_CMD1_RX_COUNT	GENMASK(14, 4)	/* RX data bytes */

/* EXSPI command register 2 SSP_EXSPI_CMD2 */
/* Set 1 for 1-wire, 2 for 2-wire, 3 for 4-wire */
#define EXSPI_CMD2_CMD_IO_MODE	GENMASK(1, 0)	/* cmd IO mode */
#define EXSPI_CMD2_ADDR_IO_MODE	GENMASK(3, 2)	/* addr IO mode */
#define EXSPI_CMD2_DATA_IO_MODE	GENMASK(5, 4)	/* data IO mode */

#define SF_READ_TIMEOUT		(10 * HZ)
#define MAX_S_BUF			100

struct sf_qspi {
	void __iomem *base;
	struct clk *clk;
	struct device *dev;
	u32 freq;
	int mode_bit;
};

static void sf_qspi_flush_rxfifo(struct sf_qspi *s)
{
	while (readw(s->base + SSP_SR) & SSP_SR_MASK_RNE)
		readw(s->base + SSP_DR);
}

static int sf_qspi_wait_not_busy(struct sf_qspi *s)
{
	unsigned long timeout = jiffies + SF_READ_TIMEOUT;

	do {
		if (!(readw(s->base + SSP_SR) & SSP_SR_MASK_BSY))
			return 0;

		cond_resched();
	} while (time_after(timeout, jiffies));

	dev_err(s->dev, "I/O timed out\n");
	return -ETIMEDOUT;
}

static int sf_qspi_wait_rx_not_empty(struct sf_qspi *s)
{
	unsigned long timeout = jiffies + SF_READ_TIMEOUT;

	do {
		if (readw(s->base + SSP_SR) & SSP_SR_MASK_RNE)
			return 0;

		cond_resched();
	} while (time_after(timeout, jiffies));

	dev_err(s->dev, "read timed out\n");
	return -ETIMEDOUT;
}

static int sf_qspi_wait_rxfifo(struct sf_qspi *s)
{
	unsigned long timeout = jiffies + SF_READ_TIMEOUT;

	do {
		if (readw(s->base + SSP_RIS) & SSP_RIS_MASK_RXRIS)
			return 0;

		cond_resched();
	} while (time_after(timeout, jiffies));

	dev_err(s->dev, "read timed out\n");
	return -ETIMEDOUT;
}

static void sf_qspi_enable(struct sf_qspi *s)
{
	/* Enable the SPI hardware */
	writew(SSP_CR1_MASK_SSE, s->base + SSP_CR1);
}

static void sf_qspi_disable(struct sf_qspi *s)
{
	/* Disable the SPI hardware */
	writew(0, s->base + SSP_CR1);
}

static void sf_qspi_xmit(struct sf_qspi *s, unsigned int nbytes, const u8 *out)
{
	while (nbytes--)
		writew(*out++, s->base + SSP_DR);
}

static int sf_qspi_rcv(struct sf_qspi *s, unsigned int nbytes, u8 *in)
{
	int ret, i;

	while (nbytes >= DFLT_THRESH_RX) {
		/* wait for RX FIFO to reach the threshold */
		ret = sf_qspi_wait_rxfifo(s);
		if (ret)
			return ret;

		for (i = 0; i < DFLT_THRESH_RX; i++)
			*in++ = readw(s->base + SSP_DR);

		nbytes -= DFLT_THRESH_RX;
	}

	/* read the remaining data */
	while (nbytes) {
		ret = sf_qspi_wait_rx_not_empty(s);
		if (ret)
			return ret;

		*in++ = readw(s->base + SSP_DR);
		nbytes--;
	}

	return 0;
}

static inline u32 spi_rate(u32 rate, u16 csdvsr, u16 scr)
{
	return rate / (csdvsr * (1 + scr));
}

static void sf_qspi_set_param(struct sf_qspi *s, const struct spi_mem_op *op)
{
	unsigned int tx_count = 0, rx_count = 0;
	u8 cmd_io, addr_io, data_io;
	u8 cmd_count, addr_count, ehc_count;

	cmd_io = op->cmd.buswidth == 4 ? 3 : op->cmd.buswidth;
	addr_io = op->addr.buswidth == 4 ? 3 : op->addr.buswidth;
	data_io = op->data.buswidth == 4 ? 3 : op->data.buswidth;

	if (op->data.nbytes) {
		if (op->data.dir == SPI_MEM_DATA_IN)
			rx_count = op->data.nbytes;
		else
			tx_count = op->data.nbytes;
	}
	if (op->addr.nbytes > 3) {
		addr_count = 3;
		ehc_count = 1;
	} else {
		addr_count = op->addr.nbytes;
		ehc_count = 0;
	}
	cmd_count = op->cmd.nbytes;

	writew(FIELD_PREP(EXSPI_CMD2_CMD_IO_MODE, cmd_io) |
	       FIELD_PREP(EXSPI_CMD2_ADDR_IO_MODE, addr_io) |
	       FIELD_PREP(EXSPI_CMD2_DATA_IO_MODE, data_io),
	       s->base + SSP_EXSPI_CMD2);
	writew(FIELD_PREP(EXSPI_CMD1_DUMMY_COUNT, op->dummy.nbytes) |
	       FIELD_PREP(EXSPI_CMD1_RX_COUNT, rx_count),
	       s->base + SSP_EXSPI_CMD1);
	writew(EXSPI_CMD0_VALID |
	       FIELD_PREP(EXSPI_CMD0_CMD_COUNT, op->cmd.nbytes) |
	       FIELD_PREP(EXSPI_CMD0_ADDR_COUNT, addr_count) |
	       FIELD_PREP(EXSPI_CMD0_EHC_COUNT, ehc_count) |
	       FIELD_PREP(EXSPI_CMD0_TX_COUNT, tx_count),
	       s->base + SSP_EXSPI_CMD0);
}

static int sf_qspi_exec_op(struct spi_mem *mem, const struct spi_mem_op *op)
{
	struct sf_qspi *s = spi_controller_get_devdata(mem->spi->master);
	unsigned int pops = 0;
	int ret, i, op_len;
	const u8 *tx_buf = NULL;
	u8 *rx_buf = NULL, op_buf[MAX_S_BUF];

	if (op->data.nbytes) {
		if (op->data.dir == SPI_MEM_DATA_IN)
			rx_buf = op->data.buf.in;
		else
			tx_buf = op->data.buf.out;
	}
	op_len = op->cmd.nbytes + op->addr.nbytes + op->dummy.nbytes;
	sf_qspi_set_param(s, op);
	/*
	 * Avoid using malloc() here so that we can use this code in SPL where
	 * simple malloc may be used. That implementation does not allow free()
	 * so repeated calls to this code can exhaust the space.
	 *
	 * The value of op_len is small, since it does not include the actual
	 * data being sent, only the op-code and address. In fact, it should be
	 * popssible to just use a small fixed value here instead of op_len.
	 */
	op_buf[pops++] = op->cmd.opcode;
	if (op->addr.nbytes) {
		for (i = 0; i < op->addr.nbytes; i++)
			op_buf[pops + i] = op->addr.val >>
					   (8 * (op->addr.nbytes - i - 1));
		pops += op->addr.nbytes;
	}

	sf_qspi_flush_rxfifo(s);
	memset(op_buf + pops, 0xff, op->dummy.nbytes);
	sf_qspi_xmit(s, op_len, op_buf);
	if (tx_buf)
		sf_qspi_xmit(s, op->data.nbytes, tx_buf);

	sf_qspi_enable(s);
	if (rx_buf)
		ret = sf_qspi_rcv(s, op->data.nbytes, rx_buf);
	else
		ret = sf_qspi_wait_not_busy(s);

	sf_qspi_disable(s);
	return ret;
}

static int sf_qspi_adjust_op_size(struct spi_mem *mem, struct spi_mem_op *op)
{
	u32 nbytes;

	nbytes = op->cmd.nbytes + op->addr.nbytes + op->dummy.nbytes;
	if (nbytes >= SF_SSP_FIFO_LEVEL)
		return -EOPNOTSUPP;

	if (op->data.dir == SPI_MEM_DATA_IN)
		op->data.nbytes = min_t(unsigned int, op->data.nbytes,
					SF_SSP_FIFO_LEVEL);
	else
		op->data.nbytes = min_t(unsigned int, op->data.nbytes,
					SF_SSP_FIFO_LEVEL - nbytes);

	return 0;
}

static int sf_qspi_default_setup(struct sf_qspi *s)
{
	u16 scr = SSP_SCR_MIN, cr0 = 0, cpsr = SSP_CPSR_MIN, best_scr = scr, best_cpsr = cpsr;
	u32 min, max, best_freq = 0, tmp;
	u32 rate = clk_get_rate(s->clk), speed = s->freq;
	bool found = false;

	writew(DFLT_PRESCALE, s->base + SSP_CPSR);

	max = spi_rate(rate, SSP_CPSR_MIN, SSP_SCR_MIN);
	min = spi_rate(rate, SSP_CPSR_MAX, SSP_SCR_MAX);

	if (speed > max || speed < min) {
		dev_err(s->dev, "Tried to set speed to %dHz but min=%d and max=%d\n",
			speed, min, max);
		return -EINVAL;
	}
	while (cpsr <= SSP_CPSR_MAX && !found) {
		while (scr <= SSP_SCR_MAX) {
			tmp = spi_rate(rate, cpsr, scr);
			if (abs(speed - tmp) < abs(speed - best_freq)) {
				best_freq = tmp;
				best_cpsr = cpsr;
				best_scr = scr;
				if (tmp == speed) {
					found = true;
					break;
				}
			}
			scr++;
		}
		cpsr += 2;
		scr = SSP_SCR_MIN;
	}
	writew(best_cpsr, s->base + SSP_CPSR);
	cr0 = SSP_CR0_BIT_MODE(8);
	cr0 |= best_scr << 8;
	/* set module */
	cr0 &= ~(SSP_CR0_SPH | SSP_CR0_SPO);
	if (s->mode_bit & SPI_CPHA)
		cr0 |= SSP_CR0_SPH;
	if (s->mode_bit & SPI_CPOL)
		cr0 |= SSP_CR0_SPO;
	cr0 |= SSP_CR0_EXSPI_FRAME;
	writew(cr0, s->base + SSP_CR0);
	/* clear and enable interrupt */
	writew(FIELD_PREP(SSP_FIFO_LEVEL_RX, DFLT_THRESH_RX) |
	       FIELD_PREP(SSP_FIFO_LEVEL_TX, DFLT_THRESH_TX),
	       s->base + SSP_FIFO_LEVEL);

	return 0;
}

static const struct spi_controller_mem_ops sf_qspi_mem_ops = {
	.adjust_op_size = sf_qspi_adjust_op_size,
	.exec_op = sf_qspi_exec_op,
};

static int sf_qspi_probe(struct platform_device *pdev)
{
	struct spi_controller *master;
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node, *nc;
	struct sf_qspi *s;

	master = devm_spi_alloc_host(&pdev->dev, sizeof(*s));
	if (!master)
		return -ENOMEM;
	master->mode_bits = SPI_RX_DUAL | SPI_RX_QUAD | SPI_TX_DUAL |
			    SPI_TX_QUAD;
	s = spi_controller_get_devdata(master);
	s->dev = dev;
	s->mode_bit = 0;
	platform_set_drvdata(pdev, s);
	s->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(s->base))
		return PTR_ERR(s->base);

	s->clk = devm_clk_get_enabled(dev, NULL);
	if (IS_ERR(s->clk))
		return PTR_ERR(s->clk);

	for_each_available_child_of_node(dev->of_node, nc) {
		of_property_read_u32(nc, "spi-max-frequency", &s->freq);
	}

	master->bus_num = pdev->id;
	master->num_chipselect = 1;

	master->mem_ops = &sf_qspi_mem_ops;
	sf_qspi_default_setup(s);
	master->dev.of_node = np;
	return devm_spi_register_controller(dev, master);
}

static const struct of_device_id sf_qspi_ids[] = {
	{ .compatible = "siflower,qspi" },
	{},
};
MODULE_DEVICE_TABLE(of, sf_qspi_ids);

static struct platform_driver sf_qspi_driver = {
	.driver = {
		.name = "siflower_qspi",
		.of_match_table = sf_qspi_ids,
	},
	.probe		= sf_qspi_probe,
};
module_platform_driver(sf_qspi_driver);

MODULE_LICENSE("GPL");
