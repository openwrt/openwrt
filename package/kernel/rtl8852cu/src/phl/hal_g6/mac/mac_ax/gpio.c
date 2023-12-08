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
#include "gpio.h"

u32 mac_sel_uart_tx_pin(struct mac_ax_adapter *adapter,
			enum mac_ax_uart_tx_pin uart_pin)
{
	struct mac_ax_intf_ops *ops = adapter->ops->intf_ops;
	struct mac_ax_gpio_info *info = &adapter->gpio_info;
	u8 val = MAC_REG_R8(R_AX_CAL_TIMER + 3);
	u32 ret = MACSUCCESS;

	switch (uart_pin) {
	case MAC_AX_UART_TX_GPIO5:
		val |= BIT(6);
		if (!info->uart_tx_gpio5)
			PLTFM_MSG_WARN("Pinmux function is not switched\n");
		break;
	case MAC_AX_UART_TX_GPIO7:
		val &= ~(BIT(7) | BIT(6));
		if (!info->uart_tx_gpio7)
			PLTFM_MSG_WARN("Pinmux function is not switched\n");
		break;
	case MAC_AX_UART_TX_GPIO8:
		val |= BIT(7);
		if (!info->uart_tx_gpio8)
			PLTFM_MSG_WARN("Pinmux function is not switched\n");
		break;
	case MAC_AX_UART_TX_GPIO5_GPIO8:
		val |= (BIT(6) | BIT(7));
		if (!info->uart_tx_gpio8 || !info->uart_tx_gpio5)
			PLTFM_MSG_WARN("Pinmux function is not switched\n");
		break;
	default:
		PLTFM_MSG_ERR("Wrong UART GPIO\n");
		ret = MACNOITEM;
		break;
	}

	MAC_REG_W8(R_AX_CAL_TIMER + 3, val);

	return ret;
}

u32 mac_sel_uart_rx_pin(struct mac_ax_adapter *adapter,
			enum mac_ax_uart_rx_pin uart_pin)
{
	struct mac_ax_intf_ops *ops = adapter->ops->intf_ops;
	struct mac_ax_gpio_info *info = &adapter->gpio_info;
	u8 val = MAC_REG_R8(R_AX_CAL_TIMER + 3);
	u32 ret = MACSUCCESS;

	switch (uart_pin) {
	case MAC_AX_UART_RX_GPIO6:
		val &= ~BIT(5);
		if (!info->uart_rx_gpio6)
			PLTFM_MSG_WARN("Pinmux function is not switched\n");
		break;
	case MAC_AX_UART_RX_GPIO14:
		val |= BIT(5);
		if (!info->uart_rx_gpio14)
			PLTFM_MSG_WARN("Pinmux function is not switched\n");
		break;
	default:
		PLTFM_MSG_ERR("Wrong UART GPIO\n");
		ret = MACNOITEM;
		break;
	}

	MAC_REG_W8(R_AX_CAL_TIMER + 3, val);

	return ret;
}

