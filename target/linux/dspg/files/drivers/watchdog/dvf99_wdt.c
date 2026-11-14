// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/math64.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/watchdog.h>

#define DVF99_WDT_CTRL			0x00
#define DVF99_WDT_ENABLE		0x04
#define DVF99_WDT_REFRESH		0x08
#define DVF99_WDT_SMST			0x0c
#define DVF99_WDT_CVR1			0x10
#define DVF99_WDT_CVR2			0x14
#define DVF99_WDT_CVR3			0x18
#define DVF99_WDT_INTCLR		0x1c

#define DVF99_WDT_CTRL_PERIOD		GENMASK(1, 0)
#define DVF99_WDT_CTRL_INTEN		BIT(2)

#define DVF99_WDT_SMST_STATE		GENMASK(2, 0)
#define DVF99_WDT_SMST_RUNNING		3

#define DVF99_WDT_CVR1_MASK		GENMASK(7, 0)
#define DVF99_WDT_CVR2_MASK		GENMASK(7, 0)
#define DVF99_WDT_CVR3_MASK		GENMASK(8, 0)

#define DVF99_WDT_ENABLE_KEY		0xabcd
#define DVF99_WDT_REFRESH_KEY1		0xa5a5
#define DVF99_WDT_REFRESH_KEY2		0x5a5a
#define DVF99_WDT_INTCLR_ACK		BIT(0)

#define DVF99_WDT_ENABLE_RETRIES	10
#define DVF99_WDT_REFRESH_RETRIES	1000
#define DVF99_WDT_PERIODS		4
#define DVF99_WDT_BASE_TICKS		32768000UL

struct dvf99_wdt {
	struct watchdog_device wdd;
	void __iomem *regs;
	unsigned long rate;
	spinlock_t lock;
};

static unsigned int dvf99_wdt_period_ms(struct dvf99_wdt *wdt,
					unsigned int period)
{
	u64 ms = div_u64((u64)DVF99_WDT_BASE_TICKS * MSEC_PER_SEC, wdt->rate);
	unsigned int i;

	for (i = period; i < DVF99_WDT_PERIODS - 1; i++)
		ms = div_u64(ms, 10);

	return max_t(u64, ms, 1);
}

static unsigned int dvf99_wdt_period_seconds(struct dvf99_wdt *wdt,
					     unsigned int period)
{
	return max(dvf99_wdt_period_ms(wdt, period) / MSEC_PER_SEC, 1U);
}

static u32 dvf99_wdt_counter_read(struct dvf99_wdt *wdt)
{
	return (readw(wdt->regs + DVF99_WDT_CVR3) & DVF99_WDT_CVR3_MASK) << 16 |
	       (readw(wdt->regs + DVF99_WDT_CVR2) & DVF99_WDT_CVR2_MASK) << 8 |
	       (readw(wdt->regs + DVF99_WDT_CVR1) & DVF99_WDT_CVR1_MASK);
}

static bool dvf99_wdt_is_running(struct dvf99_wdt *wdt)
{
	return FIELD_GET(DVF99_WDT_SMST_STATE,
			 readl(wdt->regs + DVF99_WDT_SMST)) ==
	       DVF99_WDT_SMST_RUNNING;
}

static int dvf99_wdt_refresh(struct dvf99_wdt *wdt)
{
	unsigned int retries = DVF99_WDT_REFRESH_RETRIES;
	unsigned long flags;
	u32 counter;
	int ret = 0;

	spin_lock_irqsave(&wdt->lock, flags);

	counter = dvf99_wdt_counter_read(wdt);
	if (!counter)
		goto out;

	do {
		writew(DVF99_WDT_REFRESH_KEY1, wdt->regs + DVF99_WDT_REFRESH);
		writew(DVF99_WDT_REFRESH_KEY2, wdt->regs + DVF99_WDT_REFRESH);
	} while (dvf99_wdt_counter_read(wdt) >= counter && --retries);

	if (!retries)
		ret = -EIO;

out:
	spin_unlock_irqrestore(&wdt->lock, flags);

	return ret;
}

static int dvf99_wdt_ping(struct watchdog_device *wdd)
{
	struct dvf99_wdt *wdt = watchdog_get_drvdata(wdd);

	return dvf99_wdt_refresh(wdt);
}

static int dvf99_wdt_start(struct watchdog_device *wdd)
{
	struct dvf99_wdt *wdt = watchdog_get_drvdata(wdd);
	unsigned int retries = DVF99_WDT_ENABLE_RETRIES;
	unsigned long flags;
	u16 ctrl;

	spin_lock_irqsave(&wdt->lock, flags);

	while (!dvf99_wdt_is_running(wdt) && retries--) {
		writew(DVF99_WDT_ENABLE_KEY, wdt->regs + DVF99_WDT_ENABLE);
		writew(DVF99_WDT_ENABLE_KEY, wdt->regs + DVF99_WDT_ENABLE);
	}

	ctrl = readw(wdt->regs + DVF99_WDT_CTRL);
	writew(ctrl | DVF99_WDT_CTRL_INTEN, wdt->regs + DVF99_WDT_CTRL);

	spin_unlock_irqrestore(&wdt->lock, flags);

	if (!dvf99_wdt_is_running(wdt))
		return -EIO;

	return dvf99_wdt_refresh(wdt);
}

