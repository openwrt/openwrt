/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/kobject.h>
#include <linux/workqueue.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <linux/platform_device.h>
#include <net/sock.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/semaphore.h>

#include <lantiq_soc.h>
#include <svip_mux.h>
#include <base_reg.h>
#include <port_reg.h>

#define DRV_NAME			"ifxmips_gpio"

int gpio_to_irq(unsigned int gpio)
{
	return -EINVAL;
}
EXPORT_SYMBOL(gpio_to_irq);

int irq_to_gpio(unsigned int gpio)
{
	return -EINVAL;
}
EXPORT_SYMBOL(irq_to_gpio);

struct ltq_port_base {
	struct svip_reg_port *base;
	u32 pins;
};

/* Base addresses for ports */
static const struct ltq_port_base ltq_port_base[] = {
	{ (struct svip_reg_port *)LTQ_PORT_P0_BASE, 20 },
	{ (struct svip_reg_port *)LTQ_PORT_P1_BASE, 20 },
	{ (struct svip_reg_port *)LTQ_PORT_P2_BASE, 19 },
	{ (struct svip_reg_port *)LTQ_PORT_P3_BASE, 20 },
	{ (struct svip_reg_port *)LTQ_PORT_P4_BASE, 24 }
};

#define MAX_PORTS		ARRAY_SIZE(ltq_port_base)
#define PINS_PER_PORT(port)	(ltq_port_base[port].pins)

static inline
void ltq_port_set_exintcr0(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return;

	port_w32(port_r32(ltq_port_base[port].base->exintcr0) | (1 << pin),
		 ltq_port_base[port].base->exintcr0);
}

static inline
void ltq_port_clear_exintcr0(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return;

	port_w32(port_r32(ltq_port_base[port].base->exintcr0) & ~(1 << pin),
		 ltq_port_base[port].base->exintcr0);
}

static inline
void ltq_port_set_exintcr1(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return;

	port_w32(port_r32(ltq_port_base[port].base->exintcr1) | (1 << pin),
		 ltq_port_base[port].base->exintcr1);
}

static inline
void ltq_port_clear_exintcr1(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return;

	port_w32(port_r32(ltq_port_base[port].base->exintcr1) & ~(1 << pin),
		 ltq_port_base[port].base->exintcr1);
}

static inline
void ltq_port_set_irncfg(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return;

	port_w32(port_r32(ltq_port_base[port].base->irncfg) | (1 << pin),
		 ltq_port_base[port].base->irncfg);
}

static inline
void ltq_port_clear_irncfg(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return;

	port_w32(port_r32(ltq_port_base[port].base->irncfg) & ~(1 << pin),
		 ltq_port_base[port].base->irncfg);
}

static inline
void ltq_port_set_irnen(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return;

	port_w32(1 << pin, ltq_port_base[port].base->irnenset);
}

static inline
void ltq_port_clear_irnen(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return;

	port_w32(1 << pin, ltq_port_base[port].base->irnenclr);
}

static inline
void ltq_port_set_dir_out(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return;

	port_w32(port_r32(ltq_port_base[port].base->dir) | (1 << pin),
		 ltq_port_base[port].base->dir);
}

static inline
void ltq_port_set_dir_in(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return;

	port_w32(port_r32(ltq_port_base[port].base->dir) & ~(1 << pin),
		 ltq_port_base[port].base->dir);
}

static inline
void ltq_port_set_output(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return;

	port_w32(port_r32(ltq_port_base[port].base->out) | (1 << pin),
		 ltq_port_base[port].base->out);
}

static inline
void ltq_port_clear_output(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return;

	port_w32(port_r32(ltq_port_base[port].base->out) & ~(1 << pin),
		 ltq_port_base[port].base->out);
}

static inline
int ltq_port_get_input(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return -EINVAL;

	return (port_r32(ltq_port_base[port].base->in) & (1 << pin)) == 0;
}

static inline
void ltq_port_set_puen(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return;

	port_w32(port_r32(ltq_port_base[port].base->puen) | (1 << pin),
		 ltq_port_base[port].base->puen);
}

static inline
void ltq_port_clear_puen(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return;

	port_w32(port_r32(ltq_port_base[port].base->puen) & ~(1 << pin),
		 ltq_port_base[port].base->puen);
}

static inline
void ltq_port_set_altsel0(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return;

	port_w32(port_r32(ltq_port_base[port].base->altsel0) | (1 << pin),
		 ltq_port_base[port].base->altsel0);
}

