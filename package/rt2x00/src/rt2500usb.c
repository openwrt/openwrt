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
	Module: rt2500usb
	Abstract: rt2500usb device specific routines.
	Supported chipsets: RT2570.
 */

/*
 * Set enviroment defines for rt2x00.h
 */
#define DRV_NAME "rt2500usb"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/delay.h>
#include <linux/etherdevice.h>

#include "rt2x00.h"
#include "rt2x00usb.h"
#include "rt2500usb.h"

/*
 * Register access.
 * All access to the CSR registers will go through the methods
 * rt2500usb_register_read and rt2500usb_register_write.
 * BBP and RF register require indirect register access,
 * and use the CSR registers BBPCSR and RFCSR to achieve this.
 * These indirect registers work with busy bits,
 * and we will try maximal REGISTER_BUSY_COUNT times to access
 * the register while taking a REGISTER_BUSY_DELAY us delay
 * between each attampt. When the busy bit is still set at that time,
 * the access attempt is considered to have failed,
 * and we will print an error.
 */
static inline void rt2500usb_register_read(
	const struct rt2x00_dev *rt2x00dev,
	const u16 offset, u16 *value)
{
	__le16 reg;
	rt2x00usb_vendor_request(
		rt2x00dev, USB_MULTI_READ, USB_VENDOR_REQUEST_IN,
		offset, 0x00, &reg, sizeof(u16), REGISTER_TIMEOUT);
	*value = le16_to_cpu(reg);
}

static inline void rt2500usb_register_multiread(
	const struct rt2x00_dev *rt2x00dev,
	const u16 offset, void *value, const u16 length)
{
	rt2x00usb_vendor_request(
		rt2x00dev, USB_MULTI_READ, USB_VENDOR_REQUEST_IN,
		offset, 0x00, value, length,
		REGISTER_TIMEOUT * (length / sizeof(u16)));
}

static inline void rt2500usb_register_write(
	const struct rt2x00_dev *rt2x00dev,
	const u16 offset, u16 value)
{
	__le16 reg = cpu_to_le16(value);
	rt2x00usb_vendor_request(
		rt2x00dev, USB_MULTI_WRITE, USB_VENDOR_REQUEST_OUT,
		offset, 0x00, &reg, sizeof(u16), REGISTER_TIMEOUT);
}

static inline void rt2500usb_register_multiwrite(
	const struct rt2x00_dev *rt2x00dev,
	const u16 offset, void *value, const u16 length)
{
	rt2x00usb_vendor_request(
		rt2x00dev, USB_MULTI_WRITE, USB_VENDOR_REQUEST_OUT,
		offset, 0x00, value, length,
		REGISTER_TIMEOUT * (length / sizeof(u16)));
}

static u16 rt2500usb_bbp_check(const struct rt2x00_dev *rt2x00dev)
{
	u16 reg;
	unsigned int i;

	for (i = 0; i < REGISTER_BUSY_COUNT; i++) {
		rt2500usb_register_read(rt2x00dev, PHY_CSR8, &reg);
		if (!rt2x00_get_field16(reg, PHY_CSR8_BUSY))
			break;
		udelay(REGISTER_BUSY_DELAY);
	}

	return reg;
}

static void rt2500usb_bbp_write(const struct rt2x00_dev *rt2x00dev,
	const u8 reg_id, const u8 value)
{
	u16 reg;

	/*
	 *  Wait until the BBP becomes ready.
	 */
	reg = rt2500usb_bbp_check(rt2x00dev);
	if (rt2x00_get_field16(reg, PHY_CSR8_BUSY)) {
		ERROR(rt2x00dev, "PHY_CSR8 register busy. Write failed.\n");
		return;
	}

	/*
	 * Write the data into the BBP.
	 */
	reg = 0;
	rt2x00_set_field16(&reg, PHY_CSR7_DATA, value);
	rt2x00_set_field16(&reg, PHY_CSR7_REG_ID, reg_id);
	rt2x00_set_field16(&reg, PHY_CSR7_READ_CONTROL, 0);

	rt2500usb_register_write(rt2x00dev, PHY_CSR7, reg);
}

static void rt2500usb_bbp_read(const struct rt2x00_dev *rt2x00dev,
	const u8 reg_id, u8 *value)
{
	u16 reg;

	/*
	 *  Wait until the BBP becomes ready.
	 */
	reg = rt2500usb_bbp_check(rt2x00dev);
	if (rt2x00_get_field16(reg, PHY_CSR8_BUSY)) {
		ERROR(rt2x00dev, "PHY_CSR8 register busy. Read failed.\n");
		return;
	}

	/*
	 * Write the request into the BBP.
	 */
	reg =0;
	rt2x00_set_field16(&reg, PHY_CSR7_REG_ID, reg_id);
	rt2x00_set_field16(&reg, PHY_CSR7_READ_CONTROL, 1);

	rt2500usb_register_write(rt2x00dev, PHY_CSR7, reg);

	/*
	 *  Wait until the BBP becomes ready.
	 */
	reg = rt2500usb_bbp_check(rt2x00dev);
	if (rt2x00_get_field16(reg, PHY_CSR8_BUSY)) {
		ERROR(rt2x00dev, "PHY_CSR8 register busy. Read failed.\n");
		*value = 0xff;
		return;
	}

	rt2500usb_register_read(rt2x00dev, PHY_CSR7, &reg);
	*value = rt2x00_get_field16(reg, PHY_CSR7_DATA);
}

static void rt2500usb_rf_write(const struct rt2x00_dev *rt2x00dev,
	const u32 value)
{
	u16 reg;
	unsigned int i;

	for (i = 0; i < REGISTER_BUSY_COUNT; i++) {
		rt2500usb_register_read(rt2x00dev, PHY_CSR10, &reg);
		if (!rt2x00_get_field16(reg, PHY_CSR10_RF_BUSY))
			goto rf_write;
		udelay(REGISTER_BUSY_DELAY);
	}

	ERROR(rt2x00dev, "PHY_CSR10 register busy. Write failed.\n");
	return;

rf_write:
	reg = 0;
	rt2x00_set_field16(&reg, PHY_CSR9_RF_VALUE, value);
	rt2500usb_register_write(rt2x00dev, PHY_CSR9, reg);

	reg = 0;
	rt2x00_set_field16(&reg, PHY_CSR10_RF_VALUE, value >> 16);
	rt2x00_set_field16(&reg, PHY_CSR10_RF_NUMBER_OF_BITS, 20);
	rt2x00_set_field16(&reg, PHY_CSR10_RF_IF_SELECT, 0);
	rt2x00_set_field16(&reg, PHY_CSR10_RF_BUSY, 1);

	rt2500usb_register_write(rt2x00dev, PHY_CSR10, reg);
}

#ifdef CONFIG_RT2X00_LIB_DEBUGFS
#define CSR_OFFSET(__word)	( CSR_REG_BASE + ((__word) * sizeof(u16)) )

static void rt2500usb_read_csr(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt2500usb_register_read(rt2x00dev, CSR_OFFSET(word), data);
}

static void rt2500usb_write_csr(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt2500usb_register_write(rt2x00dev, CSR_OFFSET(word), *((u16*)data));
}

static void rt2500usb_read_eeprom(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt2x00_eeprom_read(rt2x00dev, word, data);
}

static void rt2500usb_write_eeprom(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt2x00_eeprom_write(rt2x00dev, word, *((u16*)data));
}

static void rt2500usb_read_bbp(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt2500usb_bbp_read(rt2x00dev, word, data);
}

static void rt2500usb_write_bbp(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt2500usb_bbp_write(rt2x00dev, word, *((u8*)data));
}

static const struct rt2x00debug rt2500usb_rt2x00debug = {
	.owner 		= THIS_MODULE,
	.reg_csr	= {
		.read		= rt2500usb_read_csr,
		.write		= rt2500usb_write_csr,
		.word_size	= sizeof(u16),
		.word_count	= CSR_REG_SIZE / sizeof(u16),
	},
	.reg_eeprom	= {
		.read		= rt2500usb_read_eeprom,
		.write		= rt2500usb_write_eeprom,
		.word_size	= sizeof(u16),
		.word_count	= EEPROM_SIZE / sizeof(u16),
	},
	.reg_bbp	= {
		.read		= rt2500usb_read_bbp,
		.write		= rt2500usb_write_bbp,
		.word_size	= sizeof(u8),
		.word_count	= BBP_SIZE / sizeof(u8),
	},
};
#endif /* CONFIG_RT2X00_LIB_DEBUGFS */

/*
 * Configuration handlers.
 */
