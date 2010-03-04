/*
 * RDC321x GPIO driver
 *
 * Copyright (C) 2008, Volker Weiss <dev@tintuc.de>
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
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/platform_device.h>
#include <linux/pci.h>
#include <linux/gpio.h>

#include <asm/rdc321x_defs.h>

struct rdc321x_gpio {
	spinlock_t	lock;
	u32 		data_reg[2];
} rdc321x_gpio_dev;

extern int rdc321x_pci_write(int reg, u32 val);
extern int rdc321x_pci_read(int reg, u32 *val);

/* read GPIO pin */
static int rdc_gpio_get_value(struct gpio_chip *chip, unsigned gpio)
{
	u32 value = 0;
	int reg;

	reg = gpio < 32 ? RDC321X_GPIO_DATA_REG1 : RDC321X_GPIO_DATA_REG2;

	spin_lock(&rdc321x_gpio_dev.lock);
	rdc321x_pci_write(reg, rdc321x_gpio_dev.data_reg[gpio < 32 ? 0 : 1]);
	rdc321x_pci_read(reg, &value);
	spin_unlock(&rdc321x_gpio_dev.lock);

	return (1 << (gpio & 0x1f)) & value ? 1 : 0;
}

static void rdc_gpio_set_value_impl(struct gpio_chip *chip,
				unsigned gpio, int value)
{
	int reg = (gpio < 32) ? 0 : 1;

	if (value)
		rdc321x_gpio_dev.data_reg[reg] |= 1 << (gpio & 0x1f);
	else
		rdc321x_gpio_dev.data_reg[reg] &= ~(1 << (gpio & 0x1f));

	rdc321x_pci_write(reg ? RDC321X_GPIO_DATA_REG2 : RDC321X_GPIO_DATA_REG1,
			       rdc321x_gpio_dev.data_reg[reg]);
}

/* set GPIO pin to value */
static void rdc_gpio_set_value(struct gpio_chip *chip,
				unsigned gpio, int value)
{
	spin_lock(&rdc321x_gpio_dev.lock);
	rdc_gpio_set_value_impl(chip, gpio, value);
	spin_unlock(&rdc321x_gpio_dev.lock);
}

static int rdc_gpio_config(struct gpio_chip *chip,
				unsigned gpio, int value)
{
	int err;
	u32 reg;

	spin_lock(&rdc321x_gpio_dev.lock);
	err = rdc321x_pci_read(gpio < 32 ? RDC321X_GPIO_CTRL_REG1 : RDC321X_GPIO_CTRL_REG2,
										&reg);
	if (err)
		goto unlock;

	reg |= 1 << (gpio & 0x1f);

	err = rdc321x_pci_write(gpio < 32 ? RDC321X_GPIO_CTRL_REG1 : RDC321X_GPIO_CTRL_REG2,
										reg);
	if (err)
		goto unlock;

	rdc_gpio_set_value_impl(chip, gpio, value);

unlock:
	spin_unlock(&rdc321x_gpio_dev.lock);

	return err;
}

/* configure GPIO pin as input */
static int rdc_gpio_direction_input(struct gpio_chip *chip, unsigned gpio)
{
	return rdc_gpio_config(chip, gpio, 1);
}

static struct gpio_chip rdc321x_gpio_chip = {
	.label			= "rdc321x-gpio",
	.direction_input	= rdc_gpio_direction_input,
	.direction_output	= rdc_gpio_config,
	.get			= rdc_gpio_get_value,
	.set			= rdc_gpio_set_value,
	.base			= 0,
	.ngpio			= RDC321X_MAX_GPIO,
};

/* initially setup the 2 copies of the gpio data registers.
   This function is called before the platform setup code. */
static int __devinit rdc321x_gpio_probe(struct platform_device *pdev)
{
	int err;

	/* this might not be, what others (BIOS, bootloader, etc.)
	   wrote to these registers before, but it's a good guess. Still
	   better than just using 0xffffffff. */
	err = rdc321x_pci_read(RDC321X_GPIO_DATA_REG1, &rdc321x_gpio_dev.data_reg[0]);
	if (err)
		return err;

	err = rdc321x_pci_read(RDC321X_GPIO_DATA_REG2, &rdc321x_gpio_dev.data_reg[1]);
	if (err)
		return err;

	spin_lock_init(&rdc321x_gpio_dev.lock);

	printk(KERN_INFO "rdc321x: registering %d GPIOs\n", rdc321x_gpio_chip.ngpio);
	return gpiochip_add(&rdc321x_gpio_chip);
}

static int __devexit rdc321x_gpio_remove(struct platform_device *pdev)
{
	gpiochip_remove(&rdc321x_gpio_chip);
	return 0;
}

static struct platform_driver rdc321x_gpio_driver = {
	.driver.name	= "rdc321x-gpio",
	.driver.owner	= THIS_MODULE,
	.probe		= rdc321x_gpio_probe,
	.remove		= __devexit_p(rdc321x_gpio_remove),
};

static int __init rdc321x_gpio_init(void)
{
	return platform_driver_register(&rdc321x_gpio_driver);
}

static void __exit rdc321x_gpio_exit(void)
{
	platform_driver_unregister(&rdc321x_gpio_driver);
}

module_init(rdc321x_gpio_init);
module_exit(rdc321x_gpio_exit);

MODULE_AUTHOR("Florian Fainelli <florian@openwrt.org>");
MODULE_DESCRIPTION("RDC321x GPIO driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:rdc321x-gpio");
