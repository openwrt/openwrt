/*
 * $Id$
 * 
 * Copyright (C) 2006, 2007 OpenWrt.org
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/mtd/physmap.h>
#include <linux/serial.h>
#include <linux/serial_8250.h>
#include <linux/ioport.h>
#include <asm/addrspace.h>
#include <asm/io.h>
#include <asm/ar7/ar7.h>
#include <asm/ar7/gpio.h>
#include <asm/ar7/vlynq.h>

struct plat_vlynq_data {
	struct plat_vlynq_ops ops;
	int gpio_bit;
	int reset_bit;
};


static int vlynq_on(struct vlynq_device *dev)
{
	int result;
	struct plat_vlynq_data *pdata = dev->dev.platform_data;

	if ((result = gpio_request(pdata->gpio_bit, "vlynq")))
		goto out;

	ar7_device_reset(pdata->reset_bit);

	if ((result = ar7_gpio_disable(pdata->gpio_bit)))
		goto out_enabled;

	if ((result = ar7_gpio_enable(pdata->gpio_bit)))
		goto out_enabled;

	if ((result = gpio_direction_output(pdata->gpio_bit)))
		goto out_gpio_enabled;

	gpio_set_value(pdata->gpio_bit, 0);
	mdelay(50);

	gpio_set_value(pdata->gpio_bit, 1);
	mdelay(50);

	return 0;

out_gpio_enabled:
	ar7_gpio_disable(pdata->gpio_bit);
out_enabled:
	ar7_device_disable(pdata->reset_bit);
	gpio_free(pdata->gpio_bit);
out:
	return result;
}

static void vlynq_off(struct vlynq_device *dev)
{
	struct plat_vlynq_data *pdata = dev->dev.platform_data;
	ar7_gpio_disable(pdata->gpio_bit);
	gpio_free(pdata->gpio_bit);
	ar7_device_disable(pdata->reset_bit);
}

static struct resource physmap_flash_resource = {
	.name = "mem",
	.flags = IORESOURCE_MEM,
	.start = 0x10000000,
	.end = 0x103fffff,
};	

static struct resource cpmac_low_res[] = {
	{
		.name = "regs",
		.flags = IORESOURCE_MEM,
		.start = AR7_REGS_MAC0,
		.end = AR7_REGS_MAC0 + 0x7FF,
	},
	{
		.name = "irq",
		.flags = IORESOURCE_IRQ,
		.start = 27,
		.end = 27,
        },
};

static struct resource cpmac_high_res[] = {
	{
		.name = "regs",
		.flags = IORESOURCE_MEM,
		.start = AR7_REGS_MAC1,
		.end = AR7_REGS_MAC1 + 0x7FF,
	},
	{
		.name = "irq",
		.flags = IORESOURCE_IRQ,
		.start = 41,
		.end = 41,
        },
};

static struct resource vlynq_low_res[] = {
	{
		.name = "regs",
		.flags = IORESOURCE_MEM,
		.start = AR7_REGS_VLYNQ0,
		.end = AR7_REGS_VLYNQ0 + 0xff,
	},
	{
		.name = "irq",
		.flags = IORESOURCE_IRQ,
		.start = 29,
		.end = 29,
        },
	{
		.name = "mem",
		.flags = IORESOURCE_MEM,
		.start = 0x04000000,
		.end = 0x04ffffff,
	},
	{
		.name = "devirq",
		.flags = IORESOURCE_IRQ,
		.start = 80,
		.end = 111,
	},
};

static struct resource vlynq_high_res[] = {
	{
		.name = "regs",
		.flags = IORESOURCE_MEM,
		.start = AR7_REGS_VLYNQ1,
		.end = AR7_REGS_VLYNQ1 + 0xFF,
	},
	{
		.name = "irq",
		.flags = IORESOURCE_IRQ,
		.start = 33,
		.end = 33,
        },
	{
		.name = "mem",
		.flags = IORESOURCE_MEM,
		.start = 0x0c000000,
		.end = 0x0cffffff,
	},
	{
		.name = "devirq",
		.flags = IORESOURCE_IRQ,
		.start = 112,
		.end = 143,
	},
};

static struct physmap_flash_data physmap_flash_data = {
	.width = 2,
};

static struct plat_cpmac_data cpmac_low_data = {
	.reset_bit = 17,
	.power_bit = 20,
	.phy_mask = 0x80000000,
};

static struct plat_cpmac_data cpmac_high_data = {
	.reset_bit = 21,
	.power_bit = 22,
	.phy_mask = 0x7fffffff,
};

static struct plat_vlynq_data vlynq_low_data = {
	.ops.on = vlynq_on,
	.ops.off = vlynq_off,
	.reset_bit = 20,
	.gpio_bit = 18,
};

static struct plat_vlynq_data vlynq_high_data = {
	.ops.on = vlynq_on,
	.ops.off = vlynq_off,
	.reset_bit = 16,
	.gpio_bit = 19,
};

static struct platform_device physmap_flash = {
	.id = 0,
	.name = "physmap-flash",
	.dev.platform_data = &physmap_flash_data,
	.resource = &physmap_flash_resource,
	.num_resources = 1,
};

static struct platform_device cpmac_low = {
	.id = 0,
	.name = "cpmac",
	.dev.platform_data = &cpmac_low_data,
	.resource = cpmac_low_res,
	.num_resources = ARRAY_SIZE(cpmac_low_res),
};

static struct platform_device cpmac_high = {
	.id = 1,
	.name = "cpmac",
	.dev.platform_data = &cpmac_high_data,
	.resource = cpmac_high_res,
	.num_resources = ARRAY_SIZE(cpmac_high_res),
};

static struct platform_device vlynq_low = {
	.id = 0,
	.name = "vlynq",
	.dev.platform_data = &vlynq_low_data,
	.resource = vlynq_low_res,
	.num_resources = ARRAY_SIZE(vlynq_low_res),
};

static struct platform_device vlynq_high = {
	.id = 1,
	.name = "vlynq",
	.dev.platform_data = &vlynq_high_data,
	.resource = vlynq_high_res,
	.num_resources = ARRAY_SIZE(vlynq_high_res),
};


/* This is proper way to define uart ports, but they are then detected
 * as xscale and, obviously, don't work...
 */