static void rt2500usb_config_bssid(struct rt2x00_dev *rt2x00dev, u8 *bssid)
{
	u16 reg[3];

	memset(&reg, 0, sizeof(reg));
	memcpy(&reg, bssid, ETH_ALEN);

	/*
	 * The BSSID is passed to us as an array of bytes,
	 * that array is little endian, so no need for byte ordering.
	 */
	rt2500usb_register_multiwrite(rt2x00dev, MAC_CSR5, &reg, sizeof(reg));
}

static void rt2500usb_config_promisc(struct rt2x00_dev *rt2x00dev,
	const int promisc)
{
	u16 reg;

	rt2500usb_register_read(rt2x00dev, TXRX_CSR2, &reg);
	rt2x00_set_field16(&reg, TXRX_CSR2_DROP_NOT_TO_ME, !promisc);
	rt2500usb_register_write(rt2x00dev, TXRX_CSR2, reg);
}

static void rt2500usb_config_type(struct rt2x00_dev *rt2x00dev,
	const int type)
{
	u16 reg;

	rt2500usb_register_write(rt2x00dev, TXRX_CSR19, 0);

	/*
	 * Apply hardware packet filter.
	 */
	rt2500usb_register_read(rt2x00dev, TXRX_CSR2, &reg);

	if (!is_monitor_present(&rt2x00dev->interface) &&
	    (type == IEEE80211_IF_TYPE_IBSS || type == IEEE80211_IF_TYPE_STA))
		rt2x00_set_field16(&reg, TXRX_CSR2_DROP_TODS, 1);
	else
		rt2x00_set_field16(&reg, TXRX_CSR2_DROP_TODS, 0);

	rt2x00_set_field16(&reg, TXRX_CSR2_DROP_CRC, 1);
	if (is_monitor_present(&rt2x00dev->interface)) {
		rt2x00_set_field16(&reg, TXRX_CSR2_DROP_PHYSICAL, 0);
		rt2x00_set_field16(&reg, TXRX_CSR2_DROP_CONTROL, 0);
		rt2x00_set_field16(&reg, TXRX_CSR2_DROP_VERSION_ERROR, 0);
	} else {
		rt2x00_set_field16(&reg, TXRX_CSR2_DROP_PHYSICAL, 1);
		rt2x00_set_field16(&reg, TXRX_CSR2_DROP_CONTROL, 1);
		rt2x00_set_field16(&reg, TXRX_CSR2_DROP_VERSION_ERROR, 1);
	}

	rt2500usb_register_write(rt2x00dev, TXRX_CSR2, reg);

	/*
	 * Enable beacon config
	 */
	rt2500usb_register_read(rt2x00dev, TXRX_CSR20, &reg);
	rt2x00_set_field16(&reg, TXRX_CSR20_OFFSET,
		(PREAMBLE + get_duration(IEEE80211_HEADER, 2)) >> 6);
	if (type == IEEE80211_IF_TYPE_STA)
		rt2x00_set_field16(&reg, TXRX_CSR20_BCN_EXPECT_WINDOW, 0);
	else
		rt2x00_set_field16(&reg, TXRX_CSR20_BCN_EXPECT_WINDOW, 2);
	rt2500usb_register_write(rt2x00dev, TXRX_CSR20, reg);

	/*
	 * Enable synchronisation.
	 */
	rt2500usb_register_read(rt2x00dev, TXRX_CSR18, &reg);
	rt2x00_set_field16(&reg, TXRX_CSR18_OFFSET, 0);
	rt2500usb_register_write(rt2x00dev, TXRX_CSR18, reg);

	rt2500usb_register_read(rt2x00dev, TXRX_CSR19, &reg);
	if (is_interface_present(&rt2x00dev->interface)) {
		rt2x00_set_field16(&reg, TXRX_CSR19_TSF_COUNT, 1);
		rt2x00_set_field16(&reg, TXRX_CSR19_TBCN, 1);
	}

	rt2x00_set_field16(&reg, TXRX_CSR19_BEACON_GEN, 0);
	if (type == IEEE80211_IF_TYPE_IBSS || type == IEEE80211_IF_TYPE_AP)
		rt2x00_set_field16(&reg, TXRX_CSR19_TSF_SYNC, 2);
	else if (type == IEEE80211_IF_TYPE_STA)
		rt2x00_set_field16(&reg, TXRX_CSR19_TSF_SYNC, 1);
	else if (is_monitor_present(&rt2x00dev->interface) &&
		 !is_interface_present(&rt2x00dev->interface))
		rt2x00_set_field16(&reg, TXRX_CSR19_TSF_SYNC, 0);

	rt2500usb_register_write(rt2x00dev, TXRX_CSR19, reg);
}

static void rt2500usb_config_channel(struct rt2x00_dev *rt2x00dev,
	const int value, const int channel, const int txpower)
{
	u32 rf1 = rt2x00dev->rf1;
	u32 rf2 = value;
	u32 rf3 = rt2x00dev->rf3;
	u32 rf4 = rt2x00dev->rf4;

	if (rt2x00_rf(&rt2x00dev->chip, RF2525))
		rf2 |= 0x00080000;

	if ((rt2x00_rf(&rt2x00dev->chip, RF2523) ||
	     rt2x00_rf(&rt2x00dev->chip, RF2524) ||
	     rt2x00_rf(&rt2x00dev->chip, RF2525)) &&
	     channel == 14)
		rf4 &= ~0x00000018;

	if (rt2x00_rf(&rt2x00dev->chip, RF2525E)) {
		if (channel & 0x01)
			rf4 = 0x00000e1b;
		else
			rf4 = 0x00000e07;
		if (channel == 14)
			rf4 = 0x00000e23;
	}

	if (rt2x00_rf(&rt2x00dev->chip, RF5222)) {
		if (channel < 14) {
			rf1 = 0x00022020;
			rf4 = 0x00000a0b;
		} else if (channel == 14) {
			rf1 = 0x00022010;
			rf4 = 0x00000a1b;
		} else if (channel < 64) {
			rf1 = 0x00022010;
			rf4 = 0x00000a1f;
		} else if (channel < 140) {
			rf1 = 0x00022010;
			rf4 = 0x00000a0f;
		} else if (channel < 161) {
			rf1 = 0x00022020;
			rf4 = 0x00000a07;
		}
	}

	/*
	 * Set TXpower.
	 */
	rt2x00_set_field32(&rf3, RF3_TXPOWER, TXPOWER_TO_DEV(txpower));

	/*
	 * For RT2525E we should first set the channel to half band higher.
	 */
	if (rt2x00_rf(&rt2x00dev->chip, RF2525E)) {
		static const u32 vals[] = {
			0x000008aa, 0x000008ae, 0x000008ae, 0x000008b2,
			0x000008b2, 0x000008b6, 0x000008b6, 0x000008ba,
			0x000008ba, 0x000008be, 0x000008b7, 0x00000902,
			0x00000902, 0x00000906
		};

		rt2500usb_rf_write(rt2x00dev, vals[channel - 1]);
		if (rf4)
			rt2500usb_rf_write(rt2x00dev, rf4);
	}

	rt2500usb_rf_write(rt2x00dev, rf1);
	rt2500usb_rf_write(rt2x00dev, rf2);
	rt2500usb_rf_write(rt2x00dev, rf3);
	if (rf4)
		rt2500usb_rf_write(rt2x00dev, rf4);

	/*
	 * Update rf fields
	 */
	rt2x00dev->rf1 = rf1;
	rt2x00dev->rf2 = rf2;
	rt2x00dev->rf3 = rf3;
	rt2x00dev->rf4 = rf4;
	rt2x00dev->tx_power = txpower;
}

static void rt2500usb_config_txpower(struct rt2x00_dev *rt2x00dev,
	const int txpower)
{
	rt2x00_set_field32(&rt2x00dev->rf3, RF3_TXPOWER,
		TXPOWER_TO_DEV(txpower));
	rt2500usb_rf_write(rt2x00dev, rt2x00dev->rf3);
}