static inline
void ltq_port_clear_altsel0(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return;

	port_w32(port_r32(ltq_port_base[port].base->altsel0) & ~(1 << pin),
		 ltq_port_base[port].base->altsel0);
}

static inline
void ltq_port_set_altsel1(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return;

	port_w32(port_r32(ltq_port_base[port].base->altsel1) | (1 << pin),
		 ltq_port_base[port].base->altsel1);
}

static inline
void ltq_port_clear_altsel1(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return;

	port_w32(port_r32(ltq_port_base[port].base->altsel1) & ~(1 << pin),
		 ltq_port_base[port].base->altsel1);
}

void ltq_gpio_configure(int port, int pin, bool dirin, bool puen,
			bool altsel0, bool altsel1)
{
	if (dirin)
		ltq_port_set_dir_in(port, pin);
	else
		ltq_port_set_dir_out(port, pin);

	if (puen)
		ltq_port_set_puen(port, pin);
	else
		ltq_port_clear_puen(port, pin);

	if (altsel0)
		ltq_port_set_altsel0(port, pin);
	else
		ltq_port_clear_altsel0(port, pin);

	if (altsel1)
		ltq_port_set_altsel1(port, pin);
	else
		ltq_port_clear_altsel1(port, pin);
}

int ltq_port_get_dir(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return -EINVAL;

	return (port_r32(ltq_port_base[port].base->dir) & (1 << pin)) != 0;
}

int ltq_port_get_puden(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return -EINVAL;

	return (port_r32(ltq_port_base[port].base->puen) & (1 << pin)) != 0;
}

int ltq_port_get_altsel0(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return -EINVAL;

	return (port_r32(ltq_port_base[port].base->altsel0) & (1 << pin)) != 0;
}

int ltq_port_get_altsel1(unsigned int port, unsigned int pin)
{
	if (port >= MAX_PORTS || pin >= PINS_PER_PORT(port))
		return -EINVAL;

	return (port_r32(ltq_port_base[port].base->altsel1) & (1 << pin)) != 0;
}

struct ltq_gpio_port {
	struct gpio_chip gpio_chip;
	unsigned int irq_base;
	unsigned int chained_irq;
};

static struct ltq_gpio_port ltq_gpio_port[MAX_PORTS];

static int gpio_exported;
static int __init gpio_export_setup(char *str)
{
	get_option(&str, &gpio_exported);
	return 1;
}
__setup("gpio_exported=", gpio_export_setup);

static inline unsigned int offset2port(unsigned int offset)
{
	unsigned int i;
	unsigned int prev = 0;

	for (i = 0; i < ARRAY_SIZE(ltq_port_base); i++) {
		if (offset >= prev &&
		    offset < prev + ltq_port_base[i].pins)
			return i;

		prev = ltq_port_base[i].pins;
	}

	return 0;
}

static inline unsigned int offset2pin(unsigned int offset)
{
	unsigned int i;
	unsigned int prev = 0;

	for (i = 0; i < ARRAY_SIZE(ltq_port_base); i++) {
		if (offset >= prev &&
		    offset < prev + ltq_port_base[i].pins)
			return offset - prev;

		prev = ltq_port_base[i].pins;
	}

	return 0;
}

static int ltq_gpio_direction_input(struct gpio_chip *chip, unsigned int offset)
{
	ltq_port_set_dir_in(offset2port(offset), offset2pin(offset));
	return 0;
}

static int ltq_gpio_direction_output(struct gpio_chip *chip,
				     unsigned int offset, int value)
{
	ltq_port_set_dir_out(offset2port(offset), offset2pin(offset));
	return 0;
}

static int ltq_gpio_get(struct gpio_chip *chip, unsigned int offset)
{
	return ltq_port_get_input(offset2port(offset), offset2pin(offset));
}

static void ltq_gpio_set(struct gpio_chip *chip, unsigned int offset, int value)
{
	if (value)
		ltq_port_set_output(offset2port(offset), offset2pin(offset));
	else
		ltq_port_clear_output(offset2port(offset), offset2pin(offset));
}

static int svip_gpio_request(struct gpio_chip *chip, unsigned offset)
{
	return 0;
}

static void ltq_gpio_free(struct gpio_chip *chip, unsigned offset)
{
}

