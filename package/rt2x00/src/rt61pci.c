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
	Module: rt61pci
	Abstract: rt61pci device specific routines.
	Supported chipsets: RT2561, RT2561s, RT2661.
 */

/*
 * Set enviroment defines for rt2x00.h
 */
#define DRV_NAME "rt61pci"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/dma-mapping.h>
#include <linux/delay.h>
#include <linux/etherdevice.h>
#include <linux/eeprom_93cx6.h>

#include <asm/io.h>

#include "rt2x00.h"
#include "rt2x00lib.h"
#include "rt2x00pci.h"
#include "rt61pci.h"

/*
 * Register access.
 * BBP and RF register require indirect register access,
 * and use the CSR registers PHY_CSR3 and PHY_CSR4 to achieve this.
 * These indirect registers work with busy bits,
 * and we will try maximal REGISTER_BUSY_COUNT times to access
 * the register while taking a REGISTER_BUSY_DELAY us delay
 * between each attampt. When the busy bit is still set at that time,
 * the access attempt is considered to have failed,
 * and we will print an error.
 */
static u32 rt61pci_bbp_check(const struct rt2x00_dev *rt2x00dev)
{
	u32 reg;
	unsigned int i;

	for (i = 0; i < REGISTER_BUSY_COUNT; i++) {
		rt2x00pci_register_read(rt2x00dev, PHY_CSR3, &reg);
		if (!rt2x00_get_field32(reg, PHY_CSR3_BUSY))
			break;
		udelay(REGISTER_BUSY_DELAY);
	}

	return reg;
}

static void rt61pci_bbp_write(const struct rt2x00_dev *rt2x00dev,
	const u8 reg_id, const u8 value)
{
	u32 reg;

	/*
	 *  Wait until the BBP becomes ready.
	 */
	reg = rt61pci_bbp_check(rt2x00dev);
	if (rt2x00_get_field32(reg, PHY_CSR3_BUSY)) {
		ERROR(rt2x00dev, "PHY_CSR3 register busy. Write failed.\n");
		return;
	}

	/*
	 * Write the data into the BBP.
	 */
	reg = 0;
	rt2x00_set_field32(&reg, PHY_CSR3_VALUE, value);
	rt2x00_set_field32(&reg, PHY_CSR3_REGNUM, reg_id);
	rt2x00_set_field32(&reg, PHY_CSR3_BUSY, 1);
	rt2x00_set_field32(&reg, PHY_CSR3_READ_CONTROL, 0);

	rt2x00pci_register_write(rt2x00dev, PHY_CSR3, reg);
}

static void rt61pci_bbp_read(const struct rt2x00_dev *rt2x00dev,
	const u8 reg_id, u8 *value)
{
	u32 reg;

	/*
	 *  Wait until the BBP becomes ready.
	 */
	reg = rt61pci_bbp_check(rt2x00dev);
	if (rt2x00_get_field32(reg, PHY_CSR3_BUSY)) {
		ERROR(rt2x00dev, "PHY_CSR3 register busy. Read failed.\n");
		return;
	}

	/*
	 * Write the request into the BBP.
	 */
	reg =0;
	rt2x00_set_field32(&reg, PHY_CSR3_REGNUM, reg_id);
	rt2x00_set_field32(&reg, PHY_CSR3_BUSY, 1);
	rt2x00_set_field32(&reg, PHY_CSR3_READ_CONTROL, 1);

	rt2x00pci_register_write(rt2x00dev, PHY_CSR3, reg);

	/*
	 *  Wait until the BBP becomes ready.
	 */
	reg = rt61pci_bbp_check(rt2x00dev);
	if (rt2x00_get_field32(reg, PHY_CSR3_BUSY)) {
		ERROR(rt2x00dev, "PHY_CSR3 register busy. Read failed.\n");
		*value = 0xff;
		return;
	}

	*value = rt2x00_get_field32(reg, PHY_CSR3_VALUE);
}

static void rt61pci_rf_write(const struct rt2x00_dev *rt2x00dev,
	const u32 value)
{
	u32 reg;
	unsigned int i;

	for (i = 0; i < REGISTER_BUSY_COUNT; i++) {
		rt2x00pci_register_read(rt2x00dev, PHY_CSR4, &reg);
		if (!rt2x00_get_field32(reg, PHY_CSR4_BUSY))
			goto rf_write;
		udelay(REGISTER_BUSY_DELAY);
	}

	ERROR(rt2x00dev, "PHY_CSR4 register busy. Write failed.\n");
	return;

rf_write:
	reg = 0;
	rt2x00_set_field32(&reg, PHY_CSR4_VALUE, value);
	rt2x00_set_field32(&reg, PHY_CSR4_NUMBER_OF_BITS, 21);
	rt2x00_set_field32(&reg, PHY_CSR4_IF_SELECT, 0);
	rt2x00_set_field32(&reg, PHY_CSR4_BUSY, 1);

	rt2x00pci_register_write(rt2x00dev, PHY_CSR4, reg);
}

static void rt61pci_mcu_request(const struct rt2x00_dev *rt2x00dev,
	const u8 command, const u8 token, const u8 arg0, const u8 arg1)
{
	u32 reg;

	rt2x00pci_register_read(rt2x00dev, H2M_MAILBOX_CSR, &reg);

	if (rt2x00_get_field32(reg, H2M_MAILBOX_CSR_OWNER)) {
		ERROR(rt2x00dev, "mcu request error. "
			"Request 0x%02x failed for token 0x%02x.\n",
			command, token);
		return;
	}

	rt2x00_set_field32(&reg, H2M_MAILBOX_CSR_OWNER, 1);
	rt2x00_set_field32(&reg, H2M_MAILBOX_CSR_CMD_TOKEN, token);
	rt2x00_set_field32(&reg, H2M_MAILBOX_CSR_ARG0, arg0);
	rt2x00_set_field32(&reg, H2M_MAILBOX_CSR_ARG1, arg1);
	rt2x00pci_register_write(rt2x00dev, H2M_MAILBOX_CSR, reg);

	rt2x00pci_register_read(rt2x00dev, HOST_CMD_CSR, &reg);
	rt2x00_set_field32(&reg, HOST_CMD_CSR_HOST_COMMAND, command);
	rt2x00_set_field32(&reg, HOST_CMD_CSR_INTERRUPT_MCU, 1);
	rt2x00pci_register_write(rt2x00dev, HOST_CMD_CSR, reg);
}

static void rt61pci_eepromregister_read(struct eeprom_93cx6 *eeprom)
{
	struct rt2x00_dev *rt2x00dev = eeprom->data;
	u32 reg;

	rt2x00pci_register_read(rt2x00dev, E2PROM_CSR, &reg);

	eeprom->reg_data_in = !!rt2x00_get_field32(reg,
		E2PROM_CSR_DATA_IN);
	eeprom->reg_data_out = !!rt2x00_get_field32(reg,
		E2PROM_CSR_DATA_OUT);
	eeprom->reg_data_clock = !!rt2x00_get_field32(reg,
		E2PROM_CSR_DATA_CLOCK);
	eeprom->reg_chip_select = !!rt2x00_get_field32(reg,
		E2PROM_CSR_CHIP_SELECT);
}

static void rt61pci_eepromregister_write(struct eeprom_93cx6 *eeprom)
{
	struct rt2x00_dev *rt2x00dev = eeprom->data;
	u32 reg = 0;

	rt2x00_set_field32(&reg, E2PROM_CSR_DATA_IN,
		!!eeprom->reg_data_in);
	rt2x00_set_field32(&reg, E2PROM_CSR_DATA_OUT,
		!!eeprom->reg_data_out);
	rt2x00_set_field32(&reg, E2PROM_CSR_DATA_CLOCK,
		!!eeprom->reg_data_clock);
	rt2x00_set_field32(&reg, E2PROM_CSR_CHIP_SELECT,
		!!eeprom->reg_chip_select);

	rt2x00pci_register_write(rt2x00dev, E2PROM_CSR, reg);
}

#ifdef CONFIG_RT2X00_LIB_DEBUGFS
#define CSR_OFFSET(__word)	( CSR_REG_BASE + ((__word) * sizeof(u32)) )

static void rt61pci_read_csr(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt2x00pci_register_read(rt2x00dev, CSR_OFFSET(word), data);
}

static void rt61pci_write_csr(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt2x00pci_register_write(rt2x00dev, CSR_OFFSET(word), *((u32*)data));
}

static void rt61pci_read_eeprom(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt2x00_eeprom_read(rt2x00dev, word, data);
}

static void rt61pci_write_eeprom(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt2x00_eeprom_write(rt2x00dev, word, *((u16*)data));
}

static void rt61pci_read_bbp(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt61pci_bbp_read(rt2x00dev, word, data);
}

static void rt61pci_write_bbp(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt61pci_bbp_write(rt2x00dev, word, *((u8*)data));
}

static const struct rt2x00debug rt61pci_rt2x00debug = {
	.owner 		= THIS_MODULE,
	.reg_csr	= {
		.read		= rt61pci_read_csr,
		.write		= rt61pci_write_csr,
		.word_size	= sizeof(u32),
		.word_count	= CSR_REG_SIZE / sizeof(u32),
	},
	.reg_eeprom	= {
		.read		= rt61pci_read_eeprom,
		.write		= rt61pci_write_eeprom,
		.word_size	= sizeof(u16),
		.word_count	= EEPROM_SIZE / sizeof(u16),
	},
	.reg_bbp	= {
		.read		= rt61pci_read_bbp,
		.write		= rt61pci_write_bbp,
		.word_size	= sizeof(u8),
		.word_count	= BBP_SIZE / sizeof(u8),
	},
};
#endif /* CONFIG_RT2X00_LIB_DEBUGFS */

#ifdef CONFIG_RT61PCI_RFKILL
static int rt61pci_rfkill_poll(struct rt2x00_dev *rt2x00dev)
{
	u32 reg;

	rt2x00pci_register_read(rt2x00dev, MAC_CSR13, &reg);
	return rt2x00_get_field32(reg, MAC_CSR13_BIT5);;
}
#endif /* CONFIG_RT2400PCI_RFKILL */

/*
 * Configuration handlers.
 */
static void rt61pci_config_bssid(struct rt2x00_dev *rt2x00dev, u8 *bssid)
{
	u32 reg[2];

	memset(&reg, 0, sizeof(reg));
	memcpy(&reg, bssid, ETH_ALEN);

	rt2x00_set_field32(&reg[1], MAC_CSR5_BSS_ID_MASK, 3);

	/*
	 * The BSSID is passed to us as an array of bytes,
	 * that array is little endian, so no need for byte ordering.
	 */
	rt2x00pci_register_multiwrite(rt2x00dev, MAC_CSR4, &reg, sizeof(reg));
}

static void rt61pci_config_promisc(struct rt2x00_dev *rt2x00dev,
	const int promisc)
{
	u32 reg;

	rt2x00pci_register_read(rt2x00dev, TXRX_CSR0, &reg);
	rt2x00_set_field32(&reg, TXRX_CSR0_DROP_NOT_TO_ME, !promisc);
	rt2x00pci_register_write(rt2x00dev, TXRX_CSR0, reg);
}

static void rt61pci_config_type(struct rt2x00_dev *rt2x00dev,
	const int type)
{
	u32 reg;

	rt2x00pci_register_write(rt2x00dev, TXRX_CSR9, 0);

	/*
	 * Apply hardware packet filter.
	 */
	rt2x00pci_register_read(rt2x00dev, TXRX_CSR0, &reg);

	if (!is_monitor_present(&rt2x00dev->interface) &&
	    (type == IEEE80211_IF_TYPE_IBSS || type == IEEE80211_IF_TYPE_STA))
		rt2x00_set_field32(&reg, TXRX_CSR0_DROP_TO_DS, 1);
	else
		rt2x00_set_field32(&reg, TXRX_CSR0_DROP_TO_DS, 0);

	rt2x00_set_field32(&reg, TXRX_CSR0_DROP_CRC, 1);
	if (is_monitor_present(&rt2x00dev->interface)) {
		rt2x00_set_field32(&reg, TXRX_CSR0_DROP_PHYSICAL, 0);
		rt2x00_set_field32(&reg, TXRX_CSR0_DROP_CONTROL, 0);
		rt2x00_set_field32(&reg, TXRX_CSR0_DROP_VERSION_ERROR, 0);
	} else {
		rt2x00_set_field32(&reg, TXRX_CSR0_DROP_PHYSICAL, 1);
		rt2x00_set_field32(&reg, TXRX_CSR0_DROP_CONTROL, 1);
		rt2x00_set_field32(&reg, TXRX_CSR0_DROP_VERSION_ERROR, 1);
	}

	rt2x00_set_field32(&reg, TXRX_CSR0_DROP_MULTICAST, 0);
	rt2x00_set_field32(&reg, TXRX_CSR0_DROP_BORADCAST, 0);

	rt2x00pci_register_write(rt2x00dev, TXRX_CSR0, reg);

	/*
	 * Enable synchronisation.
	 */
	rt2x00pci_register_read(rt2x00dev, TXRX_CSR9, &reg);
	if (is_interface_present(&rt2x00dev->interface)) {
		rt2x00_set_field32(&reg, TXRX_CSR9_TSF_TICKING, 1);
		rt2x00_set_field32(&reg, TXRX_CSR9_TBTT_ENABLE, 1);
	}

	rt2x00_set_field32(&reg, TXRX_CSR9_BEACON_GEN, 0);
	if (type == IEEE80211_IF_TYPE_IBSS || type == IEEE80211_IF_TYPE_AP)
		rt2x00_set_field32(&reg, TXRX_CSR9_TSF_SYNC, 2);
	else if (type == IEEE80211_IF_TYPE_STA)
		rt2x00_set_field32(&reg, TXRX_CSR9_TSF_SYNC, 1);
	else if (is_monitor_present(&rt2x00dev->interface) &&
		 !is_interface_present(&rt2x00dev->interface))
		rt2x00_set_field32(&reg, TXRX_CSR9_TSF_SYNC, 0);

	rt2x00pci_register_write(rt2x00dev, TXRX_CSR9, reg);
}