static void rt2500usb_config_antenna(struct rt2x00_dev *rt2x00dev,
	const int antenna_tx, const int antenna_rx)
{
	u8 r2;
	u8 r14;
	u16 csr5;
	u16 csr6;

	rt2500usb_bbp_read(rt2x00dev, 2, &r2);
	rt2500usb_bbp_read(rt2x00dev, 14, &r14);
	rt2500usb_register_read(rt2x00dev, PHY_CSR5, &csr5);
	rt2500usb_register_read(rt2x00dev, PHY_CSR6, &csr6);

	/*
	 * Configure the TX antenna.
	 */
	if (antenna_tx == ANTENNA_DIVERSITY) {
		rt2x00_set_field8(&r2, BBP_R2_TX_ANTENNA, 1);
		rt2x00_set_field16(&csr5, PHY_CSR5_CCK, 1);
		rt2x00_set_field16(&csr6, PHY_CSR6_OFDM, 1);
	} else if (antenna_tx == ANTENNA_A) {
		rt2x00_set_field8(&r2, BBP_R2_TX_ANTENNA, 0);
		rt2x00_set_field16(&csr5, PHY_CSR5_CCK, 0);
		rt2x00_set_field16(&csr6, PHY_CSR6_OFDM, 0);
	} else if (antenna_tx == ANTENNA_B) {
		rt2x00_set_field8(&r2, BBP_R2_TX_ANTENNA, 2);
		rt2x00_set_field16(&csr5, PHY_CSR5_CCK, 2);
		rt2x00_set_field16(&csr6, PHY_CSR6_OFDM, 2);
	}

	/*
	 * Configure the RX antenna.
	 */
	if (antenna_rx == ANTENNA_DIVERSITY)
		rt2x00_set_field8(&r14, BBP_R14_RX_ANTENNA, 1);
	else if (antenna_rx == ANTENNA_A)
		rt2x00_set_field8(&r14, BBP_R14_RX_ANTENNA, 0);
	else if (antenna_rx == ANTENNA_B)
		rt2x00_set_field8(&r14, BBP_R14_RX_ANTENNA, 2);

	/*
	 * RT2525E and RT5222 need to flip TX I/Q
	 */
	if (rt2x00_rf(&rt2x00dev->chip, RF2525E) ||
	    rt2x00_rf(&rt2x00dev->chip, RF5222)) {
		rt2x00_set_field8(&r2, BBP_R2_TX_IQ_FLIP, 1);
		rt2x00_set_field16(&csr5, PHY_CSR5_CCK_FLIP, 1);
		rt2x00_set_field16(&csr6, PHY_CSR6_OFDM_FLIP, 1);

		/*
		 * RT2525E does not need RX I/Q Flip.
		 */
		if (rt2x00_rf(&rt2x00dev->chip, RF2525E))
			rt2x00_set_field8(&r14, BBP_R14_RX_IQ_FLIP, 0);
	} else {
		rt2x00_set_field16(&csr5, PHY_CSR5_CCK_FLIP, 0);
		rt2x00_set_field16(&csr6, PHY_CSR6_OFDM_FLIP, 0);
	}

	rt2500usb_bbp_write(rt2x00dev, 2, r2);
	rt2500usb_bbp_write(rt2x00dev, 14, r14);
	rt2500usb_register_write(rt2x00dev, PHY_CSR5, csr5);
	rt2500usb_register_write(rt2x00dev, PHY_CSR6, csr6);
}

static void rt2500usb_config_duration(struct rt2x00_dev *rt2x00dev,
	const int short_slot_time, const int beacon_int)
{
	u16 reg;

	rt2500usb_register_write(rt2x00dev, MAC_CSR10,
		short_slot_time ? SHORT_SLOT_TIME : SLOT_TIME);

	rt2500usb_register_read(rt2x00dev, TXRX_CSR18, &reg);
	rt2x00_set_field16(&reg, TXRX_CSR18_INTERVAL, beacon_int * 4);
	rt2500usb_register_write(rt2x00dev, TXRX_CSR18, reg);
}

static void rt2500usb_config_rate(struct rt2x00_dev *rt2x00dev, const int rate)
{
	struct ieee80211_conf *conf = &rt2x00dev->hw->conf;
	u16 reg;
	u16 value;
	u16 preamble;

	preamble = DEVICE_GET_RATE_FIELD(rate, PREAMBLE)
		? SHORT_PREAMBLE : PREAMBLE;

	reg = DEVICE_GET_RATE_FIELD(rate, RATEMASK) & DEV_BASIC_RATE;

	rt2500usb_register_write(rt2x00dev, TXRX_CSR11, reg);

	rt2500usb_register_read(rt2x00dev, TXRX_CSR1, &reg);
	value = ((conf->flags & IEEE80211_CONF_SHORT_SLOT_TIME) ?
		 SHORT_DIFS :  DIFS) +
		PLCP + preamble + get_duration(ACK_SIZE, 10);
	rt2x00_set_field16(&reg, TXRX_CSR1_ACK_TIMEOUT, value);
	rt2500usb_register_write(rt2x00dev, TXRX_CSR1, reg);

	rt2500usb_register_read(rt2x00dev, TXRX_CSR10, &reg);
	if (preamble == SHORT_PREAMBLE)
		rt2x00_set_field16(&reg, TXRX_CSR10_AUTORESPOND_PREAMBLE, 1);
	else
		rt2x00_set_field16(&reg, TXRX_CSR10_AUTORESPOND_PREAMBLE, 0);
	rt2500usb_register_write(rt2x00dev, TXRX_CSR10, reg);
}

static void rt2500usb_config_phymode(struct rt2x00_dev *rt2x00dev,
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

	rt2500usb_config_rate(rt2x00dev, rate->val2);

	if (phymode == MODE_IEEE80211B) {
		rt2500usb_register_write(rt2x00dev, MAC_CSR11, 0x000b);
		rt2500usb_register_write(rt2x00dev, MAC_CSR12, 0x0040);
	} else {
		rt2500usb_register_write(rt2x00dev, MAC_CSR11, 0x0005);
		rt2500usb_register_write(rt2x00dev, MAC_CSR12, 0x016c);
	}
}

static void rt2500usb_config_mac_addr(struct rt2x00_dev *rt2x00dev, u8 *addr)
{
	u16 reg[3];

	memset(&reg, 0, sizeof(reg));
	memcpy(&reg, addr, ETH_ALEN);

	/*
	 * The MAC address is passed to us as an array of bytes,
	 * that array is little endian, so no need for byte ordering.
	 */
	rt2500usb_register_multiwrite(rt2x00dev, MAC_CSR2, &reg, sizeof(reg));
}

/*
 * LED functions.
 */
static void rt2500usb_enable_led(struct rt2x00_dev *rt2x00dev)
{
	u16 reg;

	rt2500usb_register_read(rt2x00dev, MAC_CSR21, &reg);
	rt2x00_set_field16(&reg, MAC_CSR21_ON_PERIOD, 70);
	rt2x00_set_field16(&reg, MAC_CSR21_OFF_PERIOD, 30);
	rt2500usb_register_write(rt2x00dev, MAC_CSR21, reg);

	rt2500usb_register_read(rt2x00dev, MAC_CSR20, &reg);

	if (rt2x00dev->led_mode == LED_MODE_TXRX_ACTIVITY) {
		rt2x00_set_field16(&reg, MAC_CSR20_LINK, 1);
		rt2x00_set_field16(&reg, MAC_CSR20_ACTIVITY, 0);
	} else if (rt2x00dev->led_mode == LED_MODE_ASUS) {
		rt2x00_set_field16(&reg, MAC_CSR20_LINK, 0);
		rt2x00_set_field16(&reg, MAC_CSR20_ACTIVITY, 1);
	} else {
		rt2x00_set_field16(&reg, MAC_CSR20_LINK, 1);
		rt2x00_set_field16(&reg, MAC_CSR20_ACTIVITY, 1);
	}

	rt2500usb_register_write(rt2x00dev, MAC_CSR20, reg);
}

static void rt2500usb_disable_led(struct rt2x00_dev *rt2x00dev)
{
	u16 reg;

	rt2500usb_register_read(rt2x00dev, MAC_CSR20, &reg);
	rt2x00_set_field16(&reg, MAC_CSR20_LINK, 0);
	rt2x00_set_field16(&reg, MAC_CSR20_ACTIVITY, 0);
	rt2500usb_register_write(rt2x00dev, MAC_CSR20, reg);
}

/*
 * Link tuning
 */
