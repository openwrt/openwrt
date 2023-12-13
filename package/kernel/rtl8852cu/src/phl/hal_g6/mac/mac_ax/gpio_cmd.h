/** @file */
/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#ifndef _MAC_AX_GPIO_CMD_H_
#define _MAC_AX_GPIO_CMD_H_

#include "../type.h"

#define MAC_AX_GPIO_MIN 0
#define RTW_MAC_GPIO_MAX 17

/**
 * @enum pinmux_name
 *
 * @brief pinmux_name
 *
 * @var pinmux_name::MAC_AX_WL_HWPDN
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_BT_HWPDN
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_SWGPIO
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_WL_HW_EXTWOL
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_SIC
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_BT_SFALSH
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_WL_SFALSH
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_WL_LED
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_WL_SDIO_INT
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_BT_SDIO_INT
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_UART
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_BT_JTAG
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_WL_JTAG
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_LTE_UART
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_LTE_3W
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_BT_GPIO16
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_WL_OSC
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_BT_OSC
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_GPIO13_14_WL_CTRL_EN
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_BT_RF
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_DBG_GNT
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_BT_3DDLS_A
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_BT_3DDLS_B
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_BT_GPIO18
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_BT_PTA
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_WL_PTA
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_WL_UART_TX
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_WL_UART_RX
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_WLMAC_DBG
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_WLPHY_DBG
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_BT_DBG
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_MAILBOX_3W
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_MAILBOX_1W
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_PAON_LNAON_2G_S0
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_RFE_WLBT_FUNC_0
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_PAON_LNAON_2G_S1
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_RFE_WLBT_FUNC_1
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_PAON_LNAON_5G_S0
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_RFE_WLBT_FUNC_2
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_PAON_LNAON_5G_S1
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_RFE_WLBT_FUNC_3
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_BANDSEL_5_6G
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_RFE_WLBT_FUNC_4
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_BANDSEL_5G
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_RFE_WLBT_FUNC_5
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_PAON_LNAON_6G_S1
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_RFE_WLBT_FUNC_7
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_BANDSEL_5G_G7G6
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_RFE_WLBT_FUNC_8
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_EXT_XTAL_CLK
 * Please Place Description here.
 * @var pinmux_name::MAC_AX_SW_IO
 * Please Place Description here.
 */
enum pinmux_name {
	MAC_AX_WL_HWPDN,
	MAC_AX_BT_HWPDN,
	MAC_AX_SWGPIO,
	MAC_AX_WL_HW_EXTWOL,
	MAC_AX_SIC,
	MAC_AX_BT_SFALSH,
	MAC_AX_WL_SFALSH,
	MAC_AX_WL_LED,
	MAC_AX_WL_SDIO_INT,
	MAC_AX_BT_SDIO_INT,
	MAC_AX_UART,
	MAC_AX_BT_JTAG,
	MAC_AX_WL_JTAG,
	MAC_AX_LTE_UART,
	MAC_AX_LTE_3W,
	MAC_AX_BT_GPIO16,
	MAC_AX_WL_OSC,
	MAC_AX_BT_OSC,
	MAC_AX_GPIO13_14_WL_CTRL_EN,
	MAC_AX_BT_RF,
	MAC_AX_DBG_GNT,
	MAC_AX_BT_3DDLS_A,
	MAC_AX_BT_3DDLS_B,
	MAC_AX_BT_GPIO18,
	MAC_AX_BT_PTA,
	MAC_AX_WL_PTA,
	MAC_AX_WL_UART_TX,
	MAC_AX_WL_UART_RX,
	MAC_AX_WLMAC_DBG,
	MAC_AX_WLPHY_DBG,
	MAC_AX_BT_DBG,
	MAC_AX_MAILBOX_3W,
	MAC_AX_MAILBOX_1W,
	MAC_AX_PAON_LNAON_2G_S0,
	MAC_AX_RFE_WLBT_FUNC_0,
	MAC_AX_PAON_LNAON_2G_S1,
	MAC_AX_RFE_WLBT_FUNC_1,
	MAC_AX_PAON_LNAON_5G_S0,
	MAC_AX_RFE_WLBT_FUNC_2,
	MAC_AX_PAON_LNAON_5G_S1,
	MAC_AX_RFE_WLBT_FUNC_3,
	MAC_AX_BANDSEL_5_6G,
	MAC_AX_RFE_WLBT_FUNC_4,
	MAC_AX_BANDSEL_5G,
	MAC_AX_RFE_WLBT_FUNC_5,
	MAC_AX_PAON_LNAON_6G_S1,
	MAC_AX_RFE_WLBT_FUNC_7,
	MAC_AX_BANDSEL_5G_G7G6,
	MAC_AX_RFE_WLBT_FUNC_8,
	MAC_AX_EXT_XTAL_CLK,
	MAC_AX_SW_IO,
};