static void rt61pci_config_channel(struct rt2x00_dev *rt2x00dev,
	const int value, const int channel, const int txpower)
{
	u8 reg = 0;
	u32 rf1 = 0;
	u32 rf2 = value;
	u32 rf3 = 0;
	u32 rf4 = 0;

	if (!test_bit(CONFIG_RF_SEQUENCE, &rt2x00dev->flags) || channel <= 14)
		rf1 = 0x00002ccc;
	else if (channel == 36 ||
		(channel >= 100 && channel <= 116) ||
		channel >= 157)
		rf1 = 0x00002cd4;
	else
		rf1 = 0x00002cd0;

	if (channel <= 14) {
		rf3 = 0x00068455;
	} else if (!test_bit(CONFIG_RF_SEQUENCE, &rt2x00dev->flags)) {
		if (channel >= 36 && channel <= 48)
			rf3 = 0x0009be55;
		else if (channel >= 52 && channel <= 64)
			rf3 = 0x0009ae55;
		else if (channel >= 100 && channel <= 112)
			rf3 = 0x000bae55;
		else
			rf3 = 0x000bbe55;
	} else {
		switch (channel) {
			case 36:
			case 40:
			case 44:
				rf3 = 0x00098455;
				break;
			case 48:
				rf3 = 0x00098655;
				break;
			case 52:
				rf3 = 0x00098855;
				break;
			case 56:
				rf3 = 0x00098c55;

			case 60:
				rf3 = 0x00098e55;
				break;
			case 64:
				rf3 = 0x00099255;
				break;
			case 100:
			case 104:
			case 108:
				rf3 = 0x000b9855;
				break;
			case 112:
			case 116:
			case 120:
			case 124:
				rf3 = 0x000b9a55;
				break;
			case 128:
			case 132:
				rf3 = 0x000b9c55;
				break;
			case 136:
			case 140:
				rf3 = 0x000b9e55;
				break;
			case 149:
			case 153:
			case 157:
			case 161:
			case 165:
				rf3 = 0x000ba255;
				break;
		}
	}

	if (channel < 14) {
		if (channel & 1)
			rf4 = 0x000ffa0b;
		else
			rf4 = 0x000ffa1f;
	} else if (channel == 14) {
		rf4 = 0x000ffa13;
	} else if (!test_bit(CONFIG_RF_SEQUENCE, &rt2x00dev->flags)) {
		switch (channel) {
			case 36:
			case 56:
			case 116:
			case 136:
				rf4 = 0x000ffa23;
				break;
			case 40:
			case 60:
			case 100:
			case 120:
			case 140:
				rf4 = 0x000ffa03;
				break;
			case 44:
			case 64:
			case 104:
			case 124:
				rf4 = 0x000ffa0b;
				break;
			case 48:
			case 108:
			case 128:
				rf4 = 0x000ffa13;
				break;
			case 52:
			case 112:
			case 132:
				rf4 = 0x000ffa1b;
				break;
			case 149:
				rf4 = 0x000ffa1f;
				break;
			case 153:
				rf4 = 0x000ffa27;
				break;
			case 157:
				rf4 = 0x000ffa07;
				break;
			case 161:
				rf4 = 0x000ffa0f;
				break;
			case 165:
				rf4 = 0x000ffa17;
				break;
		}
	} else {
		switch (channel) {
			case 36:
			case 40:
			case 60:
			case 140:
			case 100:
			case 104:
			case 108:
			case 112:
			case 116:
			case 120:
				rf4 = 0x000c0a03;
				break;
			case 44:
			case 64:
			case 124:
			case 149:
				rf4 = 0x000c0a1b;
				break;
			case 48:
			case 128:
			case 153:
				rf4 = 0x000c0a0b;
				break;
			case 52:
			case 132:
				rf4 = 0x000c0a23;
				break;
			case 56:
			case 136:
				rf4 = 0x000c0a13;
				break;
			case 157:
			case 161:
			case 165:
				rf4 = 0x000c0a17;
				break;
		}
	}

	/*
	 * Set TXpower.
	 */
	rt2x00_set_field32(&rf3, RF3_TXPOWER, TXPOWER_TO_DEV(txpower));

	/*
	 * Set Frequency offset.
	 */
	rt2x00_set_field32(&rf4, RF4_FREQ_OFFSET, rt2x00dev->freq_offset);

	rt61pci_rf_write(rt2x00dev, rf1);
	rt61pci_rf_write(rt2x00dev, rf2);
	rt61pci_rf_write(rt2x00dev, rf3 & ~0x00000004);
	rt61pci_rf_write(rt2x00dev, rf4);

	udelay(200);

	rt61pci_rf_write(rt2x00dev, rf1);
	rt61pci_rf_write(rt2x00dev, rf2);
	rt61pci_rf_write(rt2x00dev, rf3 | 0x00000004);
	rt61pci_rf_write(rt2x00dev, rf4);

	udelay(200);

	rt61pci_rf_write(rt2x00dev, rf1);
	rt61pci_rf_write(rt2x00dev, rf2);
	rt61pci_rf_write(rt2x00dev, rf3 & ~0x00000004);
	rt61pci_rf_write(rt2x00dev, rf4);

	rt61pci_bbp_read(rt2x00dev, 3, &reg);
	if (rt2x00_rf(&rt2x00dev->chip, RF5225) ||
	    rt2x00_rf(&rt2x00dev->chip, RF2527))
		reg &= ~0x01;
	else
		reg |= 0x01;
	rt61pci_bbp_write(rt2x00dev, 3, reg);

	msleep(1);

	/*
	 * Update rf fields
	 */
	rt2x00dev->rf1 = rf1;
	rt2x00dev->rf2 = rf2;
	rt2x00dev->rf3 = rf3;
	rt2x00dev->rf4 = rf4;
	rt2x00dev->tx_power = txpower;
}

static void rt61pci_config_txpower(struct rt2x00_dev *rt2x00dev,
	const int txpower)
{
	rt2x00_set_field32(&rt2x00dev->rf3, RF3_TXPOWER,
		TXPOWER_TO_DEV(txpower));

	rt61pci_rf_write(rt2x00dev, rt2x00dev->rf1);
	rt61pci_rf_write(rt2x00dev, rt2x00dev->rf2);
	rt61pci_rf_write(rt2x00dev, rt2x00dev->rf3 & ~0x00000004);
	rt61pci_rf_write(rt2x00dev, rt2x00dev->rf4);

	udelay(200);

	rt61pci_rf_write(rt2x00dev, rt2x00dev->rf1);
	rt61pci_rf_write(rt2x00dev, rt2x00dev->rf2);
	rt61pci_rf_write(rt2x00dev, rt2x00dev->rf3 | 0x00000004);
	rt61pci_rf_write(rt2x00dev, rt2x00dev->rf4);

	udelay(200);

	rt61pci_rf_write(rt2x00dev, rt2x00dev->rf1);
	rt61pci_rf_write(rt2x00dev, rt2x00dev->rf2);
	rt61pci_rf_write(rt2x00dev, rt2x00dev->rf3 & ~0x00000004);
	rt61pci_rf_write(rt2x00dev, rt2x00dev->rf4);
}

static void rt61pci_config_antenna(struct rt2x00_dev *rt2x00dev,
	const int antenna_tx, const int antenna_rx)
{
	u32 reg;
	u8 r3;
	u8 r4;
	u8 r77;

	rt2x00pci_register_read(rt2x00dev, PHY_CSR0, &reg);

	if (rt2x00dev->curr_hwmode == HWMODE_A) {
		if (test_bit(CONFIG_EXTERNAL_LNA_A, &rt2x00dev->flags)) {
			rt61pci_bbp_write(rt2x00dev, 17, 0x38);
			rt61pci_bbp_write(rt2x00dev, 96, 0x78);
			rt61pci_bbp_write(rt2x00dev, 104, 0x48);
			rt61pci_bbp_write(rt2x00dev, 75, 0x80);
			rt61pci_bbp_write(rt2x00dev, 86, 0x80);
			rt61pci_bbp_write(rt2x00dev, 88, 0x80);
		} else {
			rt61pci_bbp_write(rt2x00dev, 17, 0x28);
			rt61pci_bbp_write(rt2x00dev, 96, 0x58);
			rt61pci_bbp_write(rt2x00dev, 104, 0x38);
			rt61pci_bbp_write(rt2x00dev, 75, 0xfe);
			rt61pci_bbp_write(rt2x00dev, 86, 0xfe);
			rt61pci_bbp_write(rt2x00dev, 88, 0xfe);
		}
		rt61pci_bbp_write(rt2x00dev, 35, 0x60);
		rt61pci_bbp_write(rt2x00dev, 97, 0x58);
		rt61pci_bbp_write(rt2x00dev, 98, 0x58);

		rt2x00_set_field32(&reg, PHY_CSR0_PA_PE_BG, 0);
		rt2x00_set_field32(&reg, PHY_CSR0_PA_PE_A, 1);
	} else {
		if (test_bit(CONFIG_EXTERNAL_LNA_BG, &rt2x00dev->flags)) {
			rt61pci_bbp_write(rt2x00dev, 17, 0x30);
			rt61pci_bbp_write(rt2x00dev, 96, 0x68);
			rt61pci_bbp_write(rt2x00dev, 104, 0x3c);
			rt61pci_bbp_write(rt2x00dev, 75, 0x80);
			rt61pci_bbp_write(rt2x00dev, 86, 0x80);
			rt61pci_bbp_write(rt2x00dev, 88, 0x80);
		} else {
			rt61pci_bbp_write(rt2x00dev, 17, 0x20);
			rt61pci_bbp_write(rt2x00dev, 96, 0x48);
			rt61pci_bbp_write(rt2x00dev, 104, 0x2c);
			rt61pci_bbp_write(rt2x00dev, 75, 0xfe);
			rt61pci_bbp_write(rt2x00dev, 86, 0xfe);
			rt61pci_bbp_write(rt2x00dev, 88, 0xfe);
		}
		rt61pci_bbp_write(rt2x00dev, 35, 0x50);
		rt61pci_bbp_write(rt2x00dev, 97, 0x48);
		rt61pci_bbp_write(rt2x00dev, 98, 0x48);

		rt2x00_set_field32(&reg, PHY_CSR0_PA_PE_BG, 1);
		rt2x00_set_field32(&reg, PHY_CSR0_PA_PE_A, 0);
	}

	rt2x00pci_register_write(rt2x00dev, PHY_CSR0, reg);

	rt61pci_bbp_read(rt2x00dev, 3, &r3);
	rt61pci_bbp_read(rt2x00dev, 4, &r4);
	rt61pci_bbp_read(rt2x00dev, 77, &r77);

	if (rt2x00_rf(&rt2x00dev->chip, RF5225) ||
	    rt2x00_rf(&rt2x00dev->chip, RF2527))
		rt2x00_set_field8(&r3, BBP_R3_SMART_MODE, 0);

	if (rt2x00_rf(&rt2x00dev->chip, RF5225) ||
	    rt2x00_rf(&rt2x00dev->chip, RF5325)) {
		if (antenna_rx == ANTENNA_DIVERSITY) {
			rt2x00_set_field8(&r4, BBP_R4_RX_ANTENNA, 2);
			if (rt2x00dev->curr_hwmode != HWMODE_A)
				rt2x00_set_field8(&r4, BBP_R4_RX_BG_MODE, 1);
		} else if (antenna_rx == ANTENNA_A) {
			rt2x00_set_field8(&r4, BBP_R4_RX_ANTENNA, 1);
			if (rt2x00dev->curr_hwmode == HWMODE_A)
				rt2x00_set_field8(&r77, BBP_R77_PAIR, 0);
			else
				rt2x00_set_field8(&r77, BBP_R77_PAIR, 3);
			rt61pci_bbp_write(rt2x00dev, 77, r77);
		} else if (antenna_rx == ANTENNA_B) {
			rt2x00_set_field8(&r4, BBP_R4_RX_ANTENNA, 1);
			if (rt2x00dev->curr_hwmode == HWMODE_A)
				rt2x00_set_field8(&r77, BBP_R77_PAIR, 3);
			else
				rt2x00_set_field8(&r77, BBP_R77_PAIR, 0);
			rt61pci_bbp_write(rt2x00dev, 77, r77);
		}
	} else if (rt2x00_rf(&rt2x00dev->chip, RF2527) ||
		   (rt2x00_rf(&rt2x00dev->chip, RF2529) &&
		    test_bit(CONFIG_DOUBLE_ANTENNA, &rt2x00dev->flags))) {
		if (antenna_rx == ANTENNA_DIVERSITY) {
			rt2x00_set_field8(&r4, BBP_R4_RX_ANTENNA, 2);
			rt2x00_set_field8(&r4, BBP_R4_RX_BG_MODE, 1);
			rt2x00_set_field8(&r4, BBP_R4_RX_FRAME_END,
				test_bit(CONFIG_FRAME_TYPE, &rt2x00dev->flags));
		} else if (antenna_rx == ANTENNA_A) {
			rt2x00_set_field8(&r4, BBP_R4_RX_ANTENNA, 1);
			rt2x00_set_field8(&r4, BBP_R4_RX_BG_MODE, 1);
			rt2x00_set_field8(&r4, BBP_R4_RX_FRAME_END,
				test_bit(CONFIG_FRAME_TYPE, &rt2x00dev->flags));
			rt2x00_set_field8(&r77, BBP_R77_PAIR, 3);
			rt61pci_bbp_write(rt2x00dev, 77, r77);
		} else if (antenna_rx == ANTENNA_B) {
			rt2x00_set_field8(&r4, BBP_R4_RX_ANTENNA, 1);
			rt2x00_set_field8(&r4, BBP_R4_RX_BG_MODE, 1);
			rt2x00_set_field8(&r4, BBP_R4_RX_FRAME_END,
				test_bit(CONFIG_FRAME_TYPE, &rt2x00dev->flags));
			rt2x00_set_field8(&r77, BBP_R77_PAIR, 0);
			rt61pci_bbp_write(rt2x00dev, 77, r77);
		}
	}

	/*
	 * TODO: RF2529 with another antenna value then 2 are ignored.
	 * The legacy driver is unclear whether in those cases there is
	 * a possibility to switch antenna.
	 */

	rt61pci_bbp_write(rt2x00dev, 3, r3);
	rt61pci_bbp_write(rt2x00dev, 4, r4);
}