static void rt2500usb_link_tuner(struct rt2x00_dev *rt2x00dev, int rssi)
{
	u16 bbp_thresh;
	u16 cca_alarm;
	u16 vgc_bound;
	u16 sens;
	u16 r24;
	u16 r25;
	u16 r61;
	u16 r17_sens;
	u8 r17;
	u8 up_bound;
	u8 low_bound;

	/*
	 * Determine the BBP tuning threshold and correctly
	 * set BBP 24, 25 and 61.
	 */
	rt2x00_eeprom_read(rt2x00dev, EEPROM_BBPTUNE, &bbp_thresh);
	bbp_thresh = rt2x00_get_field16(bbp_thresh, EEPROM_BBPTUNE_THRESHOLD);

	rt2x00_eeprom_read(rt2x00dev, EEPROM_BBPTUNE_R24, &r24);
	rt2x00_eeprom_read(rt2x00dev, EEPROM_BBPTUNE_R25, &r25);
	rt2x00_eeprom_read(rt2x00dev, EEPROM_BBPTUNE_R61, &r61);

	if ((rssi + bbp_thresh) > 0) {
		r24 = rt2x00_get_field16(r24, EEPROM_BBPTUNE_R24_HIGH);
		r25 = rt2x00_get_field16(r25, EEPROM_BBPTUNE_R25_HIGH);
		r61 = rt2x00_get_field16(r61, EEPROM_BBPTUNE_R61_HIGH);
	} else {
		r24 = rt2x00_get_field16(r24, EEPROM_BBPTUNE_R24_LOW);
		r25 = rt2x00_get_field16(r25, EEPROM_BBPTUNE_R25_LOW);
		r61 = rt2x00_get_field16(r61, EEPROM_BBPTUNE_R61_LOW);
	}

	rt2500usb_bbp_write(rt2x00dev, 24, r24);
	rt2500usb_bbp_write(rt2x00dev, 25, r25);
	rt2500usb_bbp_write(rt2x00dev, 61, r61);

	/*
	 * Read current r17 value, as well as the sensitivity values
	 * for the r17 register.
	 */
	rt2500usb_bbp_read(rt2x00dev, 17, &r17);
	rt2x00_eeprom_read(rt2x00dev, EEPROM_BBPTUNE_R17, &r17_sens);

	/*
	 * A too low RSSI will cause too much false CCA which will
	 * then corrupt the R17 tuning. To remidy this the tuning should
	 * be stopped (While making sure the R17 value will not exceed limits)
	 */
	if (rssi >= -40) {
		if (r17 != 0x60)
			rt2500usb_bbp_write(rt2x00dev, 17, 0x60);
		return;
	}

	/*
	 * Special big-R17 for short distance
	 */
	if (rssi >= -58) {
		sens = rt2x00_get_field16(r17_sens, EEPROM_BBPTUNE_R17_LOW);
		if (r17 != sens)
			rt2500usb_bbp_write(rt2x00dev, 17, sens);
		return;
	}

	/*
	 * Special mid-R17 for middle distance
	 */
	if (rssi >= -74) {
		sens = rt2x00_get_field16(r17_sens, EEPROM_BBPTUNE_R17_HIGH);
		if (r17 != sens)
			rt2500usb_bbp_write(rt2x00dev, 17, sens);
		return;
	}

	/*
	 * Leave short or middle distance condition, restore r17
	 * to the dynamic tuning range.
	 */
	rt2500usb_register_read(rt2x00dev, STA_CSR3, &cca_alarm);
	rt2x00_eeprom_read(rt2x00dev, EEPROM_BBPTUNE_VGC, &vgc_bound);
	vgc_bound = rt2x00_get_field16(vgc_bound, EEPROM_BBPTUNE_VGCUPPER);

	low_bound = 0x32;
	if (rssi >= -77)
		up_bound = vgc_bound;
	else
		up_bound = vgc_bound - (-77 - rssi);

	if (up_bound < low_bound)
		up_bound = low_bound;

	if (r17 > up_bound) {
		rt2500usb_bbp_write(rt2x00dev, 17, up_bound);
		rt2x00dev->link.curr_noise = up_bound;
	} else if (cca_alarm > 512 && r17 < up_bound) {
		rt2500usb_bbp_write(rt2x00dev, 17, ++r17);
		rt2x00dev->link.curr_noise = r17;
	} else if (cca_alarm < 100 && r17 > low_bound) {
		rt2500usb_bbp_write(rt2x00dev, 17, --r17);
		rt2x00dev->link.curr_noise = r17;
	}
}

/*
 * Initialization functions.
 */
static void rt2500usb_init_rxring(struct rt2x00_dev *rt2x00dev)
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
			rt2500usb_interrupt_rxdone,
			&rt2x00dev->rx->entry[i]);
	}

	rt2x00_ring_index_clear(rt2x00dev->rx);
}

static void rt2500usb_init_txring(struct rt2x00_dev *rt2x00dev,
	const int queue)
{
	struct data_ring *ring = rt2x00_get_ring(rt2x00dev, queue);
	unsigned int i;

	for (i = 0; i < ring->stats.limit; i++)
		ring->entry[i].flags = 0;

	rt2x00_ring_index_clear(ring);
}

static int rt2500usb_init_rings(struct rt2x00_dev *rt2x00dev)
{
	rt2500usb_init_rxring(rt2x00dev);
	rt2500usb_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_DATA0);
	rt2500usb_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_DATA1);
	rt2500usb_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_AFTER_BEACON);
	rt2500usb_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_BEACON);

	return 0;
}

static int rt2500usb_init_registers(struct rt2x00_dev *rt2x00dev)
{
	u16 reg;

	rt2x00usb_vendor_request(rt2x00dev, USB_DEVICE_MODE,
		USB_VENDOR_REQUEST_OUT, 0x0001, USB_MODE_TEST, NULL, 0,
		REGISTER_TIMEOUT);
	rt2x00usb_vendor_request(rt2x00dev, USB_SINGLE_WRITE,
		USB_VENDOR_REQUEST_OUT, 0x0308, 0xf0, NULL, 0,
		REGISTER_TIMEOUT);

	rt2500usb_register_write(rt2x00dev, TXRX_CSR2, 0x0001);
	rt2500usb_register_write(rt2x00dev, MAC_CSR13, 0x1111);
	rt2500usb_register_write(rt2x00dev, MAC_CSR14, 0x1e11);

	rt2500usb_register_write(rt2x00dev, MAC_CSR1, 0x0003);
	rt2500usb_register_write(rt2x00dev, MAC_CSR1, 0x0000);
	rt2500usb_register_write(rt2x00dev, TXRX_CSR5, 0x8c8d);
	rt2500usb_register_write(rt2x00dev, TXRX_CSR6, 0x8b8a);
	rt2500usb_register_write(rt2x00dev, TXRX_CSR7, 0x8687);
	rt2500usb_register_write(rt2x00dev, TXRX_CSR8, 0x0085);
	rt2500usb_register_write(rt2x00dev, TXRX_CSR21, 0xe78f);
	rt2500usb_register_write(rt2x00dev, MAC_CSR9, 0xff1d);

	if (rt2x00dev->ops->lib->set_device_state(rt2x00dev, STATE_AWAKE))
		return -EBUSY;

	rt2500usb_register_write(rt2x00dev, MAC_CSR1, 0x0004);

	reg = 0;
	rt2500usb_register_read(rt2x00dev, MAC_CSR0, &reg);
	if (reg >= 0x0003) {
		rt2500usb_register_read(rt2x00dev, PHY_CSR2, &reg);
		reg &= ~0x0002;
	} else {
		reg = 0x3002;
	}
	rt2500usb_register_write(rt2x00dev, PHY_CSR2, reg);

	rt2500usb_register_write(rt2x00dev, MAC_CSR11, 0x0002);
	rt2500usb_register_write(rt2x00dev, MAC_CSR22, 0x0053);
	rt2500usb_register_write(rt2x00dev, MAC_CSR15, 0x01ee);
	rt2500usb_register_write(rt2x00dev, MAC_CSR16, 0x0000);

	rt2500usb_register_read(rt2x00dev, TXRX_CSR0, &reg);
	rt2x00_set_field16(&reg, TXRX_CSR0_IV_OFFSET, IEEE80211_HEADER);
	rt2x00_set_field16(&reg, TXRX_CSR0_KEY_ID, 0xff);
	rt2500usb_register_write(rt2x00dev, TXRX_CSR0, reg);

	rt2500usb_register_read(rt2x00dev, MAC_CSR8, &reg);
	rt2x00_set_field16(&reg, MAC_CSR8_MAX_FRAME_UNIT,
		rt2x00dev->rx->data_size);
	rt2500usb_register_write(rt2x00dev, MAC_CSR8, reg);

	rt2500usb_register_read(rt2x00dev, MAC_CSR18, &reg);
	rt2x00_set_field16(&reg, MAC_CSR18_DELAY_AFTER_BEACON, 0x5a);
	rt2500usb_register_write(rt2x00dev, MAC_CSR18, reg);

	rt2500usb_register_read(rt2x00dev, TXRX_CSR1, &reg);
	rt2x00_set_field16(&reg, TXRX_CSR1_AUTO_SEQUENCE, 1);
	rt2500usb_register_write(rt2x00dev, TXRX_CSR1, reg);

	rt2500usb_register_read(rt2x00dev, PHY_CSR4, &reg);
	rt2500usb_register_write(rt2x00dev, PHY_CSR4, reg | 0x0001);

	return 0;
}

