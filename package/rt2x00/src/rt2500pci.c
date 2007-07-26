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
	Module: rt2500pci
	Abstract: rt2500pci device specific routines.
	Supported chipsets: RT2560.
 */

/*
 * Set enviroment defines for rt2x00.h
 */
#define DRV_NAME "rt2500pci"

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
#include "rt2x00pci.h"
#include "rt2500pci.h"

/*
 * Register access.
 * All access to the CSR registers will go through the methods
 * rt2x00pci_register_read and rt2x00pci_register_write.
 * BBP and RF register require indirect register access,
 * and use the CSR registers BBPCSR and RFCSR to achieve this.
 * These indirect registers work with busy bits,
 * and we will try maximal REGISTER_BUSY_COUNT times to access
 * the register while taking a REGISTER_BUSY_DELAY us delay
 * between each attampt. When the busy bit is still set at that time,
 * the access attempt is considered to have failed,
 * and we will print an error.
 */
static u32 rt2500pci_bbp_check(const struct rt2x00_dev *rt2x00dev)
{
	u32 reg;
	unsigned int i;

	for (i = 0; i < REGISTER_BUSY_COUNT; i++) {
		rt2x00pci_register_read(rt2x00dev, BBPCSR, &reg);
		if (!rt2x00_get_field32(reg, BBPCSR_BUSY))
			break;
		udelay(REGISTER_BUSY_DELAY);
	}

	return reg;
}

static void rt2500pci_bbp_write(const struct rt2x00_dev *rt2x00dev,
	const u8 reg_id, const u8 value)
{
	u32 reg;

	/*
	 *  Wait until the BBP becomes ready.
	 */
	reg = rt2500pci_bbp_check(rt2x00dev);
	if (rt2x00_get_field32(reg, BBPCSR_BUSY)) {
		ERROR(rt2x00dev, "BBPCSR register busy. Write failed.\n");
		return;
	}

	/*
	 * Write the data into the BBP.
	 */
	reg = 0;
	rt2x00_set_field32(&reg, BBPCSR_VALUE, value);
	rt2x00_set_field32(&reg, BBPCSR_REGNUM, reg_id);
	rt2x00_set_field32(&reg, BBPCSR_BUSY, 1);
	rt2x00_set_field32(&reg, BBPCSR_WRITE_CONTROL, 1);

	rt2x00pci_register_write(rt2x00dev, BBPCSR, reg);
}

static void rt2500pci_bbp_read(const struct rt2x00_dev *rt2x00dev,
	const u8 reg_id, u8 *value)
{
	u32 reg;

	/*
	 *  Wait until the BBP becomes ready.
	 */
	reg = rt2500pci_bbp_check(rt2x00dev);
	if (rt2x00_get_field32(reg, BBPCSR_BUSY)) {
		ERROR(rt2x00dev, "BBPCSR register busy. Read failed.\n");
		return;
	}

	/*
	 * Write the request into the BBP.
	 */
	reg = 0;
	rt2x00_set_field32(&reg, BBPCSR_REGNUM, reg_id);
	rt2x00_set_field32(&reg, BBPCSR_BUSY, 1);
	rt2x00_set_field32(&reg, BBPCSR_WRITE_CONTROL, 0);

	rt2x00pci_register_write(rt2x00dev, BBPCSR, reg);

	/*
	 *  Wait until the BBP becomes ready.
	 */
	reg = rt2500pci_bbp_check(rt2x00dev);
	if (rt2x00_get_field32(reg, BBPCSR_BUSY)) {
		ERROR(rt2x00dev, "BBPCSR register busy. Read failed.\n");
		*value = 0xff;
		return;
	}

	*value = rt2x00_get_field32(reg, BBPCSR_VALUE);
}

static void rt2500pci_rf_write(const struct rt2x00_dev *rt2x00dev,
	const u32 value)
{
	u32 reg;
	unsigned int i;

	for (i = 0; i < REGISTER_BUSY_COUNT; i++) {
		rt2x00pci_register_read(rt2x00dev, RFCSR, &reg);
		if (!rt2x00_get_field32(reg, RFCSR_BUSY))
			goto rf_write;
		udelay(REGISTER_BUSY_DELAY);
	}

	ERROR(rt2x00dev, "RFCSR register busy. Write failed.\n");
	return;

rf_write:
	reg = 0;
	rt2x00_set_field32(&reg, RFCSR_VALUE, value);
	rt2x00_set_field32(&reg, RFCSR_NUMBER_OF_BITS, 20);
	rt2x00_set_field32(&reg, RFCSR_IF_SELECT, 0);
	rt2x00_set_field32(&reg, RFCSR_BUSY, 1);

	rt2x00pci_register_write(rt2x00dev, RFCSR, reg);
}

static void rt2500pci_eepromregister_read(struct eeprom_93cx6 *eeprom)
{
	struct rt2x00_dev *rt2x00dev = eeprom->data;
	u32 reg;

	rt2x00pci_register_read(rt2x00dev, CSR21, &reg);

	eeprom->reg_data_in = !!rt2x00_get_field32(reg,
		CSR21_EEPROM_DATA_IN);
	eeprom->reg_data_out = !!rt2x00_get_field32(reg,
		CSR21_EEPROM_DATA_OUT);
	eeprom->reg_data_clock = !!rt2x00_get_field32(reg,
		CSR21_EEPROM_DATA_CLOCK);
	eeprom->reg_chip_select = !!rt2x00_get_field32(reg,
		CSR21_EEPROM_CHIP_SELECT);
}

static void rt2500pci_eepromregister_write(struct eeprom_93cx6 *eeprom)
{
	struct rt2x00_dev *rt2x00dev = eeprom->data;
	u32 reg = 0;

	rt2x00_set_field32(&reg, CSR21_EEPROM_DATA_IN,
		!!eeprom->reg_data_in);
	rt2x00_set_field32(&reg, CSR21_EEPROM_DATA_OUT,
		!!eeprom->reg_data_out);
	rt2x00_set_field32(&reg, CSR21_EEPROM_DATA_CLOCK,
		!!eeprom->reg_data_clock);
	rt2x00_set_field32(&reg, CSR21_EEPROM_CHIP_SELECT,
		!!eeprom->reg_chip_select);

	rt2x00pci_register_write(rt2x00dev, CSR21, reg);
}

#ifdef CONFIG_RT2X00_LIB_DEBUGFS
#define CSR_OFFSET(__word)	( CSR_REG_BASE + ((__word) * sizeof(u32)) )

static void rt2500pci_read_csr(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt2x00pci_register_read(rt2x00dev, CSR_OFFSET(word), data);
}

static void rt2500pci_write_csr(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt2x00pci_register_write(rt2x00dev, CSR_OFFSET(word), *((u32*)data));
}

static void rt2500pci_read_eeprom(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt2x00_eeprom_read(rt2x00dev, word, data);
}

static void rt2500pci_write_eeprom(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt2x00_eeprom_write(rt2x00dev, word, *((u16*)data));
}

static void rt2500pci_read_bbp(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt2500pci_bbp_read(rt2x00dev, word, data);
}

static void rt2500pci_write_bbp(struct rt2x00_dev *rt2x00dev,
	const unsigned long word, void *data)
{
	rt2500pci_bbp_write(rt2x00dev, word, *((u8*)data));
}

static const struct rt2x00debug rt2500pci_rt2x00debug = {
	.owner 		= THIS_MODULE,
	.reg_csr	= {
		.read		= rt2500pci_read_csr,
		.write		= rt2500pci_write_csr,
		.word_size	= sizeof(u32),
		.word_count	= CSR_REG_SIZE / sizeof(u32),
	},
	.reg_eeprom	= {
		.read		= rt2500pci_read_eeprom,
		.write		= rt2500pci_write_eeprom,
		.word_size	= sizeof(u16),
		.word_count	= EEPROM_SIZE / sizeof(u16),
	},
	.reg_bbp	= {
		.read		= rt2500pci_read_bbp,
		.write		= rt2500pci_write_bbp,
		.word_size	= sizeof(u8),
		.word_count	= BBP_SIZE / sizeof(u8),
	},
};
#endif /* CONFIG_RT2X00_LIB_DEBUGFS */

#ifdef CONFIG_RT2500PCI_RFKILL
static int rt2500pci_rfkill_poll(struct rt2x00_dev *rt2x00dev)
{
	u32 reg;

	rt2x00pci_register_read(rt2x00dev, GPIOCSR, &reg);
	return rt2x00_get_field32(reg, GPIOCSR_BIT0);
}
#endif /* CONFIG_RT2400PCI_RFKILL */

/*
 * Configuration handlers.
 */
