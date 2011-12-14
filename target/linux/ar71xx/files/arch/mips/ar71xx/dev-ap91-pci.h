/*
 *  Atheros AP91 reference board PCI initialization
 *
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _AR71XX_DEV_AP91_PCI_H
#define _AR71XX_DEV_AP91_PCI_H

#include <linux/leds.h>

#if defined(CONFIG_AR71XX_DEV_AP91_PCI)
void ap91_pci_init(u8 *cal_data, u8 *mac_addr) __init;
void ap91_pci_setup_wmac_led_pin(int pin) __init;
void ap91_pci_setup_wmac_gpio(u32 mask, u32 val) __init;
void ap91_pci_setup_wmac_leds(struct gpio_led *leds, int num_leds) __init;
#else
static inline void ap91_pci_init(u8 *cal_data, u8 *mac_addr) { }
static inline void ap91_pci_setup_wmac_led_pin(int pin) { }
static inline void ap91_pci_setup_wmac_gpio(u32 mask, u32 gpio) { }
static inline void ap91_pci_setup_wmac_leds(struct gpio_led *leds, int num_leds) { };
#endif

#endif /* _AR71XX_DEV_AP91_PCI_H */

