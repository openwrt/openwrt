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
#ifndef __PCI_OPS_LINUX_H__
#define __PCI_OPS_LINUX_H__

u8 os_pci_read8(struct dvobj_priv *dvobj, u32 addr);
u16 os_pci_read16(struct dvobj_priv *dvobj, u32 addr);
u32 os_pci_read32(struct dvobj_priv *dvobj, u32 addr);
int os_pci_write8(struct dvobj_priv *dvobj, u32 addr, u8 val);
int os_pci_write16(struct dvobj_priv *dvobj, u32 addr, u16 val);
int os_pci_write32(struct dvobj_priv *dvobj, u32 addr, u32 val);
#endif /*__PCI_OPS_LINUX_H__*/