u32 mac_pinmux_switch(struct mac_ax_adapter *adapter,
		      enum mac_ax_gpio_func gpio_func,
		      const struct mac_ax_pinmux_list *list,
		      u32 list_size, u32 gpio_id)
{
	struct mac_ax_intf_ops *ops = adapter->ops->intf_ops;
	enum pinmux_name pinmux;
	struct mac_ax_pinmux_list const *cur_list;
	u32 i;
	u8 val;

	switch (gpio_func) {
	case MAC_AX_GPIO_SW_IO_0:
	case MAC_AX_GPIO_SW_IO_1:
	case MAC_AX_GPIO_SW_IO_2:
	case MAC_AX_GPIO_SW_IO_3:
	case MAC_AX_GPIO_SW_IO_4:
	case MAC_AX_GPIO_SW_IO_5:
	case MAC_AX_GPIO_SW_IO_6:
	case MAC_AX_GPIO_SW_IO_7:
	case MAC_AX_GPIO_SW_IO_8:
	case MAC_AX_GPIO_SW_IO_9:
	case MAC_AX_GPIO_SW_IO_10:
	case MAC_AX_GPIO_SW_IO_11:
	case MAC_AX_GPIO_SW_IO_12:
	case MAC_AX_GPIO_SW_IO_13:
	case MAC_AX_GPIO_SW_IO_14:
	case MAC_AX_GPIO_SW_IO_15:
		pinmux = MAC_AX_SW_IO;
		break;
	case MAC_AX_GPIO_UART_TX_GPIO5:
	case MAC_AX_GPIO_UART_TX_GPIO7:
	case MAC_AX_GPIO_UART_TX_GPIO8:
		pinmux = MAC_AX_WL_UART_TX;
		break;
	case MAC_AX_GPIO_UART_RX_GPIO6:
	case MAC_AX_GPIO_UART_RX_GPIO14:
		pinmux = MAC_AX_WL_UART_RX;
		break;
	default:
		PLTFM_MSG_ERR("Wrong GPIO function\n");
		return MACNOITEM;
	}

	cur_list = list;
	for (i = 0; i < list_size; i++) {
		val = MAC_REG_R8(cur_list->offset);
		val &= ~(cur_list->msk);

		if (pinmux == cur_list->func) {
			val |= (cur_list->value & cur_list->msk);
			MAC_REG_W8(cur_list->offset, val);
			break;
		}

		val |= (~cur_list->value & cur_list->msk);
		MAC_REG_W8(cur_list->offset, val);

		cur_list++;
	}

	if (i ==  list_size) {
		PLTFM_MSG_ERR("Get pinmux function error\n");
		return MACNOITEM;
	}

	switch (pinmux) {
	case MAC_AX_WL_UART_TX:
	case MAC_AX_WL_UART_RX:
		val = MAC_REG_R8(R_AX_WCPU_FW_CTRL + 3);
		MAC_REG_W8(R_AX_WCPU_FW_CTRL + 3, val | BIT(7));
		break;
	default:
		break;
	}

	return MACSUCCESS;
}

u32 mac_pinmux_record(struct mac_ax_adapter *adapter,
		      enum mac_ax_gpio_func func, u8 val)
{
	struct mac_ax_gpio_info *info = &adapter->gpio_info;

	switch (func) {
	case MAC_AX_GPIO_SW_IO_0:
		info->sw_io_0 = val;
		break;
	case MAC_AX_GPIO_SW_IO_1:
		info->sw_io_1 = val;
		break;
	case MAC_AX_GPIO_SW_IO_2:
		info->sw_io_2 = val;
		break;
	case MAC_AX_GPIO_SW_IO_3:
		info->sw_io_3 = val;
		break;
	case MAC_AX_GPIO_SW_IO_4:
		info->sw_io_4 = val;
		break;
	case MAC_AX_GPIO_SW_IO_5:
		info->sw_io_5 = val;
		break;
	case MAC_AX_GPIO_SW_IO_6:
		info->sw_io_6 = val;
		break;
	case MAC_AX_GPIO_SW_IO_7:
		info->sw_io_7 = val;
		break;
	case MAC_AX_GPIO_SW_IO_8:
		info->sw_io_8 = val;
		break;
	case MAC_AX_GPIO_SW_IO_9:
		info->sw_io_9 = val;
		break;
	case MAC_AX_GPIO_SW_IO_10:
		info->sw_io_10 = val;
		break;
	case MAC_AX_GPIO_SW_IO_11:
		info->sw_io_11 = val;
		break;
	case MAC_AX_GPIO_SW_IO_12:
		info->sw_io_12 = val;
		break;
	case MAC_AX_GPIO_SW_IO_13:
		info->sw_io_13 = val;
		break;
	case MAC_AX_GPIO_SW_IO_14:
		info->sw_io_14 = val;
		break;
	case MAC_AX_GPIO_SW_IO_15:
		info->sw_io_15 = val;
		break;
	case MAC_AX_GPIO_UART_TX_GPIO5:
		info->uart_tx_gpio5 = val;
		break;
	case MAC_AX_GPIO_UART_TX_GPIO7:
		info->uart_tx_gpio7 = val;
		break;
	case MAC_AX_GPIO_UART_TX_GPIO8:
		info->uart_tx_gpio8 = val;
		break;
	case MAC_AX_GPIO_UART_RX_GPIO6:
		info->uart_rx_gpio6 = val;
		break;
	case MAC_AX_GPIO_UART_RX_GPIO14:
		info->uart_rx_gpio14 = val;
		break;
	default:
		PLTFM_MSG_ERR("Wrong GPIO function\n");
		return MACNOITEM;
	}

