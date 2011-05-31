/*
 * Driver for the Atheros AR71xx SoC's built-in hardware watchdog timer.
 *
 * Copyright (C) 2010-2011 Jaiganesh Narayanan <jnarayanan@atheros.com>
 * Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 * Parts of this file are based on Atheros 2.6.31 BSP
 *
 * This driver was based on: drivers/watchdog/ixp4xx_wdt.c
 *	Author: Deepak Saxena <dsaxena@plexity.net>
 *	Copyright 2004 (c) MontaVista, Software, Inc.
 *
 * which again was based on sa1100 driver,
 *	Copyright (C) 2000 Oleg Drokin <green@crimea.edu>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 *
 */

#include <linux/bitops.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/watchdog.h>
#include <linux/delay.h>

#include <asm/mach-ar71xx/ar71xx.h>

#define DRV_NAME	"ar71xx-wdt"
#define DRV_DESC	"Atheros AR71xx hardware watchdog driver"
#define DRV_VERSION	"0.1.0"

#define WDT_TIMEOUT	15	/* seconds */

static int nowayout = WATCHDOG_NOWAYOUT;

#ifdef CONFIG_WATCHDOG_NOWAYOUT
module_param(nowayout, int, 0);
MODULE_PARM_DESC(nowayout, "Watchdog cannot be stopped once started "
			   "(default=" __MODULE_STRING(WATCHDOG_NOWAYOUT) ")");
#endif

static unsigned long wdt_flags;

#define WDT_FLAGS_BUSY		0
#define WDT_FLAGS_EXPECT_CLOSE	1

static int wdt_timeout = WDT_TIMEOUT;
static int boot_status;
static int max_timeout;
static u32 wdt_clk_freq;

static inline void ar71xx_wdt_keepalive(void)
{
	ar71xx_reset_wr(AR71XX_RESET_REG_WDOG, wdt_clk_freq * wdt_timeout);
}

static inline void ar71xx_wdt_enable(void)
{
	printk(KERN_DEBUG DRV_NAME ": enabling watchdog timer\n");
	ar71xx_wdt_keepalive();
	udelay(2);
	ar71xx_reset_wr(AR71XX_RESET_REG_WDOG_CTRL, WDOG_CTRL_ACTION_FCR);
}

static inline void ar71xx_wdt_disable(void)
{
	printk(KERN_DEBUG DRV_NAME ": disabling watchdog timer\n");
	ar71xx_reset_wr(AR71XX_RESET_REG_WDOG_CTRL, WDOG_CTRL_ACTION_NONE);
}

static int ar71xx_wdt_set_timeout(int val)
{
	if (val < 1 || val > max_timeout)
		return -EINVAL;

	wdt_timeout = val;
	ar71xx_wdt_keepalive();

	printk(KERN_DEBUG DRV_NAME ": timeout=%d secs\n", wdt_timeout);

	return 0;
}

static int ar71xx_wdt_open(struct inode *inode, struct file *file)
{
	if (test_and_set_bit(WDT_FLAGS_BUSY, &wdt_flags))
		return -EBUSY;

	clear_bit(WDT_FLAGS_EXPECT_CLOSE, &wdt_flags);

	ar71xx_wdt_enable();

	return nonseekable_open(inode, file);
}

static int ar71xx_wdt_release(struct inode *inode, struct file *file)
{
	if (test_bit(WDT_FLAGS_EXPECT_CLOSE, &wdt_flags)) {
		ar71xx_wdt_disable();
	} else {
		printk(KERN_CRIT DRV_NAME ": device closed unexpectedly, "
					"watchdog timer will not stop!\n");
	}

	clear_bit(WDT_FLAGS_BUSY, &wdt_flags);
	clear_bit(WDT_FLAGS_EXPECT_CLOSE, &wdt_flags);

	return 0;
}

static ssize_t ar71xx_wdt_write(struct file *file, const char *data,
				size_t len, loff_t *ppos)
{
	if (len) {
		if (!nowayout) {
			size_t i;

			clear_bit(WDT_FLAGS_EXPECT_CLOSE, &wdt_flags);

			for (i = 0; i != len; i++) {
				char c;

				if (get_user(c, data + i))
					return -EFAULT;

				if (c == 'V')
					set_bit(WDT_FLAGS_EXPECT_CLOSE,
						&wdt_flags);
			}
		}

		ar71xx_wdt_keepalive();
	}

	return len;
}