static void rt61pci_config_duration(struct rt2x00_dev *rt2x00dev,
	const int short_slot_time, const int beacon_int)
{
	u32 reg;

	rt2x00pci_register_read(rt2x00dev, MAC_CSR9, &reg);
	rt2x00_set_field32(&reg, MAC_CSR9_SLOT_TIME,
		short_slot_time ? SHORT_SLOT_TIME : SLOT_TIME);
	rt2x00pci_register_write(rt2x00dev, MAC_CSR9, reg);

	rt2x00pci_register_read(rt2x00dev, MAC_CSR8, &reg);
	rt2x00_set_field32(&reg, MAC_CSR8_SIFS, SIFS);
	rt2x00_set_field32(&reg, MAC_CSR8_SIFS_AFTER_RX_OFDM, 3);
	rt2x00_set_field32(&reg, MAC_CSR8_EIFS, EIFS);
	rt2x00pci_register_write(rt2x00dev, MAC_CSR8, reg);

	rt2x00pci_register_read(rt2x00dev, TXRX_CSR0, &reg);
	rt2x00_set_field32(&reg, TXRX_CSR0_TSF_OFFSET, IEEE80211_HEADER);
	rt2x00pci_register_write(rt2x00dev, TXRX_CSR0, reg);

	rt2x00pci_register_read(rt2x00dev, TXRX_CSR4, &reg);
	rt2x00_set_field32(&reg, TXRX_CSR4_AUTORESPOND_ENABLE, 1);
	rt2x00pci_register_write(rt2x00dev, TXRX_CSR4, reg);

	rt2x00pci_register_read(rt2x00dev, TXRX_CSR9, &reg);
	rt2x00_set_field32(&reg, TXRX_CSR9_BEACON_INTERVAL, beacon_int * 16);
	rt2x00pci_register_write(rt2x00dev, TXRX_CSR9, reg);
}

static void rt61pci_config_rate(struct rt2x00_dev *rt2x00dev, const int rate)
{
	struct ieee80211_conf *conf = &rt2x00dev->hw->conf;
	u32 reg;
	u32 value;
	u32 preamble;

	preamble = DEVICE_GET_RATE_FIELD(rate, PREAMBLE)
		? SHORT_PREAMBLE : PREAMBLE;

	/*
	 * Extract the allowed ratemask from the device specific rate value,
	 * We need to set TXRX_CSR5 to the basic rate mask so we need to mask
	 * off the non-basic rates.
	 */
	reg = DEVICE_GET_RATE_FIELD(rate, RATEMASK) & DEV_BASIC_RATE;

	rt2x00pci_register_write(rt2x00dev, TXRX_CSR5, reg);

	rt2x00pci_register_read(rt2x00dev, TXRX_CSR0, &reg);
	value = ((conf->flags & IEEE80211_CONF_SHORT_SLOT_TIME) ?
		 SHORT_DIFS :  DIFS) +
		PLCP + preamble + get_duration(ACK_SIZE, 10);
	rt2x00_set_field32(&reg, TXRX_CSR0_RX_ACK_TIMEOUT, value);
	rt2x00pci_register_write(rt2x00dev, TXRX_CSR0, reg);

	rt2x00pci_register_read(rt2x00dev, TXRX_CSR4, &reg);
	if (preamble == SHORT_PREAMBLE)
		rt2x00_set_field32(&reg, TXRX_CSR4_AUTORESPOND_PREAMBLE, 1);
	else
		rt2x00_set_field32(&reg, TXRX_CSR4_AUTORESPOND_PREAMBLE, 0);
	rt2x00pci_register_write(rt2x00dev, TXRX_CSR4, reg);
}

static void rt61pci_config_phymode(struct rt2x00_dev *rt2x00dev,
	const int phymode)
{
	struct ieee80211_hw_mode *mode;
	struct ieee80211_rate *rate;

	if (phymode == MODE_IEEE80211A)
		rt2x00dev->curr_hwmode = HWMODE_A;
	else if (phymode == MODE_IEEE80211B)
		rt2x00dev->curr_hwmode = HWMODE_B;
	else
		rt2x00dev->curr_hwmode = HWMODE_G;

	mode = &rt2x00dev->hwmodes[rt2x00dev->curr_hwmode];
	rate = &mode->rates[mode->num_rates - 1];

	rt61pci_config_rate(rt2x00dev, rate->val2);
}

static void rt61pci_config_mac_addr(struct rt2x00_dev *rt2x00dev, u8 *addr)
{
	u32 reg[2];

	memset(&reg, 0, sizeof(reg));
	memcpy(&reg, addr, ETH_ALEN);

	rt2x00_set_field32(&reg[1], MAC_CSR3_UNICAST_TO_ME_MASK, 0xff);

	/*
	 * The MAC address is passed to us as an array of bytes,
	 * that array is little endian, so no need for byte ordering.
	 */
	rt2x00pci_register_multiwrite(rt2x00dev, MAC_CSR2, &reg, sizeof(reg));
}

/*
 * LED functions.
 */
static void rt61pci_enable_led(struct rt2x00_dev *rt2x00dev)
{
	u32 reg;
	u16 led_reg;
	u8 arg0;
	u8 arg1;

	rt2x00pci_register_read(rt2x00dev, MAC_CSR14, &reg);
	rt2x00_set_field32(&reg, MAC_CSR14_ON_PERIOD, 70);
	rt2x00_set_field32(&reg, MAC_CSR14_OFF_PERIOD, 30);
	rt2x00pci_register_write(rt2x00dev, MAC_CSR14, reg);

	led_reg = rt2x00dev->led_reg;
	rt2x00_set_field16(&led_reg, MCU_LEDCS_RADIO_STATUS, 1);
	if (rt2x00dev->rx_status.phymode == MODE_IEEE80211A)
		rt2x00_set_field16(&led_reg, MCU_LEDCS_LINK_A_STATUS, 1);
	else
		rt2x00_set_field16(&led_reg, MCU_LEDCS_LINK_BG_STATUS, 1);

	arg0 = led_reg & 0xff;
	arg1 = (led_reg >> 8) & 0xff;

	rt61pci_mcu_request(rt2x00dev, MCU_LED, 0xff, arg0, arg1);
}

static void rt61pci_disable_led(struct rt2x00_dev *rt2x00dev)
{
	u16 led_reg;
	u8 arg0;
	u8 arg1;

	led_reg = rt2x00dev->led_reg;
	rt2x00_set_field16(&led_reg, MCU_LEDCS_RADIO_STATUS, 0);
	rt2x00_set_field16(&led_reg, MCU_LEDCS_LINK_BG_STATUS, 0);
	rt2x00_set_field16(&led_reg, MCU_LEDCS_LINK_A_STATUS, 0);

	arg0 = led_reg & 0xff;
	arg1 = (led_reg >> 8) & 0xff;

	rt61pci_mcu_request(rt2x00dev, MCU_LED, 0xff, arg0, arg1);
}

static void rt61pci_activity_led(struct rt2x00_dev *rt2x00dev, int rssi)
{
	u8 led;

	if (rt2x00dev->led_mode != LED_MODE_SIGNAL_STRENGTH)
		return;

	/*
	 * Led handling requires a positive value for the rssi,
	 * to do that correctly we need to add the correction.
	 */
	rssi += rt2x00dev->rssi_offset;

	if (rssi <= 30)
		led = 0;
	else if (rssi <= 39)
		led = 1;
	else if (rssi <= 49)
		led = 2;
	else if (rssi <= 53)
		led = 3;
	else if (rssi <= 63)
		led = 4;
	else
		led = 5;

	rt61pci_mcu_request(rt2x00dev, MCU_LED_STRENGTH, 0xff, led, 0);
}

/*
 * Link tuning
 */
static void rt61pci_link_tuner(struct rt2x00_dev *rt2x00dev)
{
	int rssi = rt2x00_get_link_rssi(&rt2x00dev->link);
	u32 reg;
	u8 r17;
	u8 up_bound;
	u8 low_bound;

	/*
	 * Update Led strength
	 */
	rt61pci_activity_led(rt2x00dev, rssi);

	rt61pci_bbp_read(rt2x00dev, 17, &r17);

	/*
	 * Determine r17 bounds.
	 */
	if (rt2x00dev->rx_status.phymode == MODE_IEEE80211A) {
		low_bound = 0x28;
		up_bound = 0x48;
		if (test_bit(CONFIG_EXTERNAL_LNA_A, &rt2x00dev->flags)) {
			low_bound += 0x10;
			up_bound += 0x10;
		}
	} else {
		low_bound = 0x20;
		up_bound = 0x40;
		if (test_bit(CONFIG_EXTERNAL_LNA_BG, &rt2x00dev->flags)) {
			low_bound += 0x10;
			up_bound += 0x10;
		}
	}

	/*
	 * Special big-R17 for very short distance
	 */
	if (rssi >= -35) {
		if (r17 != 0x60)
			rt61pci_bbp_write(rt2x00dev, 17, 0x60);
		return;
	}

	/*
	 * Special big-R17 for short distance
	 */
	if (rssi >= -58) {
		if (r17 != up_bound)
			rt61pci_bbp_write(rt2x00dev, 17, up_bound);
		return;
	}

	/*
	 * Special big-R17 for middle-short distance
	 */
	if (rssi >= -66) {
		low_bound += 0x10;
		if (r17 != low_bound)
			rt61pci_bbp_write(rt2x00dev, 17, low_bound);
		return;
	}

	/*
	 * Special mid-R17 for middle distance
	 */
	if (rssi >= -74) {
		low_bound += 0x08;
		if (r17 != low_bound)
			rt61pci_bbp_write(rt2x00dev, 17, low_bound);
		return;
	}

	/*
	 * Special case: Change up_bound based on the rssi.
	 * Lower up_bound when rssi is weaker then -74 dBm.
	 */
	up_bound -= 2 * (-74 - rssi);
	if (low_bound > up_bound)
		up_bound = low_bound;

	if (r17 > up_bound) {
		rt61pci_bbp_write(rt2x00dev, 17, up_bound);
		return;
	}

	/*
	 * r17 does not yet exceed upper limit, continue and base
	 * the r17 tuning on the false CCA count.
	 */
	rt2x00pci_register_read(rt2x00dev, STA_CSR1, &reg);
	rt2x00dev->link.false_cca =
		rt2x00_get_field32(reg, STA_CSR1_FALSE_CCA_ERROR);

	if (rt2x00dev->link.false_cca > 512 && r17 < up_bound) {
		if (++r17 > up_bound)
			r17 = up_bound;
		rt61pci_bbp_write(rt2x00dev, 17, r17);
		rt2x00dev->rx_status.noise = r17;
	} else if (rt2x00dev->link.false_cca < 100 && r17 > low_bound) {
		if (--r17 < low_bound)
			r17 = low_bound;
		rt61pci_bbp_write(rt2x00dev, 17, r17);
		rt2x00dev->rx_status.noise = r17;
	}
}

