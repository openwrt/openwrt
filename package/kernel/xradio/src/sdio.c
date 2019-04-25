/*
 * SDIO driver for XRadio drivers
 *
 * Copyright (c) 2013, XRadio
 * Author: XRadio
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/mmc/host.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/card.h>
#include <linux/mmc/sdio.h>
#include <asm/mach-types.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>

#include "xradio.h"
#include "sdio.h"
#include "main.h"

/* sdio vendor id and device id*/
#define SDIO_VENDOR_ID_XRADIO 0x0020
#define SDIO_DEVICE_ID_XRADIO 0x2281
static const struct sdio_device_id xradio_sdio_ids[] = {
	{ SDIO_DEVICE(SDIO_VENDOR_ID_XRADIO, SDIO_DEVICE_ID_XRADIO) },
	//{ SDIO_DEVICE(SDIO_ANY_ID, SDIO_ANY_ID) },
	{ /* end: all zeroes */			},
};

/* sbus_ops implemetation */
int sdio_data_read(struct xradio_common* self, unsigned int addr,
                          void *dst, int count)
{
	int ret = sdio_memcpy_fromio(self->sdio_func, dst, addr, count);
//	printk("sdio_memcpy_fromio 0x%x:%d ret %d\n", addr, count, ret);
//	print_hex_dump_bytes("sdio read ", 0, dst, min(count,32));
	return ret;
}

int sdio_data_write(struct xradio_common* self, unsigned int addr,
                           const void *src, int count)
{
	int ret = sdio_memcpy_toio(self->sdio_func, addr, (void *)src, count);
//	printk("sdio_memcpy_toio 0x%x:%d ret %d\n", addr, count, ret);
//	print_hex_dump_bytes("sdio write", 0, src, min(count,32));
	return ret;
}

void sdio_lock(struct xradio_common* self)
{
	sdio_claim_host(self->sdio_func);
}

void sdio_unlock(struct xradio_common *self)
{
	sdio_release_host(self->sdio_func);
}

size_t sdio_align_len(struct xradio_common *self, size_t size)
{
	return sdio_align_size(self->sdio_func, size);
}

int sdio_set_blk_size(struct xradio_common *self, size_t size)
{
	return sdio_set_block_size(self->sdio_func, size);
}

extern void xradio_irq_handler(struct xradio_common*);

static irqreturn_t sdio_irq_handler(int irq, void *dev_id)
{
	struct sdio_func *func = (struct sdio_func*) dev_id;
	struct xradio_common *self = sdio_get_drvdata(func);
	if (self != NULL)
		xradio_irq_handler(self);
	return IRQ_HANDLED;
}

static int sdio_enableint(struct sdio_func* func)
{
	int ret = 0;
	u8 cccr;
	int func_num;

	sdio_claim_host(func);

	/* Hack to access Fuction-0 */
	func_num = func->num;
	func->num = 0;
	cccr = sdio_readb(func, SDIO_CCCR_IENx, &ret);
	cccr |= BIT(0); /* Master interrupt enable ... */
	cccr |= BIT(func_num); /* ... for our function */
	sdio_writeb(func, cccr, SDIO_CCCR_IENx, &ret);

	/* Restore the WLAN function number */
	func->num = func_num;

	sdio_release_host(func);

	return ret;
}

int sdio_pm(struct xradio_common *self, bool  suspend)
{
	int ret = 0;
	if (suspend) {
		/* Notify SDIO that XRADIO will remain powered during suspend */
		ret = sdio_set_host_pm_flags(self->sdio_func, MMC_PM_KEEP_POWER);
		if (ret)
			dev_dbg(&self->sdio_func->dev, "Error setting SDIO pm flags: %i\n", ret);
	}

	return ret;
}

static const struct of_device_id xradio_sdio_of_match_table[] = {
	{ .compatible = "xradio,xr819" },
	{ }
};

static int xradio_probe_of(struct sdio_func *func)
{
	struct device *dev = &func->dev;
	struct device_node *np = dev->of_node;
	const struct of_device_id *of_id;
	int irq;
	int ret;

	of_id = of_match_node(xradio_sdio_of_match_table, np);
	if (!of_id)
		return -ENODEV;

	//pdev_data->family = of_id->data;

	irq = irq_of_parse_and_map(np, 0);
	if (!irq) {
		dev_err(dev, "No irq in platform data\n");
		return -EINVAL;
	}

	ret = devm_request_irq(dev, irq, sdio_irq_handler, 0, "xradio", func);
	if (ret) {
		dev_err(dev, "Failed to request irq_wakeup.\n");
		return -EINVAL;
	}

	return 0;
}

/* Probe Function to be called by SDIO stack when device is discovered */
static int sdio_probe(struct sdio_func *func,
                      const struct sdio_device_id *id)
{
	dev_dbg(&func->dev, "XRadio Device:sdio clk=%d\n",
	            func->card->host->ios.clock);
	dev_dbg(&func->dev, "sdio func->class=%x\n", func->class);
	dev_dbg(&func->dev, "sdio_vendor: 0x%04x\n", func->vendor);
	dev_dbg(&func->dev, "sdio_device: 0x%04x\n", func->device);
	dev_dbg(&func->dev, "Function#: 0x%04x\n",   func->num);

#if 0  //for odly and sdly debug.
{
	u32 sdio_param = 0;
	sdio_param = readl(__io_address(0x01c20088));
	sdio_param &= ~(0xf<<8);
	sdio_param |= 3<<8;
	sdio_param &= ~(0xf<<20);
	sdio_param |= s_dly<<20;
	writel(sdio_param, __io_address(0x01c20088));
	sbus_printk(XRADIO_DBG_ALWY, "%s: 0x01c20088=0x%08x\n", __func__, sdio_param);
}
#endif

	xradio_probe_of(func);

	func->card->quirks |= MMC_QUIRK_BROKEN_BYTE_MODE_512;
	sdio_claim_host(func);
	sdio_enable_func(func);
	sdio_release_host(func);

	sdio_enableint(func);

	xradio_core_init(func);

	try_module_get(func->dev.driver->owner);
	return 0;
}
/* Disconnect Function to be called by SDIO stack when
 * device is disconnected */
static void sdio_remove(struct sdio_func *func)
{
	sdio_claim_host(func);
	sdio_disable_func(func);
	sdio_release_host(func);
	module_put(func->dev.driver->owner);
}

static int sdio_suspend(struct device *dev)
{
	int ret = 0;
	/*
	struct sdio_func *func = dev_to_sdio_func(dev);
	ret = sdio_set_host_pm_flags(func, MMC_PM_KEEP_POWER);
	if (ret)
		sbus_printk(XRADIO_DBG_ERROR, "set MMC_PM_KEEP_POWER error\n");
	*/
	return ret;
}

static int sdio_resume(struct device *dev)
{
	return 0;
}

static const struct dev_pm_ops sdio_pm_ops = {
	.suspend = sdio_suspend,
	.resume  = sdio_resume,
};

static struct sdio_driver sdio_driver = {
	.name     = "xradio_wlan",
	.id_table = xradio_sdio_ids,
	.probe    = sdio_probe,
	.remove   = sdio_remove,
	.drv = {
			.owner = THIS_MODULE,
			.pm = &sdio_pm_ops,
	}
};


int xradio_sdio_register(){
	return sdio_register_driver(&sdio_driver);
}

void xradio_sdio_unregister(){
	sdio_unregister_driver(&sdio_driver);
}

MODULE_DEVICE_TABLE(sdio, xradio_sdio_ids);
