/*
 * arch/m68k/coldfire/m547x/mcf548x-devices.c
 *
 * Coldfire M548x Platform Device Configuration
 *
 * Based on the Freescale MXC devices.c
 *
 * Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * Kurt Mahan <kmahan@freescale.com>
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mtd/physmap.h>
#include <linux/platform_device.h>
#include <linux/fsl_devices.h>

#include <asm/coldfire.h>
#include <asm/mcfsim.h>
#include <asm/mcfuart.h>

static struct resource coldfire_i2c_resources[] = {
	{		/* I/O */
		.start		= MCF_MBAR + 0x008F00,
		.end		= MCF_MBAR + 0x008F20,
		.flags		= IORESOURCE_MEM,
	},
	{		/* IRQ */
		.start		= 40,
		.end		= 40,
		.flags		= IORESOURCE_IRQ,
	},
};

static struct platform_device coldfire_i2c_device = {
	.name			= "mcf-i2c",
	.id			= 0,	/*bus number*/
	.num_resources		= ARRAY_SIZE(coldfire_i2c_resources),
	.resource		= coldfire_i2c_resources,
};

static struct resource coldfire_sec_resources[] = {
	[0] = {         /* I/O */
		.start          = MCF_MBAR + 0x00020000,
		.end            = MCF_MBAR + 0x00033000,
		.flags          = IORESOURCE_MEM,
	},
	[2] = {         /* IRQ */
		.start          = ISC_SEC,
		.end            = ISC_SEC,
		.flags          = IORESOURCE_IRQ,
	},
};

static struct platform_device coldfire_sec_device = {
	.name                   = "fsl-sec1",
	.id                     = -1,
	.num_resources          = ARRAY_SIZE(coldfire_sec_resources),
	.resource               = coldfire_sec_resources,
};

static struct physmap_flash_data mcf5485_flash_data = {
       .width          = 2,
};

static struct resource mcf5485_flash_resource = {
       .start          = 0xff800000,
       .end            = 0xffbfffff,
       .flags          = IORESOURCE_MEM,
};

static struct platform_device mcf5485_flash_device = {
       .name           = "physmap-flash",
       .id             = 0,
       .dev            = {
               .platform_data  = &mcf5485_flash_data,
       },
       .num_resources  = 1,
       .resource       = &mcf5485_flash_resource,
};

static int __init mcf5485_init_devices(void)
{
	printk(KERN_INFO "MCF5485x INIT_DEVICES\n");

	platform_device_register(&coldfire_i2c_device);
	platform_device_register(&coldfire_sec_device);
	platform_device_register(&mcf5485_flash_device);
	return 0;
}
arch_initcall(mcf5485_init_devices);

static struct mcf_platform_uart m548x_uart_platform[] = {
	{
		.mapbase	= MCF_MBAR + MCFUART_BASE1,
		.irq		= MCFINT_VECBASE + MCFINT_UART0,
	},
	{
		.mapbase	= MCF_MBAR + MCFUART_BASE2,
		.irq		= MCFINT_VECBASE + MCFINT_UART1,
	},
	{
		.mapbase	= MCF_MBAR + MCFUART_BASE3,
		.irq		= MCFINT_VECBASE + MCFINT_UART2,
	},
	{ },
};

static struct platform_device m548x_uart = {
		.name			= "mcfuart",
		.id			= 0,
		.dev.platform_data	= m548x_uart_platform,
};

static struct platform_device *m548x_devices[] __initdata = {
		&m548x_uart,
};

void m548x_uarts_init(void)
{
	const int nrlines = ARRAY_SIZE(m548x_uart_platform);
	int line;

	/* Set GPIO port register to enable PSC(port) signals */
	for (line = 0; (line < nrlines); line++) {
		MCF_PAR_PSCn(line) = (0
			| MCF_PAR_PSC_TXD
			| MCF_PAR_PSC_RXD);

		MCF_ICR(m548x_uart_platform[line].irq - 64) = ILP_PSCn(line);
	}
}
/***************************************************************************/

static int __init init_BSP(void)
{
	m548x_uarts_init();
	platform_add_devices(m548x_devices, ARRAY_SIZE(m548x_devices));
	return 0;
}

arch_initcall(init_BSP);
