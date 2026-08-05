// SPDX-License-Identifier: GPL-2.0-only
/*
 * Ralink RT6855 built-in hardware watchdog timer
 *
 * Copyright (C) 2014 John Crispin <john at phrozen.org>
 * Copyright (C) 2026 Nickolay Savchenko <n.savchenko@axioma.lv>
 */

#include <linux/clk.h>
#include <linux/kernel.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/watchdog.h>

#define TIMER_REG_TMR1CTL	0x00
#define TIMER_REG_TMR1LOAD	0x2c
#define TIMER_REG_TMRSTAT	0x38

#define TMR1CTL_ENABLE		(BIT(5) | BIT(25))
#define TMR1CTL_RESTART		BIT(0)

struct rt6855_wdt {
	void __iomem *base;
	struct reset_control *reset;
	unsigned long rate;
	struct watchdog_device wdd;
};

static bool nowayout = WATCHDOG_NOWAYOUT;
module_param(nowayout, bool, 0);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started");

static void rt6855_wdt_write(struct rt6855_wdt *priv, u32 reg, u32 val)
{
	iowrite32(val, priv->base + reg);
}

static u32 rt6855_wdt_read(struct rt6855_wdt *priv, u32 reg)
{
	return ioread32(priv->base + reg);
}

static int rt6855_wdt_ping(struct watchdog_device *wdd)
{
	struct rt6855_wdt *priv = watchdog_get_drvdata(wdd);

	rt6855_wdt_write(priv, TIMER_REG_TMRSTAT, TMR1CTL_RESTART);
	return 0;
}

static int rt6855_wdt_set_timeout(struct watchdog_device *wdd,
				  unsigned int timeout)
{
	struct rt6855_wdt *priv = watchdog_get_drvdata(wdd);
	u64 load = (u64)timeout * priv->rate;

	if (load > U32_MAX)
		return -EINVAL;

	wdd->timeout = timeout;
	rt6855_wdt_write(priv, TIMER_REG_TMR1LOAD, load);
	return rt6855_wdt_ping(wdd);
}

static int rt6855_wdt_start(struct watchdog_device *wdd)
{
	struct rt6855_wdt *priv = watchdog_get_drvdata(wdd);
	u32 value;
	int ret;

	ret = rt6855_wdt_set_timeout(wdd, wdd->timeout);
	if (ret)
		return ret;

	value = rt6855_wdt_read(priv, TIMER_REG_TMR1CTL);
	rt6855_wdt_write(priv, TIMER_REG_TMR1CTL, value | TMR1CTL_ENABLE);
	return 0;
}

static int rt6855_wdt_stop(struct watchdog_device *wdd)
{
	struct rt6855_wdt *priv = watchdog_get_drvdata(wdd);
	u32 value;

	rt6855_wdt_ping(wdd);
	value = rt6855_wdt_read(priv, TIMER_REG_TMR1CTL);
	rt6855_wdt_write(priv, TIMER_REG_TMR1CTL, value & ~TMR1CTL_ENABLE);
	return 0;
}

static bool rt6855_wdt_is_running(struct rt6855_wdt *priv)
{
	return rt6855_wdt_read(priv, TIMER_REG_TMR1CTL) & TMR1CTL_ENABLE;
}

static const struct watchdog_info rt6855_wdt_info = {
	.identity = "Ralink RT6855 watchdog",
	.options = WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING | WDIOF_MAGICCLOSE,
};

static const struct watchdog_ops rt6855_wdt_ops = {
	.owner = THIS_MODULE,
	.start = rt6855_wdt_start,
	.stop = rt6855_wdt_stop,
	.ping = rt6855_wdt_ping,
	.set_timeout = rt6855_wdt_set_timeout,
};

static int rt6855_wdt_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct rt6855_wdt *priv;
	struct clk *clk;
	int ret;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(priv->base))
		return PTR_ERR(priv->base);

	clk = devm_clk_get_enabled(dev, NULL);
	if (IS_ERR(clk))
		return dev_err_probe(dev, PTR_ERR(clk), "failed to get clock\n");

	priv->rate = clk_get_rate(clk) / 2;
	if (!priv->rate)
		return dev_err_probe(dev, -EINVAL, "invalid timer clock\n");

	priv->reset = devm_reset_control_get_optional_exclusive(dev, NULL);
	if (IS_ERR(priv->reset))
		return dev_err_probe(dev, PTR_ERR(priv->reset),
				     "failed to get reset\n");

	ret = reset_control_deassert(priv->reset);
	if (ret)
		return dev_err_probe(dev, ret, "failed to deassert reset\n");

	priv->wdd.info = &rt6855_wdt_info;
	priv->wdd.ops = &rt6855_wdt_ops;
	priv->wdd.parent = dev;
	priv->wdd.min_timeout = 1;
	priv->wdd.max_timeout = U32_MAX / priv->rate;
	priv->wdd.timeout = priv->wdd.max_timeout;
	watchdog_set_drvdata(&priv->wdd, priv);
	watchdog_init_timeout(&priv->wdd, 0, dev);
	watchdog_set_nowayout(&priv->wdd, nowayout);
	platform_set_drvdata(pdev, priv);

	if (rt6855_wdt_is_running(priv)) {
		ret = rt6855_wdt_stop(&priv->wdd);
		if (ret)
			return ret;
		ret = rt6855_wdt_start(&priv->wdd);
		if (ret)
			return ret;
		set_bit(WDOG_HW_RUNNING, &priv->wdd.status);
	}

	return devm_watchdog_register_device(dev, &priv->wdd);
}

static void rt6855_wdt_shutdown(struct platform_device *pdev)
{
	struct rt6855_wdt *priv = platform_get_drvdata(pdev);

	rt6855_wdt_stop(&priv->wdd);
}

static const struct of_device_id rt6855_wdt_match[] = {
	{ .compatible = "ralink,rt6855-wdt" },
	{}
};
MODULE_DEVICE_TABLE(of, rt6855_wdt_match);

static struct platform_driver rt6855_wdt_driver = {
	.probe = rt6855_wdt_probe,
	.shutdown = rt6855_wdt_shutdown,
	.driver = {
		.name = KBUILD_MODNAME,
		.of_match_table = rt6855_wdt_match,
	},
};
module_platform_driver(rt6855_wdt_driver);

MODULE_DESCRIPTION("Ralink RT6855 hardware watchdog driver");
MODULE_AUTHOR("Nickolay Savchenko <n.savchenko@axioma.lv>");
MODULE_LICENSE("GPL");
