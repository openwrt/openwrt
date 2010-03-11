/*
 * RDC321x southbrige driver
 *
 * Copyright (C) 2007-2010 Florian Fainelli <florian@openwrt.org>
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/pci.h>

#include <asm/rdc321x_defs.h>

static struct pci_dev *rdc321x_sb_pdev;

/*
 * Unlocked PCI configuration space accessors
 */
int rdc321x_pci_read(int reg, u32 *val)
{
	int err;

	err = pci_read_config_dword(rdc321x_sb_pdev, reg, val);
	if (err)
		return err;

	return err;
}
EXPORT_SYMBOL(rdc321x_pci_read);

int rdc321x_pci_write(int reg, u32 val)
{
	int err;

	err = pci_write_config_dword(rdc321x_sb_pdev, reg, val);
	if (err)
		return err;

	return err;
}
EXPORT_SYMBOL(rdc321x_pci_write);

static struct platform_device rdc321x_wdt_device = {
	.name	= "rdc321x-wdt"
};

static struct platform_device rdc321x_gpio_device = {
	.name	= "rdc321x-gpio"
};

static int __devinit rdc321x_sb_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	int err;

	err = pci_enable_device(pdev);
	if (err) {
		printk(KERN_ERR "failed to enable device\n");
		return err;
	}

	rdc321x_sb_pdev = pdev;

	err = platform_device_register(&rdc321x_wdt_device);
	if (err) {
		dev_err(&pdev->dev, "failed to register watchdog\n");
		return err;
	}

	panic_on_unrecovered_nmi = 1;

	err = platform_device_register(&rdc321x_gpio_device);
	if (err) {
		dev_err(&pdev->dev, "failed to register gpiochip\n");
		return err;
	}
	dev_info(&rdc321x_sb_pdev->dev, "RDC321x southhridge registered\n");

	return err;
}

static struct pci_device_id rdc321x_sb_table[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_RDC, PCI_DEVICE_ID_RDC_R6030) },
	{}
};

static struct pci_driver rdc321x_sb_driver = {
	.name = "RDC3210 Southbridge",
	.id_table = rdc321x_sb_table,
	.probe = rdc321x_sb_probe
};

static int __init rdc321x_sb_init(void)
{
	return pci_register_driver(&rdc321x_sb_driver);
}

device_initcall(rdc321x_sb_init);
