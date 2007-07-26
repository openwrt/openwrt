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
	Module: rt73usb
	Abstract: rt73usb device specific routines.
	Supported chipsets: rt2571W & rt2671.
 */

/*
 * Set enviroment defines for rt2x00.h
 */
#define DRV_NAME "rt73usb"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/delay.h>
#include <linux/etherdevice.h>

#include "rt2x00.h"
#include "rt2x00usb.h"
#include "rt73usb.h"

/*
 * Register access.
 * All access to the CSR registers will go through the methods
 * rt73usb_register_read and rt73usb_register_write.
 * BBP and RF register require indirect register access,
 * and use the CSR registers BBPCSR and RFCSR to achieve this.
 * These indirect registers work with busy bits,
 * and we will try maximal REGISTER_BUSY_COUNT times to access
 * the register while taking a REGISTER_BUSY_DELAY us delay
 * between each attampt. When the busy bit is still set at that time,
 * the access attempt is considered to have failed,
 * and we will print an error.
 */
static inline void rt73usb_register_read(
	const struct rt2x00_dev *rt2x00dev,
	const u16 offset, u32 *value)
{
	__le32 reg;
	rt2x00usb_vendor_request(
		rt2x00dev, USB_MULTI_READ, USB_VENDOR_REQUEST_IN,
		offset, 0x00, &reg, sizeof(u32), REGISTER_TIMEOUT);
	*value = le32_to_cpu(reg);
}

static inline void rt73usb_register_multiread(
	const struct rt2x00_dev *rt2x00dev,
	const u16 offset, void *value, const u32 length)
{
	rt2x00usb_vendor_request(
		rt2x00dev, USB_MULTI_READ, USB_VENDOR_REQUEST_IN,
		offset, 0x00, value, length,
		REGISTER_TIMEOUT * (length / sizeof(u32)));
}

static inline void rt73usb_register_write(
	const struct rt2x00_dev *rt2x00dev,
	const u16 offset, u32 value)
{
	__le32 reg = cpu_to_le32(value);
	rt2x00usb_vendor_request(
		rt2x00dev, USB_MULTI_WRITE, USB_VENDOR_REQUEST_OUT,
		offset, 0x00, &reg, sizeof(u32), REGISTER_TIMEOUT);
}

static inline void rt73usb_register_multiwrite(
	const struct rt2x00_dev *rt2x00dev,
	const u16 offset, void *value, const u32 length)
{
	rt2x00usb_vendor_request(
		rt2x00dev, USB_MULTI_WRITE, USB_VENDOR_REQUEST_OUT,
		offset, 0x00, value, length,
		REGISTER_TIMEOUT * (length / sizeof(u32)));
}

static u32 rt73usb_bbp_check(const struct rt2x00_dev *rt2x00dev)
{
	u32 reg;
	unsigned int i;

	for (i = 0; i < REGISTER_BUSY_COUNT; i++) {
		rt73usb_register_read(rt2x00dev, PHY_CSR3, &reg);
		if (!rt2x00_get_field32(reg, PHY_CSR3_BUSY))
			break;
		udelay(REGISTER_BUSY_DELAY);
	}

	return reg;
}

static void rt73usb_bbp_write(const struct rt2x00_dev *rt2x00dev,
	const u8 reg_id, const u8 value)
{
	u32 reg;

	/*
	 *  Wait until the BBP becomes ready.
	 */
	reg = rt73usb_bbp_check(rt2x00dev);
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

	rt73usb_register_write(rt2x00dev, PHY_CSR3, reg);
}

static void rt73usb_bbp_read(const struct rt2x00_dev *rt2x00dev,
	const u8 reg_id, u8 *value)
{
	u32 reg;

	/*
	 *  Wait until the BBP becomes ready.
	 */
	reg = rt73usb_bbp_check(rt2x00dev);
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

	rt73usb_register_write(rt2x00dev, PHY_CSR3, reg);

	/*
	 *  Wait until the BBP becomes ready.
	 */
	reg = rt73usb_bbp_check(rt2x00dev);
	if (rt2x00_get_field32(reg, PHY_CSR3_BUSY)) {
		ERROR(rt2x00dev, "PHY_CSR3 register busy. Read failed.\n");
		*value = 0xff;
		return;
	}

	*value = rt2x00_get_field32(reg, PHY_CSR3_VALUE);
}

static void rt73usb_rf_write(const struct rt2x00_dev *rt2x00dev,
	const u32 value)
{
	u32 reg;
	unsigned int i;

	for (i = 0; i < REGISTER_BUSY_COUNT; i++) {
		rt73usb_register_read(rt2x00dev, PHY_CSR4, &reg);
		if (!rt2x00_get_field32(reg, PHY_CSR4_BUSY))
			goto rf_write;
		udelay(REGISTER_BUSY_DELAY);
	}

	ERROR(rt2x00dev, "PHY_CSR4 register busy. Write failed.\n");
	return;

rf_write:
	reg = 0;
	rt2x00_set_field32(&reg, PHY_CSR4_VALUE, value);
	rt2x00_set_field32(&reg, PHY_CSR4_NUMBER_OF_BITS, 20);
	rt2x00_set_field32(&reg, PHY_CSR4_IF_SELECT, 0);
	rt2x00_set_field32(&reg, PHY_CSR4_BUSY, 1);

	rt73usb_register_write(rt2x00dev, PHY_CSR4, reg);
}

#ifdef CONFIG_RT2X00_LIB_DEBUGFS
#define CSR_OFFSET(__word)	( CSR_REG_BASE + ((__word) * sizeof(u32)) )

static void rt73usb_read_csr(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt73usb_register_read(rt2x00dev, CSR_OFFSET(word), data);
}

static void rt73usb_write_csr(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt73usb_register_write(rt2x00dev, CSR_OFFSET(word), *((u32*)data));
}

static void rt73usb_read_eeprom(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt2x00_eeprom_read(rt2x00dev, word, data);
}

static void rt73usb_write_eeprom(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt2x00_eeprom_write(rt2x00dev, word, *((u16*)data));
}

static void rt73usb_read_bbp(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt73usb_bbp_read(rt2x00dev, word, data);
}

static void rt73usb_write_bbp(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt73usb_bbp_write(rt2x00dev, word, *((u8*)data));
}

static const struct rt2x00debug rt73usb_rt2x00debug = {
	.owner 		= THIS_MODULE,
	.reg_csr	= {
		.read		= rt73usb_read_csr,
		.write		= rt73usb_write_csr,
		.word_size	= sizeof(u32),
		.word_count	= CSR_REG_SIZE / sizeof(u32),
	},
	.reg_eeprom	= {
		.read		= rt73usb_read_eeprom,
		.write		= rt73usb_write_eeprom,
		.word_size	= sizeof(u16),
		.word_count	= EEPROM_SIZE / sizeof(u16),
	},
	.reg_bbp	= {
		.read		= rt73usb_read_bbp,
		.write		= rt73usb_write_bbp,
		.word_size	= sizeof(u8),
		.word_count	= BBP_SIZE / sizeof(u8),
	},
};
#endif /* CONFIG_RT2X00_LIB_DEBUGFS */

/*
 * Configuration handlers.
 */
static void rt73usb_config_bssid(struct rt2x00_dev *rt2x00dev, u8 *bssid)
{
	u32 reg[2];

	memset(&reg, 0, sizeof(reg));
	memcpy(&reg, bssid, ETH_ALEN);

	rt2x00_set_field32(&reg[1], MAC_CSR5_BSS_ID_MASK, 3);

	/*
	 * The BSSID is passed to us as an array of bytes,
	 * that array is little endian, so no need for byte ordering.
	 */
	rt73usb_register_multiwrite(rt2x00dev, MAC_CSR4, &reg, sizeof(reg));
}

static void rt73usb_config_promisc(struct rt2x00_dev *rt2x00dev,
	const int promisc)
{
	u32 reg;

	rt73usb_register_read(rt2x00dev, TXRX_CSR0, &reg);
	rt2x00_set_field32(&reg, TXRX_CSR0_DROP_NOT_TO_ME, !promisc);
	rt73usb_register_write(rt2x00dev, TXRX_CSR0, reg);
}

static void rt73usb_config_type(struct rt2x00_dev *rt2x00dev,
	const int type)
{
	u32 reg;

	rt73usb_register_write(rt2x00dev, TXRX_CSR9, 0);

	/*
	 * Apply hardware packet filter.
	 */
	rt73usb_register_read(rt2x00dev, TXRX_CSR0, &reg);

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

	rt73usb_register_write(rt2x00dev, TXRX_CSR0, reg);

	/*
	 * Enable synchronisation.
	 */
	rt73usb_register_read(rt2x00dev, TXRX_CSR9, &reg);
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

	rt73usb_register_write(rt2x00dev, TXRX_CSR9, reg);
}

