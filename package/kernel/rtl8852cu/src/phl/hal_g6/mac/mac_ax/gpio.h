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

#ifndef _MAC_AX_GPIO_H_
#define _MAC_AX_GPIO_H_

#include "../type.h"
#include "gpio_cmd.h"

#define DFLT_GPIO_STATE \
	{RTW_MAC_GPIO_DFLT, RTW_MAC_GPIO_DFLT, RTW_MAC_GPIO_DFLT, \
	 RTW_MAC_GPIO_DFLT, RTW_MAC_GPIO_DFLT, RTW_MAC_GPIO_DFLT, \
	 RTW_MAC_GPIO_DFLT, RTW_MAC_GPIO_DFLT, RTW_MAC_GPIO_DFLT, \
	 RTW_MAC_GPIO_DFLT, RTW_MAC_GPIO_DFLT, RTW_MAC_GPIO_DFLT, \
	 RTW_MAC_GPIO_DFLT, RTW_MAC_GPIO_DFLT, RTW_MAC_GPIO_DFLT, \
	 RTW_MAC_GPIO_DFLT, RTW_MAC_GPIO_DFLT, RTW_MAC_GPIO_DFLT, \
	 RTW_MAC_GPIO_DFLT}

#define DFLT_SW_IO_MODE \
	{0, 0, 0, 0, \
	 0, 0, 0, 0, \
	 0, 0, 0, 0, \
	 0, 0, 0, 0}

/**
 * @addtogroup Common
 * @{
 * @addtogroup GPIO
 * @{
 */
/**
 * @brief mac_sel_uart_tx_pin
 *
 * @param *adapter
 * @param uart_pin
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_sel_uart_tx_pin(struct mac_ax_adapter *adapter,
			enum mac_ax_uart_tx_pin uart_pin);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup GPIO
 * @{
 */

/**
 * @brief mac_pinmux_status
 *
 * @param *adapter
 * @param func
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_pinmux_status(struct mac_ax_adapter *adapter,
		      enum mac_ax_gpio_func func);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup GPIO
 * @{
 */

/**
 * @brief mac_pinmux_free_func
 *
 * @param *adapter
 * @param func
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_pinmux_free_func(struct mac_ax_adapter *adapter,
			 enum mac_ax_gpio_func func);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup GPIO
 * @{
 */

/**
 * @brief mac_pinmux_record
 *
 * @param *adapter
 * @param func
 * @param val
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_pinmux_record(struct mac_ax_adapter *adapter,
		      enum mac_ax_gpio_func func, u8 val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup GPIO
 * @{
 */

/**
 * @brief mac_pinmux_switch
 *
 * @param *adapter
 * @param gpio_func
 * @param *list
 * @param list_size
 * @param gpio_id
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_pinmux_switch(struct mac_ax_adapter *adapter,
		      enum mac_ax_gpio_func gpio_func,
		      const struct mac_ax_pinmux_list *list,
		      u32 list_size, u32 gpio_id);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup GPIO
 * @{
 */

/**
 * @brief mac_sel_uart_rx_pin
 *
 * @param *adapter
 * @param uart_pin
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_sel_uart_rx_pin(struct mac_ax_adapter *adapter,
			enum mac_ax_uart_rx_pin uart_pin);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup GPIO
 * @{
 */

/**
 * @brief mac_set_led_mode
 *
 * @param *adapter
 * @param mode
 * @param led_id
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_set_led_mode(struct mac_ax_adapter *adapter,
		     enum mac_ax_led_mode mode, u8 led_id);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup GPIO
 * @{
 */

/**
 * @brief mac_led_ctrl
 *
 * @param *adapter
 * @param high
 * @param led_id
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_led_ctrl(struct mac_ax_adapter *adapter,
		 u8 high, u8 led_id);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup GPIO
 * @{
 */

/**
 * @brief mac_sw_gpio_ctrl
 *
 * @param *adapter
 * @param high
 * @param gpio
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_sw_gpio_ctrl(struct mac_ax_adapter *adapter,
		     u8 high, u8 gpio);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup GPIO
 * @{
 */

/**
 * @brief mac_set_sw_gpio_mode
 *
 * @param *adapter
 * @param mode
 * @param gpio
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_set_sw_gpio_mode(struct mac_ax_adapter *adapter,
			 enum rtw_gpio_mode mode, u8 gpio);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup GPIO
 * @{
 */

/**
 * @brief mac_get_gpio_status
 *
 * @param *adapter
 * @param *list
 * @return Please Place Description here.
 * @retval  rtw_mac_gfunc
 */
enum rtw_mac_gfunc mac_get_gpio_status(struct mac_ax_adapter *adapter,
				       const struct mac_ax_pin_list *list);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Common
 * @{
 * @addtogroup GPIO
 * @{
 */

/**
 * @brief mac_cfg_wps
 *
 * WPS is a driver feature to detect button pressed or released.
 * In HW view, the feature is to check the GPIO input value is 0->1 or 1->0
 * We use FW to detect GPIO val.
 * In a specified interval, if FW detects value changed, it will send a C2H
 *
 * @param *adapter
 * @param *wps
 * @return 0 for succcess, others for fail
 * @retval u32
 */
u32 mac_cfg_wps(struct mac_ax_adapter *adapter,
		struct mac_ax_cfg_wps *wps);
/**
 * @}
 * @}
 */
u32 mac_get_gpio_val(struct mac_ax_adapter *adapter, u8 gpio, u8 *val);
u32 mac_get_wl_dis_val(struct mac_ax_adapter *adapter, u8 *val);

/**
 * @addtogroup HCI
 * @{
 * @addtogroup GPIO
 * @{
 */

/**
 * @brief mac_get_uart_fw_dbg_gpio
 *
 * @param *adapter
 * @param uart_tx_pin
 * @param uart_rx_pin
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_get_uart_fw_dbg_gpio(struct mac_ax_adapter *adapter, u8 *uart_tx_gpio, u8 *uart_rx_gpio);
/**
 * @}
 * @}
 */

#endif
