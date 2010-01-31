/*
 *  Copyright (C) 2009, Lars-Peter Clausen <lars@metafoo.de>
 *  	JZ4720/JZ4740 SoC RTC driver
 *
 *  This program is free software; you can redistribute	 it and/or modify it
 *  under  the terms of	 the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the	License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/rtc.h>

#define JZ_REG_RTC_CTRL		0x00
#define JZ_REG_RTC_SEC		0x04
#define JZ_REG_RTC_SEC_ALARM	0x08
#define JZ_REG_RTC_REGULATOR	0x0C
#define JZ_REG_RTC_SCRATCHPAD	0x34

#define JZ_RTC_CTRL_WRDY	BIT(7)
#define JZ_RTC_CTRL_1HZ		BIT(6)
#define JZ_RTC_CTRL_1HZ_IRQ	BIT(5)
#define JZ_RTC_CTRL_AF		BIT(4)
#define JZ_RTC_CTRL_AF_IRQ	BIT(3)
#define JZ_RTC_CTRL_AE		BIT(2)
#define JZ_RTC_CTRL_ENABLE	BIT(0)

struct jz4740_rtc {
	struct resource *mem;
	void __iomem *base;

	struct rtc_device *rtc;

	unsigned int irq;

	spinlock_t lock;
};

static inline uint32_t jz4740_rtc_reg_read(struct jz4740_rtc *rtc, size_t reg)
{
	return readl(rtc->base + reg);
}

static inline void jz4740_rtc_wait_write_ready(struct jz4740_rtc *rtc)
{
	uint32_t ctrl;
	do {
		ctrl = jz4740_rtc_reg_read(rtc, JZ_REG_RTC_CTRL);
	} while (!(ctrl & JZ_RTC_CTRL_WRDY));
}


static inline void jz4740_rtc_reg_write(struct jz4740_rtc *rtc, size_t reg,
					uint32_t val)
{
	jz4740_rtc_wait_write_ready(rtc);
	writel(val, rtc->base + reg);
}

static void jz4740_rtc_ctrl_set_bits(struct jz4740_rtc *rtc, uint32_t mask,
					uint32_t val)
{
	unsigned long flags;
	uint32_t ctrl;

	spin_lock_irqsave(&rtc->lock, flags);

	ctrl = jz4740_rtc_reg_read(rtc, JZ_REG_RTC_CTRL);

	/* Don't clear interrupt flags by accident */
	ctrl |= JZ_RTC_CTRL_1HZ | JZ_RTC_CTRL_AF;

	ctrl &= ~mask;
	ctrl |= val;

	jz4740_rtc_reg_write(rtc, JZ_REG_RTC_CTRL, ctrl);

	spin_unlock_irqrestore(&rtc->lock, flags);
}

static inline struct jz4740_rtc *dev_to_rtc(struct device *dev)
{
	return dev_get_drvdata(dev);
}

static int jz4740_rtc_read_time(struct device *dev, struct rtc_time *time)
{
	struct jz4740_rtc *rtc = dev_to_rtc(dev);
	uint32_t secs, secs2;

	secs = jz4740_rtc_reg_read(rtc, JZ_REG_RTC_SEC);
	secs2 = jz4740_rtc_reg_read(rtc, JZ_REG_RTC_SEC);

	while (secs != secs2) {
		secs = secs2;
		secs2 = jz4740_rtc_reg_read(rtc, JZ_REG_RTC_SEC);
	}

	rtc_time_to_tm(secs, time);

	return rtc_valid_tm(time);
}

static int jz4740_rtc_set_mmss(struct device *dev, unsigned long secs)
{
	struct jz4740_rtc *rtc = dev_to_rtc(dev);

	if ((uint32_t)secs != secs)
		return -EINVAL;

	jz4740_rtc_reg_write(rtc, JZ_REG_RTC_SEC, secs);

	return 0;
}

static int jz4740_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct jz4740_rtc *rtc = dev_to_rtc(dev);
	uint32_t secs, secs2;
	uint32_t ctrl;

	secs = jz4740_rtc_reg_read(rtc, JZ_REG_RTC_SEC_ALARM);
	secs2 = jz4740_rtc_reg_read(rtc, JZ_REG_RTC_SEC_ALARM);

	while (secs != secs2){
		secs = secs2;
		secs2 = jz4740_rtc_reg_read(rtc, JZ_REG_RTC_SEC_ALARM);
	}

	ctrl = jz4740_rtc_reg_read(rtc, JZ_REG_RTC_CTRL);

	alrm->enabled = !!(ctrl & JZ_RTC_CTRL_AE);
	alrm->pending = !!(ctrl & JZ_RTC_CTRL_AF);

	rtc_time_to_tm(secs, &alrm->time);

	return rtc_valid_tm(&alrm->time);
}

static int jz4740_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct jz4740_rtc *rtc = dev_to_rtc(dev);
	unsigned long secs;

	rtc_tm_to_time(&alrm->time, &secs);

	if ((uint32_t)secs != secs)
		return -EINVAL;

	jz4740_rtc_reg_write(rtc, JZ_REG_RTC_SEC_ALARM, (uint32_t)secs);
	jz4740_rtc_ctrl_set_bits(rtc, JZ_RTC_CTRL_AE,
					alrm->enabled ? JZ_RTC_CTRL_AE : 0);

	return 0;
}

static int jz4740_rtc_update_irq_enable(struct device *dev, unsigned int enabled)
{
	struct jz4740_rtc *rtc = dev_to_rtc(dev);
	jz4740_rtc_ctrl_set_bits(rtc, JZ_RTC_CTRL_1HZ_IRQ,
					enabled ? JZ_RTC_CTRL_1HZ_IRQ : 0);
	return 0;
}


