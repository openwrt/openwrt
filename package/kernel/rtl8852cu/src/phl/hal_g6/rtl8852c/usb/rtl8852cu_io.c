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
#define _RTL8852CU_IO_C_
#include "../rtl8852c_hal.h"

void hal_usb_set_io_ops_8852cu(struct hal_info_t *hal, struct hal_io_ops *ops)
{
/*
	ops->_read8 = &usb_read8;
	ops->_read16 = &usb_read16;
	ops->_read32 = &usb_read32;

	ops->_write8 = &usb_write8;
	ops->_write16 = &usb_write16;
	ops->_write32 = &usb_write32;
	ops->_write_mem = &usb_write_mem;
*/
}