static int dvf99_wdt_set_timeout(struct watchdog_device *wdd,
				 unsigned int timeout)
{
	struct dvf99_wdt *wdt = watchdog_get_drvdata(wdd);
	unsigned int period = 0;
	unsigned long flags;
	u16 ctrl;

	wdd->timeout = timeout;

	if (dvf99_wdt_is_running(wdt))
		return 0;

	while (period < DVF99_WDT_PERIODS - 1 &&
	       dvf99_wdt_period_seconds(wdt, period + 1) <= timeout)
		period++;

	spin_lock_irqsave(&wdt->lock, flags);
	ctrl = readw(wdt->regs + DVF99_WDT_CTRL);
	ctrl &= ~DVF99_WDT_CTRL_PERIOD;
	ctrl |= FIELD_PREP(DVF99_WDT_CTRL_PERIOD, period);
	writew(ctrl, wdt->regs + DVF99_WDT_CTRL);
	spin_unlock_irqrestore(&wdt->lock, flags);

	wdd->max_hw_heartbeat_ms = dvf99_wdt_period_ms(wdt, period);

	return 0;
}

static irqreturn_t dvf99_wdt_irq(int irq, void *data)
{
	struct dvf99_wdt *wdt = data;

	writew(DVF99_WDT_INTCLR_ACK, wdt->regs + DVF99_WDT_INTCLR);

	dev_crit(wdt->wdd.parent, "watchdog pre-timeout, reset follows unless pinged\n");

	return IRQ_HANDLED;
}

static const struct watchdog_info dvf99_wdt_info = {
	.options = WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING | WDIOF_MAGICCLOSE,
	.identity = "DVF99 watchdog",
};

static const struct watchdog_ops dvf99_wdt_ops = {
	.owner = THIS_MODULE,
	.start = dvf99_wdt_start,
	.ping = dvf99_wdt_ping,
	.set_timeout = dvf99_wdt_set_timeout,
};

static int dvf99_wdt_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct dvf99_wdt *wdt;
	struct clk *clk;
	int irq, ret;

	wdt = devm_kzalloc(dev, sizeof(*wdt), GFP_KERNEL);
	if (!wdt)
		return -ENOMEM;

	spin_lock_init(&wdt->lock);

	wdt->regs = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(wdt->regs))
		return PTR_ERR(wdt->regs);

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	clk = devm_clk_get_enabled(dev, NULL);
	if (IS_ERR(clk))
		return dev_err_probe(dev, PTR_ERR(clk), "failed to get clock\n");

	wdt->rate = clk_get_rate(clk);
	if (!wdt->rate)
		return dev_err_probe(dev, -EINVAL, "clock rate is zero\n");

	wdt->wdd.info = &dvf99_wdt_info;
	wdt->wdd.ops = &dvf99_wdt_ops;
	wdt->wdd.parent = dev;
	wdt->wdd.min_timeout = dvf99_wdt_period_seconds(wdt, 0);
	wdt->wdd.max_timeout = dvf99_wdt_period_seconds(wdt,
							 DVF99_WDT_PERIODS - 1);
	wdt->wdd.timeout = wdt->wdd.max_timeout;
	wdt->wdd.max_hw_heartbeat_ms = dvf99_wdt_period_ms(wdt,
		FIELD_GET(DVF99_WDT_CTRL_PERIOD,
			  readw(wdt->regs + DVF99_WDT_CTRL)));
	watchdog_set_drvdata(&wdt->wdd, wdt);

	watchdog_init_timeout(&wdt->wdd, 0, dev);
	dvf99_wdt_set_timeout(&wdt->wdd, wdt->wdd.timeout);

	ret = devm_request_irq(dev, irq, dvf99_wdt_irq, 0, dev_name(dev), wdt);
	if (ret)
		return ret;

	if (dvf99_wdt_is_running(wdt)) {
		set_bit(WDOG_HW_RUNNING, &wdt->wdd.status);
		ret = dvf99_wdt_start(&wdt->wdd);
		if (ret)
			return ret;
	}

	return devm_watchdog_register_device(dev, &wdt->wdd);
}

static const struct of_device_id dvf99_wdt_of_match[] = {
	{ .compatible = "dspg,dvf99-wdt" },
	{ }
};
MODULE_DEVICE_TABLE(of, dvf99_wdt_of_match);

static struct platform_driver dvf99_wdt_driver = {
	.probe = dvf99_wdt_probe,
	.driver = {
		.name = "dvf99-wdt",
		.of_match_table = dvf99_wdt_of_match,
	},
};
module_platform_driver(dvf99_wdt_driver);

MODULE_AUTHOR("John Crispin <john@phrozen.org>");
MODULE_DESCRIPTION("DSPG DVF99 watchdog driver");
MODULE_LICENSE("GPL");
