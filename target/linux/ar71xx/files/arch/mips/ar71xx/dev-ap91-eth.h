/*
 *  Atheros AP91 reference board ethernet initialization
 *
 *  Copyright (C) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#ifndef _AR71XX_DEV_AP91_ETH_H
#define _AR71XX_DEV_AP91_ETH_H

#define AP91_ETH_NUM_PORT_NAMES	4

#if defined(CONFIG_AR71XX_DEV_AP91_ETH)
void ap91_eth_init(u8 *mac_addr, const char *port_names[]) __init;
#else
static inline void ap91_eth_init(u8 *mac_addr) { }
#endif

#endif /* _AR71XX_DEV_AP91_ETH_H */