	return MACSUCCESS;
}

u32 mac_pinmux_status(struct mac_ax_adapter *adapter,
		      enum mac_ax_gpio_func func)
{
	struct mac_ax_gpio_info *info = &adapter->gpio_info;
	u32 ret = MACSUCCESS;

	switch (func) {
	case MAC_AX_GPIO_SW_IO_0:
		if (info->sw_io_0)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_SW_IO_1:
		if (info->sw_io_1)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_SW_IO_2:
		if (info->sw_io_2)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_SW_IO_3:
		if (info->sw_io_3)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_SW_IO_4:
		if (info->sw_io_4)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_SW_IO_5:
		if (info->sw_io_5 || info->uart_tx_gpio5 ||
		    info->uart_tx_gpio7 || info->uart_tx_gpio8)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_UART_TX_GPIO5:
		if (info->sw_io_5 || info->uart_tx_gpio5)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_SW_IO_6:
		if (info->sw_io_6 || info->uart_rx_gpio6 ||
		    info->uart_rx_gpio14)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_UART_RX_GPIO6:
		if (info->sw_io_6 || info->uart_rx_gpio6)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_SW_IO_7:
		if (info->sw_io_7 || info->uart_tx_gpio5 ||
		    info->uart_tx_gpio7 || info->uart_tx_gpio8)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_UART_TX_GPIO7:
		if (info->sw_io_7 || info->uart_tx_gpio7)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_SW_IO_8:
		if (info->sw_io_8 || info->uart_tx_gpio5 ||
		    info->uart_tx_gpio7 || info->uart_tx_gpio8)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_UART_TX_GPIO8:
		if (info->sw_io_8 || info->uart_tx_gpio8)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_SW_IO_9:
		if (info->sw_io_9)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_SW_IO_10:
		if (info->sw_io_10)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_SW_IO_11:
		if (info->sw_io_11)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_SW_IO_12:
		if (info->sw_io_12)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_SW_IO_13:
		if (info->sw_io_13)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_SW_IO_14:
		if (info->sw_io_14 || info->uart_rx_gpio6 ||
		    info->uart_rx_gpio14)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_UART_RX_GPIO14:
		if (info->sw_io_14 || info->uart_rx_gpio14)
			goto GPIO_USED;
		break;
	case MAC_AX_GPIO_SW_IO_15:
		if (info->sw_io_15)
			goto GPIO_USED;
		break;
	default:
		ret = MACNOITEM;
		PLTFM_MSG_ERR("Wrong GPIO function\n");
	}

	return ret;
GPIO_USED:
	ret = MACGPIOUSED;
	return ret;
}

u32 mac_pinmux_free_func(struct mac_ax_adapter *adapter,
			 enum mac_ax_gpio_func func)
{
	return mac_pinmux_record(adapter, func, 0);
}

u8 get_led_gpio(u8 led_id)
{
/* LED 0 -> GPIO8 */
	switch (led_id) {
	case 0:
		return 8;
	default:
		return 0xFF;
	}
}

u32 mac_set_led_mode(struct mac_ax_adapter *adapter,
		     enum mac_ax_led_mode mode, u8 led_id)
{
#define LED_MODE_SW_CTRL 0
#define LED_MODE_AON 1
#define LED_MODE_TRX_ON 2
#define LED_MODE_TRX_OFF 3
#define LED_MODE_TX_ON 4
#define LED_MODE_TX_OFF 5
#define LED_MODE_RX_ON 6
#define LED_MODE_RX_OFF 7
	struct mac_ax_intf_ops *ops = adapter->ops->intf_ops;
	struct mac_ax_ops *mac_ops = adapter->ops;
	u32 val, ret;
	u8 tmp, gpio;