static void rt73usb_config_channel(struct rt2x00_dev *rt2x00dev,
	const int value, const int channel, const int txpower)
{
	u8 reg = 0;
	u32 rf1 = rt2x00dev->rf1;
	u32 rf2 = value;
	u32 rf3 = rt2x00dev->rf3;
	u32 rf4 = 0;

	if (rt2x00_rf(&rt2x00dev->chip, RF5225) ||
	    rt2x00_rf(&rt2x00dev->chip, RF2527))
		rf2 |= 0x00004000;

	if (rt2x00_rf(&rt2x00dev->chip, RF5225)) {
		if (channel <= 14)
			rf3 = 0x00068455;
		else if (channel >= 36 && channel <= 48)
			rf3 = 0x0009be55;
		else if (channel >= 52 && channel <= 64)
			rf3 = 0x0009ae55;
		else if (channel >= 100 && channel <= 112)
			rf3 = 0x000bae55;
		else
			rf3 = 0x000bbe55;
	}

	if (channel < 14) {
		if (channel & 0x01)
			rf4 = 0x000fea0b;
		else
			rf4 = 0x000fea1f;
	} else if (channel == 14) {
		rf4 = 0x000fea13;
	} else {
		switch (channel) {
			case 36:
			case 56:
			case 116:
			case 136:
				rf4 = 0x000fea23;
				break;
			case 40:
			case 60:
			case 100:
			case 120:
			case 140:
				rf4 = 0x000fea03;
				break;
			case 44:
			case 64:
			case 104:
			case 124:
				rf4 = 0x000fea0b;
				break;
			case 48:
			case 108:
			case 128:
				rf4 = 0x000fea13;
				break;
			case 52:
			case 112:
			case 132:
				rf4 = 0x000fea1b;
				break;
			case 149:
				rf4 = 0x000fea1f;
				break;
			case 153:
				rf4 = 0x000fea27;
				break;
			case 157:
				rf4 = 0x000fea07;
				break;
			case 161:
				rf4 = 0x000fea0f;
				break;
			case 165:
				rf4 = 0x000fea17;
				break;
		}
	}

	if (rt2x00_rf(&rt2x00dev->chip, RF2527) ||
	    rt2x00_rf(&rt2x00dev->chip, RF5225))
		rf4 |= 0x00010000;

	/*
	 * Set TXpower.
	 */
	rt2x00_set_field32(&rf3, RF3_TXPOWER, TXPOWER_TO_DEV(txpower));

	/*
	 * Set Frequency offset.
	 */
	rt2x00_set_field32(&rf4, RF4_FREQ_OFFSET, rt2x00dev->freq_offset);

	rt73usb_bbp_read(rt2x00dev, 3, &reg);
	if (rt2x00_rf(&rt2x00dev->chip, RF5225) ||
	    rt2x00_rf(&rt2x00dev->chip, RF2527))
		reg &= ~0x01;
	else
		reg |= 0x01;
	rt73usb_bbp_write(rt2x00dev, 3, reg);

	rt73usb_rf_write(rt2x00dev, rf1);
	rt73usb_rf_write(rt2x00dev, rf2);
	rt73usb_rf_write(rt2x00dev, rf3 & ~0x00000004);
	rt73usb_rf_write(rt2x00dev, rf4);

	rt73usb_rf_write(rt2x00dev, rf1);
	rt73usb_rf_write(rt2x00dev, rf2);
	rt73usb_rf_write(rt2x00dev, rf3 | 0x00000004);
	rt73usb_rf_write(rt2x00dev, rf4);

	rt73usb_rf_write(rt2x00dev, rf1);
	rt73usb_rf_write(rt2x00dev, rf2);
	rt73usb_rf_write(rt2x00dev, rf3 & ~0x00000004);
	rt73usb_rf_write(rt2x00dev, rf4);

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

static void rt73usb_config_txpower(struct rt2x00_dev *rt2x00dev,
	const int txpower)
{
	rt2x00_set_field32(&rt2x00dev->rf3, RF3_TXPOWER,
		TXPOWER_TO_DEV(txpower));

	rt73usb_rf_write(rt2x00dev, rt2x00dev->rf1);
	rt73usb_rf_write(rt2x00dev, rt2x00dev->rf2);
	rt73usb_rf_write(rt2x00dev, rt2x00dev->rf3 & ~0x00000004);
	rt73usb_rf_write(rt2x00dev, rt2x00dev->rf4);

	rt73usb_rf_write(rt2x00dev, rt2x00dev->rf1);
	rt73usb_rf_write(rt2x00dev, rt2x00dev->rf2);
	rt73usb_rf_write(rt2x00dev, rt2x00dev->rf3 | 0x00000004);
	rt73usb_rf_write(rt2x00dev, rt2x00dev->rf4);

	rt73usb_rf_write(rt2x00dev, rt2x00dev->rf1);
	rt73usb_rf_write(rt2x00dev, rt2x00dev->rf2);
	rt73usb_rf_write(rt2x00dev, rt2x00dev->rf3 & ~0x00000004);
	rt73usb_rf_write(rt2x00dev, rt2x00dev->rf4);
}

static void rt73usb_config_antenna(struct rt2x00_dev *rt2x00dev,
	const int antenna_tx, const int antenna_rx)
{
	u32 reg;
	u8 r3;
	u8 r4;
	u8 r77;

	rt73usb_register_read(rt2x00dev, PHY_CSR0, &reg);

	if (rt2x00dev->curr_hwmode == HWMODE_A) {
		if (test_bit(CONFIG_EXTERNAL_LNA, &rt2x00dev->flags)) {
			rt73usb_bbp_write(rt2x00dev, 17, 0x38);
			rt73usb_bbp_write(rt2x00dev, 96, 0x78);
			rt73usb_bbp_write(rt2x00dev, 104, 0x48);
			rt73usb_bbp_write(rt2x00dev, 75, 0x80);
			rt73usb_bbp_write(rt2x00dev, 86, 0x80);
			rt73usb_bbp_write(rt2x00dev, 88, 0x80);
		} else {
			rt73usb_bbp_write(rt2x00dev, 17, 0x28);
			rt73usb_bbp_write(rt2x00dev, 96, 0x58);
			rt73usb_bbp_write(rt2x00dev, 104, 0x38);
			rt73usb_bbp_write(rt2x00dev, 75, 0xfe);
			rt73usb_bbp_write(rt2x00dev, 86, 0xfe);
			rt73usb_bbp_write(rt2x00dev, 88, 0xfe);
		}
		rt73usb_bbp_write(rt2x00dev, 35, 0x60);
		rt73usb_bbp_write(rt2x00dev, 97, 0x58);
		rt73usb_bbp_write(rt2x00dev, 98, 0x58);

		rt2x00_set_field32(&reg, PHY_CSR0_PA_PE_BG, 0);
		rt2x00_set_field32(&reg, PHY_CSR0_PA_PE_A, 1);
	} else {
		if (test_bit(CONFIG_EXTERNAL_LNA, &rt2x00dev->flags)) {
			rt73usb_bbp_write(rt2x00dev, 17, 0x30);
			rt73usb_bbp_write(rt2x00dev, 96, 0x68);
			rt73usb_bbp_write(rt2x00dev, 104, 0x3c);
			rt73usb_bbp_write(rt2x00dev, 75, 0x80);
			rt73usb_bbp_write(rt2x00dev, 86, 0x80);
			rt73usb_bbp_write(rt2x00dev, 88, 0x80);
		} else {
			rt73usb_bbp_write(rt2x00dev, 17, 0x20);
			rt73usb_bbp_write(rt2x00dev, 96, 0x48);
			rt73usb_bbp_write(rt2x00dev, 104, 0x2c);
			rt73usb_bbp_write(rt2x00dev, 75, 0xfe);
			rt73usb_bbp_write(rt2x00dev, 86, 0xfe);
			rt73usb_bbp_write(rt2x00dev, 88, 0xfe);
		}
		rt73usb_bbp_write(rt2x00dev, 35, 0x50);
		rt73usb_bbp_write(rt2x00dev, 97, 0x48);
		rt73usb_bbp_write(rt2x00dev, 98, 0x48);

		rt2x00_set_field32(&reg, PHY_CSR0_PA_PE_BG, 1);
		rt2x00_set_field32(&reg, PHY_CSR0_PA_PE_A, 0);
	}

	rt73usb_register_write(rt2x00dev, PHY_CSR0, reg);

	rt73usb_bbp_read(rt2x00dev, 3, &r3);
	rt73usb_bbp_read(rt2x00dev, 4, &r4);
	rt73usb_bbp_read(rt2x00dev, 77, &r77);

	rt2x00_set_field8(&r3, BBP_R3_SMART_MODE, 0);

	if (rt2x00_rf(&rt2x00dev->chip, RF5226) ||
	    rt2x00_rf(&rt2x00dev->chip, RF5225)) {
		if (antenna_rx == ANTENNA_DIVERSITY) {
			rt2x00_set_field8(&r4, BBP_R4_RX_ANTENNA, 2);
			if (rt2x00dev->curr_hwmode != HWMODE_A)
				rt2x00_set_field8(&r4, BBP_R4_RX_BG_MODE, 1);
			rt2x00_set_field8(&r4, BBP_R4_RX_FRAME_END,
				test_bit(CONFIG_FRAME_TYPE, &rt2x00dev->flags));
		} else if (antenna_rx == ANTENNA_A) {
			rt2x00_set_field8(&r4, BBP_R4_RX_ANTENNA, 1);
			rt2x00_set_field8(&r4, BBP_R4_RX_FRAME_END,
				test_bit(CONFIG_FRAME_TYPE, &rt2x00dev->flags));
			if (rt2x00dev->curr_hwmode == HWMODE_A)
				rt2x00_set_field8(&r77, BBP_R77_PAIR, 0);
			else
				rt2x00_set_field8(&r77, BBP_R77_PAIR, 3);
			rt73usb_bbp_write(rt2x00dev, 77, r77);
		} else if (antenna_rx == ANTENNA_B) {
			rt2x00_set_field8(&r4, BBP_R4_RX_ANTENNA, 1);
			rt2x00_set_field8(&r4, BBP_R4_RX_FRAME_END,
				test_bit(CONFIG_FRAME_TYPE, &rt2x00dev->flags));
			if (rt2x00dev->curr_hwmode == HWMODE_A)
				rt2x00_set_field8(&r77, BBP_R77_PAIR, 3);
			else
				rt2x00_set_field8(&r77, BBP_R77_PAIR, 0);
			rt73usb_bbp_write(rt2x00dev, 77, r77);
		}
	} else if (rt2x00_rf(&rt2x00dev->chip, RF2528) ||
		   rt2x00_rf(&rt2x00dev->chip, RF2527)) {
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
			rt73usb_bbp_write(rt2x00dev, 77, r77);
		} else if (antenna_rx == ANTENNA_B) {
			rt2x00_set_field8(&r4, BBP_R4_RX_ANTENNA, 1);
			rt2x00_set_field8(&r4, BBP_R4_RX_BG_MODE, 1);
			rt2x00_set_field8(&r4, BBP_R4_RX_FRAME_END,
				test_bit(CONFIG_FRAME_TYPE, &rt2x00dev->flags));
			rt2x00_set_field8(&r77, BBP_R77_PAIR, 0);
		}
	}

	rt73usb_bbp_write(rt2x00dev, 3, r3);
	rt73usb_bbp_write(rt2x00dev, 4, r4);
}