/**
 * @enum pinmux_gpio
 *
 * @brief pinmux_gpio
 *
 * @var pinmux_gpio::MAC_AX_GPIO0
 * Please Place Description here.
 * @var pinmux_gpio::MAC_AX_GPIO1
 * Please Place Description here.
 * @var pinmux_gpio::MAC_AX_GPIO2
 * Please Place Description here.
 * @var pinmux_gpio::MAC_AX_GPIO3
 * Please Place Description here.
 * @var pinmux_gpio::MAC_AX_GPIO4
 * Please Place Description here.
 * @var pinmux_gpio::MAC_AX_GPIO5
 * Please Place Description here.
 * @var pinmux_gpio::MAC_AX_GPIO6
 * Please Place Description here.
 * @var pinmux_gpio::MAC_AX_GPIO7
 * Please Place Description here.
 * @var pinmux_gpio::MAC_AX_GPIO8
 * Please Place Description here.
 * @var pinmux_gpio::MAC_AX_GPIO9
 * Please Place Description here.
 * @var pinmux_gpio::MAC_AX_GPIO10
 * Please Place Description here.
 * @var pinmux_gpio::MAC_AX_GPIO11
 * Please Place Description here.
 * @var pinmux_gpio::MAC_AX_GPIO12
 * Please Place Description here.
 * @var pinmux_gpio::MAC_AX_GPIO13
 * Please Place Description here.
 * @var pinmux_gpio::MAC_AX_GPIO14
 * Please Place Description here.
 * @var pinmux_gpio::MAC_AX_GPIO15
 * Please Place Description here.
 */
enum pinmux_gpio {
	MAC_AX_GPIO0,
	MAC_AX_GPIO1,
	MAC_AX_GPIO2,
	MAC_AX_GPIO3,
	MAC_AX_GPIO4,
	MAC_AX_GPIO5,
	MAC_AX_GPIO6,
	MAC_AX_GPIO7,
	MAC_AX_GPIO8,
	MAC_AX_GPIO9,
	MAC_AX_GPIO10,
	MAC_AX_GPIO11,
	MAC_AX_GPIO12,
	MAC_AX_GPIO13,
	MAC_AX_GPIO14,
	MAC_AX_GPIO15,
};

/**
 * @enum pinmux_gpio_type
 *
 * @brief pinmux_gpio_type
 *
 * @var pinmux_gpio_type::MAC_AX_GPIO_IN
 * Please Place Description here.
 * @var pinmux_gpio_type::MAC_AX_GPIO_OUT
 * Please Place Description here.
 * @var pinmux_gpio_type::MAC_AX_GPIO_IN_OUT
 * Please Place Description here.
 */
enum pinmux_gpio_type {
	MAC_AX_GPIO_IN,
	MAC_AX_GPIO_OUT,
	MAC_AX_GPIO_IN_OUT,
};

/**
 * @struct mac_ax_pinmux_list
 * @brief mac_ax_pinmux_list
 *
 * @var mac_ax_pinmux_list::func
 * Please Place Description here.
 * @var mac_ax_pinmux_list::id
 * Please Place Description here.
 * @var mac_ax_pinmux_list::type
 * Please Place Description here.
 * @var mac_ax_pinmux_list::offset
 * Please Place Description here.
 * @var mac_ax_pinmux_list::msk
 * Please Place Description here.
 * @var mac_ax_pinmux_list::value
 * Please Place Description here.
 */
struct mac_ax_pinmux_list {
	enum pinmux_name func;
	enum pinmux_gpio id;
	enum pinmux_gpio_type type;
	u32 offset;
	u8 msk;
	u8 value;
};

/**
 * @struct mac_ax_pin_list
 * @brief mac_ax_pin_list
 *
 * @var mac_ax_pin_list::func
 * Please Place Description here.
 * @var mac_ax_pin_list::offset
 * Please Place Description here.
 * @var mac_ax_pin_list::msk
 * Please Place Description here.
 * @var mac_ax_pin_list::value
 * Please Place Description here.
 */
struct mac_ax_pin_list {
	enum rtw_mac_gfunc func;
	u32 offset;
	u8 msk;
	u8 value;
};

/**
 * @struct mac_ax_gpio_func_list
 * @brief mac_ax_gpio_func_list
 *
 * @var mac_ax_gpio_func_list::func
 * Please Place Description here.
 * @var mac_ax_gpio_func_list::list
 * Please Place Description here.
 */
struct mac_ax_gpio_func_list {
	enum rtw_mac_gfunc func;
	struct mac_ax_pin_list *list;
};
#endif
