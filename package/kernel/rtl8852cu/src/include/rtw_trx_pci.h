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
#ifndef _RTW_TRX_PCI_H_
#define _RTW_TRX_PCI_H_

extern struct rtw_intf_ops pci_ops;

static inline u8 is_pci_support_dma64(struct dvobj_priv *dvobj)
{
	PPCI_DATA pci_data = dvobj_to_pci(dvobj);

	return (pci_data->bdma64 == _TRUE) ? _TRUE : _FALSE;
}
#endif /* _RTW_TRX_PCI_H_ */