static void rt73usb_config_duration(struct rt2x00_dev *rt2x00dev,
	const int short_slot_time, const int beacon_int)
{
	u32 reg;

	rt73usb_register_read(rt2x00dev, MAC_CSR9, &reg);
	rt2x00_set_field32(&reg, MAC_CSR9_SLOT_TIME,
		short_slot_time ? SHORT_SLOT_TIME : SLOT_TIME);
	rt73usb_register_write(rt2x00dev, MAC_CSR9, reg);

	rt73usb_register_read(rt2x00dev, MAC_CSR8, &reg);
	rt2x00_set_field32(&reg, MAC_CSR8_SIFS, SIFS);
	rt2x00_set_field32(&reg, MAC_CSR8_SIFS_AFTER_RX_OFDM, 3);
	rt2x00_set_field32(&reg, MAC_CSR8_EIFS, EIFS);
	rt73usb_register_write(rt2x00dev, MAC_CSR8, reg);

	rt73usb_register_read(rt2x00dev, TXRX_CSR0, &reg);
	rt2x00_set_field32(&reg, TXRX_CSR0_TSF_OFFSET, IEEE80211_HEADER);
	rt73usb_register_write(rt2x00dev, TXRX_CSR0, reg);

	rt73usb_register_read(rt2x00dev, TXRX_CSR4, &reg);
	rt2x00_set_field32(&reg, TXRX_CSR4_AUTORESPOND_ENABLE, 1);
	rt73usb_register_write(rt2x00dev, TXRX_CSR4, reg);

	rt73usb_register_read(rt2x00dev, TXRX_CSR9, &reg);
	rt2x00_set_field32(&reg, TXRX_CSR9_BEACON_INTERVAL, beacon_int * 16);
	rt73usb_register_write(rt2x00dev, TXRX_CSR9, reg);
}

static void rt73usb_config_rate(struct rt2x00_dev *rt2x00dev, const int rate)
{
	struct ieee80211_conf *conf = &rt2x00dev->hw->conf;
	u32 reg;
	u32 value;
	u32 preamble;

	preamble = DEVICE_GET_RATE_FIELD(rate, PREAMBLE)
		? SHORT_PREAMBLE : PREAMBLE;

	reg = DEVICE_GET_RATE_FIELD(rate, RATEMASK) & DEV_BASIC_RATE;

	rt73usb_register_write(rt2x00dev, TXRX_CSR5, reg);

	rt73usb_register_read(rt2x00dev, TXRX_CSR0, &reg);
	value = ((conf->flags & IEEE80211_CONF_SHORT_SLOT_TIME) ?
		 SHORT_DIFS :  DIFS) +
		PLCP + preamble + get_duration(ACK_SIZE, 10);
	rt2x00_set_field32(&reg, TXRX_CSR0_RX_ACK_TIMEOUT, value);
	rt73usb_register_write(rt2x00dev, TXRX_CSR0, reg);

	rt73usb_register_read(rt2x00dev, TXRX_CSR4, &reg);
	if (preamble == SHORT_PREAMBLE)
		rt2x00_set_field32(&reg, TXRX_CSR4_AUTORESPOND_PREAMBLE, 1);
	else
		rt2x00_set_field32(&reg, TXRX_CSR4_AUTORESPOND_PREAMBLE, 0);
	rt73usb_register_write(rt2x00dev, TXRX_CSR4, reg);
}

static void rt73usb_config_phymode(struct rt2x00_dev *rt2x00dev,
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

	rt73usb_config_rate(rt2x00dev, rate->val2);
}

static void rt73usb_config_mac_addr(struct rt2x00_dev *rt2x00dev, u8 *addr)
{
	u32 reg[2];

	memset(&reg, 0, sizeof(reg));
	memcpy(&reg, addr, ETH_ALEN);

	rt2x00_set_field32(&reg[1], MAC_CSR3_UNICAST_TO_ME_MASK, 0xff);

	/*
	 * The MAC address is passed to us as an array of bytes,
	 * that array is little endian, so no need for byte ordering.
	 */
	rt73usb_register_multiwrite(rt2x00dev, MAC_CSR2, &reg, sizeof(reg));
}

/*
 * LED functions.
 */
static void rt73usb_enable_led(struct rt2x00_dev *rt2x00dev)
{
	u32 reg;

	rt73usb_register_read(rt2x00dev, MAC_CSR14, &reg);
	rt2x00_set_field32(&reg, MAC_CSR14_ON_PERIOD, 70);
	rt2x00_set_field32(&reg, MAC_CSR14_OFF_PERIOD, 30);
	rt73usb_register_write(rt2x00dev, MAC_CSR14, reg);

	rt2x00_set_field16(&rt2x00dev->led_reg, MCU_LEDCS_RADIO_STATUS, 1);
	if (rt2x00dev->rx_status.phymode == MODE_IEEE80211A)
		rt2x00_set_field16(
			&rt2x00dev->led_reg, MCU_LEDCS_LINK_A_STATUS, 1);
	else
		rt2x00_set_field16(
			&rt2x00dev->led_reg, MCU_LEDCS_LINK_BG_STATUS, 1);

	rt2x00usb_vendor_request(
		rt2x00dev, USB_LED_CONTROL, USB_VENDOR_REQUEST_OUT,
		0x00, rt2x00dev->led_reg, NULL, 0, REGISTER_TIMEOUT);
}