#if !defined(CONFIG_SERIAL_8250)
static struct plat_serial8250_port uart_data[] = {
	{
		.mapbase = AR7_REGS_UART0,
		.irq = AR7_IRQ_UART0,
		.regshift = 2,
		.iotype = UPIO_MEM,
		.flags = UPF_BOOT_AUTOCONF | UPF_IOREMAP,
	},
	{
		.mapbase = AR7_REGS_UART1,
		.irq = AR7_IRQ_UART1,
		.regshift = 2,
		.iotype = UPIO_MEM,
		.flags = UPF_BOOT_AUTOCONF | UPF_IOREMAP,
	},
	{
		.flags = 0,
	},
};

static struct platform_device uart = {
	.id = 0,
	.name = "serial8250",
	.dev.platform_data = uart_data,
};
#endif

static inline unsigned char char2hex(char h)
{
	switch (h) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		return h - '0';
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
		return h - 'A' + 10;
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
		return h - 'a' + 10;
	default:
		return 0;
	}
}

static void cpmac_get_mac(int instance, unsigned char *dev_addr)
{
	int i;
	char name[5], default_mac[] = "00:00:00:12:34:56", *mac;

	mac = NULL;
	sprintf(name, "mac%c", 'a' + instance);
	mac = prom_getenv(name);
	if (!mac) {
		sprintf(name, "mac%c", 'a');
		mac = prom_getenv(name);
	}
	if (!mac)
		mac = default_mac;
	for (i = 0; i < 6; i++)
		dev_addr[i] = (char2hex(mac[i * 3]) << 4) +
			char2hex(mac[i * 3 + 1]);
}

static int __init ar7_register_devices(void)
{
	int res;

#if defined(CONFIG_SERIAL_8250)
	static struct uart_port uart_port[2];

	memset(uart_port, 0, sizeof(struct uart_port) * 2);

	uart_port[0].type = PORT_16750;
	uart_port[0].line = 0;
	uart_port[0].irq = AR7_IRQ_UART0;
	uart_port[0].uartclk = ar7_bus_freq() / 2;
	uart_port[0].iotype = UPIO_MEM;
	uart_port[0].mapbase = AR7_REGS_UART0;
	uart_port[0].membase = ioremap(uart_port[0].mapbase, 256);
	uart_port[0].regshift = 2;
	res = early_serial_setup(&uart_port[0]);
	if (res)
		return res;

	uart_port[1].type = PORT_16750;
	uart_port[1].line = 1;
	uart_port[1].irq = AR7_IRQ_UART1;
	uart_port[1].uartclk = ar7_bus_freq() / 2;
	uart_port[1].iotype = UPIO_MEM;
	uart_port[1].mapbase = AR7_REGS_UART1;
	uart_port[1].membase = ioremap(uart_port[1].mapbase, 256);
	uart_port[1].regshift = 2;
	res = early_serial_setup(&uart_port[1]);
	if (res)
		return res;
#else
	uart_data[0].uartclk = ar7_bus_freq() / 2;
	uart_data[1].uartclk = uart_data[0].uartclk;
	res = platform_device_register(&uart);
	if (res)
		return res;
#endif
	res = platform_device_register(&physmap_flash);
	if (res)
		return res;

	res = platform_device_register(&vlynq_low);
	if (res)
		return res;

	ar7_device_disable(vlynq_low_data.reset_bit);
	if (ar7_has_high_vlynq()) {
		ar7_device_disable(vlynq_high_data.reset_bit);
		res = platform_device_register(&vlynq_high);
		if (res)
			return res;
	}

	if (ar7_has_high_cpmac()) {
		cpmac_get_mac(1, cpmac_high_data.dev_addr);
		res = platform_device_register(&cpmac_high);
		if (res)
			return res;
	} else {
		cpmac_low_data.phy_mask = 0xffffffff;
	}

	cpmac_get_mac(0, cpmac_low_data.dev_addr);
	res = platform_device_register(&cpmac_low);

	return res;
}


arch_initcall(ar7_register_devices);
