/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
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
/*

The purpose of rtw_io.c

a. provides the API

b. provides the protocol engine

c. provides the software interface between caller and the hardware interface


Compiler Flag Option:

1. CONFIG_SDIO_HCI:
    a. USE_SYNC_IRP:  Only sync operations are provided.
    b. USE_ASYNC_IRP:Both sync/async operations are provided.

2. CONFIG_USB_HCI:
   a. USE_ASYNC_IRP: Both sync/async operations are provided.

3. CONFIG_CFIO_HCI:
   b. USE_SYNC_IRP: Only sync operations are provided.


Only sync read/rtw_write_mem operations are provided.

jackson@realtek.com.tw

*/

#define _RTW_IO_C_

#include <drv_types.h>

/*
* Increase and check if the continual_io_error of this @param dvobjprive is larger than MAX_CONTINUAL_IO_ERR
* @return _TRUE:
* @return _FALSE:
*/
int rtw_inc_and_chk_continual_io_error(struct dvobj_priv *dvobj)
{
	int ret = _FALSE;
	int value;

	value = ATOMIC_INC_RETURN(&dvobj->continual_io_error);
	if (value > MAX_CONTINUAL_IO_ERR) {
		RTW_INFO("[dvobj:%p][ERROR] continual_io_error:%d > %d\n", dvobj, value, MAX_CONTINUAL_IO_ERR);
		ret = _TRUE;
	} else {
		/* RTW_INFO("[dvobj:%p] continual_io_error:%d\n", dvobj, value); */
	}
	return ret;
}

/*
* Set the continual_io_error of this @param dvobjprive to 0
*/
void rtw_reset_continual_io_error(struct dvobj_priv *dvobj)
{
	ATOMIC_SET(&dvobj->continual_io_error, 0);
}
