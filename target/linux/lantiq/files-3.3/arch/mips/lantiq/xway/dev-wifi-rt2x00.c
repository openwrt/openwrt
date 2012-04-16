/*
 *  Copyright (C) 2011 John Crispin <blogic@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/rt2x00_platform.h>
#include <linux/pci.h>

#include "dev-wifi-rt2x00.h"

extern int (*ltqpci_plat_dev_init)(struct pci_dev *dev);
struct rt2x00_platform_data rt2x00_pdata;

static int
rt2x00_pci_plat_dev_init(struct pci_dev *dev)
{
	dev->dev.platform_data = &rt2x00_pdata;
	return 0;
}

void __init
ltq_register_rt2x00(const char *firmware)
{
	rt2x00_pdata.eeprom_file_name =  kstrdup(firmware, GFP_KERNEL);
	ltqpci_plat_dev_init = rt2x00_pci_plat_dev_init;
}
