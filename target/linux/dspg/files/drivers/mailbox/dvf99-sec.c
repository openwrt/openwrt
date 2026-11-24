// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2012 DSP Group
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <dt-bindings/mailbox/dspg,dvf99-sec.h>
#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/mailbox_controller.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/spinlock.h>

#define SEC_CPU2COM(n)		(0x000 + (n) * 4)
#define SEC_COM2CPU(n)		(0x100 + (n) * 4)
#define SEC_COM2CPU_STAT(n)	(0x640 + ((n) / 32) * 4)
#define SEC_COM2CPU_MASK(n)	(0x648 + ((n) / 32) * 4)
#define SEC_COM2CPU_OVR(n)	(0x650 + ((n) / 32) * 4)
#define SEC_COM2CPU_CAUS(n)	(0x658 + ((n) / 32) * 4)

#define SEC_NUM_CHANS		64
#define SEC_FLAGS_MASK		(DVF99_SEC_OVERWRITE | DVF99_SEC_POLLED)

struct dvf99_sec {
	struct mbox_controller mbox;
	struct mbox_chan chans[SEC_NUM_CHANS];
	u32 flags[SEC_NUM_CHANS];
	void __iomem *base;
	spinlock_t lock;
};

static unsigned int dvf99_sec_chan_id(struct dvf99_sec *sec,
				      struct mbox_chan *chan)
{
	return chan - sec->chans;
}

static void dvf99_sec_bit_update(struct dvf99_sec *sec, unsigned int reg,
				 unsigned int id, bool set)
{
	unsigned long flags;
	u32 val;

	spin_lock_irqsave(&sec->lock, flags);
	val = readl(sec->base + reg);
	if (set)
		val |= BIT(id % 32);
	else
		val &= ~BIT(id % 32);
	writel(val, sec->base + reg);
	spin_unlock_irqrestore(&sec->lock, flags);
}

static int dvf99_sec_send_data(struct mbox_chan *chan, void *data)
{
	struct dvf99_sec *sec = dev_get_drvdata(chan->mbox->dev);
	unsigned int id = dvf99_sec_chan_id(sec, chan);

	writel(*(u32 *)data, sec->base + SEC_CPU2COM(id));

	return 0;
}

static int dvf99_sec_startup(struct mbox_chan *chan)
{
	struct dvf99_sec *sec = dev_get_drvdata(chan->mbox->dev);
	unsigned int id = dvf99_sec_chan_id(sec, chan);
	u32 flags = sec->flags[id];

	dvf99_sec_bit_update(sec, SEC_COM2CPU_OVR(id), id,
			     flags & DVF99_SEC_OVERWRITE);
	writel(BIT(id % 32), sec->base + SEC_COM2CPU_STAT(id));

	if (!(flags & DVF99_SEC_POLLED))
		dvf99_sec_bit_update(sec, SEC_COM2CPU_MASK(id), id, true);

	return 0;
}

static void dvf99_sec_shutdown(struct mbox_chan *chan)
{
	struct dvf99_sec *sec = dev_get_drvdata(chan->mbox->dev);
	unsigned int id = dvf99_sec_chan_id(sec, chan);

	dvf99_sec_bit_update(sec, SEC_COM2CPU_MASK(id), id, false);
	dvf99_sec_bit_update(sec, SEC_COM2CPU_OVR(id), id, false);
}

static bool dvf99_sec_peek_data(struct mbox_chan *chan)
{
	struct dvf99_sec *sec = dev_get_drvdata(chan->mbox->dev);
	unsigned int id = dvf99_sec_chan_id(sec, chan);
	u32 val;

	if (!(sec->flags[id] & DVF99_SEC_POLLED))
		return false;

	val = readl(sec->base + SEC_COM2CPU(id));
	mbox_chan_received_data(chan, &val);

	return true;
}