/*
 * Firmware name function.
 */
static char *rt61pci_get_fw_name(struct rt2x00_dev *rt2x00dev)
{
	char *fw_name;

	switch (rt2x00dev->chip.rt) {
	case RT2561:
		fw_name = FIRMWARE_RT2561;
		break;
	case RT2561s:
		fw_name = FIRMWARE_RT2561s;
		break;
	case RT2661:
		fw_name = FIRMWARE_RT2661;
		break;
	default:
		fw_name = NULL;
		break;
	}

	return fw_name;
}

/*
 * Initialization functions.
 */
static int rt61pci_load_firmware(struct rt2x00_dev *rt2x00dev, void *data,
	const size_t len)
{
	int i;
	u32 reg;

	/*
	 * Wait for stable hardware.
	 */
	for (i = 0; i < 100; i++) {
		rt2x00pci_register_read(rt2x00dev, MAC_CSR0, &reg);
		if (reg)
			break;
		msleep(1);
	}

	if (!reg) {
		ERROR(rt2x00dev, "Unstable hardware.\n");
		return -EBUSY;
	}

	/*
	 * Prepare MCU and mailbox for firmware loading.
	 */
	reg = 0;
	rt2x00_set_field32(&reg, MCU_CNTL_CSR_RESET, 1);
	rt2x00pci_register_write(rt2x00dev, MCU_CNTL_CSR, reg);
	rt2x00pci_register_write(rt2x00dev, M2H_CMD_DONE_CSR, 0xffffffff);
	rt2x00pci_register_write(rt2x00dev, H2M_MAILBOX_CSR, 0);
	rt2x00pci_register_write(rt2x00dev, HOST_CMD_CSR, 0);

	/*
	 * Write firmware to device.
	 */
	reg = 0;
	rt2x00_set_field32(&reg, MCU_CNTL_CSR_RESET, 1);
	rt2x00_set_field32(&reg, MCU_CNTL_CSR_SELECT_BANK, 1);
	rt2x00pci_register_write(rt2x00dev, MCU_CNTL_CSR, reg);

	rt2x00pci_register_multiwrite(
		rt2x00dev, FIRMWARE_IMAGE_BASE, data, len);

	rt2x00_set_field32(&reg, MCU_CNTL_CSR_SELECT_BANK, 0);
	rt2x00pci_register_write(rt2x00dev, MCU_CNTL_CSR, reg);

	rt2x00_set_field32(&reg, MCU_CNTL_CSR_RESET, 0);
	rt2x00pci_register_write(rt2x00dev, MCU_CNTL_CSR, reg);

	for (i = 0; i < 100; i++) {
		rt2x00pci_register_read(rt2x00dev, MCU_CNTL_CSR, &reg);
		if (rt2x00_get_field32(reg, MCU_CNTL_CSR_READY))
			break;
		msleep(1);
	}

	if (i == 100) {
		ERROR(rt2x00dev, "MCU Control register not ready.\n");
		return -EBUSY;
	}

	/*
	 * Reset MAC and BBP registers.
	 */
	reg = 0;
	rt2x00_set_field32(&reg, MAC_CSR1_SOFT_RESET, 1);
	rt2x00_set_field32(&reg, MAC_CSR1_BBP_RESET, 1);
	rt2x00pci_register_write(rt2x00dev, MAC_CSR1, reg);

	rt2x00pci_register_read(rt2x00dev, MAC_CSR1, &reg);
	rt2x00_set_field32(&reg, MAC_CSR1_SOFT_RESET, 0);
	rt2x00_set_field32(&reg, MAC_CSR1_BBP_RESET, 0);
	rt2x00pci_register_write(rt2x00dev, MAC_CSR1, reg);

	rt2x00pci_register_read(rt2x00dev, MAC_CSR1, &reg);
	rt2x00_set_field32(&reg, MAC_CSR1_HOST_READY, 1);
	rt2x00pci_register_write(rt2x00dev, MAC_CSR1, reg);

	return 0;
}

static void rt61pci_init_rxring(struct rt2x00_dev *rt2x00dev)
{
	struct data_desc *rxd;
	unsigned int i;
	u32 word;

	memset(rt2x00dev->rx->data_addr, 0x00,
		rt2x00_get_ring_size(rt2x00dev->rx));

	for (i = 0; i < rt2x00dev->rx->stats.limit; i++) {
		rxd = rt2x00dev->rx->entry[i].priv;

		rt2x00_desc_read(rxd, 5, &word);
		rt2x00_set_field32(&word, RXD_W5_BUFFER_PHYSICAL_ADDRESS,
			rt2x00dev->rx->entry[i].data_dma);
		rt2x00_desc_write(rxd, 5, word);

		rt2x00_desc_read(rxd, 0, &word);
		rt2x00_set_field32(&word, RXD_W0_OWNER_NIC, 1);
		rt2x00_desc_write(rxd, 0, word);
	}

	rt2x00_ring_index_clear(rt2x00dev->rx);
}

static void rt61pci_init_txring(struct rt2x00_dev *rt2x00dev,
	const int queue)
{
	struct data_ring *ring = rt2x00_get_ring(rt2x00dev, queue);
	struct data_desc *txd;
	unsigned int i;
	u32 word;

	memset(ring->data_addr, 0x00, rt2x00_get_ring_size(ring));

	for (i = 0; i < ring->stats.limit; i++) {
		txd = ring->entry[i].priv;

		rt2x00_desc_read(txd, 1, &word);
		rt2x00_set_field32(&word, TXD_W1_BUFFER_COUNT, 1);
		rt2x00_desc_write(txd, 1, word);

		rt2x00_desc_read(txd, 5, &word);
		rt2x00_set_field32(&word, TXD_W5_PID_TYPE, queue);
		rt2x00_set_field32(&word, TXD_W5_PID_SUBTYPE, i);
		rt2x00_desc_write(txd, 5, word);

		rt2x00_desc_read(txd, 6, &word);
		rt2x00_set_field32(&word, TXD_W6_BUFFER_PHYSICAL_ADDRESS,
			ring->entry[i].data_dma);
		rt2x00_desc_write(txd, 6, word);

		rt2x00_desc_read(txd, 0, &word);
		rt2x00_set_field32(&word, TXD_W0_VALID, 0);
		rt2x00_set_field32(&word, TXD_W0_OWNER_NIC, 0);
		rt2x00_desc_write(txd, 0, word);
	}

	rt2x00_ring_index_clear(ring);
}

static int rt61pci_init_rings(struct rt2x00_dev *rt2x00dev)
{
	u32 reg;

	/*
	 * Initialize rings.
	 */
	rt61pci_init_rxring(rt2x00dev);
	rt61pci_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_DATA0);
	rt61pci_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_DATA1);
	rt61pci_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_DATA2);
	rt61pci_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_DATA3);
	rt61pci_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_DATA4);
	rt61pci_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_BEACON);

	/*
	 * Initialize registers.
	 */
	rt2x00pci_register_read(rt2x00dev, TX_RING_CSR0, &reg);
	rt2x00_set_field32(&reg, TX_RING_CSR0_AC0_RING_SIZE,
		rt2x00dev->tx[IEEE80211_TX_QUEUE_DATA0].stats.limit);
	rt2x00_set_field32(&reg, TX_RING_CSR0_AC1_RING_SIZE,
		rt2x00dev->tx[IEEE80211_TX_QUEUE_DATA1].stats.limit);
	rt2x00_set_field32(&reg, TX_RING_CSR0_AC2_RING_SIZE,
		rt2x00dev->tx[IEEE80211_TX_QUEUE_DATA2].stats.limit);
	rt2x00_set_field32(&reg, TX_RING_CSR0_AC3_RING_SIZE,
		rt2x00dev->tx[IEEE80211_TX_QUEUE_DATA3].stats.limit);
	rt2x00pci_register_write(rt2x00dev, TX_RING_CSR0, reg);

	rt2x00pci_register_read(rt2x00dev, TX_RING_CSR1, &reg);
	rt2x00_set_field32(&reg, TX_RING_CSR1_MGMT_RING_SIZE,
		rt2x00dev->tx[IEEE80211_TX_QUEUE_DATA4].stats.limit);
	rt2x00_set_field32(&reg, TX_RING_CSR1_TXD_SIZE,
		rt2x00dev->tx[IEEE80211_TX_QUEUE_DATA0].desc_size / 4);
	rt2x00pci_register_write(rt2x00dev, TX_RING_CSR1, reg);

	rt2x00pci_register_read(rt2x00dev, AC0_BASE_CSR, &reg);
	rt2x00_set_field32(&reg, AC0_BASE_CSR_RING_REGISTER,
		rt2x00dev->tx[IEEE80211_TX_QUEUE_DATA0].data_dma);
	rt2x00pci_register_write(rt2x00dev, AC0_BASE_CSR, reg);

	rt2x00pci_register_read(rt2x00dev, AC1_BASE_CSR, &reg);
	rt2x00_set_field32(&reg, AC1_BASE_CSR_RING_REGISTER,
		rt2x00dev->tx[IEEE80211_TX_QUEUE_DATA1].data_dma);
	rt2x00pci_register_write(rt2x00dev, AC1_BASE_CSR, reg);

	rt2x00pci_register_read(rt2x00dev, AC2_BASE_CSR, &reg);
	rt2x00_set_field32(&reg, AC2_BASE_CSR_RING_REGISTER,
		rt2x00dev->tx[IEEE80211_TX_QUEUE_DATA2].data_dma);
	rt2x00pci_register_write(rt2x00dev, AC2_BASE_CSR, reg);

	rt2x00pci_register_read(rt2x00dev, AC3_BASE_CSR, &reg);
	rt2x00_set_field32(&reg, AC3_BASE_CSR_RING_REGISTER,
		rt2x00dev->tx[IEEE80211_TX_QUEUE_DATA3].data_dma);
	rt2x00pci_register_write(rt2x00dev, AC3_BASE_CSR, reg);

	rt2x00pci_register_read(rt2x00dev, MGMT_BASE_CSR, &reg);
	rt2x00_set_field32(&reg, MGMT_BASE_CSR_RING_REGISTER,
		rt2x00dev->tx[IEEE80211_TX_QUEUE_DATA4].data_dma);
	rt2x00pci_register_write(rt2x00dev, MGMT_BASE_CSR, reg);

	rt2x00pci_register_read(rt2x00dev, RX_RING_CSR, &reg);
	rt2x00_set_field32(&reg, RX_RING_CSR_RING_SIZE,
		rt2x00dev->rx->stats.limit);
	rt2x00_set_field32(&reg, RX_RING_CSR_RXD_SIZE,
		rt2x00dev->rx->desc_size / 4);
	rt2x00_set_field32(&reg, RX_RING_CSR_RXD_WRITEBACK_SIZE, 4);
	rt2x00pci_register_write(rt2x00dev, RX_RING_CSR, reg);

	rt2x00pci_register_read(rt2x00dev, RX_BASE_CSR, &reg);
	rt2x00_set_field32(&reg, RX_BASE_CSR_RING_REGISTER,
		rt2x00dev->rx->data_dma);
	rt2x00pci_register_write(rt2x00dev, RX_BASE_CSR, reg);

	rt2x00pci_register_write(rt2x00dev, TX_DMA_DST_CSR, 0x000000aa);
	rt2x00pci_register_write(rt2x00dev, LOAD_TX_RING_CSR, 0x0000001f);
	rt2x00pci_register_write(rt2x00dev, RX_CNTL_CSR, 0x00000002);

	return 0;
}