static struct watchdog_info ar71xx_wdt_info = {
	.options		= WDIOF_SETTIMEOUT | WDIOF_KEEPALIVEPING |
				  WDIOF_MAGICCLOSE | WDIOF_CARDRESET,
	.firmware_version	= 0,
	.identity		= "AR71XX watchdog",
};

static long ar71xx_wdt_ioctl(struct file *file,
			    unsigned int cmd, unsigned long arg)
{
	int t;
	int ret;

	switch (cmd) {
	case WDIOC_GETSUPPORT:
		ret = copy_to_user((struct watchdog_info *)arg,
				   &ar71xx_wdt_info,
				   sizeof(ar71xx_wdt_info)) ? -EFAULT : 0;
		break;

	case WDIOC_GETSTATUS:
		ret = put_user(0, (int *)arg) ? -EFAULT : 0;
		break;

	case WDIOC_GETBOOTSTATUS:
		ret = put_user(boot_status, (int *)arg) ? -EFAULT : 0;
		break;

	case WDIOC_KEEPALIVE:
		ar71xx_wdt_keepalive();
		ret = 0;
		break;

	case WDIOC_SETTIMEOUT:
		ret = get_user(t, (int *)arg) ? -EFAULT : 0;
		if (ret)
			break;

		ret = ar71xx_wdt_set_timeout(t);
		if (ret)
			break;

		/* fallthrough */
	case WDIOC_GETTIMEOUT:
		ret = put_user(wdt_timeout, (int *)arg) ? -EFAULT : 0;
		break;

	default:
		ret = -ENOTTY;
		break;
	}

	return ret;
}

static const struct file_operations ar71xx_wdt_fops = {
	.owner		= THIS_MODULE,
	.write		= ar71xx_wdt_write,
	.unlocked_ioctl	= ar71xx_wdt_ioctl,
	.open		= ar71xx_wdt_open,
	.release	= ar71xx_wdt_release,
};

static struct miscdevice ar71xx_wdt_miscdev = {
	.minor = WATCHDOG_MINOR,
	.name = "watchdog",
	.fops = &ar71xx_wdt_fops,
};

static int __devinit ar71xx_wdt_probe(struct platform_device *pdev)
{
	int ret;

	switch (ar71xx_soc) {
	case AR71XX_SOC_AR7130:
	case AR71XX_SOC_AR7141:
	case AR71XX_SOC_AR7161:
	case AR71XX_SOC_AR7240:
	case AR71XX_SOC_AR7241:
	case AR71XX_SOC_AR7242:
		wdt_clk_freq = ar71xx_ahb_freq;
		break;

	case AR71XX_SOC_AR9330:
	case AR71XX_SOC_AR9331:
	case AR71XX_SOC_AR9341:
	case AR71XX_SOC_AR9342:
	case AR71XX_SOC_AR9344:
		wdt_clk_freq = ar71xx_ref_freq;
		break;

	default:
		BUG();
	}

	max_timeout = (0xfffffffful / wdt_clk_freq);
	wdt_timeout = (max_timeout < WDT_TIMEOUT) ? max_timeout : WDT_TIMEOUT;

	if (ar71xx_reset_rr(AR71XX_RESET_REG_WDOG_CTRL) & WDOG_CTRL_LAST_RESET)
		boot_status = WDIOF_CARDRESET;

	ret = misc_register(&ar71xx_wdt_miscdev);
	if (ret)
		goto err_out;

	printk(KERN_INFO DRV_DESC " version " DRV_VERSION "\n");

	printk(KERN_DEBUG DRV_NAME ": timeout=%d secs (max=%d)\n",
					wdt_timeout, max_timeout);

	return 0;

err_out:
	return ret;
}

static int __devexit ar71xx_wdt_remove(struct platform_device *pdev)
{
	misc_deregister(&ar71xx_wdt_miscdev);
	return 0;
}

static struct platform_driver ar71xx_wdt_driver = {
	.probe		= ar71xx_wdt_probe,
	.remove		= __devexit_p(ar71xx_wdt_remove),
	.driver		= {
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init ar71xx_wdt_init(void)
{
	return platform_driver_register(&ar71xx_wdt_driver);
}
module_init(ar71xx_wdt_init);

static void __exit ar71xx_wdt_exit(void)
{
	platform_driver_unregister(&ar71xx_wdt_driver);
}
module_exit(ar71xx_wdt_exit);

MODULE_DESCRIPTION(DRV_DESC);
MODULE_VERSION(DRV_VERSION);
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org");
MODULE_AUTHOR("Imre Kaloz <kaloz@openwrt.org");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRV_NAME);
MODULE_ALIAS_MISCDEV(WATCHDOG_MINOR);