static int ltq_gpio_probe(struct platform_device *pdev)
{
	int ret = 0;
	struct ltq_gpio_port *gpio_port;

	if (pdev->id >= MAX_PORTS)
		return -ENODEV;

	gpio_port = &ltq_gpio_port[pdev->id];
	gpio_port->gpio_chip.label = "ltq-gpio";

	gpio_port->gpio_chip.direction_input = ltq_gpio_direction_input;
	gpio_port->gpio_chip.direction_output = ltq_gpio_direction_output;
	gpio_port->gpio_chip.get = ltq_gpio_get;
	gpio_port->gpio_chip.set = ltq_gpio_set;
	gpio_port->gpio_chip.request = svip_gpio_request;
	gpio_port->gpio_chip.free = ltq_gpio_free;
	gpio_port->gpio_chip.base = 100 * pdev->id;
	gpio_port->gpio_chip.ngpio = 32;
	gpio_port->gpio_chip.dev = &pdev->dev;
	gpio_port->gpio_chip.exported = gpio_exported;

	ret = gpiochip_add(&gpio_port->gpio_chip);
	if (ret < 0) {
		dev_err(&pdev->dev, "Could not register gpiochip %d, %d\n",
			pdev->id, ret);
		goto err;
	}
	platform_set_drvdata(pdev, gpio_port);

	return 0;

err:
	return ret;
}

static int ltq_gpio_remove(struct platform_device *pdev)
{
	struct ltq_gpio_port *gpio_port = platform_get_drvdata(pdev);
	int ret;

	ret = gpiochip_remove(&gpio_port->gpio_chip);

	return ret;
}

static struct platform_driver ltq_gpio_driver = {
	.probe = ltq_gpio_probe,
	.remove = __devexit_p(ltq_gpio_remove),
	.driver = {
		.name = DRV_NAME,
		.owner = THIS_MODULE,
	},
};

int __init ltq_gpio_init(void)
{
	int ret = platform_driver_register(&ltq_gpio_driver);
	if (ret)
		printk(KERN_INFO DRV_NAME
		       ": Error registering platform driver!");
	return ret;
}

postcore_initcall(ltq_gpio_init);

/**
 * Convert interrupt number to corresponding port/pin pair
 * Returns the port/pin pair serving the selected external interrupt;
 * needed since mapping not linear.
 *
 * \param exint     External interrupt number
 * \param port      Pointer for resulting port
 * \param pin       Pointer for resutling pin
 * \return -EINVAL  Invalid exint
 * \return 0        port/pin updated
 * \ingroup API
 */
static int ltq_exint2port(u32 exint, int *port, int *pin)
{
	if ((exint >= 0) && (exint <= 10)) {
		*port = 0;
		*pin  = exint + 7;
	} else if ((exint >= 11) && (exint <= 14)) {
		*port = 1;
		*pin  = 18 - (exint - 11) ;
	} else if (exint == 15) {
		*port = 1;
		*pin  = 19;
	} else if (exint == 16) {
		*port = 0;
		*pin  = 19;
	} else {
		return -EINVAL;
	}
	return 0;
}

/**
 * Enable external interrupt.
 * This function enables an external interrupt and sets the given mode.
 * valid values for mode are:
 *   - 0 = Interrupt generation disabled
 *   - 1 = Interrupt on rising edge
 *   - 2 = Interrupt on falling edge
 *   - 3 = Interrupt on rising and falling edge
 *   - 5 = Interrupt on high level detection
 *   - 6 = Interrupt on low level detection
 *
 * \param   exint - Number of external interrupt
 * \param   mode  - Trigger mode
 * \return  0 on success
 * \ingroup API
 */
int ifx_enable_external_int(u32 exint, u32 mode)
{
	int port;
	int pin;

	if ((mode < 0) || (mode > 6))
		return -EINVAL;

	if (ltq_exint2port(exint, &port, &pin))
		return -EINVAL;

	ltq_port_clear_exintcr0(port, pin);
	ltq_port_clear_exintcr1(port, pin);
	ltq_port_clear_irncfg(port, pin);

	if (mode & 0x1)
		ltq_port_set_exintcr0(port, pin);
	if (mode & 0x2)
		ltq_port_set_exintcr1(port, pin);
	if (mode & 0x4)
		ltq_port_set_irncfg(port, pin);

	ltq_port_set_irnen(port, pin);
	return 0;
}
EXPORT_SYMBOL(ifx_enable_external_int);

/**
 * Disable external interrupt.
 * This function disables an external interrupt and sets mode to 0x00.
 *
 * \param   exint - Number of external interrupt
 * \return  0 on success
 * \ingroup API
 */
int ifx_disable_external_int(u32 exint)
{
	int port;
	int pin;

	if (ltq_exint2port(exint, &port, &pin))
		return -EINVAL;

	ltq_port_clear_irnen(port, pin);
	return 0;
}
EXPORT_SYMBOL(ifx_disable_external_int);