	gpio = get_led_gpio(led_id);
	if (gpio == 0xFF) {
		PLTFM_MSG_ERR("%s: Wrong LED ID: %d", __func__, led_id);
		ret = MACNOITEM;
		goto END;
	}

	val = MAC_REG_R32(R_AX_LED_CFG);

	switch (mode) {
	case MAC_AX_LED_MODE_TRX_ON:
		tmp = LED_MODE_TRX_ON;
		break;
	case MAC_AX_LED_MODE_TX_ON:
		tmp = LED_MODE_TX_ON;
		break;
	case MAC_AX_LED_MODE_RX_ON:
		tmp = LED_MODE_RX_ON;
		break;
	case MAC_AX_LED_MODE_SW_CTRL_OD:
		/* use SW IO to control LED */
		ret = mac_set_sw_gpio_mode(adapter,
					   RTW_AX_SW_IO_MODE_OUTPUT_OD, 8);
		if (ret)
			PLTFM_MSG_ERR("%s: config SW GPIO fail: %d",
				      __func__, ret);
		goto END;
	case MAC_AX_LED_MODE_SW_CTRL_PP:
		/* use SW IO to control LED */
		ret = mac_set_sw_gpio_mode(adapter,
					   RTW_AX_SW_IO_MODE_OUTPUT_PP, 8);
		if (ret)
			PLTFM_MSG_ERR("%s: config SW GPIO fail: %d",
				      __func__, ret);
		goto END;
	default:
		PLTFM_MSG_ERR("%s: Wrong LED mode: %d", __func__, mode);
		return MACNOITEM;
	}

	ret = mac_ops->set_gpio_func(adapter, RTW_MAC_GPIO_WL_LED, gpio);
	if (ret) {
		PLTFM_MSG_ERR("%s: Config LED pinmux fail", __func__);
		goto END;
	}

	val = SET_CLR_WORD(val, tmp, B_AX_LED2CM);
	val = val & ~(B_AX_LED2_EN);
	MAC_REG_W32(R_AX_LED_CFG, val);

END:
	return ret;
}

u32 mac_led_ctrl(struct mac_ax_adapter *adapter,
		 u8 high, u8 led_id)
{
	u32 ret = MACSUCCESS;
	u8 gpio;

	gpio = get_led_gpio(led_id);
	if (gpio == 0xFF) {
		PLTFM_MSG_ERR("%s: Wrong LED ID: %d", __func__, led_id);
		ret = MACNOITEM;
		goto END;
	}

	/* use SW IO to control LED */
	ret = mac_sw_gpio_ctrl(adapter, high, gpio);

END:
	return ret;
}

u32 _mac_set_sw_gpio_mode(struct mac_ax_adapter *adapter,
			  u8 output, u8 gpio)
{
	struct mac_ax_intf_ops *ops = adapter->ops->intf_ops;
	u32 reg;
	u16 val16;
	u8 in_out;

	if (gpio <= 7) {
		reg = R_AX_GPIO_PIN_CTRL + 2;
	} else if (gpio >= 8 && gpio <= 15) {
		reg = R_AX_GPIO_EXT_CTRL + 2;
		gpio = gpio - 8;
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
	} else if (gpio >= 16 && gpio <= 18 &&
		   (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D))) {
		reg = R_AX_GPIO_16_TO_18_EXT_CTRL + 2;
		gpio = gpio - 16;
#endif
	} else {
		PLTFM_MSG_ERR("%s: Wrong GPIO num: %d", __func__, gpio);
		return MACNOITEM;
	}

	in_out = (output == 0) ? 0 : 1;

	val16 = MAC_REG_R16(reg);
	val16 = (val16 & ~((u16)BIT(gpio) | (u16)BIT(gpio + 8))) |
		      (u16)(in_out << gpio) | (u16)(in_out << gpio << 8);
	MAC_REG_W16(reg, val16);

	return MACSUCCESS;
}

