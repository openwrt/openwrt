/*
 *  MikroTik RouterBOARD 941-2nD support
 *
 *  Copyright (C) 2016 Sergey Sergeev <adron@yapic.net>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/export.h>
#include <linux/pci.h>
#include <linux/ath9k_platform.h>
#include <linux/platform_device.h>
#include <linux/phy.h>
#include <linux/ar8216_platform.h>
#include <linux/rle.h>
#include <linux/routerboot.h>
#include <linux/gpio.h>

#include <linux/mtd/mtd.h>
//#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/irq.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-spi.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
//#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "routerboot.h"

#define RB941_GPIO_LED_ACT      14
#define RB941_GPIO_BTN_RESET    16

#define RB941_KEYS_POLL_INTERVAL 20 /* msecs */
#define RB941_KEYS_DEBOUNCE_INTERVAL (3 * RB941_KEYS_POLL_INTERVAL)

#define RB_ROUTERBOOT_OFFSET    0x0000
#define RB_ROUTERBOOT_SIZE      0xe000
#define RB_HARD_CFG_OFFSET      0xe000
#define RB_HARD_CFG_SIZE        0x1000
#define RB_BIOS_OFFSET          0xf000
#define RB_BIOS_SIZE            0x1000
#define RB_ROUTERBOOT2_OFFSET   0x10000
#define RB_ROUTERBOOT2_SIZE     0xf000
#define RB_SOFT_CFG_OFFSET      0x1f000
#define RB_SOFT_CFG_SIZE        0x1000
#define RB_ROOTFS_OFFSET        0x20000
#define RB_ROOTFS_SIZE          0x9e0000
#define RB_KERNEL_OFFSET        0xa00000
#define RB_KERNEL_SIZE          MTDPART_SIZ_FULL

void __init rb941_wlan_init(void)
{
    char *art_buf;
    u8 wlan_mac[ETH_ALEN];

    art_buf = rb_get_wlan_data();
    if (art_buf == NULL)
            return;

    ath79_init_mac(wlan_mac, ath79_mac_base, 11);
    ath79_register_wmac(art_buf + 0x1000, wlan_mac);

    kfree(art_buf);
}

static struct mtd_partition rb941_spi_partitions[] = {
    {
            .name = "routerboot",
            .offset = RB_ROUTERBOOT_OFFSET,
            .mask_flags = MTD_WRITEABLE,
            .size = RB_ROUTERBOOT_SIZE,
    }, {
            .name = "hard_config",
            .offset = RB_HARD_CFG_OFFSET,
            .size = RB_HARD_CFG_SIZE,
            .mask_flags = MTD_WRITEABLE,
    }, {
            .name = "bios",
            .offset = RB_BIOS_OFFSET,
            .size = RB_BIOS_SIZE,
            .mask_flags = MTD_WRITEABLE,
    }, {
            .name = "routerboot2",
            .offset = RB_ROUTERBOOT2_OFFSET,
            .size = RB_ROUTERBOOT2_SIZE,
            .mask_flags = MTD_WRITEABLE,
    }, {
            .name = "soft_config",
            .offset = RB_SOFT_CFG_OFFSET,
            .size = RB_SOFT_CFG_SIZE,
    }, {
            .name = "rootfs",
            .offset = RB_ROOTFS_OFFSET,
            .size = RB_ROOTFS_SIZE,
    }, {
            .name = "kernel",
            .offset = RB_KERNEL_OFFSET,
            .size = RB_KERNEL_SIZE,
    }
};

static struct flash_platform_data rb941_spi_flash_data = {
    .parts          = rb941_spi_partitions,
    .nr_parts       = ARRAY_SIZE(rb941_spi_partitions),
};

static struct gpio_led rb941_leds[] __initdata = {
    {
        .name = "rb:green:act",
        .gpio = RB941_GPIO_LED_ACT,
        .active_low = 1,
    },
};

static struct gpio_keys_button rb941_gpio_keys[] __initdata = {
    {
        .desc = "Reset button",
        .type = EV_KEY,
        .code = KEY_RESTART,
        .debounce_interval = RB941_KEYS_DEBOUNCE_INTERVAL,
        .gpio = RB941_GPIO_BTN_RESET,
        .active_low = 1,
    },
};

static void __init rb941_setup(void)
{
    const struct rb_info *info;
    //try to get rb_info data
    info = rb_init_info((void *)(KSEG1ADDR(AR71XX_SPI_BASE)), 0x20000);
    if (!info){
        pr_err("%s: Can't get rb_info data from flash!\n", __func__);
        //return -EINVAL; //Not critical ... continue!
    }
    ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_SW_ONLY_MODE);
    ath79_register_m25p80(&rb941_spi_flash_data);
    ath79_register_mdio(0, 0x0);

    /* WAN. We have no WAN. Only LAN. */
    /*ath79_switch_data.phy4_mii_en = 1;
    ath79_switch_data.phy_poll_mask = BIT(4);
    ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
    ath79_eth0_data.phy_mask = BIT(4);
    ath79_init_mac(ath79_eth0_data.mac_addr, ath79_mac_base, 0);
    ath79_register_eth(0); */

    /* LAN */
    ath79_init_mac(ath79_eth1_data.mac_addr, ath79_mac_base, 0);
    ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
    ath79_register_eth(1);

    //ath79_register_usb();

    rb941_wlan_init();

    ath79_register_leds_gpio(-1, ARRAY_SIZE(rb941_leds), rb941_leds);
    ath79_register_gpio_keys_polled(-1, RB941_KEYS_POLL_INTERVAL,
                                    ARRAY_SIZE(rb941_gpio_keys),
                                    rb941_gpio_keys);
}

MIPS_MACHINE(ATH79_MACH_RB_941, "H951L", "MikroTik RouterBOARD 941-2nD", rb941_setup);