static void rt2500pci_config_bssid(struct rt2x00_dev *rt2x00dev, u8 *bssid)
{
	u32 reg[2];

	memset(&reg, 0, sizeof(reg));
	memcpy(&reg, bssid, ETH_ALEN);

	/*
	 * The BSSID is passed to us as an array of bytes,
	 * that array is little endian, so no need for byte ordering.
	 */
	rt2x00pci_register_multiwrite(rt2x00dev, CSR5, &reg, sizeof(reg));
}

static void rt2500pci_config_promisc(struct rt2x00_dev *rt2x00dev,
	const int promisc)
{
	u32 reg;

	rt2x00pci_register_read(rt2x00dev, RXCSR0, &reg);
	rt2x00_set_field32(&reg, RXCSR0_DROP_NOT_TO_ME, !promisc);
	rt2x00pci_register_write(rt2x00dev, RXCSR0, reg);
}

static void rt2500pci_config_type(struct rt2x00_dev *rt2x00dev,
	const int type)
{
	u32 reg;

	rt2x00pci_register_write(rt2x00dev, CSR14, 0);

	/*
	 * Apply hardware packet filter.
	 */
	rt2x00pci_register_read(rt2x00dev, RXCSR0, &reg);

	if (!is_monitor_present(&rt2x00dev->interface) &&
	    (type == IEEE80211_IF_TYPE_IBSS || type == IEEE80211_IF_TYPE_STA))
		rt2x00_set_field32(&reg, RXCSR0_DROP_TODS, 1);
	else
		rt2x00_set_field32(&reg, RXCSR0_DROP_TODS, 0);

	rt2x00_set_field32(&reg, RXCSR0_DROP_CRC, 1);
	if (is_monitor_present(&rt2x00dev->interface)) {
		rt2x00_set_field32(&reg, RXCSR0_DROP_PHYSICAL, 0);
		rt2x00_set_field32(&reg, RXCSR0_DROP_CONTROL, 0);
		rt2x00_set_field32(&reg, RXCSR0_DROP_VERSION_ERROR, 0);
	} else {
		rt2x00_set_field32(&reg, RXCSR0_DROP_PHYSICAL, 1);
		rt2x00_set_field32(&reg, RXCSR0_DROP_CONTROL, 1);
		rt2x00_set_field32(&reg, RXCSR0_DROP_VERSION_ERROR, 1);
	}

	rt2x00_set_field32(&reg, RXCSR0_DROP_MCAST, 0);
	rt2x00_set_field32(&reg, RXCSR0_DROP_BCAST, 0);

	rt2x00pci_register_write(rt2x00dev, RXCSR0, reg);

	/*
	 * Enable beacon config
	 */
	rt2x00pci_register_read(rt2x00dev, BCNCSR1, &reg);
	rt2x00_set_field32(&reg, BCNCSR1_PRELOAD,
		PREAMBLE + get_duration(IEEE80211_HEADER, 2));
	rt2x00_set_field32(&reg, BCNCSR1_BEACON_CWMIN,
		rt2x00_get_ring(rt2x00dev, IEEE80211_TX_QUEUE_BEACON)
			->tx_params.cw_min);
	rt2x00pci_register_write(rt2x00dev, BCNCSR1, reg);

	/*
	 * Enable synchronisation.
	 */
	rt2x00pci_register_read(rt2x00dev, CSR14, &reg);
	if (is_interface_present(&rt2x00dev->interface)) {
		rt2x00_set_field32(&reg, CSR14_TSF_COUNT, 1);
		rt2x00_set_field32(&reg, CSR14_TBCN, 1);
	}

	rt2x00_set_field32(&reg, CSR14_BEACON_GEN, 0);
	if (type == IEEE80211_IF_TYPE_IBSS || type == IEEE80211_IF_TYPE_AP)
		rt2x00_set_field32(&reg, CSR14_TSF_SYNC, 2);
	else if (type == IEEE80211_IF_TYPE_STA)
		rt2x00_set_field32(&reg, CSR14_TSF_SYNC, 1);
	else if (is_monitor_present(&rt2x00dev->interface) &&
		 !is_interface_present(&rt2x00dev->interface))
		rt2x00_set_field32(&reg, CSR14_TSF_SYNC, 0);

	rt2x00pci_register_write(rt2x00dev, CSR14, reg);
}

static void rt2500pci_config_channel(struct rt2x00_dev *rt2x00dev,
	const int value, const int channel, const int txpower)
{
	u32 rf1 = rt2x00dev->rf1;
	u32 rf2 = value;
	u32 rf3 = rt2x00dev->rf3;
	u32 rf4 = rt2x00dev->rf4;

	if (rt2x00_rf(&rt2x00dev->chip, RF2525) ||
	    rt2x00_rf(&rt2x00dev->chip, RF2525E))
		rf2 |= 0x00080000;

	if (rt2x00_rf(&rt2x00dev->chip, RF2525E) && channel == 14)
		rf4 |= 0x00000010;

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
	 * Switch on tuning bits.
	 * For RT2523 devices we do not need to update the R1 register.
	 */
	if (!rt2x00_rf(&rt2x00dev->chip, RF2523))
		rt2x00_set_field32(&rf1, RF1_TUNER, 1);
	rt2x00_set_field32(&rf3, RF3_TUNER, 1);

	/*
	 * For RT2525 we should first set the channel to half band higher.
	 */
	if (rt2x00_rf(&rt2x00dev->chip, RF2525)) {
		static const u32 vals[] = {
			0x00080cbe, 0x00080d02, 0x00080d06, 0x00080d0a,
			0x00080d0e, 0x00080d12, 0x00080d16, 0x00080d1a,
			0x00080d1e, 0x00080d22, 0x00080d26, 0x00080d2a,
			0x00080d2e, 0x00080d3a
		};

		rt2500pci_rf_write(rt2x00dev, rf1);
		rt2500pci_rf_write(rt2x00dev, vals[channel - 1]);
		rt2500pci_rf_write(rt2x00dev, rf3);
		if (rf4)
			rt2500pci_rf_write(rt2x00dev, rf4);
	}

	rt2500pci_rf_write(rt2x00dev, rf1);
	rt2500pci_rf_write(rt2x00dev, rf2);
	rt2500pci_rf_write(rt2x00dev, rf3);
	if (rf4)
		rt2500pci_rf_write(rt2x00dev, rf4);

	/*
	 * Channel 14 requires the Japan filter bit to be set.
	 */
	rt2500pci_bbp_write(rt2x00dev, 70, (channel == 14) ? 0x4e : 0x46);

	msleep(1);

	/*
	 * Switch off tuning bits.
	 * For RT2523 devices we do not need to update the R1 register.
	 */
	rt2x00_set_field32(&rf1, RF1_TUNER, 0);
	rt2x00_set_field32(&rf3, RF3_TUNER, 0);


	if (!rt2x00_rf(&rt2x00dev->chip, RF2523))
		rt2500pci_rf_write(rt2x00dev, rf1);

	rt2500pci_rf_write(rt2x00dev, rf3);

	/*
	 * Update rf fields
	 */
	rt2x00dev->rf1 = rf1;
	rt2x00dev->rf2 = rf2;
	rt2x00dev->rf3 = rf3;
	rt2x00dev->rf4 = rf4;
	rt2x00dev->tx_power = txpower;

	/*
	 * Clear false CRC during channel switch.
	 */
	rt2x00pci_register_read(rt2x00dev, CNT0, &rf1);
}

static void rt2500pci_config_txpower(struct rt2x00_dev *rt2x00dev,
	const int txpower)
{
	rt2x00_set_field32(&rt2x00dev->rf3, RF3_TXPOWER,
		TXPOWER_TO_DEV(txpower));
	rt2500pci_rf_write(rt2x00dev, rt2x00dev->rf3);

}