static int rt2500usb_init_bbp(struct rt2x00_dev *rt2x00dev)
{
	unsigned int i;
	u16 eeprom;
	u8 value;
	u8 reg_id;

	for (i = 0; i < REGISTER_BUSY_COUNT; i++) {
		rt2500usb_bbp_read(rt2x00dev, 0, &value);
		if ((value != 0xff) && (value != 0x00))
			goto continue_csr_init;
		NOTICE(rt2x00dev, "Waiting for BBP register.\n");
		udelay(REGISTER_BUSY_DELAY);
	}

	ERROR(rt2x00dev, "BBP register access failed, aborting.\n");
	return -EACCES;

continue_csr_init:
	rt2500usb_bbp_write(rt2x00dev, 3, 0x02);
	rt2500usb_bbp_write(rt2x00dev, 4, 0x19);
	rt2500usb_bbp_write(rt2x00dev, 14, 0x1c);
	rt2500usb_bbp_write(rt2x00dev, 15, 0x30);
	rt2500usb_bbp_write(rt2x00dev, 16, 0xac);
	rt2500usb_bbp_write(rt2x00dev, 17, 0x48);
	rt2500usb_bbp_write(rt2x00dev, 18, 0x18);
	rt2500usb_bbp_write(rt2x00dev, 19, 0xff);
	rt2500usb_bbp_write(rt2x00dev, 20, 0x1e);
	rt2500usb_bbp_write(rt2x00dev, 21, 0x08);
	rt2500usb_bbp_write(rt2x00dev, 22, 0x08);
	rt2500usb_bbp_write(rt2x00dev, 23, 0x08);
	rt2500usb_bbp_write(rt2x00dev, 24, 0x80);
	rt2500usb_bbp_write(rt2x00dev, 25, 0x50);
	rt2500usb_bbp_write(rt2x00dev, 26, 0x08);
	rt2500usb_bbp_write(rt2x00dev, 27, 0x23);
	rt2500usb_bbp_write(rt2x00dev, 30, 0x10);
	rt2500usb_bbp_write(rt2x00dev, 31, 0x2b);
	rt2500usb_bbp_write(rt2x00dev, 32, 0xb9);
	rt2500usb_bbp_write(rt2x00dev, 34, 0x12);
	rt2500usb_bbp_write(rt2x00dev, 35, 0x50);
	rt2500usb_bbp_write(rt2x00dev, 39, 0xc4);
	rt2500usb_bbp_write(rt2x00dev, 40, 0x02);
	rt2500usb_bbp_write(rt2x00dev, 41, 0x60);
	rt2500usb_bbp_write(rt2x00dev, 53, 0x10);
	rt2500usb_bbp_write(rt2x00dev, 54, 0x18);
	rt2500usb_bbp_write(rt2x00dev, 56, 0x08);
	rt2500usb_bbp_write(rt2x00dev, 57, 0x10);
	rt2500usb_bbp_write(rt2x00dev, 58, 0x08);
	rt2500usb_bbp_write(rt2x00dev, 61, 0x60);
	rt2500usb_bbp_write(rt2x00dev, 62, 0x10);
	rt2500usb_bbp_write(rt2x00dev, 75, 0xff);

	DEBUG(rt2x00dev, "Start initialization from EEPROM...\n");
	for (i = 0; i < EEPROM_BBP_SIZE; i++) {
		rt2x00_eeprom_read(rt2x00dev, EEPROM_BBP_START + i, &eeprom);

		if (eeprom != 0xffff && eeprom != 0x0000) {
			reg_id = rt2x00_get_field16(eeprom, EEPROM_BBP_REG_ID);
			value = rt2x00_get_field16(eeprom, EEPROM_BBP_VALUE);
			DEBUG(rt2x00dev, "BBP: 0x%02x, value: 0x%02x.\n",
				reg_id, value);
			rt2500usb_bbp_write(rt2x00dev, reg_id, value);
		}
	}
	DEBUG(rt2x00dev, "...End initialization from EEPROM.\n");

	rt2x00_eeprom_read(rt2x00dev, EEPROM_BBPTUNE_R24, &eeprom);
	value = rt2x00_get_field16(eeprom, EEPROM_BBPTUNE_R24_LOW);
	rt2500usb_bbp_write(rt2x00dev, 24, value);

	rt2x00_eeprom_read(rt2x00dev, EEPROM_BBPTUNE_R25, &eeprom);
	value = rt2x00_get_field16(eeprom, EEPROM_BBPTUNE_R25_LOW);
	rt2500usb_bbp_write(rt2x00dev, 25, value);

	rt2x00_eeprom_read(rt2x00dev, EEPROM_BBPTUNE_R61, &eeprom);
	value = rt2x00_get_field16(eeprom, EEPROM_BBPTUNE_R61_LOW);
	rt2500usb_bbp_write(rt2x00dev, 61, value);

	rt2x00_eeprom_read(rt2x00dev, EEPROM_BBPTUNE_VGC, &eeprom);
	value = rt2x00_get_field16(eeprom, EEPROM_BBPTUNE_VGCUPPER);
	rt2500usb_bbp_write(rt2x00dev, 17, value);

	return 0;
}

/*
 * Device state switch handlers.
 */
static void rt2500usb_toggle_rx(struct rt2x00_dev *rt2x00dev,
	enum dev_state state)
{
	u16 reg;

	rt2500usb_register_read(rt2x00dev, TXRX_CSR2, &reg);
	rt2x00_set_field16(&reg, TXRX_CSR2_DISABLE_RX,
		state == STATE_RADIO_RX_OFF);
	rt2500usb_register_write(rt2x00dev, TXRX_CSR2, reg);
}

static int rt2500usb_enable_radio(struct rt2x00_dev *rt2x00dev)
{
	/*
	 * Initialize all registers.
	 */
	if (rt2500usb_init_rings(rt2x00dev) ||
	    rt2500usb_init_registers(rt2x00dev) ||
	    rt2500usb_init_bbp(rt2x00dev)) {
		ERROR(rt2x00dev, "Register initialization failed.\n");
		return -EIO;
	}

	rt2x00usb_enable_radio(rt2x00dev);

	/*
	 * Enable LED
	 */
	rt2500usb_enable_led(rt2x00dev);

	return 0;
}

static void rt2500usb_disable_radio(struct rt2x00_dev *rt2x00dev)
{
	/*
	 * Disable LED
	 */
	rt2500usb_disable_led(rt2x00dev);

	rt2500usb_register_write(rt2x00dev, MAC_CSR13, 0x2121);
	rt2500usb_register_write(rt2x00dev, MAC_CSR14, 0x2121);

	/*
	 * Disable synchronisation.
	 */
	rt2500usb_register_write(rt2x00dev, TXRX_CSR19, 0);

	rt2x00usb_disable_radio(rt2x00dev);
}

static int rt2500usb_set_state(struct rt2x00_dev *rt2x00dev,
	enum dev_state state)
{
	u16 reg;
	u16 reg2;
	unsigned int i;
	char put_to_sleep;
	char bbp_state;
	char rf_state;

	put_to_sleep = (state != STATE_AWAKE);

	reg = 0;
	rt2x00_set_field16(&reg, MAC_CSR17_BBP_DESIRE_STATE, state);
	rt2x00_set_field16(&reg, MAC_CSR17_RF_DESIRE_STATE, state);
	rt2x00_set_field16(&reg, MAC_CSR17_PUT_TO_SLEEP, put_to_sleep);
	rt2500usb_register_write(rt2x00dev, MAC_CSR17, reg);
	rt2x00_set_field16(&reg, MAC_CSR17_SET_STATE, 1);
	rt2500usb_register_write(rt2x00dev, MAC_CSR17, reg);

	/*
	 * Device is not guaranteed to be in the requested state yet.
	 * We must wait until the register indicates that the
	 * device has entered the correct state.
	 */
	for (i = 0; i < REGISTER_BUSY_COUNT; i++) {
		rt2500usb_register_read(rt2x00dev, MAC_CSR17, &reg2);
		bbp_state = rt2x00_get_field16(reg2, MAC_CSR17_BBP_CURR_STATE);
		rf_state = rt2x00_get_field16(reg2, MAC_CSR17_RF_CURR_STATE);
		if (bbp_state == state && rf_state == state)
			return 0;
		rt2500usb_register_write(rt2x00dev, MAC_CSR17, reg);
		msleep(30);
	}

	NOTICE(rt2x00dev, "Device failed to enter state %d, "
		"current device state: bbp %d and rf %d.\n",
		state, bbp_state, rf_state);