static const struct mbox_chan_ops dvf99_sec_ops = {
	.send_data = dvf99_sec_send_data,
	.startup = dvf99_sec_startup,
	.shutdown = dvf99_sec_shutdown,
	.peek_data = dvf99_sec_peek_data,
};

static struct mbox_chan *dvf99_sec_of_xlate(struct mbox_controller *mbox,
					    const struct of_phandle_args *sp)
{
	struct dvf99_sec *sec = dev_get_drvdata(mbox->dev);
	unsigned int id;

	if (sp->args_count != 2)
		return ERR_PTR(-EINVAL);

	id = sp->args[0];
	if (id >= SEC_NUM_CHANS || sp->args[1] & ~SEC_FLAGS_MASK)
		return ERR_PTR(-EINVAL);

	sec->flags[id] = sp->args[1];

	return &sec->chans[id];
}

static irqreturn_t dvf99_sec_irq(int irq, void *data)
{
	struct dvf99_sec *sec = data;
	unsigned long pending[BITS_TO_LONGS(SEC_NUM_CHANS)];
	u32 caus[SEC_NUM_CHANS / 32];
	unsigned int id;
	u32 val;

	caus[0] = readl(sec->base + SEC_COM2CPU_CAUS(0));
	caus[1] = readl(sec->base + SEC_COM2CPU_CAUS(32));
	bitmap_from_arr32(pending, caus, SEC_NUM_CHANS);

	if (bitmap_empty(pending, SEC_NUM_CHANS))
		return IRQ_NONE;

	for_each_set_bit(id, pending, SEC_NUM_CHANS) {
		val = readl(sec->base + SEC_COM2CPU(id));
		mbox_chan_received_data(&sec->chans[id], &val);
		writel(BIT(id % 32), sec->base + SEC_COM2CPU_STAT(id));
	}

	return IRQ_HANDLED;
}

static int dvf99_sec_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct reset_control *rst;
	struct dvf99_sec *sec;
	struct clk *clk;
	int irq, ret;

	sec = devm_kzalloc(dev, sizeof(*sec), GFP_KERNEL);
	if (!sec)
		return -ENOMEM;

	spin_lock_init(&sec->lock);
	platform_set_drvdata(pdev, sec);

	sec->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(sec->base))
		return PTR_ERR(sec->base);

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	clk = devm_clk_get_enabled(dev, NULL);
	if (IS_ERR(clk))
		return dev_err_probe(dev, PTR_ERR(clk), "no clock\n");

	rst = devm_reset_control_get_optional_exclusive_deasserted(dev, NULL);
	if (IS_ERR(rst))
		return dev_err_probe(dev, PTR_ERR(rst), "no reset\n");

	writel(0, sec->base + SEC_COM2CPU_MASK(0));
	writel(0, sec->base + SEC_COM2CPU_MASK(32));

	ret = devm_request_irq(dev, irq, dvf99_sec_irq, 0, dev_name(dev), sec);
	if (ret)
		return ret;

	sec->mbox.dev = dev;
	sec->mbox.ops = &dvf99_sec_ops;
	sec->mbox.chans = sec->chans;
	sec->mbox.num_chans = SEC_NUM_CHANS;
	sec->mbox.of_xlate = dvf99_sec_of_xlate;

	return devm_mbox_controller_register(dev, &sec->mbox);
}

static const struct of_device_id dvf99_sec_of_match[] = {
	{ .compatible = "dspg,dvf99-sec" },
	{ }
};
MODULE_DEVICE_TABLE(of, dvf99_sec_of_match);

static struct platform_driver dvf99_sec_driver = {
	.probe = dvf99_sec_probe,
	.driver = {
		.name = "dvf99-sec",
		.of_match_table = dvf99_sec_of_match,
	},
};
module_platform_driver(dvf99_sec_driver);

MODULE_DESCRIPTION("DSPG DVF99 semaphore exchange center mailbox driver");
MODULE_LICENSE("GPL");