static void rt2500pci_config_antenna(struct rt2x00_dev *rt2x00dev,
	const int antenna_tx, const int antenna_rx)
{
	u32 reg;
	u8 r14;
	u8 r2;

	rt2x00pci_register_read(rt2x00dev, BBPCSR1, &reg);
	rt2500pci_bbp_read(rt2x00dev, 14, &r14);
	rt2500pci_bbp_read(rt2x00dev, 2, &r2);

	/*
	 * Configure the TX antenna.
	 */
	if (antenna_tx == ANTENNA_DIVERSITY) {
		rt2x00_set_field8(&r2, BBP_R2_TX_ANTENNA, 2);
		rt2x00_set_field32(&reg, BBPCSR1_CCK, 2);
		rt2x00_set_field32(&reg, BBPCSR1_OFDM, 2);
	} else if (antenna_tx == ANTENNA_A) {
		rt2x00_set_field8(&r2, BBP_R2_TX_ANTENNA, 0);
		rt2x00_set_field32(&reg, BBPCSR1_CCK, 0);
		rt2x00_set_field32(&reg, BBPCSR1_OFDM, 0);
	} else if (antenna_tx == ANTENNA_B) {
		rt2x00_set_field8(&r2, BBP_R2_TX_ANTENNA, 2);
		rt2x00_set_field32(&reg, BBPCSR1_CCK, 2);
		rt2x00_set_field32(&reg, BBPCSR1_OFDM, 2);
	}

	/*
	 * Configure the RX antenna.
	 */
	if (antenna_rx == ANTENNA_DIVERSITY)
		rt2x00_set_field8(&r14, BBP_R14_RX_ANTENNA, 2);
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
		rt2x00_set_field32(&reg, BBPCSR1_CCK_FLIP, 1);
		rt2x00_set_field32(&reg, BBPCSR1_OFDM_FLIP, 1);

		/*
		 * RT2525E does not need RX I/Q Flip.
		 */
		if (rt2x00_rf(&rt2x00dev->chip, RF2525E))
			rt2x00_set_field8(&r14, BBP_R14_RX_IQ_FLIP, 0);
	} else {
		rt2x00_set_field32(&reg, BBPCSR1_CCK_FLIP, 0);
		rt2x00_set_field32(&reg, BBPCSR1_OFDM_FLIP, 0);
	}

	rt2x00pci_register_write(rt2x00dev, BBPCSR1, reg);
	rt2500pci_bbp_write(rt2x00dev, 14, r14);
	rt2500pci_bbp_write(rt2x00dev, 2, r2);
}

static void rt2500pci_config_duration(struct rt2x00_dev *rt2x00dev,
	const int short_slot_time, const int beacon_int)
{
	u32 reg;

	rt2x00pci_register_read(rt2x00dev, CSR11, &reg);
	rt2x00_set_field32(&reg, CSR11_SLOT_TIME,
		short_slot_time ? SHORT_SLOT_TIME : SLOT_TIME);
	rt2x00pci_register_write(rt2x00dev, CSR11, reg);

	rt2x00pci_register_read(rt2x00dev, CSR18, &reg);
	rt2x00_set_field32(&reg, CSR18_SIFS, SIFS);
	rt2x00_set_field32(&reg, CSR18_PIFS,
		short_slot_time ? SHORT_PIFS : PIFS);
	rt2x00pci_register_write(rt2x00dev, CSR18, reg);

	rt2x00pci_register_read(rt2x00dev, CSR19, &reg);
	rt2x00_set_field32(&reg, CSR19_DIFS,
		short_slot_time ? SHORT_DIFS : DIFS);
	rt2x00_set_field32(&reg, CSR19_EIFS, EIFS);
	rt2x00pci_register_write(rt2x00dev, CSR19, reg);

	rt2x00pci_register_read(rt2x00dev, TXCSR1, &reg);
	rt2x00_set_field32(&reg, TXCSR1_TSF_OFFSET, IEEE80211_HEADER);
	rt2x00_set_field32(&reg, TXCSR1_AUTORESPONDER, 1);
	rt2x00pci_register_write(rt2x00dev, TXCSR1, reg);

	rt2x00pci_register_read(rt2x00dev, CSR12, &reg);
	rt2x00_set_field32(&reg, CSR12_BEACON_INTERVAL, beacon_int * 16);
	rt2x00_set_field32(&reg, CSR12_CFP_MAX_DURATION, beacon_int * 16);
	rt2x00pci_register_write(rt2x00dev, CSR12, reg);
}

static void rt2500pci_config_rate(struct rt2x00_dev *rt2x00dev, const int rate)
{
	struct ieee80211_conf *conf = &rt2x00dev->hw->conf;
	u32 reg;
	u32 preamble;
	u16 value;

	preamble = DEVICE_GET_RATE_FIELD(rate, PREAMBLE)
		? SHORT_PREAMBLE : PREAMBLE;

	reg = DEVICE_GET_RATE_FIELD(rate, RATEMASK) & DEV_BASIC_RATE;
	rt2x00pci_register_write(rt2x00dev, ARCSR1, reg);

	rt2x00pci_register_read(rt2x00dev, TXCSR1, &reg);
	value = ((conf->flags & IEEE80211_CONF_SHORT_SLOT_TIME) ?
		 SHORT_DIFS :  DIFS) +
		PLCP + preamble + get_duration(ACK_SIZE, 10);
	rt2x00_set_field32(&reg, TXCSR1_ACK_TIMEOUT, value);
	value = SIFS + PLCP + preamble + get_duration(ACK_SIZE, 10);
	rt2x00_set_field32(&reg, TXCSR1_ACK_CONSUME_TIME, value);
	rt2x00pci_register_write(rt2x00dev, TXCSR1, reg);

	preamble = DEVICE_GET_RATE_FIELD(rate, PREAMBLE) ? 0x08 : 0x00;

	rt2x00pci_register_read(rt2x00dev, ARCSR2, &reg);
	rt2x00_set_field32(&reg, ARCSR2_SIGNAL, 0x00 | preamble);
	rt2x00_set_field32(&reg, ARCSR2_SERVICE, 0x04);
	rt2x00_set_field32(&reg, ARCSR2_LENGTH, get_duration(ACK_SIZE, 10));
	rt2x00pci_register_write(rt2x00dev, ARCSR2, reg);

	rt2x00pci_register_read(rt2x00dev, ARCSR3, &reg);
	rt2x00_set_field32(&reg, ARCSR3_SIGNAL, 0x01 | preamble);
	rt2x00_set_field32(&reg, ARCSR3_SERVICE, 0x04);
	rt2x00_set_field32(&reg, ARCSR2_LENGTH, get_duration(ACK_SIZE, 20));
	rt2x00pci_register_write(rt2x00dev, ARCSR3, reg);

	rt2x00pci_register_read(rt2x00dev, ARCSR4, &reg);
	rt2x00_set_field32(&reg, ARCSR4_SIGNAL, 0x02 | preamble);
	rt2x00_set_field32(&reg, ARCSR4_SERVICE, 0x04);
	rt2x00_set_field32(&reg, ARCSR2_LENGTH, get_duration(ACK_SIZE, 55));
	rt2x00pci_register_write(rt2x00dev, ARCSR4, reg);

	rt2x00pci_register_read(rt2x00dev, ARCSR5, &reg);
	rt2x00_set_field32(&reg, ARCSR5_SIGNAL, 0x03 | preamble);
	rt2x00_set_field32(&reg, ARCSR5_SERVICE, 0x84);
	rt2x00_set_field32(&reg, ARCSR2_LENGTH, get_duration(ACK_SIZE, 110));
	rt2x00pci_register_write(rt2x00dev, ARCSR5, reg);
}

static void rt2500pci_config_phymode(struct rt2x00_dev *rt2x00dev,
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

	rt2500pci_config_rate(rt2x00dev, rate->val2);
}

static void rt2500pci_config_mac_addr(struct rt2x00_dev *rt2x00dev, u8 *addr)
{
	u32 reg[2];

	memset(&reg, 0, sizeof(reg));
	memcpy(&reg, addr, ETH_ALEN);

	/*
	 * The MAC address is passed to us as an array of bytes,
	 * that array is little endian, so no need for byte ordering.
	 */
	rt2x00pci_register_multiwrite(rt2x00dev, CSR3, &reg, sizeof(reg));
}

/*
 * LED functions.
 */
static void rt2500pci_enable_led(struct rt2x00_dev *rt2x00dev)
{
	u32 reg;

	rt2x00pci_register_read(rt2x00dev, LEDCSR, &reg);

	rt2x00_set_field32(&reg, LEDCSR_ON_PERIOD, 70);
	rt2x00_set_field32(&reg, LEDCSR_OFF_PERIOD, 30);

	if (rt2x00dev->led_mode == LED_MODE_TXRX_ACTIVITY) {
		rt2x00_set_field32(&reg, LEDCSR_LINK, 1);
		rt2x00_set_field32(&reg, LEDCSR_ACTIVITY, 0);
	} else if (rt2x00dev->led_mode == LED_MODE_ASUS) {
		rt2x00_set_field32(&reg, LEDCSR_LINK, 0);
		rt2x00_set_field32(&reg, LEDCSR_ACTIVITY, 1);
	} else {
		rt2x00_set_field32(&reg, LEDCSR_LINK, 1);
		rt2x00_set_field32(&reg, LEDCSR_ACTIVITY, 1);
	}

	rt2x00pci_register_write(rt2x00dev, LEDCSR, reg);
}

