/*
 *  $Id$
 *
 *  Mikrotik RouterBOARD 1xx series
 *
 *  Copyright (C) 2007 OpenWrt.org
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
 *
 *  NAND initialization code was based on a driver for Linux 2.6.19+ which
 *  was derived from the driver for Linux 2.4.xx published by Mikrotik for
 *  their RouterBoard 1xx and 5xx series boards.
 *    Copyright (C) 2007 David Goodenough <david.goodenough@linkchoose.co.uk>
 *    Copyright (C) 2007 Florian Fainelli <florian@openwrt.org>
 *    The original Mikrotik code seems not to have a license.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>

#include <asm/bootinfo.h>
#include <asm/gpio.h>

#include <adm5120_defs.h>
#include <adm5120_irq.h>
#include <adm5120_nand.h>
#include <adm5120_board.h>
#include <adm5120_platform.h>
#include <adm5120_info.h>

#include <prom/routerboot.h>

#define RB1XX_NAND_CHIP_DELAY	25

#define RB150_NAND_BASE		0x1FC80000
#define RB150_NAND_SIZE		1

#define RB150_GPIO_NAND_READY	ADM5120_GPIO_PIN0
#define RB150_GPIO_NAND_NCE	ADM5120_GPIO_PIN1
#define RB150_GPIO_NAND_CLE	ADM5120_GPIO_P2L2
#define RB150_GPIO_NAND_ALE	ADM5120_GPIO_P3L2

#define RB150_NAND_DELAY	100

#define RB150_NAND_WRITE(v) \
	writeb((v), (void __iomem *)KSEG1ADDR(RB150_NAND_BASE))

#define RB153_GPIO_CF_RDY	ADM5120_GPIO_P1L1
#define RB153_GPIO_CF_WT	ADM5120_GPIO_P0L0

extern struct rb_hard_settings rb_hs;

/*--------------------------------------------------------------------------*/

static struct adm5120_pci_irq rb1xx_pci_irqs[] __initdata = {
	PCIIRQ(1, 0, 1, ADM5120_IRQ_PCI0),
	PCIIRQ(2, 0, 1, ADM5120_IRQ_PCI1),
	PCIIRQ(3, 0, 1, ADM5120_IRQ_PCI2)
};

static struct mtd_partition rb1xx_nor_parts[] = {
	{
		.name	= "booter",
		.offset	= 0,
		.size	= 64*1024,
		.mask_flags = MTD_WRITEABLE,
	} , {
		.name	= "firmware",
		.offset	= MTDPART_OFS_APPEND,
		.size	= MTDPART_SIZ_FULL,
	}
};

static struct mtd_partition rb1xx_nand_parts[] = {
	{
		.name	= "kernel",
		.offset	= 0,
		.size	= 4 * 1024 * 1024,
	} , {
		.name	= "rootfs",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= MTDPART_SIZ_FULL
	}
};

static struct platform_device *rb1xx_devices[] __initdata = {
	&adm5120_flash0_device,
	&adm5120_nand_device,
};

/*
 * We need to use the OLD Yaffs-1 OOB layout, otherwise the RB bootloader
 * will not be able to find the kernel that we load.  So set the oobinfo
 * when creating the partitions
 */
static struct nand_ecclayout rb1xx_nand_ecclayout = {
	.eccbytes	= 6,
	.eccpos		= { 8, 9, 10, 13, 14, 15 },
	.oobavail	= 9,
	.oobfree	= { { 0, 4 }, { 6, 2 }, { 11, 2 }, { 4, 1 } }
};