static int rt61pci_init_registers(struct rt2x00_dev *rt2x00dev)
{
	u32 reg;

	if (rt2x00dev->ops->lib->set_device_state(rt2x00dev, STATE_AWAKE))
		return -EBUSY;

	rt2x00pci_register_write(rt2x00dev, MAC_CSR10, 0x00000718);

	rt2x00pci_register_read(rt2x00dev, TXRX_CSR0, &reg);
	rt2x00_set_field32(&reg, TXRX_CSR0_AUTO_TX_SEQ, 1);
	rt2x00_set_field32(&reg, TXRX_CSR0_DISABLE_RX, 1);
	rt2x00_set_field32(&reg, TXRX_CSR0_DROP_ACK_CTS, 1);
	rt2x00_set_field32(&reg, TXRX_CSR0_TX_WITHOUT_WAITING, 0);
	rt2x00pci_register_write(rt2x00dev, TXRX_CSR0, reg);

	rt2x00pci_register_write(rt2x00dev, TXRX_CSR1, 0x9eb39eb3);
	rt2x00pci_register_write(rt2x00dev, TXRX_CSR2, 0x8a8b8c8d);
	rt2x00pci_register_write(rt2x00dev, TXRX_CSR3, 0x00858687);

	rt2x00pci_register_write(rt2x00dev, TXRX_CSR7, 0x2e31353b);
	rt2x00pci_register_write(rt2x00dev, TXRX_CSR8, 0x2a2a2a2c);

	rt2x00pci_register_write(rt2x00dev, TXRX_CSR15, 0x0000000f);

	rt2x00pci_register_write(rt2x00dev, MAC_CSR6, 0x00000fff);

	rt2x00pci_register_write(rt2x00dev, MAC_CSR13, 0x0000e000);

	rt2x00pci_register_write(rt2x00dev, SEC_CSR0, 0x00000000);
	rt2x00pci_register_write(rt2x00dev, SEC_CSR1, 0x00000000);
	rt2x00pci_register_write(rt2x00dev, SEC_CSR5, 0x00000000);

	rt2x00pci_register_read(rt2x00dev, AC_TXOP_CSR0, &reg);
	rt2x00_set_field32(&reg, AC_TXOP_CSR0_AC0_TX_OP, 0);
	rt2x00_set_field32(&reg, AC_TXOP_CSR0_AC1_TX_OP, 0);
	rt2x00pci_register_write(rt2x00dev, AC_TXOP_CSR0, reg);

	rt2x00pci_register_read(rt2x00dev, AC_TXOP_CSR1, &reg);
	rt2x00_set_field32(&reg, AC_TXOP_CSR1_AC2_TX_OP, 192);
	rt2x00_set_field32(&reg, AC_TXOP_CSR1_AC3_TX_OP, 48);
	rt2x00pci_register_write(rt2x00dev, AC_TXOP_CSR1, reg);

	rt2x00pci_register_read(rt2x00dev, MAC_CSR9, &reg);
	rt2x00_set_field32(&reg, MAC_CSR9_CW_SELECT, 0);
	rt2x00pci_register_write(rt2x00dev, MAC_CSR9, reg);

	rt2x00pci_register_write(rt2x00dev, PHY_CSR1, 0x000023b0);
	rt2x00pci_register_write(rt2x00dev, PHY_CSR5, 0x060a100c);
	rt2x00pci_register_write(rt2x00dev, PHY_CSR6, 0x00080606);
	rt2x00pci_register_write(rt2x00dev, PHY_CSR7, 0x00000a08);

	rt2x00pci_register_write(rt2x00dev, PCI_CFG_CSR, 0x28ca4404);

	rt2x00pci_register_write(rt2x00dev, TEST_MODE_CSR, 0x00000200);

	rt2x00pci_register_write(rt2x00dev, M2H_CMD_DONE_CSR, 0xffffffff);

	/*
	 * We must clear the error counters.
	 * These registers are cleared on read,
	 * so we may pass a useless variable to store the value.
	 */
	rt2x00pci_register_read(rt2x00dev, STA_CSR0, &reg);
	rt2x00pci_register_read(rt2x00dev, STA_CSR1, &reg);
	rt2x00pci_register_read(rt2x00dev, STA_CSR2, &reg);

	/*
	 * Reset MAC and BBP registers.
	 */
	reg = 0;
	rt2x00_set_field32(&reg, MAC_CSR1_SOFT_RESET, 1);
	rt2x00_set_field32(&reg, MAC_CSR1_BBP_RESET, 1);
	rt2x00pci_register_write(rt2x00dev, MAC_CSR1, reg);

	rt2x00pci_register_read(rt2x00dev, MAC_CSR1, &reg);
	rt2x00_set_field32(&reg, MAC_CSR1_SOFT_RESET, 0);
	rt2x00_set_field32(&reg, MAC_CSR1_BBP_RESET, 0);
	rt2x00pci_register_write(rt2x00dev, MAC_CSR1, reg);

	rt2x00pci_register_read(rt2x00dev, MAC_CSR1, &reg);
	rt2x00_set_field32(&reg, MAC_CSR1_HOST_READY, 1);
	rt2x00pci_register_write(rt2x00dev, MAC_CSR1, reg);

	return 0;
}

static int rt61pci_init_bbp(struct rt2x00_dev *rt2x00dev)
{
	unsigned int i;
	u16 eeprom;
	u8 reg_id;
	u8 value;

	for (i = 0; i < REGISTER_BUSY_COUNT; i++) {
		rt61pci_bbp_read(rt2x00dev, 0, &value);
		if ((value != 0xff) && (value != 0x00))
			goto continue_csr_init;
		NOTICE(rt2x00dev, "Waiting for BBP register.\n");
		udelay(REGISTER_BUSY_DELAY);
	}

	ERROR(rt2x00dev, "BBP register access failed, aborting.\n");
	return -EACCES;

continue_csr_init:
	rt61pci_bbp_write(rt2x00dev, 3, 0x00);
	rt61pci_bbp_write(rt2x00dev, 15, 0x30);
	rt61pci_bbp_write(rt2x00dev, 17, 0x20);
	rt61pci_bbp_write(rt2x00dev, 21, 0xc8);
	rt61pci_bbp_write(rt2x00dev, 22, 0x38);
	rt61pci_bbp_write(rt2x00dev, 23, 0x06);
	rt61pci_bbp_write(rt2x00dev, 24, 0xfe);
	rt61pci_bbp_write(rt2x00dev, 25, 0x0a);
	rt61pci_bbp_write(rt2x00dev, 26, 0x0d);
	rt61pci_bbp_write(rt2x00dev, 34, 0x12);
	rt61pci_bbp_write(rt2x00dev, 37, 0x07);
	rt61pci_bbp_write(rt2x00dev, 39, 0xf8);
	rt61pci_bbp_write(rt2x00dev, 41, 0x60);
	rt61pci_bbp_write(rt2x00dev, 53, 0x10);
	rt61pci_bbp_write(rt2x00dev, 54, 0x18);
	rt61pci_bbp_write(rt2x00dev, 60, 0x10);
	rt61pci_bbp_write(rt2x00dev, 61, 0x04);
	rt61pci_bbp_write(rt2x00dev, 62, 0x04);
	rt61pci_bbp_write(rt2x00dev, 75, 0xfe);
	rt61pci_bbp_write(rt2x00dev, 86, 0xfe);
	rt61pci_bbp_write(rt2x00dev, 88, 0xfe);
	rt61pci_bbp_write(rt2x00dev, 90, 0x0f);
	rt61pci_bbp_write(rt2x00dev, 99, 0x00);
	rt61pci_bbp_write(rt2x00dev, 102, 0x16);
	rt61pci_bbp_write(rt2x00dev, 107, 0x04);

	DEBUG(rt2x00dev, "Start initialization from EEPROM...\n");
	for (i = 0; i < EEPROM_BBP_SIZE; i++) {
		rt2x00_eeprom_read(rt2x00dev, EEPROM_BBP_START + i, &eeprom);

		if (eeprom != 0xffff && eeprom != 0x0000) {
			reg_id = rt2x00_get_field16(eeprom, EEPROM_BBP_REG_ID);
			value = rt2x00_get_field16(eeprom, EEPROM_BBP_VALUE);
			DEBUG(rt2x00dev, "BBP: 0x%02x, value: 0x%02x.\n",
				reg_id, value);
			rt61pci_bbp_write(rt2x00dev, reg_id, value);
		}
	}
	DEBUG(rt2x00dev, "...End initialization from EEPROM.\n");

	return 0;
}

/*
 * Device state switch handlers.
 */
static void rt61pci_toggle_rx(struct rt2x00_dev *rt2x00dev,
	enum dev_state state)
{
	u32 reg;

	rt2x00pci_register_read(rt2x00dev, TXRX_CSR0, &reg);
	rt2x00_set_field32(&reg, TXRX_CSR0_DISABLE_RX,
		state == STATE_RADIO_RX_OFF);
	rt2x00pci_register_write(rt2x00dev, TXRX_CSR0, reg);
}

static void rt61pci_toggle_irq(struct rt2x00_dev *rt2x00dev, int enabled)
{
	u32 reg;

	/*
	 * When interrupts are being enabled, the interrupt registers
	 * should clear the register to assure a clean state.
	 */
	if (enabled) {
		rt2x00pci_register_read(rt2x00dev, INT_SOURCE_CSR, &reg);
		rt2x00pci_register_write(rt2x00dev, INT_SOURCE_CSR, reg);

		rt2x00pci_register_read(rt2x00dev, MCU_INT_SOURCE_CSR, &reg);
		rt2x00pci_register_write(rt2x00dev, MCU_INT_SOURCE_CSR, reg);
	}

	/*
	 * Only toggle the interrupts bits we are going to use.
	 * Non-checked interrupt bits are disabled by default.
	 */
	rt2x00pci_register_read(rt2x00dev, INT_MASK_CSR, &reg);
	rt2x00_set_field32(&reg, INT_MASK_CSR_TXDONE, !enabled);
	rt2x00_set_field32(&reg, INT_MASK_CSR_RXDONE, !enabled);
	rt2x00_set_field32(&reg, INT_MASK_CSR_BEACON_DONE, !enabled);
	rt2x00_set_field32(&reg, INT_MASK_CSR_ENABLE_MITIGATION, !enabled);
	rt2x00_set_field32(&reg, INT_MASK_CSR_MITIGATION_PERIOD, 0xff);
	rt2x00pci_register_write(rt2x00dev, INT_MASK_CSR, reg);

	rt2x00pci_register_read(rt2x00dev, MCU_INT_MASK_CSR, &reg);
	rt2x00_set_field32(&reg, MCU_INT_MASK_CSR_0, !enabled);
	rt2x00_set_field32(&reg, MCU_INT_MASK_CSR_1, !enabled);
	rt2x00_set_field32(&reg, MCU_INT_MASK_CSR_2, !enabled);
	rt2x00_set_field32(&reg, MCU_INT_MASK_CSR_3, !enabled);
	rt2x00_set_field32(&reg, MCU_INT_MASK_CSR_4, !enabled);
	rt2x00_set_field32(&reg, MCU_INT_MASK_CSR_5, !enabled);
	rt2x00_set_field32(&reg, MCU_INT_MASK_CSR_6, !enabled);
	rt2x00_set_field32(&reg, MCU_INT_MASK_CSR_7, !enabled);
	rt2x00pci_register_write(rt2x00dev, MCU_INT_MASK_CSR, reg);
}

static int rt61pci_enable_radio(struct rt2x00_dev *rt2x00dev)
{
	/*
	 * Initialize all registers.
	 */
	if (rt61pci_init_rings(rt2x00dev) ||
	    rt61pci_init_registers(rt2x00dev) ||
	    rt61pci_init_bbp(rt2x00dev)) {
		ERROR(rt2x00dev, "Register initialization failed.\n");
		return -EIO;
	}

	/*
	 * Enable interrupts.
	 */
	rt61pci_toggle_irq(rt2x00dev, 1);

	/*
	 * Enable RX.
	 */
	rt2x00pci_register_write(rt2x00dev, RX_CNTL_CSR, 0x00000001);

	/*
	 * Enable LED
	 */
	rt61pci_enable_led(rt2x00dev);

	return 0;
}

static void rt61pci_disable_radio(struct rt2x00_dev *rt2x00dev)
{
	u32 reg;

	/*
	 * Disable LED
	 */
	rt61pci_disable_led(rt2x00dev);

	rt2x00pci_register_write(rt2x00dev, MAC_CSR10, 0x00001818);

	/*
	 * Disable synchronisation.
	 */
	rt2x00pci_register_write(rt2x00dev, TXRX_CSR9, 0);

	/*
	 * Cancel RX and TX.
	 */
	rt2x00pci_register_read(rt2x00dev, TX_CNTL_CSR, &reg);
	rt2x00_set_field32(&reg, TX_CNTL_CSR_ABORT_TX_AC0, 1);
	rt2x00_set_field32(&reg, TX_CNTL_CSR_ABORT_TX_AC1, 1);
	rt2x00_set_field32(&reg, TX_CNTL_CSR_ABORT_TX_AC2, 1);
	rt2x00_set_field32(&reg, TX_CNTL_CSR_ABORT_TX_AC3, 1);
	rt2x00_set_field32(&reg, TX_CNTL_CSR_ABORT_TX_MGMT, 1);
	rt2x00pci_register_write(rt2x00dev, TX_CNTL_CSR, reg);

	/*
	 * Disable interrupts.
	 */
	rt61pci_toggle_irq(rt2x00dev, 0);
}

static int rt61pci_set_state(struct rt2x00_dev *rt2x00dev,
	enum dev_state state)
{
	u32 reg;
	unsigned int i;
	char put_to_sleep;
	char current_state;

	put_to_sleep = (state != STATE_AWAKE);