static void rt2500pci_disable_led(struct rt2x00_dev *rt2x00dev)
{
	u32 reg;

	rt2x00pci_register_read(rt2x00dev, LEDCSR, &reg);
	rt2x00_set_field32(&reg, LEDCSR_LINK, 0);
	rt2x00_set_field32(&reg, LEDCSR_ACTIVITY, 0);
	rt2x00pci_register_write(rt2x00dev, LEDCSR, reg);
}

/*
 * Link tuning
 */
static void rt2500pci_link_tuner(struct rt2x00_dev *rt2x00dev, int rssi)
{
	u32 reg;
	u8 r17;

	/*
	 * To prevent collisions with MAC ASIC on chipsets
	 * up to version C the link tuning should halt after 20
	 * seconds.
	 */
	if (rt2x00_rev(&rt2x00dev->chip) < RT2560_VERSION_D &&
	    rt2x00dev->link.count > 20)
		return;

	rt2500pci_bbp_read(rt2x00dev, 17, &r17);

	/*
	 * Chipset versions C and lower should directly continue
	 * to the dynamic CCA tuning.
	 */
	if (rt2x00_rev(&rt2x00dev->chip) < RT2560_VERSION_D)
		goto dynamic_cca_tune;

	/*
	 * A too low RSSI will cause too much false CCA which will
	 * then corrupt the R17 tuning. To remidy this the tuning should
	 * be stopped (While making sure the R17 value will not exceed limits)
	 */
	if (rssi < -80 && rt2x00dev->link.count > 20) {
		if (r17 >= 0x41) {
			r17 = rt2x00dev->link.curr_noise;
			rt2500pci_bbp_write(rt2x00dev, 17, r17);
		}
		return;
	}

	/*
	 * Special big-R17 for short distance
	 */
	if (rssi >= -58) {
		if (r17 != 0x50)
			rt2500pci_bbp_write(rt2x00dev, 17, 0x50);
		return;
	}

	/*
	 * Special mid-R17 for middle distance
	 */
	if (rssi >= -74) {
		if (r17 != 0x41)
			rt2500pci_bbp_write(rt2x00dev, 17, 0x41);
		return;
	}

	/*
	 * Leave short or middle distance condition, restore r17
	 * to the dynamic tuning range.
	 */
	if (r17 >= 0x41) {
		rt2500pci_bbp_write(rt2x00dev, 17, rt2x00dev->link.curr_noise);
		return;
	}

dynamic_cca_tune:

	/*
	 * R17 is inside the dynamic tuning range,
	 * start tuning the link based on the false cca counter.
	 */
	rt2x00pci_register_read(rt2x00dev, CNT3, &reg);
	rt2x00dev->link.false_cca = rt2x00_get_field32(reg, CNT3_FALSE_CCA);

	if (rt2x00dev->link.false_cca > 512 && r17 < 0x40) {
		rt2500pci_bbp_write(rt2x00dev, 17, ++r17);
		rt2x00dev->link.curr_noise = r17;
	} else if (rt2x00dev->link.false_cca < 100 && r17 > 0x32) {
		rt2500pci_bbp_write(rt2x00dev, 17, --r17);
		rt2x00dev->link.curr_noise = r17;
	}
}

/*
 * Initialization functions.
 */
static void rt2500pci_init_rxring(struct rt2x00_dev *rt2x00dev)
{
	struct data_desc *rxd;
	unsigned int i;
	u32 word;

	memset(rt2x00dev->rx->data_addr, 0x00,
		rt2x00_get_ring_size(rt2x00dev->rx));

	for (i = 0; i < rt2x00dev->rx->stats.limit; i++) {
		rxd = rt2x00dev->rx->entry[i].priv;

		rt2x00_desc_read(rxd, 1, &word);
		rt2x00_set_field32(&word, RXD_W1_BUFFER_ADDRESS,
			rt2x00dev->rx->entry[i].data_dma);
		rt2x00_desc_write(rxd, 1, word);

		rt2x00_desc_read(rxd, 0, &word);
		rt2x00_set_field32(&word, RXD_W0_OWNER_NIC, 1);
		rt2x00_desc_write(rxd, 0, word);
	}

	rt2x00_ring_index_clear(rt2x00dev->rx);
}

static void rt2500pci_init_txring(struct rt2x00_dev *rt2x00dev,
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
		rt2x00_set_field32(&word, TXD_W1_BUFFER_ADDRESS,
			ring->entry[i].data_dma);
		rt2x00_desc_write(txd, 1, word);

		rt2x00_desc_read(txd, 0, &word);
		rt2x00_set_field32(&word, TXD_W0_VALID, 0);
		rt2x00_set_field32(&word, TXD_W0_OWNER_NIC, 0);
		rt2x00_desc_write(txd, 0, word);
	}

	rt2x00_ring_index_clear(ring);
}

static int rt2500pci_init_rings(struct rt2x00_dev *rt2x00dev)
{
	u32 reg;

	/*
	 * Initialize rings.
	 */
	rt2500pci_init_rxring(rt2x00dev);
	rt2500pci_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_DATA0);
	rt2500pci_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_DATA1);
	rt2500pci_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_AFTER_BEACON);
	rt2500pci_init_txring(rt2x00dev, IEEE80211_TX_QUEUE_BEACON);

	/*
	 * Initialize registers.
	 */
	rt2x00pci_register_read(rt2x00dev, TXCSR2, &reg);
	rt2x00_set_field32(&reg, TXCSR2_TXD_SIZE,
		rt2x00dev->tx[IEEE80211_TX_QUEUE_DATA0].desc_size);
	rt2x00_set_field32(&reg, TXCSR2_NUM_TXD,
		rt2x00dev->tx[IEEE80211_TX_QUEUE_DATA1].stats.limit);
	rt2x00_set_field32(&reg, TXCSR2_NUM_ATIM,
		rt2x00dev->bcn[1].stats.limit);
	rt2x00_set_field32(&reg, TXCSR2_NUM_PRIO,
		rt2x00dev->tx[IEEE80211_TX_QUEUE_DATA0].stats.limit);
	rt2x00pci_register_write(rt2x00dev, TXCSR2, reg);

	rt2x00pci_register_read(rt2x00dev, TXCSR3, &reg);
	rt2x00_set_field32(&reg, TXCSR3_TX_RING_REGISTER,
		rt2x00dev->tx[IEEE80211_TX_QUEUE_DATA1].data_dma);
	rt2x00pci_register_write(rt2x00dev, TXCSR3, reg);

	rt2x00pci_register_read(rt2x00dev, TXCSR5, &reg);
	rt2x00_set_field32(&reg, TXCSR5_PRIO_RING_REGISTER,
		rt2x00dev->tx[IEEE80211_TX_QUEUE_DATA0].data_dma);
	rt2x00pci_register_write(rt2x00dev, TXCSR5, reg);

	rt2x00pci_register_read(rt2x00dev, TXCSR4, &reg);
	rt2x00_set_field32(&reg, TXCSR4_ATIM_RING_REGISTER,
		rt2x00dev->bcn[1].data_dma);
	rt2x00pci_register_write(rt2x00dev, TXCSR4, reg);

	rt2x00pci_register_read(rt2x00dev, TXCSR6, &reg);
	rt2x00_set_field32(&reg, TXCSR6_BEACON_RING_REGISTER,
		rt2x00dev->bcn[0].data_dma);
	rt2x00pci_register_write(rt2x00dev, TXCSR6, reg);

	rt2x00pci_register_read(rt2x00dev, RXCSR1, &reg);
	rt2x00_set_field32(&reg, RXCSR1_RXD_SIZE,
		rt2x00dev->rx->desc_size);
	rt2x00_set_field32(&reg, RXCSR1_NUM_RXD,
		rt2x00dev->rx->stats.limit);
	rt2x00pci_register_write(rt2x00dev, RXCSR1, reg);

	rt2x00pci_register_read(rt2x00dev, RXCSR2, &reg);
	rt2x00_set_field32(&reg, RXCSR2_RX_RING_REGISTER,
		rt2x00dev->rx->data_dma);
	rt2x00pci_register_write(rt2x00dev, RXCSR2, reg);

	return 0;
}