	return -EBUSY;
}

static int rt2500usb_set_device_state(struct rt2x00_dev *rt2x00dev,
	enum dev_state state)
{
	int retval = 0;

	switch (state) {
		case STATE_RADIO_ON:
			retval = rt2500usb_enable_radio(rt2x00dev);
		break;
		case STATE_RADIO_OFF:
			rt2500usb_disable_radio(rt2x00dev);
		break;
		case STATE_RADIO_RX_ON:
		case STATE_RADIO_RX_OFF:
			rt2500usb_toggle_rx(rt2x00dev, state);
		break;
		case STATE_DEEP_SLEEP:
		case STATE_SLEEP:
		case STATE_STANDBY:
		case STATE_AWAKE:
			retval = rt2500usb_set_state(rt2x00dev, state);
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
static void rt2500usb_write_tx_desc(struct rt2x00_dev *rt2x00dev,
	struct data_entry *entry, struct data_desc *txd,
	struct data_entry_desc *desc, struct ieee80211_hdr *ieee80211hdr,
	unsigned int length, struct ieee80211_tx_control *control)
{
	u32 word;

	/*
	 * Start writing the descriptor words.
	 */
	rt2x00_desc_read(txd, 1, &word);
	rt2x00_set_field32(&word, TXD_W1_IV_OFFSET, IEEE80211_HEADER);
	rt2x00_set_field32(&word, TXD_W1_AIFS, entry->ring->tx_params.aifs);
	rt2x00_set_field32(&word, TXD_W1_CWMIN, entry->ring->tx_params.cw_min);
	rt2x00_set_field32(&word, TXD_W1_CWMAX, entry->ring->tx_params.cw_max);
	rt2x00_desc_write(txd, 1, word);

	rt2x00_desc_read(txd, 2, &word);
	rt2x00_set_field32(&word, TXD_W2_PLCP_SIGNAL, desc->signal);
	rt2x00_set_field32(&word, TXD_W2_PLCP_SERVICE, desc->service);
	rt2x00_set_field32(&word, TXD_W2_PLCP_LENGTH_LOW, desc->length_low);
	rt2x00_set_field32(&word, TXD_W2_PLCP_LENGTH_HIGH, desc->length_high);
	rt2x00_desc_write(txd, 2, word);

	rt2x00_desc_read(txd, 0, &word);
	rt2x00_set_field32(&word, TXD_W0_RETRY_LIMIT, control->retry_limit);
	rt2x00_set_field32(&word, TXD_W0_MORE_FRAG,
		test_bit(ENTRY_TXD_MORE_FRAG, &entry->flags));
	rt2x00_set_field32(&word, TXD_W0_ACK,
		test_bit(ENTRY_TXD_REQ_ACK, &entry->flags));
	rt2x00_set_field32(&word, TXD_W0_TIMESTAMP,
		test_bit(ENTRY_TXD_REQ_TIMESTAMP, &entry->flags));
	rt2x00_set_field32(&word, TXD_W0_OFDM,
		test_bit(ENTRY_TXD_OFDM_RATE, &entry->flags));
	rt2x00_set_field32(&word, TXD_W0_NEW_SEQ,
		test_bit(ENTRY_TXD_NEW_SEQ, &entry->flags));
	rt2x00_set_field32(&word, TXD_W0_IFS, desc->ifs);
	rt2x00_set_field32(&word, TXD_W0_DATABYTE_COUNT, length);
	rt2x00_set_field32(&word, TXD_W0_CIPHER, CIPHER_NONE);
	rt2x00_desc_write(txd, 0, word);
}

/*
 * TX data initialization
 */
static void rt2500usb_kick_tx_queue(struct rt2x00_dev *rt2x00dev, int queue)
{
	u16 reg;

	if (queue != IEEE80211_TX_QUEUE_BEACON)
		return;

	rt2500usb_register_read(rt2x00dev, TXRX_CSR19, &reg);
	if (!rt2x00_get_field16(reg, TXRX_CSR19_BEACON_GEN)) {
		rt2x00_set_field16(&reg, TXRX_CSR19_BEACON_GEN, 1);
		/*
		 * Beacon generation will fail initially.
		 * To prevent this we need to register the TXRX_CSR19
		 * register several times.
		 */
		rt2500usb_register_write(rt2x00dev, TXRX_CSR19, reg);
		rt2500usb_register_write(rt2x00dev, TXRX_CSR19, 0);
		rt2500usb_register_write(rt2x00dev, TXRX_CSR19, reg);
		rt2500usb_register_write(rt2x00dev, TXRX_CSR19, 0);
		rt2500usb_register_write(rt2x00dev, TXRX_CSR19, reg);
	}
}

/*
 * Interrupt functions.
 */
static void rt2500usb_interrupt_rxdone(struct urb *urb)
{
	struct data_entry *entry = (struct data_entry*)urb->context;
	struct data_ring *ring = entry->ring;
	struct rt2x00_dev *rt2x00dev = ring->rt2x00dev;
	struct data_desc *rxd = (struct data_desc*)
		(entry->skb->data + urb->actual_length - ring->desc_size);
	u32 word0;
	u32 word1;
	int signal;
	int rssi;
	int ofdm;
	u16 size;

	if (!test_bit(DEVICE_ENABLED_RADIO, &rt2x00dev->flags) ||
	    !test_and_clear_bit(ENTRY_OWNER_NIC, &entry->flags))
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
	    rt2x00_get_field32(word0, RXD_W0_PHYSICAL_ERROR))
		goto skip_entry;

	/*
	 * Obtain the status about this packet.
	 */
	size = rt2x00_get_field32(word0, RXD_W0_DATABYTE_COUNT) - FCS_LEN;
	signal = rt2x00_get_field32(word1, RXD_W1_SIGNAL);
	rssi = rt2x00_get_field32(word1, RXD_W1_RSSI);
	ofdm = rt2x00_get_field32(word0, RXD_W0_OFDM);

	/*
	 * Trim the skb_buffer to only contain the valid
	 * frame data (so ignore the device's descriptor).
	 */
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
static int rt2500usb_alloc_eeprom(struct rt2x00_dev *rt2x00dev)
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
		rt2x00_set_field16(&word, EEPROM_ANTENNA_TX_DEFAULT, 0);
		rt2x00_set_field16(&word, EEPROM_ANTENNA_RX_DEFAULT, 0);
		rt2x00_set_field16(&word, EEPROM_ANTENNA_LED_MODE, 0);
		rt2x00_set_field16(&word, EEPROM_ANTENNA_DYN_TXAGC, 0);
		rt2x00_set_field16(&word, EEPROM_ANTENNA_HARDWARE_RADIO, 0);
		rt2x00_set_field16(&word, EEPROM_ANTENNA_RF_TYPE, RF2522);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_ANTENNA, word);
		EEPROM(rt2x00dev, "Antenna: 0x%04x\n", word);
	}

	rt2x00_eeprom_read(rt2x00dev, EEPROM_NIC, &word);
	if (word == 0xffff) {
		rt2x00_set_field16(&word, EEPROM_NIC_CARDBUS_ACCEL, 0);
		rt2x00_set_field16(&word, EEPROM_NIC_DYN_BBP_TUNE, 0);
		rt2x00_set_field16(&word, EEPROM_NIC_CCK_TX_POWER, 0);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_NIC, word);
		EEPROM(rt2x00dev, "NIC: 0x%04x\n", word);
	}

	rt2x00_eeprom_read(rt2x00dev, EEPROM_CALIBRATE_OFFSET, &word);
	if (word == 0xffff) {
		rt2x00_set_field16(&word, EEPROM_CALIBRATE_OFFSET_RSSI,
			MAX_RX_SSI);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_CALIBRATE_OFFSET, word);
		EEPROM(rt2x00dev, "Calibrate offset: 0x%04x\n", word);
	}

	rt2x00_eeprom_read(rt2x00dev, EEPROM_BBPTUNE, &word);
	if (word == 0xffff) {
		rt2x00_set_field16(&word, EEPROM_BBPTUNE_THRESHOLD, 45);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_BBPTUNE, word);
		EEPROM(rt2x00dev, "BBPtune: 0x%04x\n", word);
	}

	rt2x00_eeprom_read(rt2x00dev, EEPROM_BBPTUNE_VGC, &word);
	if (word == 0xffff) {
		rt2x00_set_field16(&word, EEPROM_BBPTUNE_VGCUPPER, 0x40);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_BBPTUNE_VGC, word);
		EEPROM(rt2x00dev, "BBPtune vgc: 0x%04x\n", word);
	}

	rt2x00_eeprom_read(rt2x00dev, EEPROM_BBPTUNE_R17, &word);
	if (word == 0xffff) {
		rt2x00_set_field16(&word, EEPROM_BBPTUNE_R17_LOW, 0x48);
		rt2x00_set_field16(&word, EEPROM_BBPTUNE_R17_HIGH, 0x41);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_BBPTUNE_R17, word);
		EEPROM(rt2x00dev, "BBPtune r17: 0x%04x\n", word);
	}

	rt2x00_eeprom_read(rt2x00dev, EEPROM_BBPTUNE_R24, &word);
	if (word == 0xffff) {
		rt2x00_set_field16(&word, EEPROM_BBPTUNE_R24_LOW, 0x40);
		rt2x00_set_field16(&word, EEPROM_BBPTUNE_R24_HIGH, 0x80);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_BBPTUNE_R24, word);
		EEPROM(rt2x00dev, "BBPtune r24: 0x%04x\n", word);
	}

	rt2x00_eeprom_read(rt2x00dev, EEPROM_BBPTUNE_R25, &word);
	if (word == 0xffff) {
		rt2x00_set_field16(&word, EEPROM_BBPTUNE_R25_LOW, 0x40);
		rt2x00_set_field16(&word, EEPROM_BBPTUNE_R25_HIGH, 0x50);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_BBPTUNE_R25, word);
		EEPROM(rt2x00dev, "BBPtune r25: 0x%04x\n", word);
	}

	rt2x00_eeprom_read(rt2x00dev, EEPROM_BBPTUNE_R61, &word);
	if (word == 0xffff) {
		rt2x00_set_field16(&word, EEPROM_BBPTUNE_R61_LOW, 0x60);
		rt2x00_set_field16(&word, EEPROM_BBPTUNE_R61_HIGH, 0x6d);
		rt2x00_eeprom_write(rt2x00dev, EEPROM_BBPTUNE_R61, word);
		EEPROM(rt2x00dev, "BBPtune r61: 0x%04x\n", word);
	}

	return 0;
}

