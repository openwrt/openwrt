/*
 * Copyright (c) 2007 Atheros Communications, Inc.
 * All rights reserved.
 *
 *
 * $ATH_LICENSE_HOSTSDK0_C$
 *
 */

#ifndef __WLAN_DSET_H__
#define __WKAN_DSET_H__

typedef PREPACK struct wow_config_dset {

    A_UINT8 valid_dset;
    A_UINT8 gpio_enable;
    A_UINT16 gpio_pin;
} POSTPACK WOW_CONFIG_DSET;

#endif