static int rt2500pci_init_registers(struct rt2x00_dev *rt2x00dev)
{
	u32 reg;

	if (rt2x00dev->ops->lib->set_device_state(rt2x00dev, STATE_AWAKE))
		return -EBUSY;

	rt2x00pci_register_write(rt2x00dev, PWRCSR0, 0x3f3b3100);
	rt2x00pci_register_write(rt2x00dev, PCICSR, 0x000003b8);

	rt2x00pci_register_write(rt2x00dev, PSCSR0, 0x00020002);
	rt2x00pci_register_write(rt2x00dev, PSCSR1, 0x00000002);
	rt2x00pci_register_write(rt2x00dev, PSCSR2, 0x00020002);
	rt2x00pci_register_write(rt2x00dev, PSCSR3, 0x00000002);

	rt2x00pci_register_read(rt2x00dev, TIMECSR, &reg);
	rt2x00_set_field32(&reg, TIMECSR_US_COUNT, 33);
	rt2x00_set_field32(&reg, TIMECSR_US_64_COUNT, 63);
	rt2x00_set_field32(&reg, TIMECSR_BEACON_EXPECT, 0);
	rt2x00pci_register_write(rt2x00dev, TIMECSR, reg);

	rt2x00pci_register_read(rt2x00dev, CSR9, &reg);
	rt2x00_set_field32(&reg, CSR9_MAX_FRAME_UNIT,
		rt2x00dev->rx->data_size / 128);
	rt2x00pci_register_write(rt2x00dev, CSR9, reg);

	rt2x00pci_register_write(rt2x00dev, CNT3, 0);

	rt2x00pci_register_write(rt2x00dev, GPIOCSR, 0x0000ff00);
	rt2x00pci_register_write(rt2x00dev, TESTCSR, 0x000000f0);

	rt2x00pci_register_write(rt2x00dev, MACCSR0, 0x00213223);
	rt2x00pci_register_write(rt2x00dev, MACCSR1, 0x00235518);

	rt2x00pci_register_read(rt2x00dev, MACCSR2, &reg);
	rt2x00_set_field32(&reg, MACCSR2_DELAY, 64);
	rt2x00pci_register_write(rt2x00dev, MACCSR2, reg);

	/*
	 * Always use CWmin and CWmax set in descriptor.
	 */
	rt2x00pci_register_read(rt2x00dev, CSR11, &reg);
	rt2x00_set_field32(&reg, CSR11_CW_SELECT, 0);
	rt2x00pci_register_write(rt2x00dev, CSR11, reg);

	rt2x00pci_register_read(rt2x00dev, RXCSR3, &reg);
	/*
	 * Signal.
	 */
	rt2x00_set_field32(&reg, RXCSR3_BBP_ID0, 47);
	rt2x00_set_field32(&reg, RXCSR3_BBP_ID0_VALID, 1);
	/*
	 * Rssi.
	 */
	rt2x00_set_field32(&reg, RXCSR3_BBP_ID1, 51);
	rt2x00_set_field32(&reg, RXCSR3_BBP_ID1_VALID, 1);
	/*
	 * OFDM Rate.
	 */
	rt2x00_set_field32(&reg, RXCSR3_BBP_ID2, 42);
	rt2x00_set_field32(&reg, RXCSR3_BBP_ID2_VALID, 1);
	/*
	 * OFDM.
	 */
	rt2x00_set_field32(&reg, RXCSR3_BBP_ID3, 51);
	rt2x00_set_field32(&reg, RXCSR3_BBP_ID3_VALID, 1);
	rt2x00pci_register_write(rt2x00dev, RXCSR3, reg);

	rt2x00pci_register_read(rt2x00dev, RALINKCSR, &reg);
	rt2x00_set_field32(&reg, RALINKCSR_AR_BBP_DATA0, 17);
	rt2x00_set_field32(&reg, RALINKCSR_AR_BBP_ID0, 26);
	rt2x00_set_field32(&reg, RALINKCSR_AR_BBP_VALID0, 1);
	rt2x00_set_field32(&reg, RALINKCSR_AR_BBP_DATA1, 0);
	rt2x00_set_field32(&reg, RALINKCSR_AR_BBP_ID1, 26);
	rt2x00_set_field32(&reg, RALINKCSR_AR_BBP_VALID1, 1);
	rt2x00pci_register_write(rt2x00dev, RALINKCSR, reg);

	rt2x00pci_register_write(rt2x00dev, BBPCSR1, 0x82188200);

	rt2x00pci_register_write(rt2x00dev, TXACKCSR0, 0x00000020);

	rt2x00pci_register_write(rt2x00dev, ARTCSR0, 0x7038140a);
	rt2x00pci_register_write(rt2x00dev, ARTCSR1, 0x1d21252d);
	rt2x00pci_register_write(rt2x00dev, ARTCSR2, 0x1919191d);

	rt2x00pci_register_read(rt2x00dev, CSR1, &reg);
	rt2x00_set_field32(&reg, CSR1_SOFT_RESET, 1);
	rt2x00_set_field32(&reg, CSR1_BBP_RESET, 0);
	rt2x00_set_field32(&reg, CSR1_HOST_READY, 0);
	rt2x00pci_register_write(rt2x00dev, CSR1, reg);

	rt2x00pci_register_read(rt2x00dev, CSR1, &reg);
	rt2x00_set_field32(&reg, CSR1_SOFT_RESET, 0);
	rt2x00_set_field32(&reg, CSR1_HOST_READY, 1);
	rt2x00pci_register_write(rt2x00dev, CSR1, reg);

	/*
	 * We must clear the FCS and FIFO error count.
	 * These registers are cleared on read,
	 * so we may pass a useless variable to store the value.
	 */
	rt2x00pci_register_read(rt2x00dev, CNT0, &reg);
	rt2x00pci_register_read(rt2x00dev, CNT4, &reg);

	return 0;
}

static int rt2500pci_init_bbp(struct rt2x00_dev *rt2x00dev)
{
	unsigned int i;
	u16 eeprom;
	u8 reg_id;
	u8 value;

	for (i = 0; i < REGISTER_BUSY_COUNT; i++) {
		rt2500pci_bbp_read(rt2x00dev, 0, &value);
		if ((value != 0xff) && (value != 0x00))
			goto continue_csr_init;
		NOTICE(rt2x00dev, "Waiting for BBP register.\n");
		udelay(REGISTER_BUSY_DELAY);
	}

	ERROR(rt2x00dev, "BBP register access failed, aborting.\n");
	return -EACCES;

continue_csr_init:
	rt2500pci_bbp_write(rt2x00dev, 3, 0x02);
	rt2500pci_bbp_write(rt2x00dev, 4, 0x19);
	rt2500pci_bbp_write(rt2x00dev, 14, 0x1c);
	rt2500pci_bbp_write(rt2x00dev, 15, 0x30);
	rt2500pci_bbp_write(rt2x00dev, 16, 0xac);
	rt2500pci_bbp_write(rt2x00dev, 17, 0x48);
	rt2500pci_bbp_write(rt2x00dev, 18, 0x18);
	rt2500pci_bbp_write(rt2x00dev, 19, 0xff);
	rt2500pci_bbp_write(rt2x00dev, 20, 0x1e);
	rt2500pci_bbp_write(rt2x00dev, 21, 0x08);
	rt2500pci_bbp_write(rt2x00dev, 22, 0x08);
	rt2500pci_bbp_write(rt2x00dev, 23, 0x08);
	rt2500pci_bbp_write(rt2x00dev, 24, 0x70);
	rt2500pci_bbp_write(rt2x00dev, 25, 0x40);
	rt2500pci_bbp_write(rt2x00dev, 26, 0x08);
	rt2500pci_bbp_write(rt2x00dev, 27, 0x23);
	rt2500pci_bbp_write(rt2x00dev, 30, 0x10);
	rt2500pci_bbp_write(rt2x00dev, 31, 0x2b);
	rt2500pci_bbp_write(rt2x00dev, 32, 0xb9);
	rt2500pci_bbp_write(rt2x00dev, 34, 0x12);
	rt2500pci_bbp_write(rt2x00dev, 35, 0x50);
	rt2500pci_bbp_write(rt2x00dev, 39, 0xc4);
	rt2500pci_bbp_write(rt2x00dev, 40, 0x02);
	rt2500pci_bbp_write(rt2x00dev, 41, 0x60);
	rt2500pci_bbp_write(rt2x00dev, 53, 0x10);
	rt2500pci_bbp_write(rt2x00dev, 54, 0x18);
	rt2500pci_bbp_write(rt2x00dev, 56, 0x08);
	rt2500pci_bbp_write(rt2x00dev, 57, 0x10);
	rt2500pci_bbp_write(rt2x00dev, 58, 0x08);
	rt2500pci_bbp_write(rt2x00dev, 61, 0x6d);
	rt2500pci_bbp_write(rt2x00dev, 62, 0x10);

	DEBUG(rt2x00dev, "Start initialization from EEPROM...\n");
	for (i = 0; i < EEPROM_BBP_SIZE; i++) {
		rt2x00_eeprom_read(rt2x00dev, EEPROM_BBP_START + i, &eeprom);

		if (eeprom != 0xffff && eeprom != 0x0000) {
			reg_id = rt2x00_get_field16(eeprom, EEPROM_BBP_REG_ID);
			value = rt2x00_get_field16(eeprom, EEPROM_BBP_VALUE);
			DEBUG(rt2x00dev, "BBP: 0x%02x, value: 0x%02x.\n",
				reg_id, value);
			rt2500pci_bbp_write(rt2x00dev, reg_id, value);
		}
	}
	DEBUG(rt2x00dev, "...End initialization from EEPROM.\n");

	return 0;
}