static void rt73usb_disable_led(struct rt2x00_dev *rt2x00dev)
{
	rt2x00_set_field16(&rt2x00dev->led_reg, MCU_LEDCS_RADIO_STATUS, 0);
	rt2x00_set_field16(&rt2x00dev->led_reg, MCU_LEDCS_LINK_BG_STATUS, 0);
	rt2x00_set_field16(&rt2x00dev->led_reg, MCU_LEDCS_LINK_A_STATUS, 0);

	rt2x00usb_vendor_request(
		rt2x00dev, USB_LED_CONTROL, USB_VENDOR_REQUEST_OUT,
		0x00, rt2x00dev->led_reg, NULL, 0, REGISTER_TIMEOUT);
}

static void rt73usb_activity_led(struct rt2x00_dev *rt2x00dev, char rssi)
{
	u32 led;

	if (rt2x00dev->led_mode != LED_MODE_SIGNAL_STRENGTH)
		return;

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

	rt2x00usb_vendor_request(
		rt2x00dev, USB_LED_CONTROL, USB_VENDOR_REQUEST_OUT,
		led, rt2x00dev->led_reg, NULL, 0, REGISTER_TIMEOUT);
}

/*
 * Link tuning
 */
static void rt73usb_link_tuner(struct rt2x00_dev *rt2x00dev, int rssi)
{
	u32 reg;
	u8 r17;
	u8 up_bound;
	u8 low_bound;

	/*
	 * Update Led strength
	 */
	rt73usb_activity_led(rt2x00dev, rssi);

	rt73usb_bbp_read(rt2x00dev, 17, &r17);

	/*
	 * Determine r17 bounds.
	 */
	if (rt2x00dev->rx_status.phymode == MODE_IEEE80211A) {
		low_bound = 0x28;
		up_bound = 0x48;

		if (test_bit(CONFIG_EXTERNAL_LNA, &rt2x00dev->flags)) {
			low_bound += 0x10;
			up_bound += 0x10;
		}
	} else {
		if (rssi > -82) {
			low_bound = 0x1c;
			up_bound = 0x40;
		} else if (rssi > -84) {
			low_bound = 0x1c;
			up_bound = 0x20;
		} else {
			low_bound = 0x1c;
			up_bound = 0x1c;
		}

		if (test_bit(CONFIG_EXTERNAL_LNA, &rt2x00dev->flags)) {
			low_bound += 0x14;
			up_bound += 0x10;
		}
	}

	/*
	 * Special big-R17 for very short distance
	 */
	if (rssi > -35) {
		if (r17 != 0x60)
			rt73usb_bbp_write(rt2x00dev, 17, 0x60);
		return;
	}

	/*
	 * Special big-R17 for short distance
	 */
	if (rssi >= -58) {
		if (r17 != up_bound)
			rt73usb_bbp_write(rt2x00dev, 17, up_bound);
		return;
	}

	/*
	 * Special big-R17 for middle-short distance
	 */
	if (rssi >= -66) {
		low_bound += 0x10;
		if (r17 != low_bound)
			rt73usb_bbp_write(rt2x00dev, 17, low_bound);
		return;
	}

	/*
	 * Special mid-R17 for middle distance
	 */
	if (rssi >= -74) {
		if (r17 != (low_bound + 0x10))
			rt73usb_bbp_write(rt2x00dev, 17, low_bound + 0x08);
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
		rt73usb_bbp_write(rt2x00dev, 17, up_bound);
		return;
	}

	/*
	 * r17 does not yet exceed upper limit, continue and base
	 * the r17 tuning on the false CCA count.
	 */
	rt73usb_register_read(rt2x00dev, STA_CSR1, &reg);
	reg = rt2x00_get_field32(reg, STA_CSR1_FALSE_CCA_ERROR);
	rt2x00dev->link.false_cca =
		rt2x00_get_field32(reg, STA_CSR1_FALSE_CCA_ERROR);

	if (rt2x00dev->link.false_cca > 512 && r17 < up_bound) {
		r17 += 4;
		if (r17 > up_bound)
			r17 = up_bound;
		rt73usb_bbp_write(rt2x00dev, 17, r17);
	} else if (rt2x00dev->link.false_cca < 100 && r17 > low_bound) {
		r17 -= 4;
		if (r17 < low_bound)
			r17 = low_bound;
		rt73usb_bbp_write(rt2x00dev, 17, r17);
	}
}

/*
 * Firmware name function.
 */
static char *rt73usb_get_fw_name(struct rt2x00_dev *rt2x00dev)
{
	return FIRMWARE_RT2571;
}

/*
 * Initialization functions.
 */
static int rt73usb_load_firmware(struct rt2x00_dev *rt2x00dev, void *data,
	const size_t len)
{
	unsigned int i;
	int status;
	u32 reg;
	char buf[64];
	char *ptr = data;
	int buflen;

	/*
	 * Wait for stable hardware.
	 */
	for (i = 0; i < 100; i++) {
		rt73usb_register_read(rt2x00dev, MAC_CSR0, &reg);
		if (reg)
			break;
		msleep(1);
	}

	if (!reg) {
		ERROR(rt2x00dev, "Unstable hardware.\n");
		return -EBUSY;
	}

	/*
	 * Write firmware to device.
	 */
	for (i = 0; i < len; i += sizeof(buf)) {
		buflen = min(len - i, sizeof(buf));
		memcpy(buf, ptr, buflen);
		rt73usb_register_multiwrite(rt2x00dev, FIRMWARE_IMAGE_BASE + i,
			buf, buflen);
		ptr += buflen;
	}

	/*
	 * Send firmware request to device to load firmware,
	 * we need to specify a long timeout time.
	 */
	status = rt2x00usb_vendor_request(rt2x00dev, USB_DEVICE_MODE,
		USB_VENDOR_REQUEST_OUT, 0x00, USB_MODE_FIRMWARE,
		NULL, 0, REGISTER_TIMEOUT_FIRMWARE);
	if (status  < 0) {
		ERROR(rt2x00dev, "Failed to write Firmware to device.\n");
		return status;
	}

	rt73usb_disable_led(rt2x00dev);

	return 0;
}

static void rt73usb_init_rxring(struct rt2x00_dev *rt2x00dev)
{
	struct usb_device *usb_dev =
		interface_to_usbdev(rt2x00dev_usb(rt2x00dev));
	unsigned int i;

	for (i = 0; i < rt2x00dev->rx->stats.limit; i++) {
		usb_fill_bulk_urb(
			rt2x00dev->rx->entry[i].priv,
			usb_dev,
			usb_rcvbulkpipe(usb_dev, 1),
			rt2x00dev->rx->entry[i].skb->data,
			rt2x00dev->rx->entry[i].skb->len,
			rt73usb_interrupt_rxdone,
			&rt2x00dev->rx->entry[i]);
	}

	rt2x00_ring_index_clear(rt2x00dev->rx);
}

static void rt73usb_init_txring(struct rt2x00_dev *rt2x00dev,
	const int queue)
{
	struct data_ring *ring = rt2x00_get_ring(rt2x00dev, queue);
	unsigned int i;

	for (i = 0; i < ring->stats.limit; i++)
		ring->entry[i].flags = 0;

	rt2x00_ring_index_clear(ring);
}

static int rt73usb_init_rings(struct rt2x00_dev *rt2x00dev)
{
	rt73usb_init_rxring(rt2x00dev);
	rt73usb_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_DATA0);
	rt73usb_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_DATA1);
	rt73usb_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_DATA2);
	rt73usb_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_DATA3);
	rt73usb_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_DATA4);
	rt73usb_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_BEACON);

	return 0;
}