	rt2x00pci_register_read(rt2x00dev, MAC_CSR12, &reg);
	rt2x00_set_field32(&reg, MAC_CSR12_FORCE_WAKEUP, !put_to_sleep);
	rt2x00_set_field32(&reg, MAC_CSR12_PUT_TO_SLEEP, put_to_sleep);
	rt2x00pci_register_write(rt2x00dev, MAC_CSR12, reg);

	if (put_to_sleep) {
		rt2x00pci_register_write(rt2x00dev, SOFT_RESET_CSR, 0x00000005);
		rt2x00pci_register_write(rt2x00dev, IO_CNTL_CSR, 0x0000001c);
		rt2x00pci_register_write(rt2x00dev, PCI_USEC_CSR, 0x00000060);
		rt61pci_mcu_request(rt2x00dev, MCU_SLEEP, 0xff, 0x00, 0x00);
	} else {
		rt2x00pci_register_write(rt2x00dev, SOFT_RESET_CSR, 0x00000007);
		rt2x00pci_register_write(rt2x00dev, IO_CNTL_CSR, 0x00000018);
		rt2x00pci_register_write(rt2x00dev, PCI_USEC_CSR, 0x00000020);
		rt61pci_mcu_request(rt2x00dev, MCU_WAKEUP, 0xff, 0x00, 0x00);
	}

	/*
	 * Device is not guaranteed to be in the requested state yet.
	 * We must wait until the register indicates that the
	 * device has entered the correct state.
	 */
	for (i = 0; i < REGISTER_BUSY_COUNT; i++) {
		rt2x00pci_register_read(rt2x00dev, MAC_CSR12, &reg);
		current_state = rt2x00_get_field32(reg,
			MAC_CSR12_BBP_CURRENT_STATE);
		if (current_state == !put_to_sleep)
			return 0;
		msleep(10);
	}

	NOTICE(rt2x00dev, "Device failed to enter state %d, "
		"current device state %d.\n", !put_to_sleep, current_state);

	return -EBUSY;
}

static int rt61pci_set_device_state(struct rt2x00_dev *rt2x00dev,
	enum dev_state state)
{
	int retval = 0;

	switch (state) {
		case STATE_RADIO_ON:
			retval = rt61pci_enable_radio(rt2x00dev);
		break;
		case STATE_RADIO_OFF:
			rt61pci_disable_radio(rt2x00dev);
		break;
		case STATE_RADIO_RX_ON:
		case STATE_RADIO_RX_OFF:
			rt61pci_toggle_rx(rt2x00dev, state);
		break;
		case STATE_DEEP_SLEEP:
		case STATE_SLEEP:
		case STATE_STANDBY:
		case STATE_AWAKE:
			retval = rt61pci_set_state(rt2x00dev, state);
		break;
		default:
			retval = -ENOTSUPP;
		break;
	}

	return retval;
}

/*
 * TX descriptor initialization
 */
static void rt61pci_write_tx_desc(struct rt2x00_dev *rt2x00dev,
	struct data_entry *entry, struct data_desc *txd,
	struct data_entry_desc *desc, struct ieee80211_hdr *ieee80211hdr,
	unsigned int length, struct ieee80211_tx_control *control)
{
	u32 word;

	/*
	 * Start writing the descriptor words.
	 */
	rt2x00_desc_read(txd, 1, &word);
	rt2x00_set_field32(&word, TXD_W1_HOST_Q_ID, desc->queue);
	rt2x00_set_field32(&word, TXD_W1_AIFSN, entry->ring->tx_params.aifs);
	rt2x00_set_field32(&word, TXD_W1_CWMIN, entry->ring->tx_params.cw_min);
	rt2x00_set_field32(&word, TXD_W1_CWMAX, entry->ring->tx_params.cw_max);
	rt2x00_set_field32(&word, TXD_W1_IV_OFFSET, IEEE80211_HEADER);
	rt2x00_set_field32(&word, TXD_W1_HW_SEQUENCE, 1);
	rt2x00_desc_write(txd, 1, word);

	rt2x00_desc_read(txd, 2, &word);
	rt2x00_set_field32(&word, TXD_W2_PLCP_SIGNAL, desc->signal);
	rt2x00_set_field32(&word, TXD_W2_PLCP_SERVICE, desc->service);
	rt2x00_set_field32(&word, TXD_W2_PLCP_LENGTH_LOW, desc->length_low);
	rt2x00_set_field32(&word, TXD_W2_PLCP_LENGTH_HIGH, desc->length_high);
	rt2x00_desc_write(txd, 2, word);

	rt2x00_desc_read(txd, 5, &word);
	rt2x00_set_field32(&word, TXD_W5_TX_POWER,
		TXPOWER_TO_DEV(control->power_level));
	rt2x00_set_field32(&word, TXD_W5_WAITING_DMA_DONE_INT, 1);
	rt2x00_desc_write(txd, 5, word);

	rt2x00_desc_read(txd, 11, &word);
	rt2x00_set_field32(&word, TXD_W11_BUFFER_LENGTH0, length);
	rt2x00_desc_write(txd, 11, word);

	rt2x00_desc_read(txd, 0, &word);
	rt2x00_set_field32(&word, TXD_W0_OWNER_NIC, 1);
	rt2x00_set_field32(&word, TXD_W0_VALID, 1);
	rt2x00_set_field32(&word, TXD_W0_MORE_FRAG,
		test_bit(ENTRY_TXD_MORE_FRAG, &entry->flags));
	rt2x00_set_field32(&word, TXD_W0_ACK,
		test_bit(ENTRY_TXD_REQ_ACK, &entry->flags));
	rt2x00_set_field32(&word, TXD_W0_TIMESTAMP,
		test_bit(ENTRY_TXD_REQ_TIMESTAMP, &entry->flags));
	rt2x00_set_field32(&word, TXD_W0_OFDM,
		test_bit(ENTRY_TXD_OFDM_RATE, &entry->flags));
	rt2x00_set_field32(&word, TXD_W0_IFS, desc->ifs);
	rt2x00_set_field32(&word, TXD_W0_RETRY_MODE, 0);
	rt2x00_set_field32(&word, TXD_W0_TKIP_MIC, 0);
	rt2x00_set_field32(&word, TXD_W0_DATABYTE_COUNT, length);
	rt2x00_set_field32(&word, TXD_W0_CIPHER_ALG, CIPHER_NONE);
	rt2x00_desc_write(txd, 0, word);
}

/*
 * TX data initialization
 */
static void rt61pci_kick_tx_queue(struct rt2x00_dev *rt2x00dev, int queue)
{
	u32 reg;

	if (queue == IEEE80211_TX_QUEUE_BEACON) {
		rt2x00pci_register_read(rt2x00dev, TXRX_CSR9, &reg);
		if (!rt2x00_get_field32(reg, TXRX_CSR9_BEACON_GEN)) {
			rt2x00_set_field32(&reg, TXRX_CSR9_BEACON_GEN, 1);
			rt2x00pci_register_write(rt2x00dev, TXRX_CSR9, reg);
		}
		return;
	}

	rt2x00pci_register_read(rt2x00dev, TX_CNTL_CSR, &reg);
	if (queue == IEEE80211_TX_QUEUE_DATA0)
		rt2x00_set_field32(&reg, TX_CNTL_CSR_KICK_TX_AC0, 1);
	else if (queue == IEEE80211_TX_QUEUE_DATA1)
		rt2x00_set_field32(&reg, TX_CNTL_CSR_KICK_TX_AC1, 1);
	else if (queue == IEEE80211_TX_QUEUE_DATA2)
		rt2x00_set_field32(&reg, TX_CNTL_CSR_KICK_TX_AC2, 1);
	else if (queue == IEEE80211_TX_QUEUE_DATA3)
		rt2x00_set_field32(&reg, TX_CNTL_CSR_KICK_TX_AC3, 1);
	else if (queue == IEEE80211_TX_QUEUE_DATA4)
		rt2x00_set_field32(&reg, TX_CNTL_CSR_KICK_TX_MGMT, 1);
	rt2x00pci_register_write(rt2x00dev, TX_CNTL_CSR, reg);
}

/*
 * RX control handlers
 */
static int rt61pci_agc_to_rssi(struct rt2x00_dev *rt2x00dev, int rxd_w1)
{
	u16 eeprom;
	char offset;
	char lna;

	lna = rt2x00_get_field32(rxd_w1, RXD_W1_RSSI_LNA);
	switch (lna) {
		case 3:
			offset = 90;
		break;
		case 2:
			offset = 74;
		break;
		case 1:
			offset = 64;
		break;
		default:
			return 0;
	}

	if (rt2x00dev->rx_status.phymode == MODE_IEEE80211A) {
		if (test_bit(CONFIG_EXTERNAL_LNA_A, &rt2x00dev->flags))
			offset += 14;

		if (lna == 3 || lna == 2)
			offset += 10;

		rt2x00_eeprom_read(rt2x00dev, EEPROM_RSSI_OFFSET_A, &eeprom);
		offset -= rt2x00_get_field16(eeprom, EEPROM_RSSI_OFFSET_A_1);
	} else {
		if (test_bit(CONFIG_EXTERNAL_LNA_BG, &rt2x00dev->flags))
			offset += 14;

		rt2x00_eeprom_read(rt2x00dev, EEPROM_RSSI_OFFSET_BG, &eeprom);
		offset -= rt2x00_get_field16(eeprom, EEPROM_RSSI_OFFSET_BG_1);
	}

	return rt2x00_get_field32(rxd_w1, RXD_W1_RSSI_AGC) * 2 - offset;
}

static int rt61pci_fill_rxdone(struct data_entry *entry,
	int *signal, int *rssi, int *ofdm)
{
	struct data_desc *rxd = entry->priv;
	u32 word0;
	u32 word1;

	rt2x00_desc_read(rxd, 0, &word0);
	rt2x00_desc_read(rxd, 1, &word1);

	/*
	 * TODO: Don't we need to keep statistics
	 * updated about these errors?
	 */
	if (rt2x00_get_field32(word0, RXD_W0_CRC) ||
	    rt2x00_get_field32(word0, RXD_W0_CIPHER_ERROR))
		return -EINVAL;

	/*
	 * Obtain the status about this packet.
	 */
	*signal = rt2x00_get_field32(word1, RXD_W1_SIGNAL);
	*rssi = rt61pci_agc_to_rssi(entry->ring->rt2x00dev, word1);
	*ofdm = rt2x00_get_field32(word0, RXD_W0_OFDM);

	return rt2x00_get_field32(word0, RXD_W0_DATABYTE_COUNT);
}

/*
 * Interrupt functions.
 */
static void rt61pci_txdone(struct rt2x00_dev *rt2x00dev)
{
	struct data_ring *ring;
	struct data_entry *entry;
	struct data_desc *txd;
	u32 word;
	u32 reg;
	int index;
	int tx_status;
	int retry;

	while (1) {
		rt2x00pci_register_read(rt2x00dev, STA_CSR4, &reg);
		if (!rt2x00_get_field32(reg, STA_CSR4_VALID))
			break;

		/*
		 * Skip this entry when it contains an invalid
		 * ring identication number.
		 */
		ring = rt2x00_get_ring(rt2x00dev,
			rt2x00_get_field32(reg, STA_CSR4_PID_TYPE));
		if (unlikely(!ring))
			continue;

		/*
		 * Skip this entry when it contains an invalid
		 * index number.
		 */
		index = rt2x00_get_field32(reg, STA_CSR4_PID_SUBTYPE);
		if (unlikely(index >= ring->stats.limit))
			continue;

		entry = &ring->entry[index];
		txd = entry->priv;
		rt2x00_desc_read(txd, 0, &word);

		if (rt2x00_get_field32(word, TXD_W0_OWNER_NIC) ||
		    !rt2x00_get_field32(word, TXD_W0_VALID))
			return;

		/*
		 * Obtain the status about this packet.
		 */
		tx_status = rt2x00_get_field32(reg, STA_CSR4_TX_RESULT);
		retry = rt2x00_get_field32(reg, STA_CSR4_RETRY_COUNT);

		rt2x00lib_txdone(entry, tx_status, retry);

		/*
		 * Make this entry available for reuse.
		 */
		entry->flags = 0;
		rt2x00_set_field32(&word, TXD_W0_VALID, 0);
		rt2x00_desc_write(txd, 0, word);
		rt2x00_ring_index_done_inc(entry->ring);

		/*
		 * If the data ring was full before the txdone handler
		 * we must make sure the packet queue in the mac80211 stack
		 * is reenabled when the txdone handler has finished.
		 */
		if (!rt2x00_ring_full(ring))
			ieee80211_wake_queue(rt2x00dev->hw,
				entry->tx_status.control.queue);
	}
}

