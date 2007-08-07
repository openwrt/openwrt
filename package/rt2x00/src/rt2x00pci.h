/*
	Copyright (C) 2004 - 2007 rt2x00 SourceForge Project
	<http://rt2x00.serialmonkey.com>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the
	Free Software Foundation, Inc.,
	59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/*
	Module: rt2x00pci
	Abstract: Data structures for the rt2x00pci module.
	Supported chipsets: rt2460, rt2560, rt2561, rt2561s & rt2661.
 */

#ifndef RT2X00PCI_H
#define RT2X00PCI_H

/*
 * This variable should be used with the
 * pci_driver structure initialization.
 */
#define PCI_DEVICE_DATA(__ops)	.driver_data = (kernel_ulong_t)(__ops)

/*
 * Register defines.
 * When register access attempts should be repeated
 * only REGISTER_BUSY_COUNT attempts with a delay
 * of REGISTER_BUSY_DELAY us should be taken.
 */
#define REGISTER_BUSY_COUNT	5
#define REGISTER_BUSY_DELAY	100

/*
 * Descriptor availability flags.
 * All PCI device descriptors have these 2 flags
 * with the exact same definition.
 */
#define TXD_ENTRY_OWNER_NIC	FIELD32(0x00000001)
#define TXD_ENTRY_VALID		FIELD32(0x00000002)
#define RXD_ENTRY_OWNER_NIC	FIELD32(0x00000001)

/*
 * Register access.
 */
static inline void rt2x00pci_register_read(const struct rt2x00_dev *rt2x00dev,
	const unsigned long offset, u32 *value)
{
	*value = readl(rt2x00dev->csr_addr + offset);
}

static inline void rt2x00pci_register_multiread(
	const struct rt2x00_dev *rt2x00dev,
	const unsigned long offset, void *value, const u16 length)
{
	memcpy_fromio(value, rt2x00dev->csr_addr + offset, length);
}

static inline void rt2x00pci_register_write(const struct rt2x00_dev *rt2x00dev,
	const unsigned long offset, u32 value)
{
	writel(value, rt2x00dev->csr_addr + offset);
}

static inline void rt2x00pci_register_multiwrite(
	const struct rt2x00_dev *rt2x00dev,
	const unsigned long offset, void *value, const u16 length)
{
	memcpy_toio(rt2x00dev->csr_addr + offset, value, length);
}

/*
 * Beacon handlers.
 */
int rt2x00pci_beacon_update(struct ieee80211_hw *hw, struct sk_buff *skb,
	struct ieee80211_tx_control *control);
void rt2x00pci_beacondone(struct rt2x00_dev *rt2x00dev, const int queue);

/*
 * TX data handlers.
 */
int rt2x00pci_write_tx_data(struct rt2x00_dev *rt2x00dev,
	struct data_ring *ring, struct sk_buff *skb,
	struct ieee80211_tx_control *control);

/*
 * RX data handlers.
 */
void rt2x00pci_rxdone(struct rt2x00_dev *rt2x00dev);

/*
 * Device initialization handlers.
 */
int rt2x00pci_initialize(struct rt2x00_dev *rt2x00dev);
void rt2x00pci_uninitialize(struct rt2x00_dev *rt2x00dev);

/*
 * PCI driver handlers.
 */
int rt2x00pci_probe(struct pci_dev *pci_dev, const struct pci_device_id *id);
void rt2x00pci_remove(struct pci_dev *pci_dev);
#ifdef CONFIG_PM
int rt2x00pci_suspend(struct pci_dev *pci_dev, pm_message_t state);
int rt2x00pci_resume(struct pci_dev *pci_dev);
#endif /* CONFIG_PM */

#endif /* RT2X00PCI_H */