static struct resource rb150_nand_resource[] = {
	[0] = {
		.start	= RB150_NAND_BASE,
		.end	= RB150_NAND_BASE + RB150_NAND_SIZE-1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct resource rb153_cf_resources[] = {
	{
		.name	= "cf_membase",
		.start	= ADM5120_EXTIO1_BASE,
		.end	= ADM5120_EXTIO1_BASE + ADM5120_EXTIO1_SIZE-1 ,
		.flags	= IORESOURCE_MEM
	}, {
		.name	= "cf_irq",
		.start	= ADM5120_IRQ_GPIO4,
		.end	= ADM5120_IRQ_GPIO4,
		.flags	= IORESOURCE_IRQ
	}
};

static struct platform_device rb153_cf_device = {
	.name		= "pata-rb153-cf",
	.id		= -1,
	.resource	= rb153_cf_resources,
	.num_resources	= ARRAY_SIZE(rb153_cf_resources),
};

static struct platform_device *rb153_devices[] __initdata = {
	&adm5120_flash0_device,
	&adm5120_nand_device,
	&rb153_cf_device,
};

#if 0
/*
 * RB1xx boards have bad network performance with the default VLAN matrixes.
 * Disable it while the ethernet driver gets fixed.
 */
static unsigned char rb11x_vlans[6] __initdata = {
	/* FIXME: untested */
	0x41, 0x00, 0x00, 0x00, 0x00, 0x00
};

static unsigned char rb133_vlans[6] __initdata = {
	/* FIXME: untested */
	0x44, 0x42, 0x41, 0x00, 0x00, 0x00
};

static unsigned char rb133c_vlans[6] __initdata = {
	/* FIXME: untested */
	0x44, 0x00, 0x00, 0x00, 0x00, 0x00
};

static unsigned char rb15x_vlans[6] __initdata = {
	/* FIXME: untested */
	0x41, 0x42, 0x44, 0x48, 0x50, 0x00
};

static unsigned char rb192_vlans[6] __initdata = {
	/* FIXME: untested */
	0x41, 0x50, 0x48, 0x44, 0x42, 0x00
};
#else
static unsigned char rb_vlans[6] __initdata = {
	0x7F, 0x00, 0x00, 0x00, 0x00, 0x00
};
#define rb11x_vlans	rb_vlans
#define rb133_vlans	rb_vlans
#define rb133c_vlans	rb_vlans
#define rb15x_vlans	rb_vlans
#define rb192_vlans	rb_vlans
#endif

/*--------------------------------------------------------------------------*/

static int rb150_nand_ready(struct mtd_info *mtd)
{
	return gpio_get_value(RB150_GPIO_NAND_READY);
}

static void rb150_nand_cmd_ctrl(struct mtd_info *mtd, int cmd,
		unsigned int ctrl)
{
	if (ctrl & NAND_CTRL_CHANGE) {
		gpio_set_value(RB150_GPIO_NAND_CLE, (ctrl & NAND_CLE) ? 1 : 0);
		gpio_set_value(RB150_GPIO_NAND_ALE, (ctrl & NAND_ALE) ? 1 : 0);
		gpio_set_value(RB150_GPIO_NAND_NCE, (ctrl & NAND_NCE) ? 0 : 1);
	}

	udelay(RB150_NAND_DELAY);

	if (cmd != NAND_CMD_NONE)
		RB150_NAND_WRITE(cmd);
}

/*--------------------------------------------------------------------------*/

static void __init rb1xx_mac_setup(void)
{
	int i, j;

	for (i = 0; i < rb_hs.mac_count; i++) {
		for (j = 0; j < RB_MAC_SIZE; j++)
			adm5120_eth_macs[i][j] = rb_hs.macs[i][j];
	}
}

static void __init rb1xx_flash_setup(void)
{
	/* setup data for flash0 device */
	adm5120_flash0_data.nr_parts = ARRAY_SIZE(rb1xx_nor_parts);
	adm5120_flash0_data.parts = rb1xx_nor_parts;

	/* setup data for NAND device */
	adm5120_nand_data.chip.nr_chips = 1;
	adm5120_nand_data.chip.nr_partitions = ARRAY_SIZE(rb1xx_nand_parts);
	adm5120_nand_data.chip.partitions = rb1xx_nand_parts;
	adm5120_nand_data.chip.ecclayout = &rb1xx_nand_ecclayout;
	adm5120_nand_data.chip.chip_delay = RB1XX_NAND_CHIP_DELAY;
	adm5120_nand_data.chip.options = NAND_NO_AUTOINCR;
}

static void __init rb153_cf_setup(void)
{
	/* enable CSX1:INTX1 on GPIO 3:4 for the CF slot */
	adm5120_gpio_csx1_enable();

	gpio_request(RB153_GPIO_CF_RDY, "cf-ready");
	gpio_direction_input(RB153_GPIO_CF_RDY);
	gpio_request(RB153_GPIO_CF_WT, "cf-wait");
	gpio_direction_output(RB153_GPIO_CF_WT, 1);
	gpio_direction_input(RB153_GPIO_CF_WT);
}

static void __init rb1xx_setup(void)
{
	/* enable NAND flash interface */
	adm5120_nand_enable();

	/* initialize NAND chip */
	adm5120_nand_set_spn(1);
	adm5120_nand_set_wpn(0);

	rb1xx_flash_setup();
	rb1xx_mac_setup();
}

static void __init rb150_setup(void)
{
	/* setup GPIO pins for NAND flash chip */
	gpio_request(RB150_GPIO_NAND_READY, "nand-ready");
	gpio_direction_input(RB150_GPIO_NAND_READY);
	gpio_request(RB150_GPIO_NAND_NCE, "nand-nce");
	gpio_direction_output(RB150_GPIO_NAND_NCE, 1);
	gpio_request(RB150_GPIO_NAND_CLE, "nand-cle");
	gpio_direction_output(RB150_GPIO_NAND_CLE, 0);
	gpio_request(RB150_GPIO_NAND_ALE, "nand-ale");
	gpio_direction_output(RB150_GPIO_NAND_ALE, 0);

	adm5120_nand_device.num_resources = ARRAY_SIZE(rb150_nand_resource);
	adm5120_nand_device.resource = rb150_nand_resource;
	adm5120_nand_data.ctrl.cmd_ctrl = rb150_nand_cmd_ctrl;
	adm5120_nand_data.ctrl.dev_ready = rb150_nand_ready;

	adm5120_flash0_data.window_size = 512*1024;

	rb1xx_flash_setup();
	rb1xx_mac_setup();
}

static void __init rb153_setup(void)
{
	rb153_cf_setup();
	rb1xx_setup();
}

/*--------------------------------------------------------------------------*/

ADM5120_BOARD_START(RB_111, "Mikrotik RouterBOARD 111")
	.board_setup	= rb1xx_setup,
	.eth_num_ports	= 1,
	.eth_vlans	= rb11x_vlans,
	.num_devices	= ARRAY_SIZE(rb1xx_devices),
	.devices	= rb1xx_devices,
	.pci_nr_irqs	= ARRAY_SIZE(rb1xx_pci_irqs),
	.pci_irq_map	= rb1xx_pci_irqs,
ADM5120_BOARD_END

ADM5120_BOARD_START(RB_112, "Mikrotik RouterBOARD 112")
	.board_setup	= rb1xx_setup,
	.eth_num_ports	= 1,
	.eth_vlans	= rb11x_vlans,
	.num_devices	= ARRAY_SIZE(rb1xx_devices),
	.devices	= rb1xx_devices,
	.pci_nr_irqs	= ARRAY_SIZE(rb1xx_pci_irqs),
	.pci_irq_map	= rb1xx_pci_irqs,
ADM5120_BOARD_END

ADM5120_BOARD_START(RB_133, "Mikrotik RouterBOARD 133")
	.board_setup	= rb1xx_setup,
	.eth_num_ports	= 3,
	.eth_vlans	= rb133_vlans,
	.num_devices	= ARRAY_SIZE(rb1xx_devices),
	.devices	= rb1xx_devices,
	.pci_nr_irqs	= ARRAY_SIZE(rb1xx_pci_irqs),
	.pci_irq_map	= rb1xx_pci_irqs,
ADM5120_BOARD_END

ADM5120_BOARD_START(RB_133C, "Mikrotik RouterBOARD 133C")
	.board_setup	= rb1xx_setup,
	.eth_num_ports	= 1,
	.eth_vlans	= rb133c_vlans,
	.num_devices	= ARRAY_SIZE(rb1xx_devices),
	.devices	= rb1xx_devices,
	.pci_nr_irqs	= ARRAY_SIZE(rb1xx_pci_irqs),
	.pci_irq_map	= rb1xx_pci_irqs,
ADM5120_BOARD_END

ADM5120_BOARD_START(RB_150, "Mikrotik RouterBOARD 150")
	.board_setup	= rb150_setup,
	.eth_num_ports	= 5,
	.eth_vlans	= rb15x_vlans,
	.num_devices	= ARRAY_SIZE(rb1xx_devices),
	.devices	= rb1xx_devices,
ADM5120_BOARD_END

ADM5120_BOARD_START(RB_153, "Mikrotik RouterBOARD 153")
	.board_setup	= rb153_setup,
	.eth_num_ports	= 5,
	.eth_vlans	= rb15x_vlans,
	.num_devices	= ARRAY_SIZE(rb153_devices),
	.devices	= rb153_devices,
	.pci_nr_irqs	= ARRAY_SIZE(rb1xx_pci_irqs),
	.pci_irq_map	= rb1xx_pci_irqs,
ADM5120_BOARD_END

ADM5120_BOARD_START(RB_192, "Mikrotik RouterBOARD 192")
	.board_setup	= rb1xx_setup,
	.eth_num_ports	= 5,
	.eth_vlans	= rb192_vlans,
	.num_devices	= ARRAY_SIZE(rb1xx_devices),
	.devices	= rb1xx_devices,
	.pci_nr_irqs	= ARRAY_SIZE(rb1xx_pci_irqs),
	.pci_irq_map	= rb1xx_pci_irqs,
ADM5120_BOARD_END