static irqreturn_t rt61pci_interrupt(int irq, void *dev_instance)
{
	struct rt2x00_dev *rt2x00dev = dev_instance;
	u32 reg;

	/*
	 * Get the interrupt sources & saved to local variable.
	 * Write register value back to clear pending interrupts.
	 */
	rt2x00pci_register_read(rt2x00dev, MCU_INT_SOURCE_CSR, &reg);
	rt2x00pci_register_write(rt2x00dev, MCU_INT_SOURCE_CSR, reg);

	rt2x00pci_register_read(rt2x00dev, INT_SOURCE_CSR, &reg);
	rt2x00pci_register_write(rt2x00dev, INT_SOURCE_CSR, reg);

	if (!reg)
		return IRQ_NONE;

	if (!test_bit(DEVICE_ENABLED_RADIO, &rt2x00dev->flags))
		return IRQ_HANDLED;

	/*
	 * Handle interrupts, walk through all bits
	 * and run the tasks, the bits are checked in order of
	 * priority.
	 */

	/*
	 * 1 - Beacon timer expired interrupt.
	 */
	if (rt2x00_get_field32(reg, INT_SOURCE_CSR_BEACON_DONE))
		rt2x00pci_beacondone(rt2x00dev, IEEE80211_TX_QUEUE_BEACON);

	/*
	 * 2 - Rx ring done interrupt.
	 */
	if (rt2x00_get_field32(reg, INT_SOURCE_CSR_RXDONE))
		rt2x00pci_rxdone(rt2x00dev);

	/*
	 * 3 - Tx ring done interrupt.
	 */
	if (rt2x00_get_field32(reg, INT_SOURCE_CSR_TXDONE))
		rt61pci_txdone(rt2x00dev);

	return IRQ_HANDLED;
}

/*
 * Device initialization functions.
 */
static int rt61pci_alloc_eeprom(struct rt2x00_dev *rt2x00dev)
{
	struct eeprom_93cx6 eeprom;
	u32 reg;
	u16 word;
	u8 *mac;
	char value;

	/*
	 * Allocate the eeprom memory, check the eeprom width
	 * and copy the entire eeprom into this allocated memory.
	 */
	rt2x00dev->eeprom = kzalloc(EEPROM_SIZE, GFP_KERNEL);
	if (!rt2x00dev->eeprom)
		return -ENOMEM;

	rt2x00pci_register_read(rt2x00dev, E2PROM_CSR, &reg);

	eeprom.data = rt2x00dev;
	eeprom.register_read = rt61pci_eepromregister_read;
	eeprom.register_write = rt61pci_eepromregister_write;
	eeprom.width = rt2x00_get_field32(reg, E2PROM_CSR_TYPE_93C46) ?
		PCI_EEPROM_WIDTH_93C46 : PCI_EEPROM_WIDTH_93C66;
	eeprom.reg_data_in = 0;
	eeprom.reg_data_out = 0;
	eeprom.reg_data_clock = 0;
	eeprom.reg_chip_select = 0;

	eeprom_93cx6_multiread(&eeprom, EEPROM_BASE, rt2x00dev->eeprom,
		EEPROM_SIZE / sizeof(u16));

	/*
	 * Start validation of the data that has been read.
	 */
	mac = rt2x00_eeprom_addr(rt2x00dev, EEPROM_MAC_ADDR_0);
	if (!is_valid_ether_addr(mac)) {
		random_ether_addr(mac);
		EEPROM(rt2x00dev, "MAC: " MAC_FMT "\n", MAC_ARG(mac));
	}

	rt2x00_eeprom_read(rt2x00dev, EEPROM_ANTENNA, &word);
	if (word == 0xffff) {
		rt2x00_set_field16(&word, EEPROM_ANTENNA_NUM, 2);
		rt2x00_set_field16(&word, EEPROM_ANTENNA_TX_DEFAULT, 2);
		rt2x00_set_field16(&word, EEPROM_ANTENNA_RX_DEFAULT, 2);
		rt2x00_set_field16(&word, EEPROM_ANTENNA_FRAME_TYPE, 0);
		rt2x00_set_field16(&word, EEPROM_ANTENNA_DYN_TXAGC, 0);
		rt2x00_set_field16(&word, EEPROM_ANTENNA_HARDWARE_RADIO, 0);
		rt2x00_set_field16(&word, EEPROM_ANTENNA_RF_TYPE, RF5225);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_ANTENNA, word);
		EEPROM(rt2x00dev, "Antenna: 0x%04x\n", word);
	}

	rt2x00_eeprom_read(rt2x00dev, EEPROM_NIC, &word);
	if (word == 0xffff) {
		rt2x00_set_field16(&word, EEPROM_NIC_ENABLE_DIVERSITY, 0);
		rt2x00_set_field16(&word, EEPROM_NIC_TX_DIVERSITY, 0);
		rt2x00_set_field16(&word, EEPROM_NIC_TX_RX_FIXED, 0);
		rt2x00_set_field16(&word, EEPROM_NIC_EXTERNAL_LNA_BG, 0);
		rt2x00_set_field16(&word, EEPROM_NIC_CARDBUS_ACCEL, 0);
		rt2x00_set_field16(&word, EEPROM_NIC_EXTERNAL_LNA_A, 0);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_NIC, word);
		EEPROM(rt2x00dev, "NIC: 0x%04x\n", word);
	}

	rt2x00_eeprom_read(rt2x00dev, EEPROM_LED, &word);
	if (word == 0xffff) {
		rt2x00_set_field16(&word, EEPROM_LED_LED_MODE,
			LED_MODE_DEFAULT);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_LED, word);
		EEPROM(rt2x00dev, "Led: 0x%04x\n", word);
	}

	rt2x00_eeprom_read(rt2x00dev, EEPROM_FREQ, &word);
	if (word == 0xffff) {
		rt2x00_set_field16(&word, EEPROM_FREQ_OFFSET, 0);
		rt2x00_set_field16(&word, EEPROM_FREQ_SEQ, 0);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_FREQ, word);
		EEPROM(rt2x00dev, "Freq: 0x%04x\n", word);
	}

	rt2x00_eeprom_read(rt2x00dev, EEPROM_RSSI_OFFSET_BG, &word);
	if (word == 0xffff) {
		rt2x00_set_field16(&word, EEPROM_RSSI_OFFSET_BG_1, 0);
		rt2x00_set_field16(&word, EEPROM_RSSI_OFFSET_BG_2, 0);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_RSSI_OFFSET_BG, word);
		EEPROM(rt2x00dev, "RSSI OFFSET BG: 0x%04x\n", word);
	} else {
		value = rt2x00_get_field16(word, EEPROM_RSSI_OFFSET_BG_1);
		if (value < -10 || value > 10)
			rt2x00_set_field16(&word, EEPROM_RSSI_OFFSET_BG_1, 0);
		value = rt2x00_get_field16(word, EEPROM_RSSI_OFFSET_BG_2);
		if (value < -10 || value > 10)
			rt2x00_set_field16(&word, EEPROM_RSSI_OFFSET_BG_2, 0);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_RSSI_OFFSET_BG, word);
	}

	rt2x00_eeprom_read(rt2x00dev, EEPROM_RSSI_OFFSET_A, &word);
	if (word == 0xffff) {
		rt2x00_set_field16(&word, EEPROM_RSSI_OFFSET_A_1, 0);
		rt2x00_set_field16(&word, EEPROM_RSSI_OFFSET_A_2, 0);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_RSSI_OFFSET_A, word);
		EEPROM(rt2x00dev, "RSSI OFFSET BG: 0x%04x\n", word);
	} else {
		value = rt2x00_get_field16(word, EEPROM_RSSI_OFFSET_A_1);
		if (value < -10 || value > 10)
			rt2x00_set_field16(&word, EEPROM_RSSI_OFFSET_A_1, 0);
		value = rt2x00_get_field16(word, EEPROM_RSSI_OFFSET_A_2);
		if (value < -10 || value > 10)
			rt2x00_set_field16(&word, EEPROM_RSSI_OFFSET_A_2, 0);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_RSSI_OFFSET_A, word);
	}

	return 0;
}

static int rt61pci_init_eeprom(struct rt2x00_dev *rt2x00dev)
{
	u32 reg;
	u16 value;
	u16 eeprom;
	u16 device;

	/*
	 * Read EEPROM word for configuration.
	 */
	rt2x00_eeprom_read(rt2x00dev, EEPROM_ANTENNA, &eeprom);

	/*
	 * Identify RF chipset.
	 * To determine the RT chip we have to read the
	 * PCI header of the device.
	 */
	pci_read_config_word(rt2x00dev_pci(rt2x00dev),
		PCI_CONFIG_HEADER_DEVICE, &device);
	value = rt2x00_get_field16(eeprom, EEPROM_ANTENNA_RF_TYPE);
	rt2x00pci_register_read(rt2x00dev, MAC_CSR0, &reg);
	rt2x00_set_chip(rt2x00dev, device, value, reg);

	if (!rt2x00_rf(&rt2x00dev->chip, RF5225) &&
	    !rt2x00_rf(&rt2x00dev->chip, RF5325) &&
	    !rt2x00_rf(&rt2x00dev->chip, RF2527) &&
	    !rt2x00_rf(&rt2x00dev->chip, RF2529)) {
		ERROR(rt2x00dev, "Invalid RF chipset detected.\n");
		return -ENODEV;
	}

	/*
	 * Identify default antenna configuration.
	 */
	rt2x00dev->hw->conf.antenna_sel_tx = rt2x00_get_field16(eeprom,
		EEPROM_ANTENNA_TX_DEFAULT);
	rt2x00dev->hw->conf.antenna_sel_rx = rt2x00_get_field16(eeprom,
		EEPROM_ANTENNA_RX_DEFAULT);

	/*
	 * Read the Frame type.
	 */
	if (rt2x00_get_field16(eeprom, EEPROM_ANTENNA_FRAME_TYPE))
		__set_bit(CONFIG_FRAME_TYPE, &rt2x00dev->flags);

	/*
	 * Determine number of antenna's.
	 */
	if (rt2x00_get_field16(eeprom, EEPROM_ANTENNA_NUM) == 2)
		__set_bit(CONFIG_DOUBLE_ANTENNA, &rt2x00dev->flags);

	/*
	 * Detect if this device has an hardware controlled radio.
	 */
	if (rt2x00_get_field16(eeprom, EEPROM_ANTENNA_HARDWARE_RADIO))
		__set_bit(DEVICE_SUPPORT_HW_BUTTON, &rt2x00dev->flags);

	/*
	 * Read frequency offset and RF programming sequence.
	 */
	rt2x00_eeprom_read(rt2x00dev, EEPROM_FREQ, &eeprom);
	if (rt2x00_get_field16(eeprom, EEPROM_FREQ_SEQ))
		__set_bit(CONFIG_RF_SEQUENCE, &rt2x00dev->flags);

	rt2x00dev->freq_offset =
		rt2x00_get_field16(eeprom, EEPROM_FREQ_OFFSET);

	/*
	 * Read external LNA informations.
	 */
	rt2x00_eeprom_read(rt2x00dev, EEPROM_NIC, &eeprom);

	if (rt2x00_get_field16(eeprom, EEPROM_NIC_EXTERNAL_LNA_A))
		__set_bit(CONFIG_EXTERNAL_LNA_A, &rt2x00dev->flags);
	if (rt2x00_get_field16(eeprom, EEPROM_NIC_EXTERNAL_LNA_BG))
		__set_bit(CONFIG_EXTERNAL_LNA_BG, &rt2x00dev->flags);

	/*
	 * Store led settings, for correct led behaviour.
	 * If the eeprom value is invalid,
	 * switch to default led mode.
	 */
	rt2x00_eeprom_read(rt2x00dev, EEPROM_LED, &eeprom);

	rt2x00dev->led_mode = rt2x00_get_field16(eeprom, EEPROM_LED_LED_MODE);

	rt2x00_set_field16(&rt2x00dev->led_reg, MCU_LEDCS_LED_MODE,
		rt2x00dev->led_mode);
	rt2x00_set_field16(&rt2x00dev->led_reg, MCU_LEDCS_POLARITY_GPIO_0,
		rt2x00_get_field16(eeprom, EEPROM_LED_POLARITY_GPIO_0));
	rt2x00_set_field16(&rt2x00dev->led_reg, MCU_LEDCS_POLARITY_GPIO_1,
		rt2x00_get_field16(eeprom, EEPROM_LED_POLARITY_GPIO_1));
	rt2x00_set_field16(&rt2x00dev->led_reg, MCU_LEDCS_POLARITY_GPIO_2,
		rt2x00_get_field16(eeprom, EEPROM_LED_POLARITY_GPIO_2));
	rt2x00_set_field16(&rt2x00dev->led_reg, MCU_LEDCS_POLARITY_GPIO_3,
		rt2x00_get_field16(eeprom, EEPROM_LED_POLARITY_GPIO_3));
	rt2x00_set_field16(&rt2x00dev->led_reg, MCU_LEDCS_POLARITY_GPIO_4,
		rt2x00_get_field16(eeprom, EEPROM_LED_POLARITY_GPIO_4));
	rt2x00_set_field16(&rt2x00dev->led_reg, MCU_LEDCS_POLARITY_ACT,
		rt2x00_get_field16(eeprom, EEPROM_LED_POLARITY_ACT));
	rt2x00_set_field16(&rt2x00dev->led_reg, MCU_LEDCS_POLARITY_READY_BG,
		rt2x00_get_field16(eeprom, EEPROM_LED_POLARITY_RDY_G));
	rt2x00_set_field16(&rt2x00dev->led_reg, MCU_LEDCS_POLARITY_READY_A,
		rt2x00_get_field16(eeprom, EEPROM_LED_POLARITY_RDY_A));

	return 0;
}

