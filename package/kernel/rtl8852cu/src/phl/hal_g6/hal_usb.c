/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
 *****************************************************************************/
#define _HAL_USB_C_
#include "hal_headers.h"
#include "hal_usb.h"

#ifdef CONFIG_USB_HCI
u8 usb_read8(struct rtw_hal_com_t *hal, u32 addr)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u8 data = 0;

	request = 0x05;
	requesttype = 0x01;/* read_in */
	index = (u16)((addr & 0x00ff0000) >> 16);
	wvalue = (u16)(addr & 0x0000ffff);
	len = 1;

	_os_usbctrl_vendorreq(hal->drv_priv, request, wvalue, index,
				&data, len, requesttype);

	return data;
}

u16 usb_read16(struct rtw_hal_com_t *hal, u32 addr)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u16 data = 0;

	request = 0x05;
	requesttype = 0x01;/* read_in */
	index = (u16)((addr & 0x00ff0000) >> 16);
	wvalue = (u16)(addr & 0x0000ffff);
	len = 2;

	_os_usbctrl_vendorreq(hal->drv_priv, request, wvalue, index,
				&data, len, requesttype);
	return le16_to_cpu(data);

}

u32 usb_read32(struct rtw_hal_com_t *hal, u32 addr)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u32 data = 0;

	request = 0x05;
	requesttype = 0x01;/* read_in */
	index = (u16)((addr & 0x00ff0000) >> 16);
	wvalue = (u16)(addr & 0x0000ffff);
	len = 4;

	_os_usbctrl_vendorreq(hal->drv_priv, request, wvalue, index,
				&data, len, requesttype);
	return le32_to_cpu(data);
}

int usb_write8(struct rtw_hal_com_t *hal, u32 addr, u8 val)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u8 data;
	int ret;


	request = 0x05;
	requesttype = 0x00;/* write_out */
	index = (u16)((addr & 0x00ff0000) >> 16);
	wvalue = (u16)(addr & 0x0000ffff);
	len = 1;
	data = val;

	ret = _os_usbctrl_vendorreq(hal->drv_priv, 
			request, wvalue, index, &data, len, requesttype);
	return ret;
}

int usb_write16(struct rtw_hal_com_t *hal, u32 addr, u16 val)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u16 data;
	int ret;

	request = 0x05;
	requesttype = 0x00;/* write_out */
	index = (u16)((addr & 0x00ff0000) >> 16);
	wvalue = (u16)(addr & 0x0000ffff);
	len = 2;
	data = cpu_to_le16(val);

	ret = _os_usbctrl_vendorreq(hal->drv_priv,
			request, wvalue, index,	&data, len, requesttype);

	return ret;

}

int usb_write32(struct rtw_hal_com_t *hal, u32 addr, u32 val)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	u32 data;
	int ret;

	request = 0x05;
	requesttype = 0x00;/* write_out */
	index = (u16)((addr & 0x00ff0000) >> 16);
	wvalue = (u16)(addr & 0x0000ffff);
	len = 4;
	data = cpu_to_le32(val);

	ret = _os_usbctrl_vendorreq(hal->drv_priv,
			request, wvalue, index,	&data, len, requesttype);

	return ret;

}

int usb_write_mem(struct rtw_hal_com_t *hal, u32 addr, u32 length, u8 *pdata)
{
	u8 request;
	u8 requesttype;
	u16 wvalue;
	u16 index;
	u16 len;
	int ret;
	u8 buf[MAX_VENDOR_REQ_CMD_SIZE] = {0};

	request = 0x05;
	requesttype = 0x00;/* write_out */
	index = (u16)((addr & 0x00ff0000) >> 16);
	wvalue = (u16)(addr & 0x0000ffff);
	len = (u16)length;

	_os_mem_cpy(hal->drv_priv, buf, pdata, len);

	ret = _os_usbctrl_vendorreq(hal->drv_priv,
			request, wvalue, index, pdata, len, requesttype);

	return ret;

}
#ifdef RTW_WKARD_BUS_WRITE
static int usb_write_post_cfg(struct rtw_hal_com_t *hal, u32 addr, u32 val)
{
	struct hal_info_t	*hal_info = hal->hal_priv;
	struct hal_ops_t	*hal_ops = hal_get_ops(hal_info);

	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	
	if(NULL != hal_ops->write_reg_post_cfg) {
		hal_status = hal_ops->write_reg_post_cfg(hal_info, addr, val);
	}

	return hal_status;
}
#endif
static void hal_usb_set_io_ops_gen(struct rtw_hal_com_t *hal, struct hal_io_ops *pops)
{
	/*_rtw_memset((u8 *)pops, 0, sizeof(struct hal_io_ops));*/
	_os_mem_set(hal->drv_priv, (u8 *)pops, 0, sizeof(struct hal_io_ops));

	pops->_read8 = &usb_read8;
	pops->_read16 = &usb_read16;
	pops->_read32 = &usb_read32;
	pops->_read_mem = NULL;

	pops->_write8 = &usb_write8;
	pops->_write16 = &usb_write16;
	pops->_write32 = &usb_write32;
	pops->_write_mem = &usb_write_mem;
#ifdef RTW_WKARD_BUS_WRITE
	pops->_write_post_cfg = &usb_write_post_cfg;
#endif
}

void hal_usb_set_io_ops(struct rtw_hal_com_t *hal, struct hal_io_ops *pops)
{
	hal_usb_set_io_ops_gen(hal, pops);
	/*hal_usb_set_io_ops_8852au(hal, pops);*/
}


/* To avoid RX affect TX throughput */
#ifdef CONFIG_PHL_USB_RX_AGGREGATION
void rtw_hal_usb_adjust_txagg(void *h)
{
	#if 0
	struct hal_info_t *hal = (struct hal_info_t *)h;
	struct registry_priv *registry_par = &padapter->registrypriv;

	if (!registry_par->dynamic_agg_enable)
		return;

	if (IS_HARDWARE_TYPE_8822BU(padapter) || IS_HARDWARE_TYPE_8821CU(padapter)
		|| IS_HARDWARE_TYPE_8822CU(padapter) || IS_HARDWARE_TYPE_8814BU(padapter))
		rtw_hal_set_hwreg(padapter, HW_VAR_RXDMA_AGG_PG_TH, NULL);
	#endif
}
#endif /* CONFIG_PHL_USB_RX_AGGREGATION */

enum rtw_hal_status rtw_hal_force_usb_switch(void *h, enum usb_type type)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	u32 mode = hal_mac_get_cur_usb_mode(hal);
	bool bswitch = false;

	bswitch = ((enum usb_type)mode != type) ? (true) : (false);
	PHL_INFO("%s, switch from %d to %d\n", __func__,
							mode , type);
	if(bswitch)
		return hal_mac_force_usb_switch(hal);
	else
		return RTW_HAL_STATUS_SUCCESS;
}

u32 rtwl_hal_get_cur_usb_mode(void *h)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	/*refer to enum usb_type*/
	return hal_mac_get_cur_usb_mode(hal);
}
u32 rtwl_hal_get_usb_support_ability(void *h)
{
	struct hal_info_t *hal = (struct hal_info_t *)h;
	/*
	* refers to _usb.h
	* #define SWITCHMODE           0x2
	* #define FORCEUSB3MODE        0x1
	* #define FORCEUSB2MODE        0x0
	*/
	return hal_mac_get_usb_support_ability(hal);
}
#endif /*CONFIG_USB_HCI*/