/*
 * Device state switch handlers.
 */
static void rt2500pci_toggle_rx(struct rt2x00_dev *rt2x00dev,
	enum dev_state state)
{
	u32 reg;

	rt2x00pci_register_read(rt2x00dev, RXCSR0, &reg);
	rt2x00_set_field32(&reg, RXCSR0_DISABLE_RX,
		state == STATE_RADIO_RX_OFF);
	rt2x00pci_register_write(rt2x00dev, RXCSR0, reg);
}

static int rt2500pci_enable_radio(struct rt2x00_dev *rt2x00dev)
{
	u32 reg;

	/*
	 * Initialize all registers.
	 */
	if (rt2500pci_init_rings(rt2x00dev) ||
	    rt2500pci_init_registers(rt2x00dev) ||
	    rt2500pci_init_bbp(rt2x00dev)) {
		ERROR(rt2x00dev, "Register initialization failed.\n");
		return -EIO;
	}

	/*
	 * Clear interrupts.
	 */
	rt2x00pci_register_read(rt2x00dev, CSR7, &reg);
	rt2x00pci_register_write(rt2x00dev, CSR7, reg);

	/*
	 * Enable interrupts.
	 */
	rt2x00pci_register_read(rt2x00dev, CSR8, &reg);
	rt2x00_set_field32(&reg, CSR8_TBCN_EXPIRE, 0);
	rt2x00_set_field32(&reg, CSR8_TXDONE_TXRING, 0);
	rt2x00_set_field32(&reg, CSR8_TXDONE_ATIMRING, 0);
	rt2x00_set_field32(&reg, CSR8_TXDONE_PRIORING, 0);
	rt2x00_set_field32(&reg, CSR8_RXDONE, 0);
	rt2x00pci_register_write(rt2x00dev, CSR8, reg);

	/*
	 * Enable LED
	 */
	rt2500pci_enable_led(rt2x00dev);

	return 0;
}

static void rt2500pci_disable_radio(struct rt2x00_dev *rt2x00dev)
{
	u32 reg;

	/*
	 * Disable LED
	 */
	rt2500pci_disable_led(rt2x00dev);

	rt2x00pci_register_write(rt2x00dev, PWRCSR0, 0);

	/*
	 * Disable synchronisation.
	 */
	rt2x00pci_register_write(rt2x00dev, CSR14, 0);

	/*
	 * Cancel RX and TX.
	 */
	rt2x00pci_register_read(rt2x00dev, TXCSR0, &reg);
	rt2x00_set_field32(&reg, TXCSR0_ABORT, 1);
	rt2x00pci_register_write(rt2x00dev, TXCSR0, reg);

	/*
	 * Disable interrupts.
	 */
	rt2x00pci_register_read(rt2x00dev, CSR8, &reg);
	rt2x00_set_field32(&reg, CSR8_TBCN_EXPIRE, 1);
	rt2x00_set_field32(&reg, CSR8_TXDONE_TXRING, 1);
	rt2x00_set_field32(&reg, CSR8_TXDONE_ATIMRING, 1);
	rt2x00_set_field32(&reg, CSR8_TXDONE_PRIORING, 1);
	rt2x00_set_field32(&reg, CSR8_RXDONE, 1);
	rt2x00pci_register_write(rt2x00dev, CSR8, reg);
}

static int rt2500pci_set_state(struct rt2x00_dev *rt2x00dev,
	enum dev_state state)
{
	u32 reg;
	unsigned int i;
	char put_to_sleep;
	char bbp_state;
	char rf_state;

	put_to_sleep = (state != STATE_AWAKE);

	rt2x00pci_register_read(rt2x00dev, PWRCSR1, &reg);
	rt2x00_set_field32(&reg, PWRCSR1_SET_STATE, 1);
	rt2x00_set_field32(&reg, PWRCSR1_BBP_DESIRE_STATE, state);
	rt2x00_set_field32(&reg, PWRCSR1_RF_DESIRE_STATE, state);
	rt2x00_set_field32(&reg, PWRCSR1_PUT_TO_SLEEP, put_to_sleep);
	rt2x00pci_register_write(rt2x00dev, PWRCSR1, reg);

	/*
	 * Device is not guaranteed to be in the requested state yet.
	 * We must wait until the register indicates that the
	 * device has entered the correct state.
	 */
	for (i = 0; i < REGISTER_BUSY_COUNT; i++) {
		rt2x00pci_register_read(rt2x00dev, PWRCSR1, &reg);
		bbp_state = rt2x00_get_field32(reg, PWRCSR1_BBP_CURR_STATE);
		rf_state = rt2x00_get_field32(reg, PWRCSR1_RF_CURR_STATE);
		if (bbp_state == state && rf_state == state)
			return 0;
		msleep(10);
	}

	NOTICE(rt2x00dev, "Device failed to enter state %d, "
		"current device state: bbp %d and rf %d.\n",
		state, bbp_state, rf_state);

	return -EBUSY;
}

static int rt2500pci_set_device_state(struct rt2x00_dev *rt2x00dev,
	enum dev_state state)
{
	int retval = 0;