/*
 * RF value list for RF5225, RF5325, RF2527 & RF2529
 * Supports: 2.4 GHz
 */
static const u32 rf_vals_bg[] = {
	0x00004786, 0x00004786, 0x0000478a, 0x0000478a, 0x0000478e,
	0x0000478e, 0x00004792, 0x00004792, 0x00004796, 0x00004796,
	0x0000479a, 0x0000479a, 0x0000479e, 0x000047a2
};

/*
 * RF value list for RF5225 & RF5325 (supplement to vals_bg)
 * Supports: 5.2 GHz, rf_sequence disabled
 */
static const u32 rf_vals_a_5x_noseq[] = {
	0x0000499a, 0x000049a2, 0x000049a6, 0x000049aa, 0x000049ae,
	0x000049b2, 0x000049ba, 0x000049be, 0x00004a2a, 0x00004a2e,
	0x00004a32, 0x00004a36, 0x00004a3a, 0x00004a82, 0x00004a86,
	0x00004a8a, 0x00004a8e, 0x00004a92, 0x00004a9a, 0x00004aa2,
	0x00004aa6, 0x00004aae, 0x00004ab2, 0x00004ab6
};

/*
 * RF value list for RF5225 & RF5325 (supplement to vals_bg)
 * Supports: 5.2 GHz, rf_sequence enabled
 */
static const u32 rf_vals_a_5x_seq[] = {
	0x0004481a, 0x00044682, 0x00044686, 0x0004468e, 0x00044692,
	0x0004469a, 0x000446a2, 0x000446a6, 0x0004489a, 0x000448a2,
	0x000448aa, 0x000448b2, 0x000448ba, 0x00044702, 0x00044706,
	0x0004470e, 0x00044712, 0x0004471a, 0x00044722, 0x0004472e,
	0x00044736, 0x0004490a, 0x00044912, 0x0004491a
};

static void rt61pci_init_hw_mode(struct rt2x00_dev *rt2x00dev)
{
	struct hw_mode_spec *spec = &rt2x00dev->spec;
	u8 *txpower;
	unsigned int i;

	/*
	 * Initialize all hw fields.
	 */
	rt2x00dev->hw->flags = IEEE80211_HW_HOST_GEN_BEACON |
		IEEE80211_HW_HOST_BROADCAST_PS_BUFFERING |
		IEEE80211_HW_WEP_INCLUDE_IV |
		IEEE80211_HW_DATA_NULLFUNC_ACK |
		IEEE80211_HW_NO_TKIP_WMM_HWACCEL |
		IEEE80211_HW_MONITOR_DURING_OPER |
		IEEE80211_HW_NO_PROBE_FILTERING;
	rt2x00dev->hw->extra_tx_headroom = 0;
	rt2x00dev->hw->max_rssi = MAX_RX_SSI;
	rt2x00dev->hw->max_noise = MAX_RX_NOISE;
	rt2x00dev->hw->queues = 5;

	SET_IEEE80211_DEV(rt2x00dev->hw, &rt2x00dev_pci(rt2x00dev)->dev);
	SET_IEEE80211_PERM_ADDR(rt2x00dev->hw,
		rt2x00_eeprom_addr(rt2x00dev, EEPROM_MAC_ADDR_0));

	/*
	 * Convert tx_power array in eeprom.
	 */
	txpower = rt2x00_eeprom_addr(rt2x00dev, EEPROM_TXPOWER_G_START);
	for (i = 0; i < 14; i++)
		txpower[i] = TXPOWER_FROM_DEV(txpower[i]);

	/*
	 * Initialize hw_mode information.
	 */
	spec->num_modes = 2;
	spec->num_rates = 12;
	spec->num_channels = 14;
	spec->tx_power_a = NULL;
	spec->tx_power_bg = txpower;
	spec->tx_power_default = DEFAULT_TXPOWER;
	spec->chan_val_a = NULL;
	spec->chan_val_bg = rf_vals_bg;

	if (rt2x00_rf(&rt2x00dev->chip, RF5225) ||
	    rt2x00_rf(&rt2x00dev->chip, RF5325)) {
		spec->num_modes = 3;
		spec->num_channels += 24;

		txpower = rt2x00_eeprom_addr(rt2x00dev, EEPROM_TXPOWER_A_START);
		for (i = 0; i < 14; i++)
			txpower[i] = TXPOWER_FROM_DEV(txpower[i]);

		spec->tx_power_a = txpower;
		if (!test_bit(CONFIG_RF_SEQUENCE, &rt2x00dev->flags))
			spec->chan_val_a = rf_vals_a_5x_noseq;
		else
			spec->chan_val_a = rf_vals_a_5x_seq;
	}
}

static int rt61pci_init_hw(struct rt2x00_dev *rt2x00dev)
{
	int retval;

	/*
	 * Allocate eeprom data.
	 */
	retval = rt61pci_alloc_eeprom(rt2x00dev);
	if (retval)
		return retval;

	retval = rt61pci_init_eeprom(rt2x00dev);
	if (retval)
		return retval;

	/*
	 * Initialize hw specifications.
	 */
	rt61pci_init_hw_mode(rt2x00dev);

	/*
	 * This device requires firmware
	 */
	__set_bit(FIRMWARE_REQUIRED, &rt2x00dev->flags);

	/*
	 * Set the rssi offset.
	 */
	rt2x00dev->rssi_offset = DEFAULT_RSSI_OFFSET;

	return 0;
}

/*
 * IEEE80211 stack callback functions.
 */
static int rt61pci_get_stats(struct ieee80211_hw *hw,
	struct ieee80211_low_level_stats *stats)
{
	struct rt2x00_dev *rt2x00dev = hw->priv;
	u32 reg;

	/*
	 * Update FCS error count from register.
	 * The dot11ACKFailureCount, dot11RTSFailureCount and
	 * dot11RTSSuccessCount are updated in interrupt time.
	 */
	rt2x00pci_register_read(rt2x00dev, STA_CSR0, &reg);
	rt2x00dev->low_level_stats.dot11FCSErrorCount +=
		rt2x00_get_field32(reg, STA_CSR0_FCS_ERROR);

	memcpy(stats, &rt2x00dev->low_level_stats, sizeof(*stats));

	return 0;
}

static int rt61pci_set_retry_limit(struct ieee80211_hw *hw,
	u32 short_retry, u32 long_retry)
{
	struct rt2x00_dev *rt2x00dev = hw->priv;
	u32 reg;

	rt2x00pci_register_read(rt2x00dev, TXRX_CSR4, &reg);
	rt2x00_set_field32(&reg, TXRX_CSR4_LONG_RETRY_LIMIT, long_retry);
	rt2x00_set_field32(&reg, TXRX_CSR4_SHORT_RETRY_LIMIT, short_retry);
	rt2x00pci_register_write(rt2x00dev, TXRX_CSR4, reg);

	return 0;
}

static u64 rt61pci_get_tsf(struct ieee80211_hw *hw)
{
	struct rt2x00_dev *rt2x00dev = hw->priv;
	u64 tsf;
	u32 reg;

	rt2x00pci_register_read(rt2x00dev, TXRX_CSR13, &reg);
	tsf = (u64)rt2x00_get_field32(reg, TXRX_CSR13_HIGH_TSFTIMER) << 32;
	rt2x00pci_register_read(rt2x00dev, TXRX_CSR12, &reg);
	tsf |= rt2x00_get_field32(reg, TXRX_CSR12_LOW_TSFTIMER);

	return tsf;
}

static void rt61pci_reset_tsf(struct ieee80211_hw *hw)
{
	struct rt2x00_dev *rt2x00dev = hw->priv;

	rt2x00pci_register_write(rt2x00dev, TXRX_CSR12, 0);
	rt2x00pci_register_write(rt2x00dev, TXRX_CSR13, 0);
}

static const struct ieee80211_ops rt61pci_mac80211_ops = {
	.tx			= rt2x00lib_tx,
	.reset			= rt2x00lib_reset,
	.add_interface		= rt2x00lib_add_interface,
	.remove_interface	= rt2x00lib_remove_interface,
	.config			= rt2x00lib_config,
	.config_interface	= rt2x00lib_config_interface,
	.set_multicast_list	= rt2x00lib_set_multicast_list,
	.get_stats		= rt61pci_get_stats,
	.set_retry_limit	= rt61pci_set_retry_limit,
	.conf_tx		= rt2x00lib_conf_tx,
	.get_tx_stats		= rt2x00lib_get_tx_stats,
	.get_tsf		= rt61pci_get_tsf,
	.reset_tsf		= rt61pci_reset_tsf,
	.beacon_update		= rt2x00pci_beacon_update,
};

static const struct rt2x00lib_ops rt61pci_rt2x00_ops = {
	.irq_handler		= rt61pci_interrupt,
	.init_hw		= rt61pci_init_hw,
	.get_fw_name		= rt61pci_get_fw_name,
	.load_firmware		= rt61pci_load_firmware,
	.initialize		= rt2x00pci_initialize,
	.uninitialize		= rt2x00pci_uninitialize,
	.set_device_state	= rt61pci_set_device_state,
#ifdef CONFIG_RT61PCI_RFKILL
	.rfkill_poll		= rt61pci_rfkill_poll,
#endif /* CONFIG_RT61PCI_RFKILL */
	.link_tuner		= rt61pci_link_tuner,
	.write_tx_desc		= rt61pci_write_tx_desc,
	.write_tx_data		= rt2x00pci_write_tx_data,
	.kick_tx_queue		= rt61pci_kick_tx_queue,
	.fill_rxdone		= rt61pci_fill_rxdone,
	.config_type		= rt61pci_config_type,
	.config_phymode		= rt61pci_config_phymode,
	.config_channel		= rt61pci_config_channel,
	.config_mac_addr	= rt61pci_config_mac_addr,
	.config_bssid		= rt61pci_config_bssid,
	.config_promisc		= rt61pci_config_promisc,
	.config_txpower		= rt61pci_config_txpower,
	.config_antenna		= rt61pci_config_antenna,
	.config_duration	= rt61pci_config_duration,
};

static const struct rt2x00_ops rt61pci_ops = {
	.name	= DRV_NAME,
	.rxd_size = RXD_DESC_SIZE,
	.txd_size = TXD_DESC_SIZE,
	.lib	= &rt61pci_rt2x00_ops,
	.hw	= &rt61pci_mac80211_ops,
#ifdef CONFIG_RT2X00_LIB_DEBUGFS
	.debugfs = &rt61pci_rt2x00debug,
#endif /* CONFIG_RT2X00_LIB_DEBUGFS */
};

/*
 * RT61pci module information.
 */
static struct pci_device_id rt61pci_device_table[] = {
	/* RT2561s */
	{ PCI_DEVICE(0x1814, 0x0301), PCI_DEVICE_DATA(&rt61pci_ops) },
	/* RT2561 v2 */
	{ PCI_DEVICE(0x1814, 0x0302), PCI_DEVICE_DATA(&rt61pci_ops) },
	/* RT2661 */
	{ PCI_DEVICE(0x1814, 0x0401), PCI_DEVICE_DATA(&rt61pci_ops) },
	{ 0, }
};

MODULE_AUTHOR(DRV_PROJECT);
MODULE_VERSION(DRV_VERSION);
MODULE_DESCRIPTION("Ralink RT61 PCI & PCMCIA Wireless LAN driver.");
MODULE_SUPPORTED_DEVICE("Ralink RT2561, RT2561s & RT2661 "
	"PCI & PCMCIA chipset based cards");
MODULE_DEVICE_TABLE(pci, rt61pci_device_table);
MODULE_FIRMWARE(FIRMWARE_RT2561);
MODULE_FIRMWARE(FIRMWARE_RT2561s);
MODULE_FIRMWARE(FIRMWARE_RT2661);
MODULE_LICENSE("GPL");

static struct pci_driver rt61pci_driver = {
	.name		= DRV_NAME,
	.id_table	= rt61pci_device_table,
	.probe		= rt2x00pci_probe,
	.remove		= __devexit_p(rt2x00pci_remove),
#ifdef CONFIG_PM
	.suspend	= rt2x00pci_suspend,
	.resume		= rt2x00pci_resume,
#endif /* CONFIG_PM */
};

static int __init rt61pci_init(void)
{
	return pci_register_driver(&rt61pci_driver);
}

static void __exit rt61pci_exit(void)
{
	pci_unregister_driver(&rt61pci_driver);
}

module_init(rt61pci_init);
module_exit(rt61pci_exit);