u32 mac_set_sw_gpio_mode(struct mac_ax_adapter *adapter,
			 enum rtw_gpio_mode mode, u8 gpio)
{
	struct mac_ax_ops *mac_ops = adapter->ops;
	struct mac_ax_gpio_info *gpio_info = &adapter->gpio_info;
	u32 ret;

	ret = mac_ops->set_gpio_func(adapter, RTW_MAC_GPIO_SW_IO, gpio);
	if (ret) {
		PLTFM_MSG_ERR("%s: Config SW IO pinmux fail", __func__);
		return ret;
	}

	switch (mode) {
	case RTW_AX_SW_IO_MODE_INPUT:
		ret = _mac_set_sw_gpio_mode(adapter, 0, gpio);
		break;
	case RTW_AX_SW_IO_MODE_OUTPUT_OD:
		ret = _mac_set_sw_gpio_mode(adapter, 0, gpio);
		gpio_info->sw_io_output[gpio] = MAC_AX_SW_IO_OUT_OD;
		break;
	case RTW_AX_SW_IO_MODE_OUTPUT_PP:
		ret = _mac_set_sw_gpio_mode(adapter, 1, gpio);
		gpio_info->sw_io_output[gpio] = MAC_AX_SW_IO_OUT_PP;
		break;
	default:
		PLTFM_MSG_ERR("%s: Wrong SW GPIO mode: %d", __func__, mode);
		ret = MACNOITEM;
		break;
	}

	return ret;
}

u32 mac_sw_gpio_ctrl(struct mac_ax_adapter *adapter,
		     u8 high, u8 gpio)
{
	struct mac_ax_intf_ops *ops = adapter->ops->intf_ops;
	struct mac_ax_gpio_info *gpio_info = &adapter->gpio_info;
	u32 reg, ret;
	u8 ctrl, val8;

	if (gpio >= MAC_AX_GPIO_NUM) {
		PLTFM_MSG_ERR("%s: Wrong GPIO num: %d", __func__, gpio);
		ret = MACNOITEM;
		goto END;
	}

	if (high && gpio_info->sw_io_output[gpio] == MAC_AX_SW_IO_OUT_OD) {
		ret = _mac_set_sw_gpio_mode(adapter, 0, gpio);
	} else {
		ret = _mac_set_sw_gpio_mode(adapter, 1, gpio);
		if (ret) {
			PLTFM_MSG_ERR("%s: Set GPIO mode fail\n", __func__);
			goto END;
		}

		if (gpio <= 7) {
			reg = R_AX_GPIO_PIN_CTRL + 1;
		} else if (gpio >= 8 && gpio <= 15) {
			reg = R_AX_GPIO_EXT_CTRL + 1;
			gpio = gpio - 8;
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		} else if (gpio >= 16 && gpio <= 18 &&
			   (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
			    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
			    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
			    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D))) {
			reg = R_AX_GPIO_16_TO_18_EXT_CTRL + 1;
			gpio = gpio - 16;
#endif
		} else {
			PLTFM_MSG_ERR("%s: Wrong GPIO num: %d", __func__, gpio);
			return MACNOITEM;
		}

		ctrl = (high == 0) ? 0 : 1;

		val8 = MAC_REG_R8(reg);
		val8 = (val8 & ~((u8)BIT(gpio))) | (u8)(ctrl << gpio);
		MAC_REG_W8(reg, val8);
	}

END:
	return ret;
}

enum rtw_mac_gfunc mac_get_gpio_status(struct mac_ax_adapter *adapter,
				       const struct mac_ax_pin_list *list)
{
	struct mac_ax_intf_ops *ops = adapter->ops->intf_ops;
	u8 val;
	enum rtw_mac_gfunc curr = RTW_MAC_GPIO_INVALID;

	while (list && list->func != RTW_MAC_GPIO_LAST) {
		/* first fit list*/
		if (curr != list->func &&
		    list->offset >= R_AX_GPIO0_7_FUNC_SEL &&
		    list->offset <= R_AX_EECS_EESK_FUNC_SEL) {
			curr = list->func;
			val = MAC_REG_R8(list->offset);
			if ((val & list->msk) == list->value)
				return list->func;
		}
		list++;
	}

	return RTW_MAC_GPIO_INVALID;
}