	switch (state) {
		case STATE_RADIO_ON:
			retval = rt2500pci_enable_radio(rt2x00dev);
		break;
		case STATE_RADIO_OFF:
			rt2500pci_disable_radio(rt2x00dev);
		break;
		case STATE_RADIO_RX_ON:
		case STATE_RADIO_RX_OFF:
			rt2500pci_toggle_rx(rt2x00dev, state);
		break;
		case STATE_DEEP_SLEEP:
		case STATE_SLEEP:
		case STATE_STANDBY:
		case STATE_AWAKE:
			retval = rt2500pci_set_state(rt2x00dev, state);
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
static void rt2500pci_write_tx_desc(struct rt2x00_dev *rt2x00dev,
	struct data_entry *entry, struct data_desc *txd,
	struct data_entry_desc *desc, struct ieee80211_hdr *ieee80211hdr,
	unsigned int length, struct ieee80211_tx_control *control)
{
	u32 word;

	/*
	 * Start writing the descriptor words.
	 */
	rt2x00_desc_read(txd, 2, &word);
	rt2x00_set_field32(&word, TXD_W2_IV_OFFSET, IEEE80211_HEADER);
	rt2x00_set_field32(&word, TXD_W2_AIFS, entry->ring->tx_params.aifs);
	rt2x00_set_field32(&word, TXD_W2_CWMIN, entry->ring->tx_params.cw_min);
	rt2x00_set_field32(&word, TXD_W2_CWMAX, entry->ring->tx_params.cw_max);
	rt2x00_desc_write(txd, 2, word);

	rt2x00_desc_read(txd, 3, &word);
	rt2x00_set_field32(&word, TXD_W3_PLCP_SIGNAL, desc->signal);
	rt2x00_set_field32(&word, TXD_W3_PLCP_SERVICE, desc->service);
	rt2x00_set_field32(&word, TXD_W3_PLCP_LENGTH_LOW, desc->length_low);
	rt2x00_set_field32(&word, TXD_W3_PLCP_LENGTH_HIGH, desc->length_high);
	rt2x00_desc_write(txd, 3, word);

	rt2x00_desc_read(txd, 10, &word);
	rt2x00_set_field32(&word, TXD_W10_RTS,
		test_bit(ENTRY_TXD_RTS_FRAME, &entry->flags));
	rt2x00_desc_write(txd, 10, word);

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
	rt2x00_set_field32(&word, TXD_W0_CIPHER_OWNER, 1);
	rt2x00_set_field32(&word, TXD_W0_IFS, desc->ifs);
	rt2x00_set_field32(&word, TXD_W0_RETRY_MODE, 0);
	rt2x00_set_field32(&word, TXD_W0_DATABYTE_COUNT, length);
	rt2x00_set_field32(&word, TXD_W0_CIPHER_ALG, CIPHER_NONE);
	rt2x00_desc_write(txd, 0, word);
}

/*
 * TX data initialization
 */
static void rt2500pci_kick_tx_queue(struct rt2x00_dev *rt2x00dev, int queue)
{
	u32 reg;

	if (queue == IEEE80211_TX_QUEUE_BEACON) {
		rt2x00pci_register_read(rt2x00dev, CSR14, &reg);
		if (!rt2x00_get_field32(reg, CSR14_BEACON_GEN)) {
			rt2x00_set_field32(&reg, CSR14_BEACON_GEN, 1);
			rt2x00pci_register_write(rt2x00dev, CSR14, reg);
		}
		return;
	}

	rt2x00pci_register_read(rt2x00dev, TXCSR0, &reg);
	if (queue == IEEE80211_TX_QUEUE_DATA0)
		rt2x00_set_field32(&reg, TXCSR0_KICK_PRIO, 1);
	else if (queue == IEEE80211_TX_QUEUE_DATA1)
		rt2x00_set_field32(&reg, TXCSR0_KICK_TX, 1);
	else if (queue == IEEE80211_TX_QUEUE_AFTER_BEACON)
		rt2x00_set_field32(&reg, TXCSR0_KICK_ATIM, 1);
	rt2x00pci_register_write(rt2x00dev, TXCSR0, reg);
}

/*
 * Interrupt functions.
 */
static void rt2500pci_rxdone(struct rt2x00_dev *rt2x00dev)
{
	struct data_ring *ring = rt2x00dev->rx;
	struct data_entry *entry;
	struct data_desc *rxd;
	u32 word0;
	u32 word2;
	int signal;
	int rssi;
	int ofdm;
	u16 size;

	while (1) {
		entry = rt2x00_get_data_entry(ring);
		rxd = entry->priv;
		rt2x00_desc_read(rxd, 0, &word0);
		rt2x00_desc_read(rxd, 2, &word2);

		if (rt2x00_get_field32(word0, RXD_W0_OWNER_NIC))
			break;

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
		size = rt2x00_get_field32(word0, RXD_W0_DATABYTE_COUNT);
		signal = rt2x00_get_field32(word2, RXD_W2_SIGNAL);
		rssi = rt2x00_get_field32(word2, RXD_W2_RSSI);
		ofdm = rt2x00_get_field32(word0, RXD_W0_OFDM);

		/*
		 * Send the packet to upper layer.
		 */
		rt2x00lib_rxdone(entry, entry->data_addr, size,
			signal, rssi, ofdm);

skip_entry:
		if (test_bit(DEVICE_ENABLED_RADIO, &ring->rt2x00dev->flags)) {
			rt2x00_set_field32(&word0, RXD_W0_OWNER_NIC, 1);
			rt2x00_desc_write(rxd, 0, word0);
		}

		rt2x00_ring_index_inc(ring);
	}
}

static void rt2500pci_txdone(struct rt2x00_dev *rt2x00dev, const int queue)
{
	struct data_ring *ring = rt2x00_get_ring(rt2x00dev, queue);
	struct data_entry *entry;
	struct data_desc *txd;
	u32 word;
	int tx_status;
	int retry;

	while (!rt2x00_ring_empty(ring)) {
		entry = rt2x00_get_data_entry_done(ring);
		txd = entry->priv;
		rt2x00_desc_read(txd, 0, &word);

		if (rt2x00_get_field32(word, TXD_W0_OWNER_NIC) ||
		    !rt2x00_get_field32(word, TXD_W0_VALID))
			break;

		/*
		 * Obtain the status about this packet.
		 */
		tx_status = rt2x00_get_field32(word, TXD_W0_RESULT);
		retry = rt2x00_get_field32(word, TXD_W0_RETRY_COUNT);

		rt2x00lib_txdone(entry, tx_status, retry);

		/*
		 * Make this entry available for reuse.
		 */
		entry->flags = 0;
		rt2x00_set_field32(&word, TXD_W0_VALID, 0);
		rt2x00_desc_write(txd, 0, word);
		rt2x00_ring_index_done_inc(ring);
	}

	/*
	 * If the data ring was full before the txdone handler
	 * we must make sure the packet queue in the mac80211 stack
	 * is reenabled when the txdone handler has finished.
	 */
	entry = ring->entry;
	if (!rt2x00_ring_full(ring))
		ieee80211_wake_queue(rt2x00dev->hw,
			entry->tx_status.control.queue);
}

static irqreturn_t rt2500pci_interrupt(int irq, void *dev_instance)
{
	struct rt2x00_dev *rt2x00dev = dev_instance;
	u32 reg;

	/*
	 * Get the interrupt sources & saved to local variable.
	 * Write register value back to clear pending interrupts.
	 */
	rt2x00pci_register_read(rt2x00dev, CSR7, &reg);
	rt2x00pci_register_write(rt2x00dev, CSR7, reg);

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
	if (rt2x00_get_field32(reg, CSR7_TBCN_EXPIRE))
		rt2x00pci_beacondone(rt2x00dev, IEEE80211_TX_QUEUE_BEACON);

	/*
	 * 2 - Rx ring done interrupt.
	 */
	if (rt2x00_get_field32(reg, CSR7_RXDONE))
		rt2500pci_rxdone(rt2x00dev);

	/*
	 * 3 - Atim ring transmit done interrupt.
	 */
	if (rt2x00_get_field32(reg, CSR7_TXDONE_ATIMRING))
		rt2500pci_txdone(rt2x00dev, IEEE80211_TX_QUEUE_AFTER_BEACON);

	/*
	 * 4 - Priority ring transmit done interrupt.
	 */
	if (rt2x00_get_field32(reg, CSR7_TXDONE_PRIORING))
		rt2500pci_txdone(rt2x00dev, IEEE80211_TX_QUEUE_DATA0);

	/*
	 * 5 - Tx ring transmit done interrupt.
	 */
	if (rt2x00_get_field32(reg, CSR7_TXDONE_TXRING))
		rt2500pci_txdone(rt2x00dev, IEEE80211_TX_QUEUE_DATA1);

	return IRQ_HANDLED;
}

/*
 * Device initialization functions.
 */
static int rt2500pci_alloc_eeprom(struct rt2x00_dev *rt2x00dev)
{
	struct eeprom_93cx6 eeprom;
	u32 reg;
	u16 word;

	/*
	 * Allocate the eeprom memory, check the eeprom width
	 * and copy the entire eeprom into this allocated memory.
	 */
	rt2x00dev->eeprom = kzalloc(EEPROM_SIZE, GFP_KERNEL);
	if (!rt2x00dev->eeprom)
		return -ENOMEM;

	rt2x00pci_register_read(rt2x00dev, CSR21, &reg);

	eeprom.data = rt2x00dev;
	eeprom.register_read = rt2500pci_eepromregister_read;
	eeprom.register_write = rt2500pci_eepromregister_write;
	eeprom.width = rt2x00_get_field32(reg, CSR21_TYPE_93C46) ?
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

	return 0;
}

static int rt2500pci_init_eeprom(struct rt2x00_dev *rt2x00dev)
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
	rt2x00pci_register_read(rt2x00dev, CSR0, &reg);
	rt2x00_set_chip(rt2x00dev, RT2560, value, reg);

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
	 * Detect if this device has an hardware controlled radio.
	 */
	if (rt2x00_get_field16(eeprom, EEPROM_ANTENNA_HARDWARE_RADIO))
		__set_bit(DEVICE_SUPPORT_HW_BUTTON, &rt2x00dev->flags);

	/*
	 * Check if the BBP tuning should be enabled.
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
	{ RF2525E,	{ 0x00022020, 0x00060111, 0x00000a0b } },
	{ RF5222,	{ 0x00000000, 0x00000101, 0x00000000 } },
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
 * RF value list for RF2525E & RF5222
 * Supports: 2.4 GHz
 */
static const u32 rf_vals_bg_5x[] = {
	0x00001136, 0x0000113a, 0x0000113e, 0x00001182, 0x00001186,
	0x0000118a, 0x0000118e, 0x00001192, 0x00001196, 0x0000119a,
	0x0000119e, 0x000011a2, 0x000011a6, 0x000011ae
};

/*
 * RF value list for RF5222 (supplement to rf_vals_bg_5x)
 * Supports: 5.2 GHz
 */
static const u32 rf_vals_a_5x[] = {
	0x00018896, 0x0001889a, 0x0001889e, 0x000188a2, 0x000188a6,
	0x000188aa, 0x000188ae, 0x000188b2, 0x00008802, 0x00008806,
	0x0000880a, 0x0000880e, 0x00008812, 0x00008816, 0x0000881a,
	0x0000881e, 0x00008822, 0x00008826, 0x0000882a, 0x000090a6,
	0x000090ae, 0x000090b6, 0x000090be
};

static void rt2500pci_init_hw_mode(struct rt2x00_dev *rt2x00dev)
{
	struct hw_mode_spec *spec = &rt2x00dev->spec;
	u8 *txpower;
	unsigned int i;

	/*
	 * Initialize all hw fields.
	 */
	rt2x00dev->hw->flags =  IEEE80211_HW_HOST_GEN_BEACON |
		IEEE80211_HW_HOST_BROADCAST_PS_BUFFERING |
		IEEE80211_HW_WEP_INCLUDE_IV |
		IEEE80211_HW_DATA_NULLFUNC_ACK |
		IEEE80211_HW_NO_TKIP_WMM_HWACCEL |
		IEEE80211_HW_MONITOR_DURING_OPER;
	rt2x00dev->hw->extra_tx_headroom = 0;
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
	else if (rt2x00_rf(&rt2x00dev->chip, RF2525E) ||
		 rt2x00_rf(&rt2x00dev->chip, RF5222))
		spec->chan_val_bg = rf_vals_bg_5x;

	if (rt2x00_rf(&rt2x00dev->chip, RF5222)) {
		spec->num_modes = 3;
		spec->num_channels += 23;
		spec->chan_val_a = rf_vals_a_5x;
	}
}

static int rt2500pci_init_hw(struct rt2x00_dev *rt2x00dev)
{
	int retval;

	/*
	 * Allocate eeprom data.
	 */
	retval = rt2500pci_alloc_eeprom(rt2x00dev);
	if (retval)
		return retval;

	retval = rt2500pci_init_eeprom(rt2x00dev);
	if (retval)
		return retval;

	/*
	 * Initialize hw specifications.
	 */
	rt2500pci_init_hw_mode(rt2x00dev);

	return 0;
}

/*
 * IEEE80211 stack callback functions.
 */
static int rt2500pci_get_stats(struct ieee80211_hw *hw,
	struct ieee80211_low_level_stats *stats)
{
	struct rt2x00_dev *rt2x00dev = hw->priv;
	u32 reg;

	/*
	 * Update FCS error count from register.
	 * The dot11ACKFailureCount, dot11RTSFailureCount and
	 * dot11RTSSuccessCount are updated in interrupt time.
	 */
	rt2x00pci_register_read(rt2x00dev, CNT0, &reg);
	rt2x00dev->low_level_stats.dot11FCSErrorCount +=
		rt2x00_get_field32(reg, CNT0_FCS_ERROR);

	memcpy(stats, &rt2x00dev->low_level_stats, sizeof(*stats));

	return 0;
}

static int rt2500pci_set_retry_limit(struct ieee80211_hw *hw,
	u32 short_retry, u32 long_retry)
{
	struct rt2x00_dev *rt2x00dev = hw->priv;
	u32 reg;

	rt2x00pci_register_read(rt2x00dev, CSR11, &reg);
	rt2x00_set_field32(&reg, CSR11_LONG_RETRY, long_retry);
	rt2x00_set_field32(&reg, CSR11_SHORT_RETRY, short_retry);
	rt2x00pci_register_write(rt2x00dev, CSR11, reg);

	return 0;
}

static u64 rt2500pci_get_tsf(struct ieee80211_hw *hw)
{
	struct rt2x00_dev *rt2x00dev = hw->priv;
	u64 tsf;
	u32 reg;

	rt2x00pci_register_read(rt2x00dev, CSR17, &reg);
	tsf = (u64)rt2x00_get_field32(reg, CSR17_HIGH_TSFTIMER) << 32;
	rt2x00pci_register_read(rt2x00dev, CSR16, &reg);
	tsf |= rt2x00_get_field32(reg, CSR16_LOW_TSFTIMER);

	return tsf;
}

static void rt2500pci_reset_tsf(struct ieee80211_hw *hw)
{
	struct rt2x00_dev *rt2x00dev = hw->priv;

	rt2x00pci_register_write(rt2x00dev, CSR16, 0);
	rt2x00pci_register_write(rt2x00dev, CSR17, 0);
}

static int rt2500pci_tx_last_beacon(struct ieee80211_hw *hw)
{
	struct rt2x00_dev *rt2x00dev = hw->priv;
	u32 reg;

	rt2x00pci_register_read(rt2x00dev, CSR15, &reg);
	return rt2x00_get_field32(reg, CSR15_BEACON_SENT);
}

static const struct ieee80211_ops rt2500pci_mac80211_ops = {
	.tx			= rt2x00lib_tx,
	.reset			= rt2x00lib_reset,
	.open			= rt2x00lib_open,
	.stop			= rt2x00lib_stop,
	.add_interface		= rt2x00lib_add_interface,
	.remove_interface	= rt2x00lib_remove_interface,
	.config			= rt2x00lib_config,
	.config_interface	= rt2x00lib_config_interface,
	.set_multicast_list	= rt2x00lib_set_multicast_list,
	.get_stats		= rt2500pci_get_stats,
	.set_retry_limit	= rt2500pci_set_retry_limit,
	.conf_tx		= rt2x00lib_conf_tx,
	.get_tx_stats		= rt2x00lib_get_tx_stats,
	.get_tsf		= rt2500pci_get_tsf,
	.reset_tsf		= rt2500pci_reset_tsf,
	.beacon_update		= rt2x00pci_beacon_update,
	.tx_last_beacon		= rt2500pci_tx_last_beacon,
};

static const struct rt2x00lib_ops rt2500pci_rt2x00_ops = {
	.irq_handler		= rt2500pci_interrupt,
	.init_hw		= rt2500pci_init_hw,
	.initialize		= rt2x00pci_initialize,
	.uninitialize		= rt2x00pci_uninitialize,
	.set_device_state	= rt2500pci_set_device_state,
#ifdef CONFIG_RT2500PCI_RFKILL
	.rfkill_poll		= rt2500pci_rfkill_poll,
#endif /* CONFIG_RT2500PCI_RFKILL */
	.link_tuner		= rt2500pci_link_tuner,
	.write_tx_desc		= rt2500pci_write_tx_desc,
	.write_tx_data		= rt2x00pci_write_tx_data,
	.kick_tx_queue		= rt2500pci_kick_tx_queue,
	.config_type		= rt2500pci_config_type,
	.config_phymode		= rt2500pci_config_phymode,
	.config_channel		= rt2500pci_config_channel,
	.config_mac_addr	= rt2500pci_config_mac_addr,
	.config_bssid		= rt2500pci_config_bssid,
	.config_promisc		= rt2500pci_config_promisc,
	.config_txpower		= rt2500pci_config_txpower,
	.config_antenna		= rt2500pci_config_antenna,
	.config_duration	= rt2500pci_config_duration,
};

static const struct rt2x00_ops rt2500pci_ops = {
	.name	= DRV_NAME,
	.rxd_size = RXD_DESC_SIZE,
	.txd_size = TXD_DESC_SIZE,
	.lib	= &rt2500pci_rt2x00_ops,
	.hw	= &rt2500pci_mac80211_ops,
#ifdef CONFIG_RT2X00_LIB_DEBUGFS
	.debugfs = &rt2500pci_rt2x00debug,
#endif /* CONFIG_RT2X00_LIB_DEBUGFS */
};

/*
 * RT2500pci module information.
 */
static struct pci_device_id rt2500pci_device_table[] = {
	{ PCI_DEVICE(0x1814, 0x0201), PCI_DEVICE_DATA(&rt2500pci_ops) },
	{ 0, }
};

MODULE_AUTHOR(DRV_PROJECT);
MODULE_VERSION(DRV_VERSION);
MODULE_DESCRIPTION("Ralink RT2500 PCI & PCMCIA Wireless LAN driver.");
MODULE_SUPPORTED_DEVICE("Ralink RT2560 PCI & PCMCIA chipset based cards");
MODULE_DEVICE_TABLE(pci, rt2500pci_device_table);
MODULE_LICENSE("GPL");

static struct pci_driver rt2500pci_driver = {
	.name		= DRV_NAME,
	.id_table	= rt2500pci_device_table,
	.probe		= rt2x00pci_probe,
	.remove		= __devexit_p(rt2x00pci_remove),
#ifdef CONFIG_PM
	.suspend	= rt2x00pci_suspend,
	.resume		= rt2x00pci_resume,
#endif /* CONFIG_PM */
};

static int __init rt2500pci_init(void)
{
	printk(KERN_INFO "Loading module: %s - %s by %s.\n",
		DRV_NAME, DRV_VERSION, DRV_PROJECT);
	return pci_register_driver(&rt2500pci_driver);
}

static void __exit rt2500pci_exit(void)
{
	printk(KERN_INFO "Unloading module: %s.\n", DRV_NAME);
	pci_unregister_driver(&rt2500pci_driver);
}

module_init(rt2500pci_init);
module_exit(rt2500pci_exit);