static int jz4740_rtc_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
	struct jz4740_rtc *rtc = dev_to_rtc(dev);
	jz4740_rtc_ctrl_set_bits(rtc, JZ_RTC_CTRL_AF_IRQ,
					enabled ? JZ_RTC_CTRL_AF_IRQ : 0);
	return 0;
}

static struct rtc_class_ops jz4740_rtc_ops = {
	.read_time	= jz4740_rtc_read_time,
	.set_mmss	= jz4740_rtc_set_mmss,
	.read_alarm	= jz4740_rtc_read_alarm,
	.set_alarm	= jz4740_rtc_set_alarm,
	.update_irq_enable = jz4740_rtc_update_irq_enable,
	.alarm_irq_enable = jz4740_rtc_alarm_irq_enable,
};

static irqreturn_t jz4740_rtc_irq(int irq, void *data)
{
	struct jz4740_rtc *rtc = data;
	uint32_t ctrl;
	unsigned long events = 0;
	ctrl = jz4740_rtc_reg_read(rtc, JZ_REG_RTC_CTRL);

	if (ctrl & JZ_RTC_CTRL_1HZ)
		events |= (RTC_UF | RTC_IRQF);

	if (ctrl & JZ_RTC_CTRL_AF)
		events |= (RTC_AF | RTC_IRQF);

	rtc_update_irq(rtc->rtc, 1, events);

	jz4740_rtc_ctrl_set_bits(rtc, JZ_RTC_CTRL_1HZ | JZ_RTC_CTRL_AF, 0);

	return IRQ_HANDLED;
}

static int __devinit jz4740_rtc_probe(struct platform_device *pdev)
{
	int ret;
	struct jz4740_rtc *rtc;
	uint32_t scratchpad;

	rtc = kmalloc(sizeof(*rtc), GFP_KERNEL);

	rtc->irq = platform_get_irq(pdev, 0);

	if (rtc->irq < 0) {
		ret = -ENOENT;
		dev_err(&pdev->dev, "Failed to get platform irq\n");
		goto err_free;
	}

	rtc->mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!rtc->mem) {
		ret = -ENOENT;
		dev_err(&pdev->dev, "Failed to get platform mmio memory\n");
		goto err_free;
	}

	rtc->mem = request_mem_region(rtc->mem->start, resource_size(rtc->mem),
					pdev->name);

	if (!rtc->mem) {
		ret = -EBUSY;
		dev_err(&pdev->dev, "Failed to request mmio memory region\n");
		goto err_free;
	}

	rtc->base = ioremap_nocache(rtc->mem->start, resource_size(rtc->mem));

	if (!rtc->base) {
		ret = -EBUSY;
		dev_err(&pdev->dev, "Failed to ioremap mmio memory\n");
		goto err_release_mem_region;
	}

	platform_set_drvdata(pdev, rtc);

	rtc->rtc = rtc_device_register(pdev->name, &pdev->dev, &jz4740_rtc_ops,
					THIS_MODULE);

	if (IS_ERR(rtc->rtc)) {
		ret = PTR_ERR(rtc->rtc);
		dev_err(&pdev->dev, "Failed to register rtc device: %d\n", ret);
		goto err_iounmap;
	}

	ret = request_irq(rtc->irq, jz4740_rtc_irq, 0,
				pdev->name,  rtc);

	if (ret) {
		dev_err(&pdev->dev, "Failed to request rtc irq: %d\n", ret);
		goto err_unregister_rtc;
	}

	scratchpad = jz4740_rtc_reg_read(rtc, JZ_REG_RTC_SCRATCHPAD);
	if (scratchpad != 0x12345678) {
		jz4740_rtc_reg_write(rtc, JZ_REG_RTC_SCRATCHPAD, 0x12345678);
		jz4740_rtc_reg_write(rtc, JZ_REG_RTC_SEC, 0);
	}

	return 0;

err_unregister_rtc:
	rtc_device_unregister(rtc->rtc);
err_iounmap:
	platform_set_drvdata(pdev, NULL);
	iounmap(rtc->base);
err_release_mem_region:
	release_mem_region(rtc->mem->start, resource_size(rtc->mem));
err_free:
	kfree(rtc);

	return ret;
}

static int __devexit jz4740_rtc_remove(struct platform_device *pdev)
{
	struct jz4740_rtc *rtc = platform_get_drvdata(pdev);

	rtc_device_unregister(rtc->rtc);

	iounmap(rtc->base);
	release_mem_region(rtc->mem->start, resource_size(rtc->mem));

	kfree(rtc);

	platform_set_drvdata(pdev, NULL);

	return 0;
}

struct platform_driver jz4740_rtc_driver = {
	.probe = jz4740_rtc_probe,
	.remove = __devexit_p(jz4740_rtc_remove),
	.driver = {
		.name = "jz4740-rtc",
		.owner = THIS_MODULE,
	},
};

static int __init jz4740_rtc_init(void)
{
	return platform_driver_register(&jz4740_rtc_driver);
}
module_init(jz4740_rtc_init);

static void __exit jz4740_rtc_exit(void)
{
	platform_driver_unregister(&jz4740_rtc_driver);
}
module_exit(jz4740_rtc_exit);

MODULE_AUTHOR("Lars-Peter Clausen <lars@metafoo.de>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("RTC driver for the JZ4720/JZ4740 SoC\n");
MODULE_ALIAS("platform:jz4740-rtc");
MODULE_ALIAS("platform:jz4720-rtc");
