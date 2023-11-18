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
#ifndef __PLATFORM_OPS_H__
#define __PLATFORM_OPS_H__

/*
 * Return:
 *0:	power on successfully
 *others: power on failed
 */

#ifdef CONFIG_PLATFORM_OPS
void pci_cache_wback(struct pci_dev *hwdev,
	dma_addr_t *bus_addr, size_t size, int direction);
void pci_cache_inv(struct pci_dev *hwdev,
	dma_addr_t *bus_addr, size_t size, int direction);
void pci_get_bus_addr(struct pci_dev *hwdev,
	void *vir_addr, dma_addr_t *bus_addr, size_t size, int direction);
void pci_unmap_bus_addr(struct pci_dev *hwdev,
	dma_addr_t *bus_addr, size_t size, int direction);
void *pci_alloc_cache_mem(struct pci_dev *pdev,
	dma_addr_t *bus_addr, size_t size, int direction);
void *pci_alloc_noncache_mem(struct pci_dev *pdev,
	dma_addr_t *bus_addr, size_t size);
void pci_free_cache_mem(struct pci_dev *pdev, void *vir_addr,
	dma_addr_t *bus_addr, size_t size, int direction);
void pci_free_noncache_mem(struct pci_dev *pdev, void *vir_addr,
	dma_addr_t *bus_addr, size_t size);
#endif
int platform_wifi_power_on(void);
void platform_wifi_power_off(void);
void platform_wifi_get_oob_irq(int *oob_irq);
void platform_wifi_mac_addr(u8 *mac_addr);
#endif /* __PLATFORM_OPS_H__ */
