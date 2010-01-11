/*
 * Copyright (C) 2009, Lars-Peter Clausen <lars@metafoo.de>
 *		JZ4720/JZ4740 SoC ADC driver
 *
 * This program is free software; you can redistribute	 it and/or modify it
 * under  the terms of	 the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the	License, or (at your
 * option) any later version.
 *
 * You should have received a copy of the  GNU General Public License along
 * with this program; if not, write  to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * This driver is meant to synchronize access to the adc core for the battery
 * and touchscreen driver. Thus these drivers should use the adc driver as a
 * parent.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/jz4740-adc.h>

#define JZ_REG_ADC_ENABLE	0x00
#define JZ_REG_ADC_CFG  	0x04
#define JZ_REG_ADC_CTRL		0x08
#define JZ_REG_ADC_STATUS	0x0C
#define JZ_REG_ADC_SAME		0x10
#define JZ_REG_ADC_WAIT		0x14
#define JZ_REG_ADC_TOUCH	0x18
#define JZ_REG_ADC_BATTERY	0x1C
#define JZ_REG_ADC_ADCIN	0x20

#define JZ_ADC_ENABLE_TOUCH		BIT(2)
#define JZ_ADC_ENABLE_BATTERY		BIT(1)
#define JZ_ADC_ENABLE_ADCIN		BIT(0)

#define JZ_ADC_CFG_SPZZ			BIT(31)
#define JZ_ADC_CFG_EX_IN		BIT(30)
#define JZ_ADC_CFG_DNUM_MASK		(0x7 << 16)
#define JZ_ADC_CFG_DMA_ENABLE		BIT(15)
#define JZ_ADC_CFG_XYZ_MASK		(0x2 << 13)
#define JZ_ADC_CFG_SAMPLE_NUM_MASK	(0x7 << 10)
#define JZ_ADC_CFG_CLKDIV		(0xf << 5)
#define JZ_ADC_CFG_BAT_MB		BIT(4)

#define JZ_ADC_CFG_DNUM_OFFSET		16
#define JZ_ADC_CFG_XYZ_OFFSET		13
#define JZ_ADC_CFG_SAMPLE_NUM_OFFSET	10
#define JZ_ADC_CFG_CLKDIV_OFFSET	5

#define JZ_ADC_IRQ_PENDOWN		BIT(4)
#define JZ_ADC_IRQ_PENUP		BIT(3)
#define JZ_ADC_IRQ_TOUCH		BIT(2)
#define JZ_ADC_IRQ_BATTERY		BIT(1)
#define JZ_ADC_IRQ_ADCIN		BIT(0)

#define JZ_ADC_TOUCH_TYPE1		BIT(31)
#define JZ_ADC_TOUCH_DATA1_MASK		0xfff
#define JZ_ADC_TOUCH_TYPE0		BIT(15)
#define JZ_ADC_TOUCH_DATA0_MASK		0xfff

#define JZ_ADC_BATTERY_MASK		0xfff

#define JZ_ADC_ADCIN_MASK		0xfff

struct jz4740_adc {
	struct resource *mem;
	void __iomem *base;

	int irq;

	struct completion bat_completion;
	struct completion adc_completion;

	spinlock_t lock;
};

static irqreturn_t jz4740_adc_irq(int irq, void *data)
{
	struct jz4740_adc *adc = data;
	uint8_t status;

	status = readb(adc->base + JZ_REG_ADC_STATUS);

	if (status & JZ_ADC_IRQ_BATTERY)
		complete(&adc->bat_completion);
	if (status & JZ_ADC_IRQ_ADCIN)
		complete(&adc->adc_completion);

	writeb(0xff, adc->base + JZ_REG_ADC_STATUS);

	return IRQ_HANDLED;
}

static void jz4740_adc_enable_irq(struct jz4740_adc *adc, int irq)
{
	unsigned long flags;
	uint8_t val;

	spin_lock_irqsave(&adc->lock, flags);

	val = readb(adc->base + JZ_REG_ADC_CTRL);
	val &= ~irq;
	writeb(val, adc->base + JZ_REG_ADC_CTRL);

	spin_unlock_irqrestore(&adc->lock, flags);
}

static void jz4740_adc_disable_irq(struct jz4740_adc *adc, int irq)
{
	unsigned long flags;
	uint8_t val;

	spin_lock_irqsave(&adc->lock, flags);

	val = readb(adc->base + JZ_REG_ADC_CTRL);
	val |= irq;
	writeb(val, adc->base + JZ_REG_ADC_CTRL);

	spin_unlock_irqrestore(&adc->lock, flags);
}

static void jz4740_adc_enable_adc(struct jz4740_adc *adc, int engine)
{
	unsigned long flags;
	uint8_t val;

	spin_lock_irqsave(&adc->lock, flags);

	val = readb(adc->base + JZ_REG_ADC_ENABLE);
	val |= engine;
	writeb(val, adc->base + JZ_REG_ADC_ENABLE);

	spin_unlock_irqrestore(&adc->lock, flags);
}

static void jz4740_adc_disable_adc(struct jz4740_adc *adc, int engine)
{
	unsigned long flags;
	uint8_t val;

	spin_lock_irqsave(&adc->lock, flags);

	val = readb(adc->base + JZ_REG_ADC_ENABLE);
	val &= ~engine;
	writeb(val, adc->base + JZ_REG_ADC_ENABLE);

	spin_unlock_irqrestore(&adc->lock, flags);
}

static inline void jz4740_adc_set_cfg(struct jz4740_adc *adc, uint32_t mask,
uint32_t val)
{
	unsigned long flags;
	uint32_t cfg;

	spin_lock_irqsave(&adc->lock, flags);

	cfg = readl(adc->base + JZ_REG_ADC_CFG);

	cfg &= ~mask;
	cfg |= val;

	writel(cfg, adc->base + JZ_REG_ADC_CFG);

	spin_unlock_irqrestore(&adc->lock, flags);
}

long jz4740_adc_read_battery_voltage(struct device *dev,
						enum jz_adc_battery_scale scale)
{
	struct jz4740_adc *adc = dev_get_drvdata(dev);
	unsigned long t;
	long long voltage;
	uint16_t val;

	if (!adc)
		return -ENODEV;

	if (scale == JZ_ADC_BATTERY_SCALE_2V5)
		jz4740_adc_set_cfg(adc, JZ_ADC_CFG_BAT_MB, JZ_ADC_CFG_BAT_MB);
	else
		jz4740_adc_set_cfg(adc, JZ_ADC_CFG_BAT_MB, 0);

	jz4740_adc_enable_irq(adc, JZ_ADC_IRQ_BATTERY);
	jz4740_adc_enable_adc(adc, JZ_ADC_ENABLE_BATTERY);

	t = wait_for_completion_interruptible_timeout(&adc->bat_completion,
							HZ);

	jz4740_adc_disable_irq(adc, JZ_ADC_IRQ_BATTERY);

	if (t <= 0) {
		jz4740_adc_disable_adc(adc, JZ_ADC_ENABLE_BATTERY);
		return t ? t : -ETIMEDOUT;
	}

	val = readw(adc->base + JZ_REG_ADC_BATTERY);

	if (scale == JZ_ADC_BATTERY_SCALE_2V5)
		voltage = (((long long)val) * 2500000LL) >> 12LL;
	else
		voltage = ((((long long)val) * 7395000LL) >> 12LL) + 33000LL;

	return voltage;
}
EXPORT_SYMBOL_GPL(jz4740_adc_read_battery_voltage);

static ssize_t jz4740_adc_read_adcin(struct device *dev,
					struct device_attribute *dev_attr,
					char *buf)
{
	struct jz4740_adc *adc = dev_get_drvdata(dev);
	unsigned long t;
	uint16_t val;

	jz4740_adc_enable_irq(adc, JZ_ADC_IRQ_ADCIN);
	jz4740_adc_enable_adc(adc, JZ_ADC_ENABLE_ADCIN);

	t = wait_for_completion_interruptible_timeout(&adc->adc_completion,
							HZ);

	jz4740_adc_disable_irq(adc, JZ_ADC_IRQ_ADCIN);

	if (t <= 0) {
		jz4740_adc_disable_adc(adc, JZ_ADC_ENABLE_ADCIN);
		return t ? t : -ETIMEDOUT;
	}

	val = readw(adc->base + JZ_REG_ADC_ADCIN);

	return sprintf(buf, "%d\n", val);
}

static DEVICE_ATTR(adcin, S_IRUGO, jz4740_adc_read_adcin, NULL);

static int __devinit jz4740_adc_probe(struct platform_device *pdev)
{
	int ret;
	struct jz4740_adc *adc;

	adc = kmalloc(sizeof(*adc), GFP_KERNEL);

	adc->irq = platform_get_irq(pdev, 0);

	if (adc->irq < 0) {
		ret = adc->irq;
		dev_err(&pdev->dev, "Failed to get platform irq: %d\n", ret);
		goto err_free;
	}

	adc->mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	if (!adc->mem) {
		ret = -ENOENT;
		dev_err(&pdev->dev, "Failed to get platform mmio resource\n");
		goto err_free;
	}

	adc->mem = request_mem_region(adc->mem->start, resource_size(adc->mem),
					pdev->name);

	if (!adc->mem) {
		ret = -EBUSY;
		dev_err(&pdev->dev, "Failed to request mmio memory region\n");
		goto err_free;
	}

	adc->base = ioremap_nocache(adc->mem->start, resource_size(adc->mem));

	if (!adc->base) {
		ret = -EBUSY;
		dev_err(&pdev->dev, "Failed to ioremap mmio memory\n");
		goto err_release_mem_region;
	}


	init_completion(&adc->bat_completion);
	init_completion(&adc->adc_completion);

	spin_lock_init(&adc->lock);

	platform_set_drvdata(pdev, adc);

	ret = request_irq(adc->irq, jz4740_adc_irq, 0, pdev->name, adc);

	if (ret) {
		dev_err(&pdev->dev, "Failed to request irq: %d\n", ret);
		goto err_iounmap;
	}

	ret = device_create_file(&pdev->dev, &dev_attr_adcin);
	if (ret) {
		dev_err(&pdev->dev, "Failed to create sysfs file: %d\n", ret);
		goto err_free_irq;
	}

	writeb(0x00, adc->base + JZ_REG_ADC_ENABLE);
	writeb(0xff, adc->base + JZ_REG_ADC_CTRL);

	return 0;

err_free_irq:
	free_irq(adc->irq, adc);
err_iounmap:
	platform_set_drvdata(pdev, NULL);
	iounmap(adc->base);
err_release_mem_region:
	release_mem_region(adc->mem->start, resource_size(adc->mem));
err_free:
	kfree(adc);

	return ret;
}

static int __devexit jz4740_adc_remove(struct platform_device *pdev)
{
	struct jz4740_adc *adc = platform_get_drvdata(pdev);

	device_remove_file(&pdev->dev, &dev_attr_adcin);

	free_irq(adc->irq, adc);

	iounmap(adc->base);
	release_mem_region(adc->mem->start, resource_size(adc->mem));

	platform_set_drvdata(pdev, NULL);

	kfree(adc);

	return 0;
}

struct platform_driver jz4740_adc_driver = {
	.probe	= jz4740_adc_probe,
	.remove = jz4740_adc_remove,
	.driver = {
		.name = "jz4740-adc",
		.owner = THIS_MODULE,
	},
};

static int __init jz4740_adc_init(void)
{
	return platform_driver_register(&jz4740_adc_driver);
}
module_init(jz4740_adc_init);

static void __exit jz4740_adc_exit(void)
{
	platform_driver_unregister(&jz4740_adc_driver);
}
module_exit(jz4740_adc_exit);

MODULE_DESCRIPTION("JZ4720/JZ4740 SoC ADC driver");
MODULE_AUTHOR("Lars-Peter Clausen <lars@metafoo.de>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:jz4740-adc");
MODULE_ALIAS("platform:jz4720-adc");
