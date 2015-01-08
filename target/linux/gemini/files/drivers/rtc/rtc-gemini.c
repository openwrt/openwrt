/*
 *  Gemini OnChip RTC
 *
 *  Copyright (C) 2009 Janos Laube <janos.dev@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Original code for older kernel 2.6.15 are form Stormlinksemi
 * first update from Janos Laube for > 2.6.29 kernels
 *
 * checkpatch fixes and usage off rtc-lib code
 * Hans Ulli Kroll <ulli.kroll@googlemail.com>
 */

#include <linux/rtc.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <mach/hardware.h>

#define DRV_NAME		"rtc-gemini"

MODULE_DESCRIPTION("RTC driver for Gemini SoC");
MODULE_ALIAS("platform:" DRV_NAME);
MODULE_AUTHOR("Hans Ulli Kroll <ulli.kroll@googlemail.com>");
MODULE_LICENSE("GPL");

struct gemini_rtc {
	struct rtc_device	*dev;
	void __iomem		*base;
	int			irq;
};

enum gemini_rtc_offsets {
	GEMINI_RTC_SECOND	= 0x00,
	GEMINI_RTC_MINUTE	= 0x04,
	GEMINI_RTC_HOUR		= 0x08,
	GEMINI_RTC_DAYS		= 0x0C,
	GEMINI_RTC_ALARM_SECOND	= 0x10,
	GEMINI_RTC_ALARM_MINUTE	= 0x14,
	GEMINI_RTC_ALARM_HOUR	= 0x18,
	GEMINI_RTC_RECORD	= 0x1C,
	GEMINI_RTC_CR		= 0x20
};

static irqreturn_t gemini_rtc_interrupt(int irq, void *dev)
{
	return IRQ_HANDLED;
}

/*
 * Looks like the RTC in the Gemini SoC is (totaly) broken
 * We can't read/write directly the time from RTC registers.
 * We must do some "offset" calculation to get the real time
 *
 * The register "day" seams to be fixed, and the register "hour"
 * has his own mind.
 *
 * Maybe we can write directly the hour and days since EPOCH
 * but in this case the RTC will recalucate to some (other) strange values.
 * If you write time to the registers you will not read the same values.
 *
 * This FIX works pretty fine and Stormlinksemi aka Cortina-Networks does
 * the same thing, without the rtc-lib.c calls.
 */

static int gemini_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	struct gemini_rtc *rtc = dev_get_drvdata(dev);

	unsigned int  days, hour, min, sec;
	unsigned long offset, time;

	sec    = readl(rtc->base + GEMINI_RTC_SECOND);
	min    = readl(rtc->base + GEMINI_RTC_MINUTE);
	hour   = readl(rtc->base + GEMINI_RTC_HOUR);
	days   = readl(rtc->base + GEMINI_RTC_DAYS);
	offset = readl(rtc->base + GEMINI_RTC_RECORD);

	time = offset + days * 86400 + hour * 3600 + min * 60 + sec;

	rtc_time_to_tm(time, tm);
	return 0;
}

/*
 * Maybe there is some hidden register to care ?
 * looks like register GEMINI_RTC_DAY can count
 * 365 days * 179 years >= 65535 (uint16)
 */

static int gemini_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	struct gemini_rtc *rtc = dev_get_drvdata(dev);
	unsigned int sec, min, hour, day;
	unsigned long offset, time;

	if (tm->tm_year >= 2148)	/* EPOCH Year + 179 */
		return -EINVAL;

	rtc_tm_to_time(tm , &time);

	sec  = readl(rtc->base + GEMINI_RTC_SECOND);
	min  = readl(rtc->base + GEMINI_RTC_MINUTE);
	hour = readl(rtc->base + GEMINI_RTC_HOUR);
	day  = readl(rtc->base + GEMINI_RTC_DAYS);

	offset = time - (day*86400 + hour*3600 + min*60 + sec);

	writel(offset, rtc->base + GEMINI_RTC_RECORD);
	writel(0x01,   rtc->base + GEMINI_RTC_CR);
	return 0;
}

static struct rtc_class_ops gemini_rtc_ops = {
	.read_time     = gemini_rtc_read_time,
	.set_time      = gemini_rtc_set_time,
};

static int gemini_rtc_probe(struct platform_device *pdev)
{
	struct gemini_rtc *rtc;
	struct device *dev = &pdev->dev;
	struct resource *res;
	int ret;

	rtc = kzalloc(sizeof(*rtc), GFP_KERNEL);
	if (unlikely(!rtc))
		return -ENOMEM;
	platform_set_drvdata(pdev, rtc);

	rtc->irq = platform_get_irq(pdev, 0);
	if (rtc->irq < 0) {
		ret = -rtc->irq;
		goto err_mem;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		ret = -ENODEV;
		goto err_mem;
	}
	rtc->base = devm_ioremap(&pdev->dev, res->start,
				res->end - res->start + 1);

	ret = request_irq(rtc->irq, gemini_rtc_interrupt,
				IRQF_SHARED, pdev->name, dev);
	if (unlikely(ret))
		goto err_mem;

	rtc->dev =	rtc_device_register(pdev->name, dev,
				&gemini_rtc_ops, THIS_MODULE);
	if (unlikely(IS_ERR(rtc->dev))) {
		ret = PTR_ERR(rtc->dev);
		goto err_irq;
	}
	return 0;

err_irq:
	free_irq(rtc->irq, dev);

err_mem:
	kfree(rtc);
	return ret;
}

static int gemini_rtc_remove(struct platform_device *pdev)
{
	struct gemini_rtc *rtc = platform_get_drvdata(pdev);
	struct device *dev = &pdev->dev;

	free_irq(rtc->irq, dev);
	rtc_device_unregister(rtc->dev);
	platform_set_drvdata(pdev, NULL);
	kfree(rtc);

	return 0;
}

static struct platform_driver gemini_rtc_driver = {
	.driver		= {
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
	},
	.probe		= gemini_rtc_probe,
	.remove		= gemini_rtc_remove,
};

static int __init gemini_rtc_init(void)
{
	int	retval;

	retval = platform_driver_register(&gemini_rtc_driver);
	if (retval == 0)
		pr_info(DRV_NAME ": registered successfully");
	return retval;
}

static void __exit gemini_rtc_exit(void)
{
	platform_driver_unregister(&gemini_rtc_driver);
}

module_init(gemini_rtc_init);
module_exit(gemini_rtc_exit);