static int rt2500usb_init_eeprom(struct rt2x00_dev *rt2x00dev)
{
	u16 reg;
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
	rt2500usb_register_read(rt2x00dev, MAC_CSR0, &reg);
	rt2x00_set_chip(rt2x00dev, RT2570, value, reg);

	if (!rt2x00_rf(&rt2x00dev->chip, RF2522) &&
	    !rt2x00_rf(&rt2x00dev->chip, RF2523) &&
	    !rt2x00_rf(&rt2x00dev->chip, RF2524) &&
	    !rt2x00_rf(&rt2x00dev->chip, RF2525) &&
	    !rt2x00_rf(&rt2x00dev->chip, RF2525E) &&
	    !rt2x00_rf(&rt2x00dev->chip, RF5222)) {
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
	 * Store led mode, for correct led behaviour.
	 */
	rt2x00dev->led_mode = rt2x00_get_field16(eeprom,
		EEPROM_ANTENNA_LED_MODE);

	/*
	 * Check if the BBP tuning should be disabled.
	 */
	rt2x00_eeprom_read(rt2x00dev, EEPROM_NIC, &eeprom);
	if (rt2x00_get_field16(eeprom, EEPROM_NIC_DYN_BBP_TUNE))
		__set_bit(CONFIG_DISABLE_LINK_TUNING, &rt2x00dev->flags);

	/*
	 * Read the RSSI <-> dBm offset information.
	 */
	rt2x00_eeprom_read(rt2x00dev, EEPROM_CALIBRATE_OFFSET, &eeprom);
	rt2x00dev->hw->max_rssi =
		rt2x00_get_field16(eeprom, EEPROM_CALIBRATE_OFFSET_RSSI);

	return 0;
}

static const struct {
	unsigned int chip;
	u32 val[3];
} rf_vals[] = {
	{ RF2522,	{ 0x00002050, 0x00000101, 0x00000000 } },
	{ RF2523,	{ 0x00022010, 0x000e0111, 0x00000a1b } },
	{ RF2524,	{ 0x00032020, 0x00000101, 0x00000a1b } },
	{ RF2525,	{ 0x00022020, 0x00060111, 0x00000a1b } },
	{ RF2525E,	{ 0x00022010, 0x00060111, 0x00000000 } },
	{ RF5222,	{ 0x00000000, 0x00000101, 0x00000000 } }
};

/*
 * RF value list for RF2522
 * Supports: 2.4 GHz
 */
static const u32 rf_vals_bg_2522[] = {
	0x000c1fda, 0x000c1fee, 0x000c2002, 0x000c2016, 0x000c202a,
	0x000c203e, 0x000c2052, 0x000c2066, 0x000c207a, 0x000c208e,
	0x000c20a2, 0x000c20b6, 0x000c20ca, 0x000c20fa
};

/*
 * RF value list for RF2523, RF2524 & RF2525
 * Supports: 2.4 GHz
 */
static const u32 rf_vals_bg_252x[] = {
	0x00000c9e, 0x00000ca2, 0x00000ca6, 0x00000caa, 0x00000cae,
	0x00000cb2, 0x00000cb6, 0x00000cba, 0x00000cbe, 0x00000d02,
	0x00000d06, 0x00000d0a, 0x00000d0e, 0x00000d1a
};

/*
 * RF value list for RF2525E
 * Supports: 2.4 GHz
 */
static const u32 rf_vals_bg_2525e[] = {
	0x0000089a, 0x0000089e, 0x0000089e, 0x000008a2, 0x000008a2,
	0x000008a6, 0x000008a6, 0x000008aa, 0x000008aa, 0x000008ae,
	0x000008ae, 0x000008b2, 0x000008b2, 0x000008b6
};

/*
 * RF value list for RF5222
 * Supports: 2.4 GHz & 5.2 GHz
 */
static const u32 rf_vals_abg_5222[] = {
	0x00001136, 0x0000113a, 0x0000113e, 0x00001182, 0x00001186,
	0x0000118a, 0x0000118e, 0x00001192, 0x00001196, 0x0000119a,
	0x0000119e, 0x000011a2, 0x000011a6, 0x000011ae, 0x0001889a,
	0x0001889a, 0x0001889e, 0x000188a2, 0x000188a6, 0x000188aa,
	0x000188ae, 0x000188b2, 0x00008802, 0x00008806, 0x0000880a,
	0x0000880e, 0x00008812, 0x00008816, 0x0000881a, 0x0000881e,
	0x00008822, 0x00008826, 0x0000882a, 0x000090a6, 0x000090ae,
	0x000090b6, 0x000090be
};

static void rt2500usb_init_hw_mode(struct rt2x00_dev *rt2x00dev)
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
	rt2x00dev->hw->queues = 2;

	/*
	 * This device supports ATIM
	 */
	__set_bit(DEVICE_SUPPORT_ATIM, &rt2x00dev->flags);

	/*
	 * Set device specific, but channel independent RF values.
	 */
	for (i = 0; i < ARRAY_SIZE(rf_vals); i++) {
		if (rt2x00_rf(&rt2x00dev->chip, rf_vals[i].chip)) {
			rt2x00dev->rf1 = rf_vals[i].val[0];
			rt2x00dev->rf3 = rf_vals[i].val[1];
			rt2x00dev->rf4 = rf_vals[i].val[2];
		}
	}

	/*
	 * Convert tx_power array in eeprom.
	 */
	txpower = rt2x00_eeprom_addr(rt2x00dev, EEPROM_TXPOWER_START);
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

	if (rt2x00_rf(&rt2x00dev->chip, RF2522))
		spec->chan_val_bg = rf_vals_bg_2522;
	else if (rt2x00_rf(&rt2x00dev->chip, RF2523) ||
		 rt2x00_rf(&rt2x00dev->chip, RF2524) ||
		 rt2x00_rf(&rt2x00dev->chip, RF2525))
		spec->chan_val_bg = rf_vals_bg_252x;
	else if (rt2x00_rf(&rt2x00dev->chip, RF2525E))
		spec->chan_val_bg = rf_vals_bg_2525e;
	else if (rt2x00_rf(&rt2x00dev->chip, RF5222))
		spec->chan_val_bg = rf_vals_abg_5222;

	if (rt2x00_rf(&rt2x00dev->chip, RF5222)) {
		spec->num_modes = 3;
		spec->num_channels += 23;
		spec->chan_val_a = &rf_vals_abg_5222[14];
	}
}

static int rt2500usb_init_hw(struct rt2x00_dev *rt2x00dev)
{
	int retval;

	/*
	 * Allocate eeprom data.
	 */
	retval = rt2500usb_alloc_eeprom(rt2x00dev);
	if (retval)
		return retval;

	retval = rt2500usb_init_eeprom(rt2x00dev);
	if (retval)
		return retval;

	/*
	 * Initialize hw specifications.
	 */
	rt2500usb_init_hw_mode(rt2x00dev);

	return 0;
}