static int rt73usb_init_registers(struct rt2x00_dev *rt2x00dev)
{
	u32 reg;

	if (rt2x00dev->ops->lib->set_device_state(rt2x00dev, STATE_AWAKE))
		return -EBUSY;

	rt73usb_register_write(rt2x00dev, MAC_CSR10, 0x00000718);

	rt73usb_register_write(rt2x00dev, TXRX_CSR0, 0x025eb032);

	rt73usb_register_write(rt2x00dev, TXRX_CSR1, 0x9eaa9eaf);
	rt73usb_register_write(rt2x00dev, TXRX_CSR2, 0x8a8b8c8d);
	rt73usb_register_write(rt2x00dev, TXRX_CSR3, 0x00858687);

	rt73usb_register_write(rt2x00dev, TXRX_CSR7, 0x2e31353b);
	rt73usb_register_write(rt2x00dev, TXRX_CSR8, 0x2a2a2a2c);

	rt73usb_register_write(rt2x00dev, TXRX_CSR15, 0x0000000f);

	rt73usb_register_write(rt2x00dev, MAC_CSR6, 0x00000fff);

	rt73usb_register_write(rt2x00dev, MAC_CSR13, 0x00007f00);

	rt73usb_register_write(rt2x00dev, SEC_CSR0, 0x00000000);
	rt73usb_register_write(rt2x00dev, SEC_CSR1, 0x00000000);
	rt73usb_register_write(rt2x00dev, SEC_CSR5, 0x00000000);

	reg = 0x000023b0;
	if (rt2x00_rf(&rt2x00dev->chip, RF5225) ||
	    rt2x00_rf(&rt2x00dev->chip, RF2527))
		rt2x00_set_field32(&reg, PHY_CSR1_RF_RPI, 1);
	rt73usb_register_write(rt2x00dev, PHY_CSR1, reg);

	rt73usb_register_write(rt2x00dev, PHY_CSR5, 0x00040a06);
	rt73usb_register_write(rt2x00dev, PHY_CSR6, 0x00080606);
	rt73usb_register_write(rt2x00dev, PHY_CSR7, 0x00000408);

	rt73usb_register_read(rt2x00dev, AC_TXOP_CSR0, &reg);
	rt2x00_set_field32(&reg, AC_TXOP_CSR0_AC0_TX_OP, 0);
	rt2x00_set_field32(&reg, AC_TXOP_CSR0_AC1_TX_OP, 0);
	rt73usb_register_write(rt2x00dev, AC_TXOP_CSR0, reg);

	rt73usb_register_read(rt2x00dev, AC_TXOP_CSR1, &reg);
	rt2x00_set_field32(&reg, AC_TXOP_CSR1_AC2_TX_OP, 192);
	rt2x00_set_field32(&reg, AC_TXOP_CSR1_AC3_TX_OP, 48);
	rt73usb_register_write(rt2x00dev, AC_TXOP_CSR1, reg);

	rt73usb_register_read(rt2x00dev, MAC_CSR9, &reg);
	rt2x00_set_field32(&reg, MAC_CSR9_CW_SELECT, 0);
	rt73usb_register_write(rt2x00dev, MAC_CSR9, reg);

	rt73usb_register_read(rt2x00dev, TXRX_CSR0, &reg);
	rt2x00_set_field32(&reg, TXRX_CSR0_AUTO_TX_SEQ, 1);
	rt73usb_register_write(rt2x00dev, TXRX_CSR0, reg);

	/*
	 * We must clear the error counters.
	 * These registers are cleared on read,
	 * so we may pass a useless variable to store the value.
	 */
	rt73usb_register_read(rt2x00dev, STA_CSR0, &reg);
	rt73usb_register_read(rt2x00dev, STA_CSR1, &reg);
	rt73usb_register_read(rt2x00dev, STA_CSR2, &reg);

	/*
	 * Reset MAC and BBP registers.
	 */
	reg = 0;
	rt2x00_set_field32(&reg, MAC_CSR1_SOFT_RESET, 1);
	rt2x00_set_field32(&reg, MAC_CSR1_BBP_RESET, 1);
	rt73usb_register_write(rt2x00dev, MAC_CSR1, reg);

	rt73usb_register_read(rt2x00dev, MAC_CSR1, &reg);
	rt2x00_set_field32(&reg, MAC_CSR1_SOFT_RESET, 0);
	rt2x00_set_field32(&reg, MAC_CSR1_BBP_RESET, 0);
	rt73usb_register_write(rt2x00dev, MAC_CSR1, reg);

	rt73usb_register_read(rt2x00dev, MAC_CSR1, &reg);
	rt2x00_set_field32(&reg, MAC_CSR1_HOST_READY, 1);
	rt73usb_register_write(rt2x00dev, MAC_CSR1, reg);

	return 0;
}

static int rt73usb_init_bbp(struct rt2x00_dev *rt2x00dev)
{
	unsigned int i;
	u16 eeprom;
	u8 reg_id;
	u8 value;

	for (i = 0; i < REGISTER_BUSY_COUNT; i++) {
		rt73usb_bbp_read(rt2x00dev, 0, &value);
		if ((value != 0xff) && (value != 0x00))
			goto continue_csr_init;
		NOTICE(rt2x00dev, "Waiting for BBP register.\n");
		udelay(REGISTER_BUSY_DELAY);
	}

	ERROR(rt2x00dev, "BBP register access failed, aborting.\n");
	return -EACCES;

continue_csr_init:
	rt73usb_bbp_write(rt2x00dev, 3, 0x80);
	rt73usb_bbp_write(rt2x00dev, 15, 0x30);
	rt73usb_bbp_write(rt2x00dev, 17, 0x20);
	rt73usb_bbp_write(rt2x00dev, 21, 0xc8);
	rt73usb_bbp_write(rt2x00dev, 22, 0x38);
	rt73usb_bbp_write(rt2x00dev, 23, 0x06);
	rt73usb_bbp_write(rt2x00dev, 24, 0xfe);
	rt73usb_bbp_write(rt2x00dev, 25, 0x0a);
	rt73usb_bbp_write(rt2x00dev, 26, 0x0d);
	rt73usb_bbp_write(rt2x00dev, 32, 0x0b);
	rt73usb_bbp_write(rt2x00dev, 34, 0x12);
	rt73usb_bbp_write(rt2x00dev, 37, 0x07);
	rt73usb_bbp_write(rt2x00dev, 39, 0xf8);
	rt73usb_bbp_write(rt2x00dev, 41, 0x60);
	rt73usb_bbp_write(rt2x00dev, 53, 0x10);
	rt73usb_bbp_write(rt2x00dev, 54, 0x18);
	rt73usb_bbp_write(rt2x00dev, 60, 0x10);
	rt73usb_bbp_write(rt2x00dev, 61, 0x04);
	rt73usb_bbp_write(rt2x00dev, 62, 0x04);
	rt73usb_bbp_write(rt2x00dev, 75, 0xfe);
	rt73usb_bbp_write(rt2x00dev, 86, 0xfe);
	rt73usb_bbp_write(rt2x00dev, 88, 0xfe);
	rt73usb_bbp_write(rt2x00dev, 90, 0x0f);
	rt73usb_bbp_write(rt2x00dev, 99, 0x00);
	rt73usb_bbp_write(rt2x00dev, 102, 0x16);
	rt73usb_bbp_write(rt2x00dev, 107, 0x04);

	DEBUG(rt2x00dev, "Start initialization from EEPROM...\n");
	for (i = 0; i < EEPROM_BBP_SIZE; i++) {
		rt2x00_eeprom_read(rt2x00dev, EEPROM_BBP_START + i, &eeprom);

		if (eeprom != 0xffff && eeprom != 0x0000) {
			reg_id = rt2x00_get_field16(eeprom, EEPROM_BBP_REG_ID);
			value = rt2x00_get_field16(eeprom, EEPROM_BBP_VALUE);
			DEBUG(rt2x00dev, "BBP: 0x%02x, value: 0x%02x.\n",
				reg_id, value);
			rt73usb_bbp_write(rt2x00dev, reg_id, value);
		}
	}
	DEBUG(rt2x00dev, "...End initialization from EEPROM.\n");

	return 0;
}

/*
 * Device state switch handlers.
 */
static void rt73usb_toggle_rx(struct rt2x00_dev *rt2x00dev,
	enum dev_state state)
{
	u32 reg;

	rt73usb_register_read(rt2x00dev, TXRX_CSR0, &reg);
	rt2x00_set_field32(&reg, TXRX_CSR0_DISABLE_RX,
		state == STATE_RADIO_RX_OFF);
	rt73usb_register_write(rt2x00dev, TXRX_CSR0, reg);
}

static int rt73usb_enable_radio(struct rt2x00_dev *rt2x00dev)
{
	/*
	 * Initialize all registers.
	 */
	if (rt73usb_init_rings(rt2x00dev) ||
	    rt73usb_init_registers(rt2x00dev) ||
	    rt73usb_init_bbp(rt2x00dev)) {
		ERROR(rt2x00dev, "Register initialization failed.\n");
		return -EIO;
	}

	rt2x00usb_enable_radio(rt2x00dev);

	/*
	 * Enable LED
	 */
	rt73usb_enable_led(rt2x00dev);

	return 0;
}

static void rt73usb_disable_radio(struct rt2x00_dev *rt2x00dev)
{
	/*
	 * Disable LED
	 */
	rt73usb_disable_led(rt2x00dev);

	rt73usb_register_write(rt2x00dev, MAC_CSR10, 0x00001818);

	/*
	 * Disable synchronisation.
	 */
	rt73usb_register_write(rt2x00dev, TXRX_CSR9, 0);

	rt2x00usb_disable_radio(rt2x00dev);
}

static int rt73usb_set_state(struct rt2x00_dev *rt2x00dev,
	enum dev_state state)
{
	u32 reg;
	unsigned int i;
	char put_to_sleep;
	char current_state;

	put_to_sleep = (state != STATE_AWAKE);

