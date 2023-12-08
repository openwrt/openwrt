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
#define _RTW_TRX_USB_C_
#include <drv_types.h>		/* struct dvobj_priv and etc. */


/********************************xmit section*******************************/
static void usb_xmit_tasklet(unsigned long data)
{
#ifdef CONFIG_TX_AMSDU_SW_MODE
	_adapter *padapter = (_adapter *) data;

	core_tx_amsdu_tasklet(padapter);
#endif
}

s32 usb_init_xmit_priv(_adapter *adapter)
{
#ifdef CONFIG_TX_AMSDU_SW_MODE
	struct xmit_priv *pxmitpriv = &adapter->xmitpriv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	rtw_tasklet_init(&pxmitpriv->xmit_tasklet,
		     usb_xmit_tasklet,
		     (unsigned long)adapter);

#endif
	return _SUCCESS;
}

void usb_free_xmit_priv(_adapter *adapter)
{
}

/******************************** recv section*******************************/
int usb_init_recv_priv(struct dvobj_priv *dvobj)
{
	int res = _SUCCESS;

	return res;
}

void usb_free_recv_priv(struct dvobj_priv *dvobj)
{

}

struct rtw_intf_ops usb_ops = {
	.init_xmit_priv = usb_init_xmit_priv,
	.free_xmit_priv = usb_free_xmit_priv,

	.init_recv_priv = usb_init_recv_priv,
	.free_recv_priv = usb_free_recv_priv,

};