/*
 * IEEE80211 stack callback functions.
 */
static int rt2500usb_get_stats(struct ieee80211_hw *hw,
	struct ieee80211_low_level_stats *stats)
{
	struct rt2x00_dev *rt2x00dev = hw->priv;
	u16 reg;

	/*
	 * Update FCS error count from register.
	 * The dot11ACKFailureCount, dot11RTSFailureCount and
	 * dot11RTSSuccessCount are updated in interrupt time.
	 */
	rt2500usb_register_read(rt2x00dev, STA_CSR0, &reg);
	rt2x00dev->low_level_stats.dot11FCSErrorCount +=
		rt2x00_get_field16(reg, STA_CSR0_FCS_ERROR);

	memcpy(stats, &rt2x00dev->low_level_stats, sizeof(*stats));

	return 0;
}

static const struct ieee80211_ops rt2500usb_mac80211_ops = {
	.tx			= rt2x00lib_tx,
	.reset			= rt2x00lib_reset,
	.open			= rt2x00lib_open,
	.stop			= rt2x00lib_stop,
	.add_interface		= rt2x00lib_add_interface,
	.remove_interface	= rt2x00lib_remove_interface,
	.config			= rt2x00lib_config,
	.config_interface	= rt2x00lib_config_interface,
	.set_multicast_list	= rt2x00lib_set_multicast_list,
	.get_stats		= rt2500usb_get_stats,
	.conf_tx		= rt2x00lib_conf_tx,
	.get_tx_stats		= rt2x00lib_get_tx_stats,
	.beacon_update		= rt2x00usb_beacon_update,
};

static const struct rt2x00lib_ops rt2500usb_rt2x00_ops = {
	.init_hw		= rt2500usb_init_hw,
	.initialize		= rt2x00usb_initialize,
	.uninitialize		= rt2x00usb_uninitialize,
	.set_device_state	= rt2500usb_set_device_state,
	.link_tuner		= rt2500usb_link_tuner,
	.write_tx_desc		= rt2500usb_write_tx_desc,
	.write_tx_data		= rt2x00usb_write_tx_data,
	.kick_tx_queue		= rt2500usb_kick_tx_queue,
	.config_type		= rt2500usb_config_type,
	.config_phymode		= rt2500usb_config_phymode,
	.config_channel		= rt2500usb_config_channel,
	.config_mac_addr	= rt2500usb_config_mac_addr,
	.config_bssid		= rt2500usb_config_bssid,
	.config_promisc		= rt2500usb_config_promisc,
	.config_txpower		= rt2500usb_config_txpower,
	.config_antenna		= rt2500usb_config_antenna,
	.config_duration	= rt2500usb_config_duration,
};

static const struct rt2x00_ops rt2500usb_ops = {
	.name	= DRV_NAME,
	.rxd_size = RXD_DESC_SIZE,
	.txd_size = TXD_DESC_SIZE,
	.lib	= &rt2500usb_rt2x00_ops,
	.hw	= &rt2500usb_mac80211_ops,
#ifdef CONFIG_RT2X00_LIB_DEBUGFS
	.debugfs = &rt2500usb_rt2x00debug,
#endif /* CONFIG_RT2X00_LIB_DEBUGFS */
};

/*
 * rt2500usb module information.
 */
static struct usb_device_id rt2500usb_device_table[] = {
	/* ASUS */
	{ USB_DEVICE(0x0b05, 0x1706), USB_DEVICE_DATA(&rt2500usb_ops) },
	{ USB_DEVICE(0x0b05, 0x1707), USB_DEVICE_DATA(&rt2500usb_ops) },
	/* Belkin */
	{ USB_DEVICE(0x050d, 0x7050), USB_DEVICE_DATA(&rt2500usb_ops) },
	{ USB_DEVICE(0x050d, 0x7051), USB_DEVICE_DATA(&rt2500usb_ops) },
	{ USB_DEVICE(0x050d, 0x705a), USB_DEVICE_DATA(&rt2500usb_ops) },
	/* Cisco Systems */
	{ USB_DEVICE(0x13b1, 0x000d), USB_DEVICE_DATA(&rt2500usb_ops) },
	{ USB_DEVICE(0x13b1, 0x0011), USB_DEVICE_DATA(&rt2500usb_ops) },
	{ USB_DEVICE(0x13b1, 0x001a), USB_DEVICE_DATA(&rt2500usb_ops) },
	/* Conceptronic */
	{ USB_DEVICE(0x14b2, 0x3c02), USB_DEVICE_DATA(&rt2500usb_ops) },
	/* D-LINK */
	{ USB_DEVICE(0x2001, 0x3c00), USB_DEVICE_DATA(&rt2500usb_ops) },
	/* Gigabyte */
	{ USB_DEVICE(0x1044, 0x8001), USB_DEVICE_DATA(&rt2500usb_ops) },
	{ USB_DEVICE(0x1044, 0x8007), USB_DEVICE_DATA(&rt2500usb_ops) },
	/* Hercules */
	{ USB_DEVICE(0x06f8, 0xe000), USB_DEVICE_DATA(&rt2500usb_ops) },
	/* Melco */
	{ USB_DEVICE(0x0411, 0x0066), USB_DEVICE_DATA(&rt2500usb_ops) },
	{ USB_DEVICE(0x0411, 0x0067), USB_DEVICE_DATA(&rt2500usb_ops) },
	{ USB_DEVICE(0x0411, 0x008b), USB_DEVICE_DATA(&rt2500usb_ops) },
	/* MSI */
	{ USB_DEVICE(0x0db0, 0x6861), USB_DEVICE_DATA(&rt2500usb_ops) },
	{ USB_DEVICE(0x0db0, 0x6865), USB_DEVICE_DATA(&rt2500usb_ops) },
	{ USB_DEVICE(0x0db0, 0x6869), USB_DEVICE_DATA(&rt2500usb_ops) },
	/* Ralink */
	{ USB_DEVICE(0x148f, 0x1706), USB_DEVICE_DATA(&rt2500usb_ops) },
	{ USB_DEVICE(0x148f, 0x2570), USB_DEVICE_DATA(&rt2500usb_ops) },
	{ USB_DEVICE(0x148f, 0x2573), USB_DEVICE_DATA(&rt2500usb_ops) },
	{ USB_DEVICE(0x148f, 0x9020), USB_DEVICE_DATA(&rt2500usb_ops) },
	/* Siemens */
	{ USB_DEVICE(0x0681, 0x3c06), USB_DEVICE_DATA(&rt2500usb_ops) },
	/* SMC */
	{ USB_DEVICE(0x0707, 0xee13), USB_DEVICE_DATA(&rt2500usb_ops) },
	/* Spairon */
	{ USB_DEVICE(0x114b, 0x0110), USB_DEVICE_DATA(&rt2500usb_ops) },
	/* Trust */
	{ USB_DEVICE(0x0eb0, 0x9020), USB_DEVICE_DATA(&rt2500usb_ops) },
	/* Zinwell */
	{ USB_DEVICE(0x5a57, 0x0260), USB_DEVICE_DATA(&rt2500usb_ops) },
	{ 0, }
};

MODULE_AUTHOR(DRV_PROJECT);
MODULE_VERSION(DRV_VERSION);
MODULE_DESCRIPTION("Ralink RT2500 USB Wireless LAN driver.");
MODULE_SUPPORTED_DEVICE("Ralink RT2570 USB chipset based cards");
MODULE_DEVICE_TABLE(usb, rt2500usb_device_table);
MODULE_LICENSE("GPL");

static struct usb_driver rt2500usb_driver = {
	.name		= DRV_NAME,
	.id_table	= rt2500usb_device_table,
	.probe		= rt2x00usb_probe,
	.disconnect	= rt2x00usb_disconnect,
#ifdef CONFIG_PM
	.suspend	= rt2x00usb_suspend,
	.resume		= rt2x00usb_resume,
#endif /* CONFIG_PM */
};

static int __init rt2500usb_init(void)
{
	printk(KERN_INFO "Loading module: %s - %s by %s.\n",
		DRV_NAME, DRV_VERSION, DRV_PROJECT);
	return usb_register(&rt2500usb_driver);
}

static void __exit rt2500usb_exit(void)
{
	printk(KERN_INFO "Unloading module: %s.\n", DRV_NAME);
	usb_deregister(&rt2500usb_driver);
}

module_init(rt2500usb_init);
module_exit(rt2500usb_exit);