u32 mac_cfg_wps(struct mac_ax_adapter *adapter,
		struct mac_ax_cfg_wps *wps)
{
	u32 ret;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	struct fwcmd_cfg_wps *ptr;
	struct mac_ax_gpio_info *info = &adapter->gpio_info;

	if (wps->gpio > RTW_MAC_GPIO_MAX) {
		PLTFM_MSG_ERR("%s: Wrong GPIO num: %d", __func__, wps->gpio);
		return MACGPIONUM;
	}

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	ptr = (struct fwcmd_cfg_wps *)h2cb_put(h2cb, sizeof(*ptr));
	if (!ptr) {
		ret = MACNOBUF;
		goto fail;
	}
	PLTFM_MEMSET(ptr, 0, sizeof(*ptr));

	ptr->dword0 = cpu_to_le32((wps->en ? FWCMD_H2C_CFG_WPS_EN : 0) |
				  SET_WORD(wps->gpio,
					   FWCMD_H2C_CFG_WPS_GPIO) |
				  SET_WORD(wps->interval,
					   FWCMD_H2C_CFG_WPS_INTL));

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_MISC,
			      FWCMD_H2C_FUNC_CFG_WPS,
			      0,
			      0);

	if (ret != MACSUCCESS)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret != MACSUCCESS)
		goto fail;

#if MAC_AX_PHL_H2C
	ret = PLTFM_TX(h2cb);
#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]platform tx\n");
		goto fail;
	}

	h2cb_free(adapter, h2cb);

	h2c_end_flow(adapter);

	if (info->status[wps->gpio] != RTW_MAC_GPIO_DFLT &&
	    info->status[wps->gpio] != RTW_MAC_GPIO_SW_IO)
		PLTFM_MSG_WARN("The gpio%d is %d\n",
			       wps->gpio, info->status[wps->gpio]);

	info->status[wps->gpio] = RTW_MAC_GPIO_SW_IO;

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_get_gpio_val(struct mac_ax_adapter *adapter, u8 gpio, u8 *val)
{
	struct mac_ax_intf_ops *ops = adapter->ops->intf_ops;
	u32 reg;

	if (gpio <= 7) {
		reg = R_AX_GPIO_PIN_CTRL;
	} else if (gpio >= 8 && gpio <= 15) {
		reg = R_AX_GPIO_EXT_CTRL;
		gpio = gpio - 8;
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
	} else if (gpio >= 16 && gpio <= 18 &&
		   (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D))) {
		reg = R_AX_GPIO_16_TO_18_EXT_CTRL;
		gpio = gpio - 16;
#endif
	} else {
		PLTFM_MSG_ERR("%s: Wrong GPIO num: %d", __func__, gpio);
		return MACNOITEM;
	}

	*val = !!(MAC_REG_R8(reg) & BIT(gpio));

	return MACSUCCESS;
}

u32 mac_get_wl_dis_gpio(struct mac_ax_adapter *adapter, u8 *gpio)
{
#define MAC_AX_HCI_SEL_SDIO_UART 0
#define MAC_AX_HCI_SEL_USB_MULT 1
#define MAC_AX_HCI_SEL_PCIE_UART 2
#define MAC_AX_HCI_SEL_PCIE_USB 3
#define MAC_AX_HCI_SEL_SDIO_MULT 4
#define MAC_AX_HCI_SEL_RSVD 5
#define MAC_AX_HCI_SEL_PCIE_GEN1_UART 6
#define MAC_AX_HCI_SEL_PCIE_GEN1_USB 7
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val;

	val = MAC_REG_R32(R_AX_SYS_STATUS1);
	val = GET_FIELD(val, B_AX_HCI_SEL_V4);

#if MAC_AX_8852A_SUPPORT
	/* In AP, */
	/*   MAC_AX_HCI_SEL_PCIE_UART and MAC_AX_HCI_SEL_SDIO_UART */
	/*   are only supported in 2G eFEM, not in 5G/5G 6G*/
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
		switch (val) {
#ifdef PHL_FEATURE_AP
		case MAC_AX_HCI_SEL_PCIE_UART:
		case MAC_AX_HCI_SEL_SDIO_UART:
			*gpio = 9;
			break;
#else
		case MAC_AX_HCI_SEL_USB_MULT:
		case MAC_AX_HCI_SEL_PCIE_UART:
		case MAC_AX_HCI_SEL_PCIE_USB:
			*gpio = 9;
			break;
		case MAC_AX_HCI_SEL_SDIO_UART:
			*gpio = 15;
			break;
#endif
		default:
			PLTFM_MSG_ERR("%s: Wrong HCI\n", __func__);
			return MACNOITEM;
		}
	}