	if (!put_to_sleep)
		rt2x00usb_vendor_request(rt2x00dev,
			USB_DEVICE_MODE, USB_VENDOR_REQUEST_OUT,
			0x00, USB_MODE_WAKEUP, NULL, 0, REGISTER_TIMEOUT);

	rt73usb_register_read(rt2x00dev, MAC_CSR12, &reg);
	rt2x00_set_field32(&reg, MAC_CSR12_FORCE_WAKEUP, !put_to_sleep);
	rt2x00_set_field32(&reg, MAC_CSR12_PUT_TO_SLEEP, put_to_sleep);
	rt73usb_register_write(rt2x00dev, MAC_CSR12, reg);

	if (put_to_sleep)
		rt2x00usb_vendor_request(rt2x00dev,
			USB_DEVICE_MODE, USB_VENDOR_REQUEST_OUT,
			0x00, USB_MODE_SLEEP, NULL, 0, REGISTER_TIMEOUT);

	/*
	 * Device is not guaranteed to be in the requested state yet.
	 * We must wait until the register indicates that the
	 * device has entered the correct state.
	 */
	for (i = 0; i < REGISTER_BUSY_COUNT; i++) {
		rt73usb_register_read(rt2x00dev, MAC_CSR12, &reg);
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

static int rt73usb_set_device_state(struct rt2x00_dev *rt2x00dev,
	enum dev_state state)
{
	int retval = 0;

	switch (state) {
		case STATE_RADIO_ON:
			retval = rt73usb_enable_radio(rt2x00dev);
		break;
		case STATE_RADIO_OFF:
			rt73usb_disable_radio(rt2x00dev);
		break;
		case STATE_RADIO_RX_ON:
		case STATE_RADIO_RX_OFF:
			rt73usb_toggle_rx(rt2x00dev, state);
		break;
		case STATE_DEEP_SLEEP:
		case STATE_SLEEP:
		case STATE_STANDBY:
		case STATE_AWAKE:
			retval = rt73usb_set_state(rt2x00dev, state);
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
static void rt73usb_write_tx_desc(struct rt2x00_dev *rt2x00dev,
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

	rt2x00_desc_read(txd, 0, &word);
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
static void rt73usb_kick_tx_queue(struct rt2x00_dev *rt2x00dev, int queue)
{
	u32 reg;

	if (queue != IEEE80211_TX_QUEUE_BEACON)
		return;

	rt73usb_register_read(rt2x00dev, TXRX_CSR9, &reg);
	if (!rt2x00_get_field32(reg, TXRX_CSR9_BEACON_GEN)) {
		rt2x00_set_field32(&reg, TXRX_CSR9_BEACON_GEN, 1);
		rt73usb_register_write(rt2x00dev, TXRX_CSR9, reg);
	}
}

/*
 * Interrupt functions.
 */
static void rt73usb_interrupt_rxdone(struct urb *urb)
{
	struct data_entry *entry = (struct data_entry*)urb->context;
	struct data_ring *ring = entry->ring;
	struct rt2x00_dev *rt2x00dev = ring->rt2x00dev;
	struct data_desc *rxd = (struct data_desc*)entry->skb->data;
	u32 word0;
	u32 word1;
	int signal;
	int rssi;
	int ofdm;
	u16 size;

	if (!test_bit(DEVICE_ENABLED_RADIO, &rt2x00dev->flags) ||
	    !__test_and_clear_bit(ENTRY_OWNER_NIC, &entry->flags))
		return;

	/*
	 * Check if the received data is simply too small
	 * to be actually valid, or if the urb is signaling
	 * a problem.
	 */
	if (urb->actual_length < entry->ring->desc_size || urb->status)
		goto skip_entry;

	rt2x00_desc_read(rxd, 0, &word0);
	rt2x00_desc_read(rxd, 1, &word1);

	/*
	 * TODO: Don't we need to keep statistics
	 * updated about events like CRC and physical errors?
	 */
	if (rt2x00_get_field32(word0, RXD_W0_CRC) ||
	    rt2x00_get_field32(word0, RXD_W0_CIPHER_ERROR))
		goto skip_entry;

	/*
	 * Obtain the status about this packet.
	 */
	size = rt2x00_get_field32(word0, RXD_W0_DATABYTE_COUNT);
	signal = rt2x00_get_field32(word1, RXD_W1_SIGNAL);
	rssi = rt2x00_get_field32(word1, RXD_W1_RSSI);
	ofdm = rt2x00_get_field32(word0, RXD_W0_OFDM);

	/*
	 * Trim the skb_buffer to only contain the valid
	 * frame data (so ignore the device's descriptor).
	 */
	skb_pull(entry->skb, ring->desc_size);
	skb_trim(entry->skb, size);

	/*
	 * Send the packet to upper layer, and update urb.
	 */
	rt2x00lib_rxdone(entry, NULL, ring->data_size + ring->desc_size,
		signal, rssi, ofdm);
	urb->transfer_buffer = entry->skb->data;
	urb->transfer_buffer_length = entry->skb->len;

skip_entry:
	if (test_bit(DEVICE_ENABLED_RADIO, &ring->rt2x00dev->flags)) {
		__set_bit(ENTRY_OWNER_NIC, &entry->flags);
		usb_submit_urb(urb, GFP_ATOMIC);
	}

	rt2x00_ring_index_inc(ring);
}

/*
 * Device initialization functions.
 */
static int rt73usb_alloc_eeprom(struct rt2x00_dev *rt2x00dev)
{
	u16 word;

	/*
	 * Allocate the eeprom memory, check the eeprom width
	 * and copy the entire eeprom into this allocated memory.
	 */
	rt2x00dev->eeprom = kzalloc(EEPROM_SIZE, GFP_KERNEL);
	if (!rt2x00dev->eeprom)
		return -ENOMEM;

	rt2x00usb_vendor_request(
		rt2x00dev, USB_EEPROM_READ, USB_VENDOR_REQUEST_IN,
		EEPROM_BASE, 0x00, rt2x00dev->eeprom, EEPROM_SIZE,
		REGISTER_TIMEOUT * (EEPROM_SIZE / sizeof(u16)));

	/*
	 * Start validation of the data that has been read.
	 */
	rt2x00_eeprom_read(rt2x00dev, EEPROM_ANTENNA, &word);
	if (word == 0xffff) {
		rt2x00_set_field16(&word, EEPROM_ANTENNA_NUM, 2);
		rt2x00_set_field16(&word, EEPROM_ANTENNA_TX_DEFAULT, 2);
		rt2x00_set_field16(&word, EEPROM_ANTENNA_RX_DEFAULT, 2);
		rt2x00_set_field16(&word, EEPROM_ANTENNA_FRAME_TYPE, 0);
		rt2x00_set_field16(&word, EEPROM_ANTENNA_DYN_TXAGC, 0);
		rt2x00_set_field16(&word, EEPROM_ANTENNA_HARDWARE_RADIO, 0);
		rt2x00_set_field16(&word, EEPROM_ANTENNA_RF_TYPE, RF5226);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_ANTENNA, word);
		EEPROM(rt2x00dev, "Antenna: 0x%04x\n", word);
	}

	rt2x00_eeprom_read(rt2x00dev, EEPROM_NIC, &word);
	if (word == 0xffff) {
		rt2x00_set_field16(&word, EEPROM_NIC_EXTERNAL_LNA, 0);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_NIC, word);
		EEPROM(rt2x00dev, "NIC: 0x%04x\n", word);
	}

	rt2x00_eeprom_read(rt2x00dev, EEPROM_LED, &word);
	if (word == 0xffff) {
		rt2x00_set_field16(&word, EEPROM_LED_POLARITY_RDY_G, 0);
		rt2x00_set_field16(&word, EEPROM_LED_POLARITY_RDY_A, 0);
		rt2x00_set_field16(&word, EEPROM_LED_POLARITY_ACT, 0);
		rt2x00_set_field16(&word, EEPROM_LED_POLARITY_GPIO_0, 0);
		rt2x00_set_field16(&word, EEPROM_LED_POLARITY_GPIO_1, 0);
		rt2x00_set_field16(&word, EEPROM_LED_POLARITY_GPIO_2, 0);
		rt2x00_set_field16(&word, EEPROM_LED_POLARITY_GPIO_3, 0);
		rt2x00_set_field16(&word, EEPROM_LED_POLARITY_GPIO_4, 0);
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

	return 0;
}

static int rt73usb_init_eeprom(struct rt2x00_dev *rt2x00dev)
{
	u32 reg;
	u16 value;
	u16 eeprom;

	/*
	 * Read EEPROM word for configuration.
	 */
	rt2x00_eeprom_read(rt2x00dev, EEPROM_ANTENNA, &eeprom);

	/*
	 * Identify RF chipset.
	 */
	value = rt2x00_get_field16(eeprom, EEPROM_ANTENNA_RF_TYPE);
	rt73usb_register_read(rt2x00dev, MAC_CSR0, &reg);
	rt2x00_set_chip(rt2x00dev, RT2571, value, reg);

	if (!rt2x00_rf(&rt2x00dev->chip, RF5226) &&
	    !rt2x00_rf(&rt2x00dev->chip, RF2528) &&
	    !rt2x00_rf(&rt2x00dev->chip, RF5225) &&
	    !rt2x00_rf(&rt2x00dev->chip, RF2527)) {
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
	 * Read frequency offset.
	 */
	rt2x00_eeprom_read(rt2x00dev, EEPROM_FREQ, &eeprom);
	rt2x00dev->freq_offset =
		rt2x00_get_field16(eeprom, EEPROM_FREQ_OFFSET);

	/*
	 * Read external LNA informations.
	 */
	rt2x00_eeprom_read(rt2x00dev, EEPROM_NIC, &eeprom);

	if (rt2x00_get_field16(eeprom, EEPROM_NIC_EXTERNAL_LNA))
		__set_bit(CONFIG_EXTERNAL_LNA, &rt2x00dev->flags);

	/*
	 * Store led settings, for correct led behaviour.
	 */
	rt2x00_eeprom_read(rt2x00dev, EEPROM_LED, &eeprom);

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

static const struct {
	unsigned int chip;
	u32 val[3];
} rf_vals[] = {
	{ RF5226,	{ 0x00002c0c, 0x00068255 } },
	{ RF2528,	{ 0x00002c0c, 0x00068255 } },
	{ RF5225,	{ 0x00002ccc, 0x00000000 } },
	{ RF2527,	{ 0x00002ccc, 0x00068455 } },
};

/*
 * RF value list for RF5226, RF2528, RF5225 & RF2527
 * Supports: 2.4 GHz
 */
static const u32 rf_vals_bg[] = {
	0x00000786, 0x00000786, 0x0000078a, 0x0000078a, 0x0000078e,
	0x0000078e, 0x00000792, 0x00000792, 0x00000796, 0x00000796,
	0x0000079a, 0x0000079a, 0x0000079e, 0x000007a2
};

/*
 * RF value list for RF5226 & RF5225 (supplement to vals_bg)
 * Supports: 5.2 GHz
 */
static const u32 rf_vals_a_5x[] = {
	0x0000099a, 0x000009a2, 0x000009a6, 0x000009aa, 0x000009ae,
	0x000009b2, 0x000009ba, 0x000009be, 0x00000a2a, 0x00000a2e,
	0x00000a32, 0x00000a36, 0x00000a3a, 0x00000a82, 0x00000a86,
	0x00000a8a, 0x00000a8e, 0x00000a92, 0x00000a9a, 0x00000aa2,
	0x00000aa6, 0x00000aae, 0x00000ab2, 0x00000ab6
};

static void rt73usb_init_hw_mode(struct rt2x00_dev *rt2x00dev)
{
	struct hw_mode_spec *spec = &rt2x00dev->spec;
	u8 *txpower;
	unsigned int i;

	/*
	 * Initialize all hw fields.
	 */
	rt2x00dev->hw->flags = IEEE80211_HW_HOST_GEN_BEACON_TEMPLATE |
		IEEE80211_HW_HOST_BROADCAST_PS_BUFFERING |
		IEEE80211_HW_WEP_INCLUDE_IV |
		IEEE80211_HW_DATA_NULLFUNC_ACK |
		IEEE80211_HW_NO_TKIP_WMM_HWACCEL |
		IEEE80211_HW_MONITOR_DURING_OPER;
	rt2x00dev->hw->extra_tx_headroom = TXD_DESC_SIZE;
	rt2x00dev->hw->max_rssi = MAX_RX_SSI;
	rt2x00dev->hw->max_noise = MAX_RX_NOISE;
	rt2x00dev->hw->queues = 5;

	/*
	 * Set device specific, but channel independent RF values.
	 */
	for (i = 0; i < ARRAY_SIZE(rf_vals); i++) {
		if (rt2x00_rf(&rt2x00dev->chip, rf_vals[i].chip)) {
			rt2x00dev->rf1 = rf_vals[i].val[0];
			rt2x00dev->rf3 = rf_vals[i].val[1];
		}
	}

	/*
	 * Convert tx_power array in eeprom.
	 */
	txpower = rt2x00_eeprom_addr(rt2x00dev, EEPROM_TXPOWER_G_START);
	for (i = 0; i < 14; i++)
		txpower[i] = TXPOWER_FROM_DEV(txpower[i]);

	/*
	 * Initialize hw_mode information.
	 */
	spec->mac_addr = rt2x00_eeprom_addr(rt2x00dev, EEPROM_MAC_ADDR_0);
	spec->num_modes = 2;
	spec->num_rates = 12;
	spec->num_channels = 14;
	spec->tx_power_a = NULL;
	spec->tx_power_bg = txpower;
	spec->tx_power_default = DEFAULT_TXPOWER;
	spec->chan_val_a = NULL;
	spec->chan_val_bg = rf_vals_bg;

	if (rt2x00_rf(&rt2x00dev->chip, RF5225) ||
	    rt2x00_rf(&rt2x00dev->chip, RF5226)) {
		spec->num_modes = 3;
		spec->num_channels += 24;

		txpower = rt2x00_eeprom_addr(rt2x00dev, EEPROM_TXPOWER_A_START);
		for (i = 0; i < 14; i++)
			txpower[i] = TXPOWER_FROM_DEV(txpower[i]);

		spec->tx_power_a = txpower;
		spec->chan_val_a = rf_vals_a_5x;
	}
}

static int rt73usb_init_hw(struct rt2x00_dev *rt2x00dev)
{
	int retval;

	/*
	 * Allocate eeprom data.
	 */
	retval = rt73usb_alloc_eeprom(rt2x00dev);
	if (retval)
		return retval;

	retval = rt73usb_init_eeprom(rt2x00dev);
	if (retval)
		return retval;

	/*
	 * Initialize hw specifications.
	 */
	rt73usb_init_hw_mode(rt2x00dev);

	/*
	 * rt73usb requires firmware
	 */
	__set_bit(FIRMWARE_REQUIRED, &rt2x00dev->flags);

	return 0;
}

/*
 * IEEE80211 stack callback functions.
 */
static int rt73usb_get_stats(struct ieee80211_hw *hw,
	struct ieee80211_low_level_stats *stats)
{
	struct rt2x00_dev *rt2x00dev = hw->priv;
	u32 reg;

	/*
	 * Update FCS error count from register.
	 * The dot11ACKFailureCount, dot11RTSFailureCount and
	 * dot11RTSSuccessCount are updated in interrupt time.
	 */
	rt73usb_register_read(rt2x00dev, STA_CSR0, &reg);
	rt2x00dev->low_level_stats.dot11FCSErrorCount +=
		rt2x00_get_field32(reg, STA_CSR0_FCS_ERROR);

	memcpy(stats, &rt2x00dev->low_level_stats, sizeof(*stats));

	return 0;
}

static int rt73usb_set_retry_limit(struct ieee80211_hw *hw,
	u32 short_retry, u32 long_retry)
{
	struct rt2x00_dev *rt2x00dev = hw->priv;
	u32 reg;

	rt73usb_register_read(rt2x00dev, TXRX_CSR4, &reg);
	rt2x00_set_field32(&reg, TXRX_CSR4_LONG_RETRY_LIMIT, long_retry);
	rt2x00_set_field32(&reg, TXRX_CSR4_SHORT_RETRY_LIMIT, short_retry);
	rt73usb_register_write(rt2x00dev, TXRX_CSR4, reg);

	return 0;
}

static u64 rt73usb_get_tsf(struct ieee80211_hw *hw)
{
	struct rt2x00_dev *rt2x00dev = hw->priv;
	u64 tsf;
	u32 reg;

	rt73usb_register_read(rt2x00dev, TXRX_CSR13, &reg);
	tsf = (u64)rt2x00_get_field32(reg, TXRX_CSR13_HIGH_TSFTIMER) << 32;
	rt73usb_register_read(rt2x00dev, TXRX_CSR12, &reg);
	tsf |= rt2x00_get_field32(reg, TXRX_CSR12_LOW_TSFTIMER);

	return tsf;
}

static void rt73usb_reset_tsf(struct ieee80211_hw *hw)
{
	struct rt2x00_dev *rt2x00dev = hw->priv;

	rt73usb_register_write(rt2x00dev, TXRX_CSR12, 0);
	rt73usb_register_write(rt2x00dev, TXRX_CSR13, 0);
}

static const struct ieee80211_ops rt73usb_mac80211_ops = {
	.tx			= rt2x00lib_tx,
	.reset			= rt2x00lib_reset,
	.open			= rt2x00lib_open,
	.stop			= rt2x00lib_stop,
	.add_interface		= rt2x00lib_add_interface,
	.remove_interface	= rt2x00lib_remove_interface,
	.config			= rt2x00lib_config,
	.config_interface	= rt2x00lib_config_interface,
	.set_multicast_list	= rt2x00lib_set_multicast_list,
	.get_stats		= rt73usb_get_stats,
	.set_retry_limit	= rt73usb_set_retry_limit,
	.conf_tx		= rt2x00lib_conf_tx,
	.get_tx_stats		= rt2x00lib_get_tx_stats,
	.get_tsf		= rt73usb_get_tsf,
	.reset_tsf		= rt73usb_reset_tsf,
	.beacon_update		= rt2x00usb_beacon_update,
};

static const struct rt2x00lib_ops rt73usb_rt2x00_ops = {
	.init_hw		= rt73usb_init_hw,
	.get_fw_name		= rt73usb_get_fw_name,
	.load_firmware		= rt73usb_load_firmware,
	.initialize		= rt2x00usb_initialize,
	.uninitialize		= rt2x00usb_uninitialize,
	.set_device_state	= rt73usb_set_device_state,
	.link_tuner		= rt73usb_link_tuner,
	.write_tx_desc		= rt73usb_write_tx_desc,
	.write_tx_data		= rt2x00usb_write_tx_data,
	.kick_tx_queue		= rt73usb_kick_tx_queue,
	.config_type		= rt73usb_config_type,
	.config_phymode		= rt73usb_config_phymode,
	.config_channel		= rt73usb_config_channel,
	.config_mac_addr	= rt73usb_config_mac_addr,
	.config_bssid		= rt73usb_config_bssid,
	.config_promisc		= rt73usb_config_promisc,
	.config_txpower		= rt73usb_config_txpower,
	.config_antenna		= rt73usb_config_antenna,
	.config_duration	= rt73usb_config_duration,
};

static const struct rt2x00_ops rt73usb_ops = {
	.name	= DRV_NAME,
	.rxd_size = RXD_DESC_SIZE,
	.txd_size = TXD_DESC_SIZE,
	.lib	= &rt73usb_rt2x00_ops,
	.hw	= &rt73usb_mac80211_ops,
#ifdef CONFIG_RT2X00_LIB_DEBUGFS
	.debugfs = &rt73usb_rt2x00debug,
#endif /* CONFIG_RT2X00_LIB_DEBUGFS */
};

/*
 * rt73usb module information.
 */
static struct usb_device_id rt73usb_device_table[] = {
	/* AboCom */
	{ USB_DEVICE(0x07b8, 0xb21d), USB_DEVICE_DATA(&rt73usb_ops) },
	/* Askey */
	{ USB_DEVICE(0x1690, 0x0722), USB_DEVICE_DATA(&rt73usb_ops) },
	/* ASUS */
	{ USB_DEVICE(0x0b05, 0x1723), USB_DEVICE_DATA(&rt73usb_ops) },
	{ USB_DEVICE(0x0b05, 0x1724), USB_DEVICE_DATA(&rt73usb_ops) },
	/* Belkin */
	{ USB_DEVICE(0x050d, 0x7050), USB_DEVICE_DATA(&rt73usb_ops) },
	{ USB_DEVICE(0x050d, 0x705a), USB_DEVICE_DATA(&rt73usb_ops) },
	{ USB_DEVICE(0x050d, 0x905b), USB_DEVICE_DATA(&rt73usb_ops) },
	/* Billionton */
	{ USB_DEVICE(0x1631, 0xc019), USB_DEVICE_DATA(&rt73usb_ops) },
	/* CNet */
	{ USB_DEVICE(0x1371, 0x9022), USB_DEVICE_DATA(&rt73usb_ops) },
	{ USB_DEVICE(0x1371, 0x9032), USB_DEVICE_DATA(&rt73usb_ops) },
	/* Conceptronic */
	{ USB_DEVICE(0x14b2, 0x3c22), USB_DEVICE_DATA(&rt73usb_ops) },
	/* D-Link */
	{ USB_DEVICE(0x07d1, 0x3c03), USB_DEVICE_DATA(&rt73usb_ops) },
	{ USB_DEVICE(0x07d1, 0x3c04), USB_DEVICE_DATA(&rt73usb_ops) },
	/* Gemtek */
	{ USB_DEVICE(0x15a9, 0x0004), USB_DEVICE_DATA(&rt73usb_ops) },
	/* Gigabyte */
	{ USB_DEVICE(0x1044, 0x8008), USB_DEVICE_DATA(&rt73usb_ops) },
	{ USB_DEVICE(0x1044, 0x800a), USB_DEVICE_DATA(&rt73usb_ops) },
	/* Huawei-3Com */
	{ USB_DEVICE(0x1472, 0x0009), USB_DEVICE_DATA(&rt73usb_ops) },
	/* Hercules */
	{ USB_DEVICE(0x06f8, 0xe010), USB_DEVICE_DATA(&rt73usb_ops) },
	{ USB_DEVICE(0x06f8, 0xe020), USB_DEVICE_DATA(&rt73usb_ops) },
	/* Linksys */
	{ USB_DEVICE(0x13b1, 0x0020), USB_DEVICE_DATA(&rt73usb_ops) },
	{ USB_DEVICE(0x13b1, 0x0023), USB_DEVICE_DATA(&rt73usb_ops) },
	/* MSI */
	{ USB_DEVICE(0x0db0, 0x6877), USB_DEVICE_DATA(&rt73usb_ops) },
	{ USB_DEVICE(0x0db0, 0xa861), USB_DEVICE_DATA(&rt73usb_ops) },
	{ USB_DEVICE(0x0db0, 0xa874), USB_DEVICE_DATA(&rt73usb_ops) },
	/* Ralink */
	{ USB_DEVICE(0x148f, 0x2573), USB_DEVICE_DATA(&rt73usb_ops) },
	{ USB_DEVICE(0x148f, 0x2671), USB_DEVICE_DATA(&rt73usb_ops) },
	/* Qcom */
	{ USB_DEVICE(0x18e8, 0x6196), USB_DEVICE_DATA(&rt73usb_ops) },
	{ USB_DEVICE(0x18e8, 0x6229), USB_DEVICE_DATA(&rt73usb_ops) },
	/* Sitecom */
	{ USB_DEVICE(0x0df6, 0x9712), USB_DEVICE_DATA(&rt73usb_ops) },
	{ USB_DEVICE(0x0df6, 0x90ac), USB_DEVICE_DATA(&rt73usb_ops) },
	/* Surecom */
	{ USB_DEVICE(0x0769, 0x31f3), USB_DEVICE_DATA(&rt73usb_ops) },
	/* Planex */
	{ USB_DEVICE(0x2019, 0xab01), USB_DEVICE_DATA(&rt73usb_ops) },
	{ USB_DEVICE(0x2019, 0xab50), USB_DEVICE_DATA(&rt73usb_ops) },
	{ 0, }
};

MODULE_AUTHOR(DRV_PROJECT);
MODULE_VERSION(DRV_VERSION);
MODULE_DESCRIPTION("Ralink RT73 USB Wireless LAN driver.");
MODULE_SUPPORTED_DEVICE("Ralink RT2571W & RT2671 USB chipset based cards");
MODULE_DEVICE_TABLE(usb, rt73usb_device_table);
MODULE_FIRMWARE(FIRMWARE_RT2571);
MODULE_LICENSE("GPL");

static struct usb_driver rt73usb_driver = {
	.name		= DRV_NAME,
	.id_table	= rt73usb_device_table,
	.probe		= rt2x00usb_probe,
	.disconnect	= rt2x00usb_disconnect,
#ifdef CONFIG_PM
	.suspend	= rt2x00usb_suspend,
	.resume		= rt2x00usb_resume,
#endif /* CONFIG_PM */
};

static int __init rt73usb_init(void)
{
	printk(KERN_INFO "Loading module: %s - %s by %s.\n",
		DRV_NAME, DRV_VERSION, DRV_PROJECT);
	return usb_register(&rt73usb_driver);
}

static void __exit rt73usb_exit(void)
{
	printk(KERN_INFO "Unloading module: %s.\n", DRV_NAME);
	usb_deregister(&rt73usb_driver);
}

module_init(rt73usb_init);
module_exit(rt73usb_exit);
