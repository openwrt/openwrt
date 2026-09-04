// SPDX-License-Identifier: GPL-2.0
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/smp.h>
#include <linux/watchdog.h>

#define TWD_WDT_LOAD		0x00
#define TWD_WDT_COUNTER		0x04
#define TWD_WDT_CONTROL		0x08
#define TWD_WDT_INTSTAT		0x0c
#define TWD_WDT_RESETSTAT	0x10
#define TWD_WDT_DISABLE		0x14

#define TWD_WDT_CTRL_ENABLE	BIT(0)
#define TWD_WDT_CTRL_AUTORELOAD	BIT(1)
#define TWD_WDT_CTRL_WDMODE	BIT(3)
#define TWD_WDT_PRESCALE	0xFF
#define TWD_WDT_CTRL_PRESCALE	(TWD_WDT_PRESCALE << 8)

#define TWD_WDT_DEFAULT_TIMEOUT	20
#define TWD_WDT_DEFAULT_CLK	150000000

struct twd_wdt {
	struct watchdog_device wdd;
	void __iomem *base;
	unsigned long rate;
};

static u32 twd_wdt_load(struct twd_wdt *wdt, unsigned int timeout)
{
	u64 ticks = (u64)wdt->rate * timeout;

	do_div(ticks, TWD_WDT_PRESCALE + 1);
	if (ticks > 0xFFFFFFFFULL)
		ticks = 0xFFFFFFFFULL;
	return (u32)ticks;
}

static void twd_wdt_hw_start(void *info)
{
	struct twd_wdt *wdt = info;

	writel(0, wdt->base + TWD_WDT_CONTROL);
	writel(twd_wdt_load(wdt, wdt->wdd.timeout), wdt->base + TWD_WDT_LOAD);
	writel(TWD_WDT_CTRL_PRESCALE | TWD_WDT_CTRL_WDMODE | TWD_WDT_CTRL_ENABLE,
	       wdt->base + TWD_WDT_CONTROL);
}

static void twd_wdt_hw_stop(void *info)
{
	struct twd_wdt *wdt = info;

	writel(0x12345678, wdt->base + TWD_WDT_DISABLE);
	writel(0x87654321, wdt->base + TWD_WDT_DISABLE);
	writel(0, wdt->base + TWD_WDT_CONTROL);
}

static void twd_wdt_hw_ping(void *info)
{
	struct twd_wdt *wdt = info;

	writel(twd_wdt_load(wdt, wdt->wdd.timeout), wdt->base + TWD_WDT_LOAD);
}

static int twd_wdt_start(struct watchdog_device *wdd)
{
	on_each_cpu(twd_wdt_hw_start, watchdog_get_drvdata(wdd), 1);
	return 0;
}

static int twd_wdt_stop(struct watchdog_device *wdd)
{
	on_each_cpu(twd_wdt_hw_stop, watchdog_get_drvdata(wdd), 1);
	return 0;
}

static int twd_wdt_ping(struct watchdog_device *wdd)
{
	on_each_cpu(twd_wdt_hw_ping, watchdog_get_drvdata(wdd), 1);
	return 0;
}

static int twd_wdt_set_timeout(struct watchdog_device *wdd, unsigned int timeout)
{
	wdd->timeout = timeout;
	return twd_wdt_ping(wdd);
}

static const struct watchdog_ops twd_wdt_ops = {
	.owner		= THIS_MODULE,
	.start		= twd_wdt_start,
	.stop		= twd_wdt_stop,
	.ping		= twd_wdt_ping,
	.set_timeout	= twd_wdt_set_timeout,
};

static const struct watchdog_info twd_wdt_info = {
	.options	= WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING | WDIOF_MAGICCLOSE,
	.identity	= "Cortex-A9 TWD watchdog",
};

static int twd_wdt_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct twd_wdt *wdt;
	struct clk *clk;
	u32 rst;

	wdt = devm_kzalloc(dev, sizeof(*wdt), GFP_KERNEL);
	if (!wdt)
		return -ENOMEM;

	wdt->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(wdt->base))
		return PTR_ERR(wdt->base);

	wdt->rate = TWD_WDT_DEFAULT_CLK;
	clk = devm_clk_get_optional_enabled(dev, NULL);
	if (IS_ERR(clk))
		return dev_err_probe(dev, PTR_ERR(clk), "failed to get clock\n");
	if (clk && clk_get_rate(clk))
		wdt->rate = clk_get_rate(clk);

	rst = readl(wdt->base + TWD_WDT_RESETSTAT);
	if (rst & 1) {
		writel(1, wdt->base + TWD_WDT_RESETSTAT);
		dev_warn(dev, "previous reset was caused by the watchdog\n");
	}

	wdt->wdd.info = &twd_wdt_info;
	wdt->wdd.ops = &twd_wdt_ops;
	wdt->wdd.min_timeout = 1;
	wdt->wdd.max_timeout = (unsigned int)div_u64((u64)0xFFFFFFFF * (TWD_WDT_PRESCALE + 1), wdt->rate);
	wdt->wdd.timeout = TWD_WDT_DEFAULT_TIMEOUT;
	wdt->wdd.parent = dev;
	if (rst & 1)
		wdt->wdd.bootstatus = WDIOF_CARDRESET;
	watchdog_init_timeout(&wdt->wdd, 0, dev);
	watchdog_set_drvdata(&wdt->wdd, wdt);
	watchdog_set_nowayout(&wdt->wdd, false);
	watchdog_stop_on_reboot(&wdt->wdd);

	dev_info(dev, "clock %lu Hz, timeout %u s, max %u s\n",
		 wdt->rate, wdt->wdd.timeout, wdt->wdd.max_timeout);
	return devm_watchdog_register_device(dev, &wdt->wdd);
}

static const struct of_device_id twd_wdt_of_match[] = {
	{ .compatible = "arm,cortex-a9-twd-wdt" },
	{},
};

static struct platform_driver twd_wdt_driver = {
	.probe = twd_wdt_probe,
	.driver = {
		.name = "cortina-twd-wdt",
		.of_match_table = twd_wdt_of_match,
	},
};
builtin_platform_driver(twd_wdt_driver);