#endif

#if MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
		switch (val) {
		case MAC_AX_HCI_SEL_USB_MULT:
		case MAC_AX_HCI_SEL_PCIE_UART:
		case MAC_AX_HCI_SEL_PCIE_USB:
		case MAC_AX_HCI_SEL_PCIE_GEN1_UART:
		case MAC_AX_HCI_SEL_PCIE_GEN1_USB:
			*gpio = 9;
			break;
		case MAC_AX_HCI_SEL_SDIO_UART:
		case MAC_AX_HCI_SEL_SDIO_MULT:
			*gpio = 15;
			break;
		default:
			PLTFM_MSG_ERR("%s: Wrong HCI\n", __func__);
			return MACNOITEM;
		}
	}
#endif

#if MAC_AX_8852C_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		switch (val) {
		case MAC_AX_HCI_SEL_PCIE_USB:
		case MAC_AX_HCI_SEL_PCIE_GEN1_UART:
		case MAC_AX_HCI_SEL_PCIE_GEN1_USB:
			*gpio = 9;
			break;
		case MAC_AX_HCI_SEL_SDIO_UART:
		case MAC_AX_HCI_SEL_SDIO_MULT:
			*gpio = 17;
			break;
		default:
			PLTFM_MSG_ERR("%s: Wrong HCI\n", __func__);
			return MACNOITEM;
		}
	}
#endif

#if MAC_AX_8192XB_SUPPORT
	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB)) {
		switch (val) {
		case MAC_AX_HCI_SEL_USB_MULT: /* USB */
		case MAC_AX_HCI_SEL_PCIE_USB: /* PCIE */
			*gpio = 9;
			break;
		case MAC_AX_HCI_SEL_SDIO_MULT: /* SDIO */
			*gpio = 15;
			break;
		default:
			PLTFM_MSG_ERR("%s: Wrong HCI\n", __func__);
			return MACNOITEM;
		}
	}
#endif

	return MACSUCCESS;
}

u32 mac_get_wl_dis_val(struct mac_ax_adapter *adapter, u8 *val)
{
	u8 gpio;
	u32 ret;
	struct mac_ax_ops *ops = adapter_to_mac_ops(adapter);

	ret = mac_get_wl_dis_gpio(adapter, &gpio);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("%s: Get WL_DIS GPIO fail\n", __func__);
		return ret;
	}

	ret = ops->set_sw_gpio_mode(adapter, RTW_AX_SW_IO_MODE_INPUT, gpio);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("%s: Set SW output mode fail\n", __func__);
		return ret;
	}

	return ops->get_gpio_val(adapter, gpio, val);
}

u32 mac_get_uart_fw_dbg_gpio(struct mac_ax_adapter *adapter, u8 *uart_tx_gpio, u8 *uart_rx_gpio)
{
	if (adapter->gpio_info.uart_tx_gpio == 0xFF &&
	    adapter->gpio_info.uart_rx_gpio == 0xFF) {
		/* not initialized*/
		return MACNOTSUP;
	}

	*uart_tx_gpio = adapter->gpio_info.uart_tx_gpio;
	*uart_rx_gpio = adapter->gpio_info.uart_rx_gpio;

	return MACSUCCESS;
}

