/*

  Broadcom BCM43xx wireless driver

  Copyright (c) 2005 Martin Langer <martin-langer@gmx.de>
  Copyright (c) 2005 Stefano Brivio <st3@riseup.net>
  Copyright (c) 2005, 2006 Michael Buesch <mb@bu3sch.de>
  Copyright (c) 2005 Danny van Dyk <kugelfang@gentoo.org>
  Copyright (c) 2005 Andreas Jaggi <andreas.jaggi@waterwave.ch>

  Some parts of the code in this file are derived from the ipw2200
  driver  Copyright(c) 2003 - 2004 Intel Corporation.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
  Boston, MA 02110-1301, USA.

*/

#include <linux/delay.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/if_arp.h>
#include <linux/etherdevice.h>
#include <linux/version.h>
#include <linux/firmware.h>
#include <linux/wireless.h>
#include <linux/workqueue.h>
#include <linux/skbuff.h>
#include <linux/dma-mapping.h>

#include "bcm43xx.h"
#include "bcm43xx_main.h"
#include "bcm43xx_debugfs.h"
#include "bcm43xx_phy.h"
#include "bcm43xx_dma.h"
#include "bcm43xx_pio.h"
#include "bcm43xx_power.h"
#include "bcm43xx_sysfs.h"
#include "bcm43xx_xmit.h"
#include "bcm43xx_sysfs.h"
#include "bcm43xx_lo.h"
#include "bcm43xx_pcmcia.h"


MODULE_DESCRIPTION("Broadcom BCM43xx wireless driver");
MODULE_AUTHOR("Martin Langer");
MODULE_AUTHOR("Stefano Brivio");
MODULE_AUTHOR("Michael Buesch");
MODULE_LICENSE("GPL");


extern char *nvram_get(char *name);


#if defined(CONFIG_BCM43XX_MAC80211_DMA) && defined(CONFIG_BCM43XX_MAC80211_PIO)
static int modparam_pio;
module_param_named(pio, modparam_pio, int, 0444);
MODULE_PARM_DESC(pio, "enable(1) / disable(0) PIO mode");
#elif defined(CONFIG_BCM43XX_MAC80211_DMA)
# define modparam_pio	0
#elif defined(CONFIG_BCM43XX_MAC80211_PIO)
# define modparam_pio	1
#endif

static int modparam_bad_frames_preempt;
module_param_named(bad_frames_preempt, modparam_bad_frames_preempt, int, 0444);
MODULE_PARM_DESC(bad_frames_preempt, "enable(1) / disable(0) Bad Frames Preemption");

static int modparam_short_retry = BCM43xx_DEFAULT_SHORT_RETRY_LIMIT;
module_param_named(short_retry, modparam_short_retry, int, 0444);
MODULE_PARM_DESC(short_retry, "Short-Retry-Limit (0 - 15)");

static int modparam_long_retry = BCM43xx_DEFAULT_LONG_RETRY_LIMIT;
module_param_named(long_retry, modparam_long_retry, int, 0444);
MODULE_PARM_DESC(long_retry, "Long-Retry-Limit (0 - 15)");

static int modparam_noleds;
module_param_named(noleds, modparam_noleds, int, 0444);
MODULE_PARM_DESC(noleds, "Turn off all LED activity");

static char modparam_fwpostfix[16];
module_param_string(fwpostfix, modparam_fwpostfix, 16, 0444);
MODULE_PARM_DESC(fwpostfix, "Postfix for the .fw files to load.");

static int modparam_mon_keep_bad;
module_param_named(mon_keep_bad, modparam_mon_keep_bad, int, 0444);
MODULE_PARM_DESC(mon_keep_bad, "Keep bad frames in monitor mode");

static int modparam_mon_keep_badplcp;
module_param_named(mon_keep_badplcp, modparam_mon_keep_bad, int, 0444);
MODULE_PARM_DESC(mon_keep_badplcp, "Keep frames with bad PLCP in monitor mode");


static const struct ssb_device_id bcm43xx_ssb_tbl[] = {
	SSB_DEVICE(SSB_VENDOR_BROADCOM, SSB_DEV_80211, SSB_ANY_REV),
	SSB_DEVTABLE_END
};
MODULE_DEVICE_TABLE(ssb, bcm43xx_ssb_tbl);


/* Channel and ratetables are shared for all devices.
 * They can't be const, because ieee80211 puts some precalculated
 * data in there. This data is the same for all devices, so we don't
 * get concurrency issues */
#define RATETAB_ENT(_rateid, _flags) \
	{							\
		.rate	= BCM43xx_RATE_TO_BASE100KBPS(_rateid),	\
		.val	= (_rateid),				\
		.val2	= (_rateid),				\
		.flags	= (_flags),				\
	}
static struct ieee80211_rate __bcm43xx_ratetable[] = {
	RATETAB_ENT(BCM43xx_CCK_RATE_1MB, IEEE80211_RATE_CCK),
	RATETAB_ENT(BCM43xx_CCK_RATE_2MB, IEEE80211_RATE_CCK_2),
	RATETAB_ENT(BCM43xx_CCK_RATE_5MB, IEEE80211_RATE_CCK_2),
	RATETAB_ENT(BCM43xx_CCK_RATE_11MB, IEEE80211_RATE_CCK_2),
	RATETAB_ENT(BCM43xx_OFDM_RATE_6MB, IEEE80211_RATE_OFDM),
	RATETAB_ENT(BCM43xx_OFDM_RATE_9MB, IEEE80211_RATE_OFDM),
	RATETAB_ENT(BCM43xx_OFDM_RATE_12MB, IEEE80211_RATE_OFDM),
	RATETAB_ENT(BCM43xx_OFDM_RATE_18MB, IEEE80211_RATE_OFDM),
	RATETAB_ENT(BCM43xx_OFDM_RATE_24MB, IEEE80211_RATE_OFDM),
	RATETAB_ENT(BCM43xx_OFDM_RATE_36MB, IEEE80211_RATE_OFDM),
	RATETAB_ENT(BCM43xx_OFDM_RATE_48MB, IEEE80211_RATE_OFDM),
	RATETAB_ENT(BCM43xx_OFDM_RATE_54MB, IEEE80211_RATE_OFDM),
};
#define bcm43xx_a_ratetable		(__bcm43xx_ratetable + 4)
#define bcm43xx_a_ratetable_size	8
#define bcm43xx_b_ratetable		(__bcm43xx_ratetable + 0)
#define bcm43xx_b_ratetable_size	4
#define bcm43xx_g_ratetable		(__bcm43xx_ratetable + 0)
#define bcm43xx_g_ratetable_size	12

#define CHANTAB_ENT(_chanid, _freq) \
	{							\
		.chan	= (_chanid),				\
		.freq	= (_freq),				\
		.val	= (_chanid),				\
		.flag	= IEEE80211_CHAN_W_SCAN |		\
			  IEEE80211_CHAN_W_ACTIVE_SCAN |	\
			  IEEE80211_CHAN_W_IBSS,		\
		.power_level	= 0xFF,				\
		.antenna_max	= 0xFF,				\
	}
static struct ieee80211_channel bcm43xx_bg_chantable[] = {
	CHANTAB_ENT(1, 2412),
	CHANTAB_ENT(2, 2417),
	CHANTAB_ENT(3, 2422),
	CHANTAB_ENT(4, 2427),
	CHANTAB_ENT(5, 2432),
	CHANTAB_ENT(6, 2437),
	CHANTAB_ENT(7, 2442),
	CHANTAB_ENT(8, 2447),
	CHANTAB_ENT(9, 2452),
	CHANTAB_ENT(10, 2457),
	CHANTAB_ENT(11, 2462),
	CHANTAB_ENT(12, 2467),
	CHANTAB_ENT(13, 2472),
	CHANTAB_ENT(14, 2484),
};
#define bcm43xx_bg_chantable_size	ARRAY_SIZE(bcm43xx_bg_chantable)
static struct ieee80211_channel bcm43xx_a_chantable[] = {
	CHANTAB_ENT(36, 5180),
	CHANTAB_ENT(40, 5200),
	CHANTAB_ENT(44, 5220),
	CHANTAB_ENT(48, 5240),
	CHANTAB_ENT(52, 5260),
	CHANTAB_ENT(56, 5280),
	CHANTAB_ENT(60, 5300),
	CHANTAB_ENT(64, 5320),
	CHANTAB_ENT(149, 5745),
	CHANTAB_ENT(153, 5765),
	CHANTAB_ENT(157, 5785),
	CHANTAB_ENT(161, 5805),
	CHANTAB_ENT(165, 5825),
};
#define bcm43xx_a_chantable_size	ARRAY_SIZE(bcm43xx_a_chantable)


static void bcm43xx_wireless_core_exit(struct bcm43xx_wldev *dev);
static int bcm43xx_wireless_core_init(struct bcm43xx_wldev *dev);
static void bcm43xx_wireless_core_stop(struct bcm43xx_wldev *dev);
static int bcm43xx_wireless_core_start(struct bcm43xx_wldev *dev);


static void bcm43xx_ram_write(struct bcm43xx_wldev *dev, u16 offset, u32 val)
{
	u32 status;

	assert(offset % 4 == 0);

	status = bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD);
	if (status & BCM43xx_SBF_XFER_REG_BYTESWAP)
		val = swab32(val);

	bcm43xx_write32(dev, BCM43xx_MMIO_RAM_CONTROL, offset);
	mmiowb();
	bcm43xx_write32(dev, BCM43xx_MMIO_RAM_DATA, val);
}

static inline
void bcm43xx_shm_control_word(struct bcm43xx_wldev *dev,
			      u16 routing, u16 offset)
{
	u32 control;

	/* "offset" is the WORD offset. */

	control = routing;
	control <<= 16;
	control |= offset;
	bcm43xx_write32(dev, BCM43xx_MMIO_SHM_CONTROL, control);
}

u32 bcm43xx_shm_read32(struct bcm43xx_wldev *dev,
		       u16 routing, u16 offset)
{
	u32 ret;

	if (routing == BCM43xx_SHM_SHARED) {
		assert((offset & 0x0001) == 0);
		if (offset & 0x0003) {
			/* Unaligned access */
			bcm43xx_shm_control_word(dev, routing, offset >> 2);
			ret = bcm43xx_read16(dev,
					     BCM43xx_MMIO_SHM_DATA_UNALIGNED);
			ret <<= 16;
			bcm43xx_shm_control_word(dev, routing, (offset >> 2) + 1);
			ret |= bcm43xx_read16(dev,
					      BCM43xx_MMIO_SHM_DATA);

			return ret;
		}
		offset >>= 2;
	}
	bcm43xx_shm_control_word(dev, routing, offset);
	ret = bcm43xx_read32(dev, BCM43xx_MMIO_SHM_DATA);

	return ret;
}

u16 bcm43xx_shm_read16(struct bcm43xx_wldev *dev,
		       u16 routing, u16 offset)
{
	u16 ret;

	if (routing == BCM43xx_SHM_SHARED) {
		assert((offset & 0x0001) == 0);
		if (offset & 0x0003) {
			/* Unaligned access */
			bcm43xx_shm_control_word(dev, routing, offset >> 2);
			ret = bcm43xx_read16(dev,
					     BCM43xx_MMIO_SHM_DATA_UNALIGNED);

			return ret;
		}
		offset >>= 2;
	}
	bcm43xx_shm_control_word(dev, routing, offset);
	ret = bcm43xx_read16(dev, BCM43xx_MMIO_SHM_DATA);

	return ret;
}

void bcm43xx_shm_write32(struct bcm43xx_wldev *dev,
			 u16 routing, u16 offset,
			 u32 value)
{
	if (routing == BCM43xx_SHM_SHARED) {
		assert((offset & 0x0001) == 0);
		if (offset & 0x0003) {
			/* Unaligned access */
			bcm43xx_shm_control_word(dev, routing, offset >> 2);
			mmiowb();
			bcm43xx_write16(dev, BCM43xx_MMIO_SHM_DATA_UNALIGNED,
					(value >> 16) & 0xffff);
			mmiowb();
			bcm43xx_shm_control_word(dev, routing, (offset >> 2) + 1);
			mmiowb();
			bcm43xx_write16(dev, BCM43xx_MMIO_SHM_DATA,
					value & 0xffff);
			return;
		}
		offset >>= 2;
	}
	bcm43xx_shm_control_word(dev, routing, offset);
	mmiowb();
	bcm43xx_write32(dev, BCM43xx_MMIO_SHM_DATA, value);
}

void bcm43xx_shm_write16(struct bcm43xx_wldev *dev,
			 u16 routing, u16 offset,
			 u16 value)
{
	if (routing == BCM43xx_SHM_SHARED) {
		assert((offset & 0x0001) == 0);
		if (offset & 0x0003) {
			/* Unaligned access */
			bcm43xx_shm_control_word(dev, routing, offset >> 2);
			mmiowb();
			bcm43xx_write16(dev, BCM43xx_MMIO_SHM_DATA_UNALIGNED,
					value);
			return;
		}
		offset >>= 2;
	}
	bcm43xx_shm_control_word(dev, routing, offset);
	mmiowb();
	bcm43xx_write16(dev, BCM43xx_MMIO_SHM_DATA, value);
}

/* Read HostFlags */
u32 bcm43xx_hf_read(struct bcm43xx_wldev *dev)
{
	u32 ret;

	ret = bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED,
				 BCM43xx_SHM_SH_HOSTFHI);
	ret <<= 16;
	ret |= bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED,
				  BCM43xx_SHM_SH_HOSTFLO);

	return ret;
}

/* Write HostFlags */
void bcm43xx_hf_write(struct bcm43xx_wldev *dev, u32 value)
{
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED,
			    BCM43xx_SHM_SH_HOSTFLO,
			    (value & 0x0000FFFF));
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED,
			    BCM43xx_SHM_SH_HOSTFHI,
			    ((value & 0xFFFF0000) >> 16));
}

void bcm43xx_tsf_read(struct bcm43xx_wldev *dev, u64 *tsf)
{
	/* We need to be careful. As we read the TSF from multiple
	 * registers, we should take care of register overflows.
	 * In theory, the whole tsf read process should be atomic.
	 * We try to be atomic here, by restaring the read process,
	 * if any of the high registers changed (overflew).
	 */
	if (dev->dev->id.revision >= 3) {
		u32 low, high, high2;

		do {
			high = bcm43xx_read32(dev, BCM43xx_MMIO_REV3PLUS_TSF_HIGH);
			low = bcm43xx_read32(dev, BCM43xx_MMIO_REV3PLUS_TSF_LOW);
			high2 = bcm43xx_read32(dev, BCM43xx_MMIO_REV3PLUS_TSF_HIGH);
		} while (unlikely(high != high2));

		*tsf = high;
		*tsf <<= 32;
		*tsf |= low;
	} else {
		u64 tmp;
		u16 v0, v1, v2, v3;
		u16 test1, test2, test3;

		do {
			v3 = bcm43xx_read16(dev, BCM43xx_MMIO_TSF_3);
			v2 = bcm43xx_read16(dev, BCM43xx_MMIO_TSF_2);
			v1 = bcm43xx_read16(dev, BCM43xx_MMIO_TSF_1);
			v0 = bcm43xx_read16(dev, BCM43xx_MMIO_TSF_0);

			test3 = bcm43xx_read16(dev, BCM43xx_MMIO_TSF_3);
			test2 = bcm43xx_read16(dev, BCM43xx_MMIO_TSF_2);
			test1 = bcm43xx_read16(dev, BCM43xx_MMIO_TSF_1);
		} while (v3 != test3 || v2 != test2 || v1 != test1);

		*tsf = v3;
		*tsf <<= 48;
		tmp = v2;
		tmp <<= 32;
		*tsf |= tmp;
		tmp = v1;
		tmp <<= 16;
		*tsf |= tmp;
		*tsf |= v0;
	}
}

static void bcm43xx_time_lock(struct bcm43xx_wldev *dev)
{
	u32 status;

	status = bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD);
	status |= BCM43xx_SBF_TIME_UPDATE;
	bcm43xx_write32(dev, BCM43xx_MMIO_STATUS_BITFIELD, status);
	mmiowb();
}

static void bcm43xx_time_unlock(struct bcm43xx_wldev *dev)
{
	u32 status;

	status = bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD);
	status &= ~BCM43xx_SBF_TIME_UPDATE;
	bcm43xx_write32(dev, BCM43xx_MMIO_STATUS_BITFIELD, status);
}

static void bcm43xx_tsf_write_locked(struct bcm43xx_wldev *dev, u64 tsf)
{
	/* Be careful with the in-progress timer.
	 * First zero out the low register, so we have a full
	 * register-overflow duration to complete the operation.
	 */
	if (dev->dev->id.revision >= 3) {
		u32 lo = (tsf & 0x00000000FFFFFFFFULL);
		u32 hi = (tsf & 0xFFFFFFFF00000000ULL) >> 32;

		bcm43xx_write32(dev, BCM43xx_MMIO_REV3PLUS_TSF_LOW, 0);
		mmiowb();
		bcm43xx_write32(dev, BCM43xx_MMIO_REV3PLUS_TSF_HIGH, hi);
		mmiowb();
		bcm43xx_write32(dev, BCM43xx_MMIO_REV3PLUS_TSF_LOW, lo);
	} else {
		u16 v0 = (tsf & 0x000000000000FFFFULL);
		u16 v1 = (tsf & 0x00000000FFFF0000ULL) >> 16;
		u16 v2 = (tsf & 0x0000FFFF00000000ULL) >> 32;
		u16 v3 = (tsf & 0xFFFF000000000000ULL) >> 48;

		bcm43xx_write16(dev, BCM43xx_MMIO_TSF_0, 0);
		mmiowb();
		bcm43xx_write16(dev, BCM43xx_MMIO_TSF_3, v3);
		mmiowb();
		bcm43xx_write16(dev, BCM43xx_MMIO_TSF_2, v2);
		mmiowb();
		bcm43xx_write16(dev, BCM43xx_MMIO_TSF_1, v1);
		mmiowb();
		bcm43xx_write16(dev, BCM43xx_MMIO_TSF_0, v0);
	}
}

void bcm43xx_tsf_write(struct bcm43xx_wldev *dev, u64 tsf)
{
	bcm43xx_time_lock(dev);
	bcm43xx_tsf_write_locked(dev, tsf);
	bcm43xx_time_unlock(dev);
}

static
void bcm43xx_macfilter_set(struct bcm43xx_wldev *dev,
			   u16 offset,
			   const u8 *mac)
{
	static const u8 zero_addr[ETH_ALEN] = { 0 };
	u16 data;

	if (!mac)
		mac = zero_addr;

	offset |= 0x0020;
	bcm43xx_write16(dev, BCM43xx_MMIO_MACFILTER_CONTROL, offset);

	data = mac[0];
	data |= mac[1] << 8;
	bcm43xx_write16(dev, BCM43xx_MMIO_MACFILTER_DATA, data);
	data = mac[2];
	data |= mac[3] << 8;
	bcm43xx_write16(dev, BCM43xx_MMIO_MACFILTER_DATA, data);
	data = mac[4];
	data |= mac[5] << 8;
	bcm43xx_write16(dev, BCM43xx_MMIO_MACFILTER_DATA, data);
}

static void bcm43xx_write_mac_bssid_templates(struct bcm43xx_wldev *dev)
{
	static const u8 zero_addr[ETH_ALEN] = { 0 };
	const u8 *mac;
	const u8 *bssid;
	u8 mac_bssid[ETH_ALEN * 2];
	int i;
	u32 tmp;

	bssid = dev->wl->bssid;
	if (!bssid)
		bssid = zero_addr;
	mac = dev->wl->mac_addr;
	if (!mac)
		mac = zero_addr;

	bcm43xx_macfilter_set(dev, BCM43xx_MACFILTER_BSSID, bssid);

	memcpy(mac_bssid, mac, ETH_ALEN);
	memcpy(mac_bssid + ETH_ALEN, bssid, ETH_ALEN);

	/* Write our MAC address and BSSID to template ram */
	for (i = 0; i < ARRAY_SIZE(mac_bssid); i += sizeof(u32)) {
		tmp =  (u32)(mac_bssid[i + 0]);
		tmp |= (u32)(mac_bssid[i + 1]) << 8;
		tmp |= (u32)(mac_bssid[i + 2]) << 16;
		tmp |= (u32)(mac_bssid[i + 3]) << 24;
		bcm43xx_ram_write(dev, 0x20 + i, tmp);
	}
}

static void bcm43xx_upload_card_macaddress(struct bcm43xx_wldev *dev,
					   const u8 *mac_addr)
{
	dev->wl->mac_addr = mac_addr;
	bcm43xx_write_mac_bssid_templates(dev);
	bcm43xx_macfilter_set(dev, BCM43xx_MACFILTER_SELF, mac_addr);
}

static void bcm43xx_set_slot_time(struct bcm43xx_wldev *dev, u16 slot_time)
{
	/* slot_time is in usec. */
	if (dev->phy.type != BCM43xx_PHYTYPE_G)
		return;
	bcm43xx_write16(dev, 0x684, 510 + slot_time);
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, 0x0010, slot_time);
}

static void bcm43xx_short_slot_timing_enable(struct bcm43xx_wldev *dev)
{
	bcm43xx_set_slot_time(dev, 9);
	dev->short_slot = 1;
}

static void bcm43xx_short_slot_timing_disable(struct bcm43xx_wldev *dev)
{
	bcm43xx_set_slot_time(dev, 20);
	dev->short_slot = 0;
}

/* Enable a Generic IRQ. "mask" is the mask of which IRQs to enable.
 * Returns the _previously_ enabled IRQ mask.
 */
static inline u32 bcm43xx_interrupt_enable(struct bcm43xx_wldev *dev, u32 mask)
{
	u32 old_mask;

	old_mask = bcm43xx_read32(dev, BCM43xx_MMIO_GEN_IRQ_MASK);
	bcm43xx_write32(dev, BCM43xx_MMIO_GEN_IRQ_MASK, old_mask | mask);

	return old_mask;
}

/* Disable a Generic IRQ. "mask" is the mask of which IRQs to disable.
 * Returns the _previously_ enabled IRQ mask.
 */
static inline u32 bcm43xx_interrupt_disable(struct bcm43xx_wldev *dev, u32 mask)
{
	u32 old_mask;

	old_mask = bcm43xx_read32(dev, BCM43xx_MMIO_GEN_IRQ_MASK);
	bcm43xx_write32(dev, BCM43xx_MMIO_GEN_IRQ_MASK, old_mask & ~mask);

	return old_mask;
}

/* Synchronize IRQ top- and bottom-half.
 * IRQs must be masked before calling this.
 * This must not be called with the irq_lock held.
 */
static void bcm43xx_synchronize_irq(struct bcm43xx_wldev *dev)
{
	synchronize_irq(dev->dev->irq);
	tasklet_kill(&dev->isr_tasklet);
}

/* DummyTransmission function, as documented on
 * http://bcm-specs.sipsolutions.net/DummyTransmission
 */
void bcm43xx_dummy_transmission(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	unsigned int i, max_loop;
	u16 value;
	u32 buffer[5] = {
		0x00000000,
		0x00D40000,
		0x00000000,
		0x01000000,
		0x00000000,
	};

	switch (phy->type) {
	case BCM43xx_PHYTYPE_A:
		max_loop = 0x1E;
		buffer[0] = 0x000201CC;
		break;
	case BCM43xx_PHYTYPE_B:
	case BCM43xx_PHYTYPE_G:
		max_loop = 0xFA;
		buffer[0] = 0x000B846E;
		break;
	default:
		assert(0);
		return;
	}

	for (i = 0; i < 5; i++)
		bcm43xx_ram_write(dev, i * 4, buffer[i]);

	bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD); /* dummy read */

	bcm43xx_write16(dev, 0x0568, 0x0000);
	bcm43xx_write16(dev, 0x07C0, 0x0000);
	value = ((phy->type == BCM43xx_PHYTYPE_A) ? 1 : 0);
	bcm43xx_write16(dev, 0x050C, value);
	bcm43xx_write16(dev, 0x0508, 0x0000);
	bcm43xx_write16(dev, 0x050A, 0x0000);
	bcm43xx_write16(dev, 0x054C, 0x0000);
	bcm43xx_write16(dev, 0x056A, 0x0014);
	bcm43xx_write16(dev, 0x0568, 0x0826);
	bcm43xx_write16(dev, 0x0500, 0x0000);
	bcm43xx_write16(dev, 0x0502, 0x0030);

	if (phy->radio_ver == 0x2050 && phy->radio_rev <= 0x5)
		bcm43xx_radio_write16(dev, 0x0051, 0x0017);
	for (i = 0x00; i < max_loop; i++) {
		value = bcm43xx_read16(dev, 0x050E);
		if (value & 0x0080)
			break;
		udelay(10);
	}
	for (i = 0x00; i < 0x0A; i++) {
		value = bcm43xx_read16(dev, 0x050E);
		if (value & 0x0400)
			break;
		udelay(10);
	}
	for (i = 0x00; i < 0x0A; i++) {
		value = bcm43xx_read16(dev, 0x0690);
		if (!(value & 0x0100))
			break;
		udelay(10);
	}
	if (phy->radio_ver == 0x2050 && phy->radio_rev <= 0x5)
		bcm43xx_radio_write16(dev, 0x0051, 0x0037);
}

static void key_write(struct bcm43xx_wldev *dev,
		      u8 index, u8 algorithm, const u8 *key)
{
	unsigned int i;
	u32 offset;
	u16 value;
	u16 kidx;

	/* Key index/algo block */
	kidx = bcm43xx_kidx_to_fw(dev, index);
	value = ((kidx << 4) | algorithm);
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED,
			    BCM43xx_SHM_SH_KEYIDXBLOCK +
			    (kidx * 2), value);

	/* Write the key to the Key Table Pointer offset */
	offset = dev->ktp + (index * BCM43xx_SEC_KEYSIZE);
	for (i = 0; i < BCM43xx_SEC_KEYSIZE; i += 2) {
		value = key[i];
		value |= (u16)(key[i + 1]) << 8;
		bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED,
				    offset + i, value);
	}
}

static void keymac_write(struct bcm43xx_wldev *dev,
			 u8 index, const u8 *addr)
{
	u32 addrtmp[2];

	assert(index >= 4 + 4);
	memcpy(dev->key[index].address, addr, 6);
	/* We have two default TX keys and two default RX keys.
	 * Physical mac 0 is mapped to physical key 8.
	 * So we must adjust the index here.
	 */
	index -= 8;

	addrtmp[0] = addr[0];
	addrtmp[0] |= ((u32)(addr[1]) << 8);
	addrtmp[0] |= ((u32)(addr[2]) << 16);
	addrtmp[0] |= ((u32)(addr[3]) << 24);
	addrtmp[1] = addr[4];
	addrtmp[1] |= ((u32)(addr[5]) << 8);

	if (dev->dev->id.revision >= 5) {
		/* Receive match transmitter address mechanism */
		bcm43xx_shm_write32(dev, BCM43xx_SHM_RCMTA,
				    (index * 2) + 0, addrtmp[0]);
		bcm43xx_shm_write16(dev, BCM43xx_SHM_RCMTA,
				    (index * 2) + 1, addrtmp[1]);
	} else {
		/* RXE (Receive Engine) and
		 * PSM (Programmable State Machine) mechanism
		 */
		if (index < 8) {
			/* TODO write to RCM 16, 19, 22 and 25 */
			TODO();
		} else {
			bcm43xx_shm_write32(dev, BCM43xx_SHM_SHARED,
					    BCM43xx_SHM_SH_PSM + (index * 6) + 0,
					    addrtmp[0]);
			bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED,
					    BCM43xx_SHM_SH_PSM + (index * 6) + 4,
					    addrtmp[1]);
		}
	}
}

static void do_key_write(struct bcm43xx_wldev *dev,
			 u8 index, u8 algorithm,
			 const u8 *key, size_t key_len,
			 const u8 *mac_addr)
{
	u8 buf[BCM43xx_SEC_KEYSIZE];

	assert(index < dev->max_nr_keys);
	assert(key_len <= BCM43xx_SEC_KEYSIZE);

	memset(buf, 0, sizeof(buf));
	if (index >= 8)
		keymac_write(dev, index, buf); /* First zero out mac. */
	memcpy(buf, key, key_len);
	key_write(dev, index, algorithm, buf);
	if (index >= 8)
		keymac_write(dev, index, mac_addr);

	dev->key[index].algorithm = algorithm;
}

static int bcm43xx_key_write(struct bcm43xx_wldev *dev,
			     int index, u8 algorithm,
			     const u8 *key, size_t key_len,
			     const u8 *mac_addr,
			     struct ieee80211_key_conf *keyconf)
{
	int i;
	int sta_keys_start;

	if (key_len > BCM43xx_SEC_KEYSIZE)
		return -EINVAL;
	if (index < 0) {
		/* Per station key with associated MAC address.
		 * Look if it already exists, if yes update, otherwise
		 * allocate a new key.
		 */
		if (bcm43xx_new_kidx_api(dev))
			sta_keys_start = 4;
		else
			sta_keys_start = 8;
		for (i = sta_keys_start; i < dev->max_nr_keys; i++) {
			if (compare_ether_addr(dev->key[i].address, mac_addr) == 0) {
				/* found existing */
				index = i;
				break;
			}
		}
		if (index < 0) {
			for (i = sta_keys_start; i < dev->max_nr_keys; i++) {
				if (!dev->key[i].enabled) {
					/* found empty */
					index = i;
					break;
				}
			}
		}
		if (index < 0) {
			dprintk(KERN_ERR PFX "Out of hw key memory\n");
			return -ENOBUFS;
		}
	} else
		assert(index <= 3);

	do_key_write(dev, index, algorithm, key, key_len, mac_addr);
	if ((index <= 3) && !bcm43xx_new_kidx_api(dev)) {
		/* Default RX key */
		assert(mac_addr == NULL);
		do_key_write(dev, index + 4, algorithm, key, key_len, NULL);
	}
	keyconf->hw_key_idx = index;

	return 0;
}

static void bcm43xx_clear_keys(struct bcm43xx_wldev *dev)
{
	static const u8 zero[BCM43xx_SEC_KEYSIZE] = { 0 };
	unsigned int i;

	BUILD_BUG_ON(BCM43xx_SEC_KEYSIZE < ETH_ALEN);
	for (i = 0; i < dev->max_nr_keys; i++) {
		do_key_write(dev, i, BCM43xx_SEC_ALGO_NONE,
			     zero, BCM43xx_SEC_KEYSIZE,
			     zero);
		dev->key[i].enabled = 0;
	}
}

/* Turn the Analog ON/OFF */
static void bcm43xx_switch_analog(struct bcm43xx_wldev *dev, int on)
{
	bcm43xx_write16(dev, BCM43xx_MMIO_PHY0, on ? 0 : 0xF4);
}

void bcm43xx_wireless_core_reset(struct bcm43xx_wldev *dev, u32 flags)
{
	u32 tmslow;
	u32 macctl;

	flags |= BCM43xx_TMSLOW_PHYCLKEN;
	flags |= BCM43xx_TMSLOW_PHYRESET;
	ssb_device_enable(dev->dev, flags);
	msleep(2); /* Wait for the PLL to turn on. */

	/* Now take the PHY out of Reset again */
	tmslow = ssb_read32(dev->dev, SSB_TMSLOW);
	tmslow |= SSB_TMSLOW_FGC;
	tmslow &= ~BCM43xx_TMSLOW_PHYRESET;
	ssb_write32(dev->dev, SSB_TMSLOW, tmslow);
	ssb_read32(dev->dev, SSB_TMSLOW); /* flush */
	msleep(1);
	tmslow &= ~SSB_TMSLOW_FGC;
	ssb_write32(dev->dev, SSB_TMSLOW, tmslow);
	ssb_read32(dev->dev, SSB_TMSLOW); /* flush */
	msleep(1);

	/* Turn Analog ON */
	bcm43xx_switch_analog(dev, 1);

	macctl = bcm43xx_read32(dev, BCM43xx_MMIO_MACCTL);
	macctl &= ~BCM43xx_MACCTL_GMODE;
	if (flags & BCM43xx_TMSLOW_GMODE)
		macctl |= BCM43xx_MACCTL_GMODE;
	macctl |= BCM43xx_MACCTL_IHR_ENABLED;
	bcm43xx_write32(dev, BCM43xx_MMIO_MACCTL, macctl);
}

static void handle_irq_transmit_status(struct bcm43xx_wldev *dev)
{
	u32 v0, v1;
	u16 tmp;
	struct bcm43xx_txstatus stat;

	while (1) {
		v0 = bcm43xx_read32(dev, BCM43xx_MMIO_XMITSTAT_0);
		if (!(v0 & 0x00000001))
			break;
		v1 = bcm43xx_read32(dev, BCM43xx_MMIO_XMITSTAT_1);

		stat.cookie = (v0 >> 16);
		stat.seq = (v1 & 0x0000FFFF);
		stat.phy_stat = ((v1 & 0x00FF0000) >> 16);
		tmp = (v0 & 0x0000FFFF);
		stat.frame_count = ((tmp & 0xF000) >> 12);
		stat.rts_count = ((tmp & 0x0F00) >> 8);
		stat.supp_reason = ((tmp & 0x001C) >> 2);
		stat.pm_indicated = !!(tmp & 0x0080);
		stat.intermediate = !!(tmp & 0x0040);
		stat.for_ampdu = !!(tmp & 0x0020);
		stat.acked = !!(tmp & 0x0002);

		bcm43xx_handle_txstatus(dev, &stat);
	}
}

static void drain_txstatus_queue(struct bcm43xx_wldev *dev)
{
	u32 dummy;

	if (dev->dev->id.revision < 5)
		return;
	/* Read all entries from the microcode TXstatus FIFO
	 * and throw them away.
	 */
	while (1) {
		dummy = bcm43xx_read32(dev, BCM43xx_MMIO_XMITSTAT_0);
		if (!(dummy & 0x00000001))
			break;
		dummy = bcm43xx_read32(dev, BCM43xx_MMIO_XMITSTAT_1);
	}
}

static u32 bcm43xx_jssi_read(struct bcm43xx_wldev *dev)
{
	u32 val = 0;

	val = bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED, 0x08A);
	val <<= 16;
	val |= bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED, 0x088);

	return val;
}

static void bcm43xx_jssi_write(struct bcm43xx_wldev *dev, u32 jssi)
{
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, 0x088,
			    (jssi & 0x0000FFFF));
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, 0x08A,
			    (jssi & 0xFFFF0000) >> 16);
}

static void bcm43xx_generate_noise_sample(struct bcm43xx_wldev *dev)
{
	bcm43xx_jssi_write(dev, 0x7F7F7F7F);
	bcm43xx_write32(dev, BCM43xx_MMIO_STATUS2_BITFIELD,
			bcm43xx_read32(dev, BCM43xx_MMIO_STATUS2_BITFIELD)
			| (1 << 4));
	assert(dev->noisecalc.channel_at_start == dev->phy.channel);
}

static void bcm43xx_calculate_link_quality(struct bcm43xx_wldev *dev)
{
	/* Top half of Link Quality calculation. */

	if (dev->noisecalc.calculation_running)
		return;
	dev->noisecalc.channel_at_start = dev->phy.channel;
	dev->noisecalc.calculation_running = 1;
	dev->noisecalc.nr_samples = 0;

	bcm43xx_generate_noise_sample(dev);
}

static void handle_irq_noise(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u16 tmp;
	u8 noise[4];
	u8 i, j;
	s32 average;

	/* Bottom half of Link Quality calculation. */

	assert(dev->noisecalc.calculation_running);
	if (dev->noisecalc.channel_at_start != phy->channel)
		goto drop_calculation;
	*((u32 *)noise) = cpu_to_le32(bcm43xx_jssi_read(dev));
	if (noise[0] == 0x7F || noise[1] == 0x7F ||
	    noise[2] == 0x7F || noise[3] == 0x7F)
		goto generate_new;

	/* Get the noise samples. */
	assert(dev->noisecalc.nr_samples < 8);
	i = dev->noisecalc.nr_samples;
	noise[0] = limit_value(noise[0], 0, ARRAY_SIZE(phy->nrssi_lt) - 1);
	noise[1] = limit_value(noise[1], 0, ARRAY_SIZE(phy->nrssi_lt) - 1);
	noise[2] = limit_value(noise[2], 0, ARRAY_SIZE(phy->nrssi_lt) - 1);
	noise[3] = limit_value(noise[3], 0, ARRAY_SIZE(phy->nrssi_lt) - 1);
	dev->noisecalc.samples[i][0] = phy->nrssi_lt[noise[0]];
	dev->noisecalc.samples[i][1] = phy->nrssi_lt[noise[1]];
	dev->noisecalc.samples[i][2] = phy->nrssi_lt[noise[2]];
	dev->noisecalc.samples[i][3] = phy->nrssi_lt[noise[3]];
	dev->noisecalc.nr_samples++;
	if (dev->noisecalc.nr_samples == 8) {
		/* Calculate the Link Quality by the noise samples. */
		average = 0;
		for (i = 0; i < 8; i++) {
			for (j = 0; j < 4; j++)
				average += dev->noisecalc.samples[i][j];
		}
		average /= (8 * 4);
		average *= 125;
		average += 64;
		average /= 128;
		tmp = bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED, 0x40C);
		tmp = (tmp / 128) & 0x1F;
		if (tmp >= 8)
			average += 2;
		else
			average -= 25;
		if (tmp == 8)
			average -= 72;
		else
			average -= 48;

		dev->stats.link_noise = average;
drop_calculation:
		dev->noisecalc.calculation_running = 0;
		return;
	}
generate_new:
	bcm43xx_generate_noise_sample(dev);
}

static void handle_irq_tbtt_indication(struct bcm43xx_wldev *dev)
{
	if (bcm43xx_is_mode(dev->wl, IEEE80211_IF_TYPE_AP)) {
		///TODO: PS TBTT
	} else {
		if (1/*FIXME: the last PSpoll frame was sent successfully */)
			bcm43xx_power_saving_ctl_bits(dev, -1, -1);
	}
	dev->reg124_set_0x4 = 0;
	if (bcm43xx_is_mode(dev->wl, IEEE80211_IF_TYPE_IBSS))
		dev->reg124_set_0x4 = 1;
}

static void handle_irq_atim_end(struct bcm43xx_wldev *dev)
{
	if (!dev->reg124_set_0x4 /*FIXME rename this variable*/)
		return;
	bcm43xx_write32(dev, BCM43xx_MMIO_STATUS2_BITFIELD,
			bcm43xx_read32(dev, BCM43xx_MMIO_STATUS2_BITFIELD)
			| 0x4);
}

static void handle_irq_pmq(struct bcm43xx_wldev *dev)
{
	u32 tmp;

	//TODO: AP mode.

	while (1) {
		tmp = bcm43xx_read32(dev, BCM43xx_MMIO_PS_STATUS);
		if (!(tmp & 0x00000008))
			break;
	}
	/* 16bit write is odd, but correct. */
	bcm43xx_write16(dev, BCM43xx_MMIO_PS_STATUS, 0x0002);
}

static void bcm43xx_write_template_common(struct bcm43xx_wldev *dev,
					  const u8* data, u16 size,
					  u16 ram_offset,
					  u16 shm_size_offset, u8 rate)
{
	u32 i, tmp;
	struct bcm43xx_plcp_hdr4 plcp;

	plcp.data = 0;
	bcm43xx_generate_plcp_hdr(&plcp, size + FCS_LEN, rate);
	bcm43xx_ram_write(dev, ram_offset, le32_to_cpu(plcp.data));
	ram_offset += sizeof(u32);
	/* The PLCP is 6 bytes long, but we only wrote 4 bytes, yet.
	 * So leave the first two bytes of the next write blank.
	 */
	tmp = (u32)(data[0]) << 16;
	tmp |= (u32)(data[1]) << 24;
	bcm43xx_ram_write(dev, ram_offset, tmp);
	ram_offset += sizeof(u32);
	for (i = 2; i < size; i += sizeof(u32)) {
		tmp = (u32)(data[i + 0]);
		if (i + 1 < size)
			tmp |= (u32)(data[i + 1]) << 8;
		if (i + 2 < size)
			tmp |= (u32)(data[i + 2]) << 16;
		if (i + 3 < size)
			tmp |= (u32)(data[i + 3]) << 24;
		bcm43xx_ram_write(dev, ram_offset + i - 2, tmp);
	}
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, shm_size_offset,
			    size + sizeof(struct bcm43xx_plcp_hdr6));
}

static void bcm43xx_write_beacon_template(struct bcm43xx_wldev *dev,
					  u16 ram_offset,
					  u16 shm_size_offset, u8 rate)
{
	int len;
	const u8 *data;

	assert(dev->cached_beacon);
	len = min((size_t)dev->cached_beacon->len,
		  0x200 - sizeof(struct bcm43xx_plcp_hdr6));
	data = (const u8 *)(dev->cached_beacon->data);
	bcm43xx_write_template_common(dev, data,
				      len, ram_offset,
				      shm_size_offset, rate);
}

static void bcm43xx_write_probe_resp_plcp(struct bcm43xx_wldev *dev,
					  u16 shm_offset, u16 size, u8 rate)
{
	struct bcm43xx_plcp_hdr4 plcp;
	u32 tmp;
	__le16 dur;

	plcp.data = 0;
	bcm43xx_generate_plcp_hdr(&plcp, size + FCS_LEN, rate);
	dur = ieee80211_generic_frame_duration(dev->wl->hw,
					       size,
					       BCM43xx_RATE_TO_BASE100KBPS(rate));
	/* Write PLCP in two parts and timing for packet transfer */
	tmp = le32_to_cpu(plcp.data);
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, shm_offset,
			    tmp & 0xFFFF);
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, shm_offset + 2,
			    tmp >> 16);
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, shm_offset + 6,
			    le16_to_cpu(dur));
}

/* Instead of using custom probe response template, this function
 * just patches custom beacon template by:
 * 1) Changing packet type
 * 2) Patching duration field
 * 3) Stripping TIM
 */
static u8 * bcm43xx_generate_probe_resp(struct bcm43xx_wldev *dev,
					u16* dest_size, u8 rate)
{
	const u8 *src_data;
	u8 *dest_data;
	u16 src_size, elem_size, src_pos, dest_pos;
	__le16 dur;
	struct ieee80211_hdr *hdr;

	assert(dev->cached_beacon);
	src_size = dev->cached_beacon->len;
	src_data = (const u8*)dev->cached_beacon->data;

	if (unlikely(src_size < 0x24)) {
		dprintk(KERN_ERR PFX "bcm43xx_generate_probe_resp: "
				     "invalid beacon\n");
		return NULL;
	}

	dest_data = kmalloc(src_size, GFP_ATOMIC);
	if (unlikely(!dest_data))
		return NULL;

	/* 0x24 is offset of first variable-len Information-Element
	 * in beacon frame.
	 */
	memcpy(dest_data, src_data, 0x24);
	src_pos = dest_pos = 0x24;
	for ( ; src_pos < src_size - 2; src_pos += elem_size) {
		elem_size = src_data[src_pos + 1] + 2;
		if (src_data[src_pos] != 0x05) { /* TIM */
			memcpy(dest_data + dest_pos, src_data + src_pos,
			       elem_size);
			dest_pos += elem_size;
		}
	}
	*dest_size = dest_pos;
	hdr = (struct ieee80211_hdr *)dest_data;

	/* Set the frame control. */
	hdr->frame_control = cpu_to_le16(IEEE80211_FTYPE_MGMT |
					 IEEE80211_STYPE_PROBE_RESP);
	dur = ieee80211_generic_frame_duration(dev->wl->hw,
					       *dest_size,
					       BCM43xx_RATE_TO_BASE100KBPS(rate));
	hdr->duration_id = dur;

	return dest_data;
}

static void bcm43xx_write_probe_resp_template(struct bcm43xx_wldev *dev,
					      u16 ram_offset,
					      u16 shm_size_offset, u8 rate)
{
	u8* probe_resp_data;
	u16 size;

	assert(dev->cached_beacon);
	size = dev->cached_beacon->len;
	probe_resp_data = bcm43xx_generate_probe_resp(dev, &size, rate);
	if (unlikely(!probe_resp_data))
		return;

	/* Looks like PLCP headers plus packet timings are stored for
	 * all possible basic rates
	 */
	bcm43xx_write_probe_resp_plcp(dev, 0x31A, size,
				      BCM43xx_CCK_RATE_1MB);
	bcm43xx_write_probe_resp_plcp(dev, 0x32C, size,
				      BCM43xx_CCK_RATE_2MB);
	bcm43xx_write_probe_resp_plcp(dev, 0x33E, size,
				      BCM43xx_CCK_RATE_5MB);
	bcm43xx_write_probe_resp_plcp(dev, 0x350, size,
				      BCM43xx_CCK_RATE_11MB);

	size = min((size_t)size,
		   0x200 - sizeof(struct bcm43xx_plcp_hdr6));
	bcm43xx_write_template_common(dev, probe_resp_data,
				      size, ram_offset,
				      shm_size_offset, rate);
	kfree(probe_resp_data);
}

static int bcm43xx_refresh_cached_beacon(struct bcm43xx_wldev *dev,
					 struct sk_buff *beacon)
{
	if (dev->cached_beacon)
		kfree_skb(dev->cached_beacon);
	dev->cached_beacon = beacon;

	return 0;
}

static void bcm43xx_update_templates(struct bcm43xx_wldev *dev)
{
	u32 status;

	assert(dev->cached_beacon);

	bcm43xx_write_beacon_template(dev, 0x68, 0x18,
				      BCM43xx_CCK_RATE_1MB);
	bcm43xx_write_beacon_template(dev, 0x468, 0x1A,
				      BCM43xx_CCK_RATE_1MB);
	bcm43xx_write_probe_resp_template(dev, 0x268, 0x4A,
					  BCM43xx_CCK_RATE_11MB);

	status = bcm43xx_read32(dev, BCM43xx_MMIO_STATUS2_BITFIELD);
	status |= 0x03;
	bcm43xx_write32(dev, BCM43xx_MMIO_STATUS2_BITFIELD, status);
}

static void bcm43xx_refresh_templates(struct bcm43xx_wldev *dev,
				      struct sk_buff *beacon)
{
	int err;

	err = bcm43xx_refresh_cached_beacon(dev, beacon);
	if (unlikely(err))
		return;
	bcm43xx_update_templates(dev);
}

static void bcm43xx_set_ssid(struct bcm43xx_wldev *dev,
			     const u8 *ssid, u8 ssid_len)
{
	u32 tmp;
	u16 i, len;

	len = min((u16)ssid_len, (u16)0x100);
	for (i = 0; i < len; i += sizeof(u32)) {
		tmp = (u32)(ssid[i + 0]);
		if (i + 1 < len)
			tmp |= (u32)(ssid[i + 1]) << 8;
		if (i + 2 < len)
			tmp |= (u32)(ssid[i + 2]) << 16;
		if (i + 3 < len)
			tmp |= (u32)(ssid[i + 3]) << 24;
		bcm43xx_shm_write32(dev, BCM43xx_SHM_SHARED,
				    0x380 + i, tmp);
	}
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED,
			    0x48, len);
}

static void bcm43xx_set_beacon_int(struct bcm43xx_wldev *dev, u16 beacon_int)
{
	bcm43xx_time_lock(dev);
	if (dev->dev->id.revision >= 3) {
		bcm43xx_write32(dev, 0x188, (beacon_int << 16));
	} else {
		bcm43xx_write16(dev, 0x606, (beacon_int >> 6));
		bcm43xx_write16(dev, 0x610, beacon_int);
	}
	bcm43xx_time_unlock(dev);
}

static void handle_irq_beacon(struct bcm43xx_wldev *dev)
{
	u32 status;

	if (!bcm43xx_is_mode(dev->wl, IEEE80211_IF_TYPE_AP))
		return;

	dev->irq_savedstate &= ~BCM43xx_IRQ_BEACON;
	status = bcm43xx_read32(dev, BCM43xx_MMIO_STATUS2_BITFIELD);

	if (!dev->cached_beacon || ((status & 0x1) && (status & 0x2))) {
		/* ACK beacon IRQ. */
		bcm43xx_write32(dev, BCM43xx_MMIO_GEN_IRQ_REASON,
				BCM43xx_IRQ_BEACON);
		dev->irq_savedstate |= BCM43xx_IRQ_BEACON;
		if (dev->cached_beacon)
			kfree_skb(dev->cached_beacon);
		dev->cached_beacon = NULL;
		return;
	}
	if (!(status & 0x1)) {
		bcm43xx_write_beacon_template(dev, 0x68, 0x18,
					      BCM43xx_CCK_RATE_1MB);
		status |= 0x1;
		bcm43xx_write32(dev, BCM43xx_MMIO_STATUS2_BITFIELD,
				status);
	}
	if (!(status & 0x2)) {
		bcm43xx_write_beacon_template(dev, 0x468, 0x1A,
					      BCM43xx_CCK_RATE_1MB);
		status |= 0x2;
		bcm43xx_write32(dev, BCM43xx_MMIO_STATUS2_BITFIELD,
				status);
	}
}

static void handle_irq_ucode_debug(struct bcm43xx_wldev *dev)
{
	//TODO
}

/* Interrupt handler bottom-half */
static void bcm43xx_interrupt_tasklet(struct bcm43xx_wldev *dev)
{
	u32 reason;
	u32 dma_reason[ARRAY_SIZE(dev->dma_reason)];
	u32 merged_dma_reason = 0;
	int i, activity = 0;
	unsigned long flags;

	spin_lock_irqsave(&dev->wl->irq_lock, flags);

	assert(bcm43xx_status(dev) == BCM43xx_STAT_INITIALIZED);
	assert(dev->started);

	reason = dev->irq_reason;
	for (i = 0; i < ARRAY_SIZE(dma_reason); i++) {
		dma_reason[i] = dev->dma_reason[i];
		merged_dma_reason |= dma_reason[i];
	}

	if (unlikely(reason & BCM43xx_IRQ_MAC_TXERR))
		printkl(KERN_ERR PFX "MAC transmission error\n");

	if (unlikely(reason & BCM43xx_IRQ_PHY_TXERR))
		printkl(KERN_ERR PFX "PHY transmission error\n");

	if (unlikely(merged_dma_reason & (BCM43xx_DMAIRQ_FATALMASK |
					  BCM43xx_DMAIRQ_NONFATALMASK))) {
		if (merged_dma_reason & BCM43xx_DMAIRQ_FATALMASK) {
			printkl(KERN_ERR PFX "FATAL ERROR: Fatal DMA error: "
					     "0x%08X, 0x%08X, 0x%08X, "
					     "0x%08X, 0x%08X, 0x%08X\n",
			        dma_reason[0], dma_reason[1],
				dma_reason[2], dma_reason[3],
				dma_reason[4], dma_reason[5]);
			bcm43xx_controller_restart(dev, "DMA error");
			mmiowb();
			spin_unlock_irqrestore(&dev->wl->irq_lock, flags);
			return;
		}
		if (merged_dma_reason & BCM43xx_DMAIRQ_NONFATALMASK) {
			printkl(KERN_ERR PFX "DMA error: "
					     "0x%08X, 0x%08X, 0x%08X, "
					     "0x%08X, 0x%08X, 0x%08X\n",
			        dma_reason[0], dma_reason[1],
				dma_reason[2], dma_reason[3],
				dma_reason[4], dma_reason[5]);
		}
	}

	if (unlikely(reason & BCM43xx_IRQ_UCODE_DEBUG))
		handle_irq_ucode_debug(dev);
	if (reason & BCM43xx_IRQ_TBTT_INDI)
		handle_irq_tbtt_indication(dev);
	if (reason & BCM43xx_IRQ_ATIM_END)
		handle_irq_atim_end(dev);
	if (reason & BCM43xx_IRQ_BEACON)
		handle_irq_beacon(dev);
	if (reason & BCM43xx_IRQ_PMQ)
		handle_irq_pmq(dev);
	if (reason & BCM43xx_IRQ_TXFIFO_FLUSH_OK)
		;/*TODO*/
	if (reason & BCM43xx_IRQ_NOISESAMPLE_OK)
		handle_irq_noise(dev);

	/* Check the DMA reason registers for received data. */
	if (dma_reason[0] & BCM43xx_DMAIRQ_RX_DONE) {
		if (bcm43xx_using_pio(dev))
			bcm43xx_pio_rx(dev->pio.queue0);
		else
			bcm43xx_dma_rx(dev->dma.rx_ring0);
		/* We intentionally don't set "activity" to 1, here. */
	}
	assert(!(dma_reason[1] & BCM43xx_DMAIRQ_RX_DONE));
	assert(!(dma_reason[2] & BCM43xx_DMAIRQ_RX_DONE));
	if (dma_reason[3] & BCM43xx_DMAIRQ_RX_DONE) {
		if (bcm43xx_using_pio(dev))
			bcm43xx_pio_rx(dev->pio.queue3);
		else
			bcm43xx_dma_rx(dev->dma.rx_ring3);
		activity = 1;
	}
	assert(!(dma_reason[4] & BCM43xx_DMAIRQ_RX_DONE));
	assert(!(dma_reason[5] & BCM43xx_DMAIRQ_RX_DONE));

	if (reason & BCM43xx_IRQ_TX_OK) {
		handle_irq_transmit_status(dev);
		activity = 1;
		//TODO: In AP mode, this also causes sending of powersave responses.
	}

	if (!modparam_noleds)
		bcm43xx_leds_update(dev, activity);
	bcm43xx_interrupt_enable(dev, dev->irq_savedstate);
	mmiowb();
	spin_unlock_irqrestore(&dev->wl->irq_lock, flags);
}

static void pio_irq_workaround(struct bcm43xx_wldev *dev,
			       u16 base, int queueidx)
{
	u16 rxctl;

	rxctl = bcm43xx_read16(dev, base + BCM43xx_PIO_RXCTL);
	if (rxctl & BCM43xx_PIO_RXCTL_DATAAVAILABLE)
		dev->dma_reason[queueidx] |= BCM43xx_DMAIRQ_RX_DONE;
	else
		dev->dma_reason[queueidx] &= ~BCM43xx_DMAIRQ_RX_DONE;
}

static void bcm43xx_interrupt_ack(struct bcm43xx_wldev *dev, u32 reason)
{
	if (bcm43xx_using_pio(dev) &&
	    (dev->dev->id.revision < 3) &&
	    (!(reason & BCM43xx_IRQ_PIO_WORKAROUND))) {
		/* Apply a PIO specific workaround to the dma_reasons */
		pio_irq_workaround(dev, BCM43xx_MMIO_PIO1_BASE, 0);
		pio_irq_workaround(dev, BCM43xx_MMIO_PIO2_BASE, 1);
		pio_irq_workaround(dev, BCM43xx_MMIO_PIO3_BASE, 2);
		pio_irq_workaround(dev, BCM43xx_MMIO_PIO4_BASE, 3);
	}

	bcm43xx_write32(dev, BCM43xx_MMIO_GEN_IRQ_REASON, reason);

	bcm43xx_write32(dev, BCM43xx_MMIO_DMA0_REASON,
			dev->dma_reason[0]);
	bcm43xx_write32(dev, BCM43xx_MMIO_DMA1_REASON,
			dev->dma_reason[1]);
	bcm43xx_write32(dev, BCM43xx_MMIO_DMA2_REASON,
			dev->dma_reason[2]);
	bcm43xx_write32(dev, BCM43xx_MMIO_DMA3_REASON,
			dev->dma_reason[3]);
	bcm43xx_write32(dev, BCM43xx_MMIO_DMA4_REASON,
			dev->dma_reason[4]);
	bcm43xx_write32(dev, BCM43xx_MMIO_DMA5_REASON,
			dev->dma_reason[5]);
}

/* Interrupt handler top-half */
static irqreturn_t bcm43xx_interrupt_handler(int irq, void *dev_id)
{
	irqreturn_t ret = IRQ_HANDLED;
	struct bcm43xx_wldev *dev = dev_id;
	u32 reason;

	if (!dev)
		return IRQ_NONE;

	spin_lock(&dev->wl->irq_lock);

	reason = bcm43xx_read32(dev, BCM43xx_MMIO_GEN_IRQ_REASON);
	if (reason == 0xffffffff) {
		/* irq not for us (shared irq) */
		ret = IRQ_NONE;
		goto out;
	}
	reason &= bcm43xx_read32(dev, BCM43xx_MMIO_GEN_IRQ_MASK);
	if (!reason)
		goto out;

	assert(bcm43xx_status(dev) == BCM43xx_STAT_INITIALIZED);
	assert(dev->started);

	dev->dma_reason[0] = bcm43xx_read32(dev, BCM43xx_MMIO_DMA0_REASON)
			     & 0x0001DC00;
	dev->dma_reason[1] = bcm43xx_read32(dev, BCM43xx_MMIO_DMA1_REASON)
			     & 0x0000DC00;
	dev->dma_reason[2] = bcm43xx_read32(dev, BCM43xx_MMIO_DMA2_REASON)
			     & 0x0000DC00;
	dev->dma_reason[3] = bcm43xx_read32(dev, BCM43xx_MMIO_DMA3_REASON)
			     & 0x0001DC00;
	dev->dma_reason[4] = bcm43xx_read32(dev, BCM43xx_MMIO_DMA4_REASON)
			     & 0x0000DC00;
	dev->dma_reason[5] = bcm43xx_read32(dev, BCM43xx_MMIO_DMA5_REASON)
			     & 0x0000DC00;

	bcm43xx_interrupt_ack(dev, reason);
	/* disable all IRQs. They are enabled again in the bottom half. */
	dev->irq_savedstate = bcm43xx_interrupt_disable(dev, BCM43xx_IRQ_ALL);
	/* save the reason code and call our bottom half. */
	dev->irq_reason = reason;
	tasklet_schedule(&dev->isr_tasklet);
out:
	mmiowb();
	spin_unlock(&dev->wl->irq_lock);

	return ret;
}

static void bcm43xx_release_firmware(struct bcm43xx_wldev *dev)
{
	release_firmware(dev->fw.ucode);
	dev->fw.ucode = NULL;
	release_firmware(dev->fw.pcm);
	dev->fw.pcm = NULL;
	release_firmware(dev->fw.initvals0);
	dev->fw.initvals0 = NULL;
	release_firmware(dev->fw.initvals1);
	dev->fw.initvals1 = NULL;
}

static int bcm43xx_request_firmware(struct bcm43xx_wldev *dev)
{
	u8 rev = dev->dev->id.revision;
	int err = 0;
	int nr;
	char buf[22 + sizeof(modparam_fwpostfix) - 1] = { 0 };

	if (!dev->fw.ucode) {
		snprintf(buf, ARRAY_SIZE(buf), "bcm43xx_microcode%d%s.fw",
			 (rev >= 5 ? 5 : rev),
			 modparam_fwpostfix);
		err = request_firmware(&dev->fw.ucode, buf, dev->dev->dev);
		if (err) {
			printk(KERN_ERR PFX
			       "Error: Microcode \"%s\" not available or load failed.\n",
			        buf);
			goto error;
		}
	}

	if (!dev->fw.pcm) {
		snprintf(buf, ARRAY_SIZE(buf),
			 "bcm43xx_pcm%d%s.fw",
			 (rev < 5 ? 4 : 5),
			 modparam_fwpostfix);
		err = request_firmware(&dev->fw.pcm, buf, dev->dev->dev);
		if (err) {
			printk(KERN_ERR PFX
			       "Error: PCM \"%s\" not available or load failed.\n",
			       buf);
			goto error;
		}
	}

	if (!dev->fw.initvals0) {
		if (rev == 2 || rev == 4) {
			switch (dev->phy.type) {
			case BCM43xx_PHYTYPE_A:
				nr = 3;
				break;
			case BCM43xx_PHYTYPE_B:
			case BCM43xx_PHYTYPE_G:
				nr = 1;
				break;
			default:
				goto err_noinitval;
			}

		} else if (rev >= 5) {
			switch (dev->phy.type) {
			case BCM43xx_PHYTYPE_A:
				nr = 7;
				break;
			case BCM43xx_PHYTYPE_B:
			case BCM43xx_PHYTYPE_G:
				nr = 5;
				break;
			default:
				goto err_noinitval;
			}
		} else
			goto err_noinitval;
		snprintf(buf, ARRAY_SIZE(buf), "bcm43xx_initval%02d%s.fw",
			 nr, modparam_fwpostfix);

		err = request_firmware(&dev->fw.initvals0, buf, dev->dev->dev);
		if (err) {
			printk(KERN_ERR PFX
			       "Error: InitVals \"%s\" not available or load failed.\n",
			        buf);
			goto error;
		}
		if (dev->fw.initvals0->size % sizeof(struct bcm43xx_initval)) {
			printk(KERN_ERR PFX "InitVals fileformat error.\n");
			goto error;
		}
	}

	if (!dev->fw.initvals1) {
		if (rev >= 5) {
			u32 sbtmstatehigh;

			switch (dev->phy.type) {
			case BCM43xx_PHYTYPE_A:
				sbtmstatehigh = ssb_read32(dev->dev, SSB_TMSHIGH);
				if (sbtmstatehigh & 0x00010000)
					nr = 9;
				else
					nr = 10;
				break;
			case BCM43xx_PHYTYPE_B:
			case BCM43xx_PHYTYPE_G:
					nr = 6;
				break;
			default:
				goto err_noinitval;
			}
			snprintf(buf, ARRAY_SIZE(buf), "bcm43xx_initval%02d%s.fw",
				 nr, modparam_fwpostfix);

			err = request_firmware(&dev->fw.initvals1, buf, dev->dev->dev);
			if (err) {
				printk(KERN_ERR PFX
				       "Error: InitVals \"%s\" not available or load failed.\n",
					buf);
				goto error;
			}
			if (dev->fw.initvals1->size % sizeof(struct bcm43xx_initval)) {
				printk(KERN_ERR PFX "InitVals fileformat error.\n");
				goto error;
			}
		}
	}

out:
	return err;
error:
	bcm43xx_release_firmware(dev);
	goto out;
err_noinitval:
	printk(KERN_ERR PFX "Error: No InitVals available!\n");
	err = -ENOENT;
	goto error;
}

static int bcm43xx_upload_microcode(struct bcm43xx_wldev *dev)
{
	const __be32 *data;
	unsigned int i, len;
	u16 fwrev, fwpatch, fwdate, fwtime;
	u32 tmp;
	int err = 0;

	/* Upload Microcode. */
	data = (__be32 *)(dev->fw.ucode->data);
	len = dev->fw.ucode->size / sizeof(__be32);
	bcm43xx_shm_control_word(dev,
				 BCM43xx_SHM_UCODE | BCM43xx_SHM_AUTOINC_W,
				 0x0000);
	for (i = 0; i < len; i++) {
		bcm43xx_write32(dev, BCM43xx_MMIO_SHM_DATA,
				be32_to_cpu(data[i]));
		udelay(10);
	}

	/* Upload PCM data. */
	data = (__be32 *)(dev->fw.pcm->data);
	len = dev->fw.pcm->size / sizeof(__be32);
	bcm43xx_shm_control_word(dev, BCM43xx_SHM_HW, 0x01EA);
	bcm43xx_write32(dev, BCM43xx_MMIO_SHM_DATA, 0x00004000);
	/* No need for autoinc bit in SHM_HW */
	bcm43xx_shm_control_word(dev, BCM43xx_SHM_HW, 0x01EB);
	for (i = 0; i < len; i++) {
		bcm43xx_write32(dev, BCM43xx_MMIO_SHM_DATA,
				be32_to_cpu(data[i]));
		udelay(10);
	}

	bcm43xx_write32(dev, BCM43xx_MMIO_GEN_IRQ_REASON, BCM43xx_IRQ_ALL);
	bcm43xx_write32(dev, BCM43xx_MMIO_STATUS_BITFIELD, 0x00020402);

	/* Wait for the microcode to load and respond */
	i = 0;
	while (1) {
		tmp = bcm43xx_read32(dev, BCM43xx_MMIO_GEN_IRQ_REASON);
		if (tmp == BCM43xx_IRQ_MAC_SUSPENDED)
			break;
		i++;
		if (i >= BCM43xx_IRQWAIT_MAX_RETRIES) {
			printk(KERN_ERR PFX "Microcode not responding\n");
			err = -ENODEV;
			goto out;
		}
		udelay(10);
	}
	bcm43xx_read32(dev, BCM43xx_MMIO_GEN_IRQ_REASON); /* dummy read */

	/* Get and check the revisions. */
	fwrev = bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED,
				   BCM43xx_SHM_SH_UCODEREV);
	fwpatch = bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED,
				     BCM43xx_SHM_SH_UCODEPATCH);
	fwdate = bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED,
				    BCM43xx_SHM_SH_UCODEDATE);
	fwtime = bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED,
				    BCM43xx_SHM_SH_UCODETIME);

	if (fwrev <= 0x128) {
		printk(KERN_ERR PFX "YOUR FIRMWARE IS TOO OLD. Firmware from "
		       "binary drivers older than version 4.x is unsupported. "
		       "You must upgrade your firmware files.\n");
		bcm43xx_write32(dev, BCM43xx_MMIO_STATUS_BITFIELD, 0);
		err = -EOPNOTSUPP;
		goto out;
	}
	printk(KERN_DEBUG PFX "Loading firmware version %u.%u "
			      "(20%.2i-%.2i-%.2i %.2i:%.2i:%.2i)\n",
	       fwrev, fwpatch,
	       (fwdate >> 12) & 0xF, (fwdate >> 8) & 0xF, fwdate & 0xFF,
	       (fwtime >> 11) & 0x1F, (fwtime >> 5) & 0x3F, fwtime & 0x1F);

	dev->fw.rev = fwrev;
	dev->fw.patch = fwpatch;

out:
	return err;
}

static int bcm43xx_write_initvals(struct bcm43xx_wldev *dev,
				  const struct bcm43xx_initval *data,
				  const unsigned int len)
{
	u16 offset, size;
	u32 value;
	unsigned int i;

	for (i = 0; i < len; i++) {
		offset = be16_to_cpu(data[i].offset);
		size = be16_to_cpu(data[i].size);
		value = be32_to_cpu(data[i].value);

		if (unlikely(offset >= 0x1000))
			goto err_format;
		if (size == 2) {
			if (unlikely(value & 0xFFFF0000))
				goto err_format;
			bcm43xx_write16(dev, offset, (u16)value);
		} else if (size == 4) {
			bcm43xx_write32(dev, offset, value);
		} else
			goto err_format;
	}

	return 0;

err_format:
	printk(KERN_ERR PFX "InitVals (bcm43xx_initvalXX.fw) file-format error. "
			    "Please fix your bcm43xx firmware files.\n");
	return -EPROTO;
}

static int bcm43xx_upload_initvals(struct bcm43xx_wldev *dev)
{
	int err;

	err = bcm43xx_write_initvals(dev, (struct bcm43xx_initval *)dev->fw.initvals0->data,
				     dev->fw.initvals0->size / sizeof(struct bcm43xx_initval));
	if (err)
		goto out;
	if (dev->fw.initvals1) {
		err = bcm43xx_write_initvals(dev, (struct bcm43xx_initval *)dev->fw.initvals1->data,
					     dev->fw.initvals1->size / sizeof(struct bcm43xx_initval));
		if (err)
			goto out;
	}
out:
	return err;
}

/* Initialize the GPIOs
 * http://bcm-specs.sipsolutions.net/GPIO
 */
static int bcm43xx_gpio_init(struct bcm43xx_wldev *dev)
{
	struct ssb_bus *bus = dev->dev->bus;
	struct ssb_device *gpiodev, *pcidev = NULL;
	u32 mask, set;

	bcm43xx_write32(dev, BCM43xx_MMIO_STATUS_BITFIELD,
			bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD)
			& 0xFFFF3FFF);

	bcm43xx_leds_switch_all(dev, 0);
	bcm43xx_write16(dev, BCM43xx_MMIO_GPIO_MASK,
			bcm43xx_read16(dev, BCM43xx_MMIO_GPIO_MASK)
			| 0x000F);

	mask = 0x0000001F;
	set = 0x0000000F;
	if (dev->dev->bus->chip_id == 0x4301) {
		mask |= 0x0060;
		set |= 0x0060;
	}
	if (0 /* FIXME: conditional unknown */) {
		bcm43xx_write16(dev, BCM43xx_MMIO_GPIO_MASK,
				bcm43xx_read16(dev, BCM43xx_MMIO_GPIO_MASK)
				| 0x0100);
		mask |= 0x0180;
		set |= 0x0180;
	}
	if (dev->dev->bus->sprom.r1.boardflags_lo & BCM43xx_BFL_PACTRL) {
		bcm43xx_write16(dev, BCM43xx_MMIO_GPIO_MASK,
				bcm43xx_read16(dev, BCM43xx_MMIO_GPIO_MASK)
				| 0x0200);
		mask |= 0x0200;
		set |= 0x0200;
	}
	if (dev->dev->id.revision >= 2)
		mask  |= 0x0010; /* FIXME: This is redundant. */

#ifdef CONFIG_SSB_DRIVER_PCICORE
	pcidev = bus->pcicore.dev;
#endif
	gpiodev = bus->chipco.dev ? : pcidev;
	if (!gpiodev)
		return 0;
	ssb_write32(gpiodev, BCM43xx_GPIO_CONTROL,
		    (ssb_read32(gpiodev, BCM43xx_GPIO_CONTROL)
		     & mask) | set);

	return 0;
}

/* Turn off all GPIO stuff. Call this on module unload, for example. */
static void bcm43xx_gpio_cleanup(struct bcm43xx_wldev *dev)
{
	struct ssb_bus *bus = dev->dev->bus;
	struct ssb_device *gpiodev, *pcidev = NULL;

#ifdef CONFIG_SSB_DRIVER_PCICORE
	pcidev = bus->pcicore.dev;
#endif
	gpiodev = bus->chipco.dev ? : pcidev;
	if (!gpiodev)
		return;
	ssb_write32(gpiodev, BCM43xx_GPIO_CONTROL, 0);
}

/* http://bcm-specs.sipsolutions.net/EnableMac */
void bcm43xx_mac_enable(struct bcm43xx_wldev *dev)
{
	dev->mac_suspended--;
	assert(dev->mac_suspended >= 0);
	if (dev->mac_suspended == 0) {
		bcm43xx_write32(dev, BCM43xx_MMIO_STATUS_BITFIELD,
		                bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD)
				| BCM43xx_SBF_MAC_ENABLED);
		bcm43xx_write32(dev, BCM43xx_MMIO_GEN_IRQ_REASON,
				BCM43xx_IRQ_MAC_SUSPENDED);
		bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD); /* dummy read */
		bcm43xx_read32(dev, BCM43xx_MMIO_GEN_IRQ_REASON); /* dummy read */
		bcm43xx_power_saving_ctl_bits(dev, -1, -1);
	}
}

/* http://bcm-specs.sipsolutions.net/SuspendMAC */
void bcm43xx_mac_suspend(struct bcm43xx_wldev *dev)
{
	int i;
	u32 tmp;

	assert(dev->mac_suspended >= 0);
	if (dev->mac_suspended == 0) {
		bcm43xx_power_saving_ctl_bits(dev, -1, 1);
		bcm43xx_write32(dev, BCM43xx_MMIO_STATUS_BITFIELD,
		                bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD)
				& ~BCM43xx_SBF_MAC_ENABLED);
		bcm43xx_read32(dev, BCM43xx_MMIO_GEN_IRQ_REASON); /* dummy read */
		for (i = 10000; i; i--) {
			tmp = bcm43xx_read32(dev, BCM43xx_MMIO_GEN_IRQ_REASON);
			if (tmp & BCM43xx_IRQ_MAC_SUSPENDED)
				goto out;
			udelay(1);
		}
		printkl(KERN_ERR PFX "MAC suspend failed\n");
	}
out:
	dev->mac_suspended++;
}

static void bcm43xx_adjust_opmode(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_wl *wl = dev->wl;
	u32 ctl;
	u16 cfp_pretbtt;

	ctl = bcm43xx_read32(dev, BCM43xx_MMIO_MACCTL);
	/* Reset status to STA infrastructure mode. */
	ctl &= ~BCM43xx_MACCTL_AP;
	ctl &= ~BCM43xx_MACCTL_KEEP_CTL;
	ctl &= ~BCM43xx_MACCTL_KEEP_BADPLCP;
	ctl &= ~BCM43xx_MACCTL_KEEP_BAD;
	ctl &= ~BCM43xx_MACCTL_PROMISC;
	ctl |= BCM43xx_MACCTL_INFRA;

	if (wl->operating) {
		switch (wl->if_type) {
		case IEEE80211_IF_TYPE_AP:
			ctl |= BCM43xx_MACCTL_AP;
			break;
		case IEEE80211_IF_TYPE_IBSS:
			ctl &= ~BCM43xx_MACCTL_INFRA;
			break;
		case IEEE80211_IF_TYPE_STA:
		case IEEE80211_IF_TYPE_MNTR:
		case IEEE80211_IF_TYPE_WDS:
			break;
		default:
			assert(0);
		}
	}
	if (wl->monitor) {
		ctl |= BCM43xx_MACCTL_KEEP_CTL;
		if (modparam_mon_keep_bad)
			ctl |= BCM43xx_MACCTL_KEEP_BAD;
		if (modparam_mon_keep_badplcp)
			ctl |= BCM43xx_MACCTL_KEEP_BADPLCP;
	}
	if (wl->promisc)
		ctl |= BCM43xx_MACCTL_PROMISC;

	bcm43xx_write32(dev, BCM43xx_MMIO_MACCTL, ctl);

	cfp_pretbtt = 2;
	if ((ctl & BCM43xx_MACCTL_INFRA) &&
	    !(ctl & BCM43xx_MACCTL_AP)) {
		if (dev->dev->bus->chip_id == 0x4306 &&
		    dev->dev->bus->chip_rev == 3)
			cfp_pretbtt = 100;
		else
			cfp_pretbtt = 50;
	}
	bcm43xx_write16(dev, 0x612, cfp_pretbtt);
}

static void bcm43xx_rate_memory_write(struct bcm43xx_wldev *dev,
				      u16 rate,
				      int is_ofdm)
{
	u16 offset;

	if (is_ofdm) {
		offset = 0x480;
		offset += (bcm43xx_plcp_get_ratecode_ofdm(rate) & 0x000F) * 2;
	} else {
		offset = 0x4C0;
		offset += (bcm43xx_plcp_get_ratecode_cck(rate) & 0x000F) * 2;
	}
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, offset + 0x20,
			    bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED, offset));
}

static void bcm43xx_rate_memory_init(struct bcm43xx_wldev *dev)
{
	switch (dev->phy.type) {
	case BCM43xx_PHYTYPE_A:
	case BCM43xx_PHYTYPE_G:
		bcm43xx_rate_memory_write(dev, BCM43xx_OFDM_RATE_6MB, 1);
		bcm43xx_rate_memory_write(dev, BCM43xx_OFDM_RATE_12MB, 1);
		bcm43xx_rate_memory_write(dev, BCM43xx_OFDM_RATE_18MB, 1);
		bcm43xx_rate_memory_write(dev, BCM43xx_OFDM_RATE_24MB, 1);
		bcm43xx_rate_memory_write(dev, BCM43xx_OFDM_RATE_36MB, 1);
		bcm43xx_rate_memory_write(dev, BCM43xx_OFDM_RATE_48MB, 1);
		bcm43xx_rate_memory_write(dev, BCM43xx_OFDM_RATE_54MB, 1);
		if (dev->phy.type == BCM43xx_PHYTYPE_A)
			break;
		/* fallthrough */
	case BCM43xx_PHYTYPE_B:
		bcm43xx_rate_memory_write(dev, BCM43xx_CCK_RATE_1MB, 0);
		bcm43xx_rate_memory_write(dev, BCM43xx_CCK_RATE_2MB, 0);
		bcm43xx_rate_memory_write(dev, BCM43xx_CCK_RATE_5MB, 0);
		bcm43xx_rate_memory_write(dev, BCM43xx_CCK_RATE_11MB, 0);
		break;
	default:
		assert(0);
	}
}

/* Set the TX-Antenna for management frames sent by firmware. */
static void bcm43xx_mgmtframe_txantenna(struct bcm43xx_wldev *dev,
					int antenna)
{
	u16 ant = 0;
	u16 tmp;

	switch (antenna) {
	case BCM43xx_ANTENNA0:
		ant |= BCM43xx_TX4_PHY_ANT0;
		break;
	case BCM43xx_ANTENNA1:
		ant |= BCM43xx_TX4_PHY_ANT1;
		break;
	case BCM43xx_ANTENNA_AUTO:
		ant |= BCM43xx_TX4_PHY_ANTLAST;
		break;
	default:
		assert(0);
	}

	/* FIXME We also need to set the other flags of the PHY control field somewhere. */

	/* For Beacons */
	tmp = bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED,
				 BCM43xx_SHM_SH_BEACPHYCTL);
	tmp = (tmp & ~BCM43xx_TX4_PHY_ANT) | ant;
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED,
			    BCM43xx_SHM_SH_BEACPHYCTL, tmp);
	/* For ACK/CTS */
	tmp = bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED,
				 BCM43xx_SHM_SH_ACKCTSPHYCTL);
	tmp = (tmp & ~BCM43xx_TX4_PHY_ANT) | ant;
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED,
			    BCM43xx_SHM_SH_ACKCTSPHYCTL, tmp);
	/* For Probe Resposes */
	tmp = bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED,
				 BCM43xx_SHM_SH_PRPHYCTL);
	tmp = (tmp & ~BCM43xx_TX4_PHY_ANT) | ant;
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED,
			    BCM43xx_SHM_SH_PRPHYCTL, tmp);
}

/* This is the opposite of bcm43xx_chip_init() */
static void bcm43xx_chip_exit(struct bcm43xx_wldev *dev)
{
	bcm43xx_radio_turn_off(dev);
	if (!modparam_noleds)
		bcm43xx_leds_exit(dev);
	bcm43xx_gpio_cleanup(dev);
	/* firmware is released later */
}

/* Initialize the chip
 * http://bcm-specs.sipsolutions.net/ChipInit
 */
static int bcm43xx_chip_init(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	int err, tmp;
	u32 value32;
	u16 value16;

	bcm43xx_write32(dev, BCM43xx_MMIO_STATUS_BITFIELD,
			BCM43xx_SBF_CORE_READY
			| BCM43xx_SBF_400);

	err = bcm43xx_request_firmware(dev);
	if (err)
		goto out;
	err = bcm43xx_upload_microcode(dev);
	if (err)
		goto out; /* firmware is released later */

	err = bcm43xx_gpio_init(dev);
	if (err)
		goto out; /* firmware is released later */
	err = bcm43xx_upload_initvals(dev);
	if (err)
		goto err_gpio_cleanup;
	bcm43xx_radio_turn_on(dev);
	dev->radio_hw_enable = bcm43xx_is_hw_radio_enabled(dev);
	dprintk(KERN_INFO PFX "Radio %s by hardware\n",
		(dev->radio_hw_enable == 0) ? "disabled" : "enabled");

	bcm43xx_write16(dev, 0x03E6, 0x0000);
	err = bcm43xx_phy_init(dev);
	if (err)
		goto err_radio_off;

	/* Select initial Interference Mitigation. */
	tmp = phy->interfmode;
	phy->interfmode = BCM43xx_INTERFMODE_NONE;
	bcm43xx_radio_set_interference_mitigation(dev, tmp);

	bcm43xx_set_rx_antenna(dev, BCM43xx_ANTENNA_DEFAULT);
	bcm43xx_mgmtframe_txantenna(dev, BCM43xx_ANTENNA_DEFAULT);

	if (phy->type == BCM43xx_PHYTYPE_B) {
		value16 = bcm43xx_read16(dev, 0x005E);
		value16 |= 0x0004;
		bcm43xx_write16(dev, 0x005E, value16);
	}
	bcm43xx_write32(dev, 0x0100, 0x01000000);
	if (dev->dev->id.revision < 5)
		bcm43xx_write32(dev, 0x010C, 0x01000000);

	value32 = bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD);
	value32 &= ~ BCM43xx_SBF_MODE_NOTADHOC;
	bcm43xx_write32(dev, BCM43xx_MMIO_STATUS_BITFIELD, value32);
	value32 = bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD);
	value32 |= BCM43xx_SBF_MODE_NOTADHOC;
	bcm43xx_write32(dev, BCM43xx_MMIO_STATUS_BITFIELD, value32);

	value32 = bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD);
	value32 |= 0x100000;
	bcm43xx_write32(dev, BCM43xx_MMIO_STATUS_BITFIELD, value32);

	if (bcm43xx_using_pio(dev)) {
		bcm43xx_write32(dev, 0x0210, 0x00000100);
		bcm43xx_write32(dev, 0x0230, 0x00000100);
		bcm43xx_write32(dev, 0x0250, 0x00000100);
		bcm43xx_write32(dev, 0x0270, 0x00000100);
		bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, 0x0034, 0x0000);
	}

	/* Probe Response Timeout value */
	/* FIXME: Default to 0, has to be set by ioctl probably... :-/ */
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, 0x0074, 0x0000);

	/* Initially set the wireless operation mode. */
	bcm43xx_adjust_opmode(dev);

	if (dev->dev->id.revision < 3) {
		bcm43xx_write16(dev, 0x060E, 0x0000);
		bcm43xx_write16(dev, 0x0610, 0x8000);
		bcm43xx_write16(dev, 0x0604, 0x0000);
		bcm43xx_write16(dev, 0x0606, 0x0200);
	} else {
		bcm43xx_write32(dev, 0x0188, 0x80000000);
		bcm43xx_write32(dev, 0x018C, 0x02000000);
	}
	bcm43xx_write32(dev, BCM43xx_MMIO_GEN_IRQ_REASON, 0x00004000);
	bcm43xx_write32(dev, BCM43xx_MMIO_DMA0_IRQ_MASK, 0x0001DC00);
	bcm43xx_write32(dev, BCM43xx_MMIO_DMA1_IRQ_MASK, 0x0000DC00);
	bcm43xx_write32(dev, BCM43xx_MMIO_DMA2_IRQ_MASK, 0x0000DC00);
	bcm43xx_write32(dev, BCM43xx_MMIO_DMA3_IRQ_MASK, 0x0001DC00);
	bcm43xx_write32(dev, BCM43xx_MMIO_DMA4_IRQ_MASK, 0x0000DC00);
	bcm43xx_write32(dev, BCM43xx_MMIO_DMA5_IRQ_MASK, 0x0000DC00);

	value32 = ssb_read32(dev->dev, SSB_TMSLOW);
	value32 |= 0x00100000;
	ssb_write32(dev->dev, SSB_TMSLOW, value32);

	bcm43xx_write16(dev, BCM43xx_MMIO_POWERUP_DELAY,
			dev->dev->bus->chipco.fast_pwrup_delay);

	assert(err == 0);
	dprintk(KERN_INFO PFX "Chip initialized\n");
out:
	return err;

err_radio_off:
	bcm43xx_radio_turn_off(dev);
err_gpio_cleanup:
	bcm43xx_gpio_cleanup(dev);
	goto out;
}

static void bcm43xx_periodic_every120sec(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;

	if (phy->type != BCM43xx_PHYTYPE_G || phy->rev < 2)
		return;

	bcm43xx_mac_suspend(dev);
	bcm43xx_lo_g_measure(dev);
	bcm43xx_mac_enable(dev);
}

static void bcm43xx_periodic_every60sec(struct bcm43xx_wldev *dev)
{
	bcm43xx_lo_g_ctl_mark_all_unused(dev);
	if (dev->dev->bus->sprom.r1.boardflags_lo & BCM43xx_BFL_RSSI) {
		bcm43xx_mac_suspend(dev);
		bcm43xx_calc_nrssi_slope(dev);
		bcm43xx_mac_enable(dev);
	}
}

static void bcm43xx_periodic_every30sec(struct bcm43xx_wldev *dev)
{
	/* Update device statistics. */
	bcm43xx_calculate_link_quality(dev);
}

static void bcm43xx_periodic_every15sec(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;

	if (phy->type == BCM43xx_PHYTYPE_G) {
		//TODO: update_aci_moving_average
		if (phy->aci_enable && phy->aci_wlan_automatic) {
			bcm43xx_mac_suspend(dev);
			if (!phy->aci_enable && 1 /*TODO: not scanning? */) {
				if (0 /*TODO: bunch of conditions*/) {
					bcm43xx_radio_set_interference_mitigation(dev,
										  BCM43xx_INTERFMODE_MANUALWLAN);
				}
			} else if (1/*TODO*/) {
				/*
				if ((aci_average > 1000) && !(bcm43xx_radio_aci_scan(dev))) {
					bcm43xx_radio_set_interference_mitigation(dev,
										  BCM43xx_INTERFMODE_NONE);
				}
				*/
			}
			bcm43xx_mac_enable(dev);
		} else if (phy->interfmode == BCM43xx_INTERFMODE_NONWLAN &&
			   phy->rev == 1) {
			//TODO: implement rev1 workaround
		}
	}
	bcm43xx_phy_xmitpower(dev); //FIXME: unless scanning?
	//TODO for APHY (temperature?)
}

static void bcm43xx_periodic_every1sec(struct bcm43xx_wldev *dev)
{
	int radio_hw_enable;

	/* check if radio hardware enabled status changed */
	radio_hw_enable = bcm43xx_is_hw_radio_enabled(dev);
	if (unlikely(dev->radio_hw_enable != radio_hw_enable)) {
		dev->radio_hw_enable = radio_hw_enable;
		dprintk(KERN_INFO PFX "Radio hardware status changed to %s\n",
			(radio_hw_enable == 0) ? "disabled" : "enabled");
		bcm43xx_leds_update(dev, 0);
	}
}

static void do_periodic_work(struct bcm43xx_wldev *dev)
{
	unsigned int state;

	state = dev->periodic_state;
	if (state % 120 == 0)
		bcm43xx_periodic_every120sec(dev);
	if (state % 60 == 0)
		bcm43xx_periodic_every60sec(dev);
	if (state % 30 == 0)
		bcm43xx_periodic_every30sec(dev);
	if (state % 15 == 0)
		bcm43xx_periodic_every15sec(dev);
	bcm43xx_periodic_every1sec(dev);
}

/* Estimate a "Badness" value based on the periodic work
 * state-machine state. "Badness" is worse (bigger), if the
 * periodic work will take longer.
 */
static int estimate_periodic_work_badness(unsigned int state)
{
	int badness = 0;

	if (state % 120 == 0) /* every 120 sec */
		badness += 10;
	if (state % 60 == 0) /* every 60 sec */
		badness += 5;
	if (state % 30 == 0) /* every 30 sec */
		badness += 1;
	if (state % 15 == 0) /* every 15 sec */
		badness += 1;

#define BADNESS_LIMIT	4
	return badness;
}

static void bcm43xx_periodic_work_handler(struct work_struct *work)
{
	struct bcm43xx_wldev *dev =
		container_of(work, struct bcm43xx_wldev, periodic_work.work);
	unsigned long flags, delay;
	u32 savedirqs = 0;
	int badness;

	mutex_lock(&dev->wl->mutex);

	if (unlikely(bcm43xx_status(dev) != BCM43xx_STAT_INITIALIZED))
		goto out;
	if (unlikely(!dev->started))
		goto out;
	if (bcm43xx_debug(dev, BCM43xx_DBG_PWORK_STOP))
		goto out_requeue;

	badness = estimate_periodic_work_badness(dev->periodic_state);
	if (badness > BADNESS_LIMIT) {
		spin_lock_irqsave(&dev->wl->irq_lock, flags);
		/* Suspend TX as we don't want to transmit packets while
		 * we recalibrate the hardware. */
		bcm43xx_tx_suspend(dev);
		savedirqs = bcm43xx_interrupt_disable(dev, BCM43xx_IRQ_ALL);
		/* Periodic work will take a long time, so we want it to
		 * be preemtible and release the spinlock. */
		spin_unlock_irqrestore(&dev->wl->irq_lock, flags);
		bcm43xx_synchronize_irq(dev);

		do_periodic_work(dev);

		spin_lock_irqsave(&dev->wl->irq_lock, flags);
		bcm43xx_interrupt_enable(dev, savedirqs);
		bcm43xx_tx_resume(dev);
		mmiowb();
		spin_unlock_irqrestore(&dev->wl->irq_lock, flags);
	} else {
		/* Take the global driver lock. This will lock any operation. */
		spin_lock_irqsave(&dev->wl->irq_lock, flags);

		do_periodic_work(dev);

		mmiowb();
		spin_unlock_irqrestore(&dev->wl->irq_lock, flags);
	}
	dev->periodic_state++;
out_requeue:
	if (bcm43xx_debug(dev, BCM43xx_DBG_PWORK_FAST))
		delay = msecs_to_jiffies(50);
	else
		delay = round_jiffies(HZ);
	queue_delayed_work(dev->wl->hw->workqueue,
			   &dev->periodic_work, delay);
out:
	mutex_unlock(&dev->wl->mutex);
}

static void bcm43xx_periodic_tasks_delete(struct bcm43xx_wldev *dev)
{
	cancel_rearming_delayed_work(&dev->periodic_work);
}

static void bcm43xx_periodic_tasks_setup(struct bcm43xx_wldev *dev)
{
	struct delayed_work *work = &dev->periodic_work;

	assert(bcm43xx_status(dev) == BCM43xx_STAT_INITIALIZED);
	dev->periodic_state = 0;
	INIT_DELAYED_WORK(work, bcm43xx_periodic_work_handler);
	queue_delayed_work(dev->wl->hw->workqueue, work, 0);
}

/* Validate access to the chip (SHM) */
static int bcm43xx_validate_chipaccess(struct bcm43xx_wldev *dev)
{
	u32 value;
	u32 shm_backup;

	shm_backup = bcm43xx_shm_read32(dev, BCM43xx_SHM_SHARED, 0);
	bcm43xx_shm_write32(dev, BCM43xx_SHM_SHARED, 0, 0xAA5555AA);
	if (bcm43xx_shm_read32(dev, BCM43xx_SHM_SHARED, 0) != 0xAA5555AA)
		goto error;
	bcm43xx_shm_write32(dev, BCM43xx_SHM_SHARED, 0, 0x55AAAA55);
	if (bcm43xx_shm_read32(dev, BCM43xx_SHM_SHARED, 0) != 0x55AAAA55)
		goto error;
	bcm43xx_shm_write32(dev, BCM43xx_SHM_SHARED, 0, shm_backup);

	value = bcm43xx_read32(dev, BCM43xx_MMIO_MACCTL);
	if ((value | BCM43xx_MACCTL_GMODE) !=
	    (BCM43xx_MACCTL_GMODE | BCM43xx_MACCTL_IHR_ENABLED))
		goto error;

	value = bcm43xx_read32(dev, BCM43xx_MMIO_GEN_IRQ_REASON);
	if (value)
		goto error;

	return 0;
error:
	printk(KERN_ERR PFX "Failed to validate the chipaccess\n");
	return -ENODEV;
}

static void bcm43xx_security_init(struct bcm43xx_wldev *dev)
{
	dev->max_nr_keys = (dev->dev->id.revision >= 5) ? 58 : 20;
	assert(dev->max_nr_keys <= ARRAY_SIZE(dev->key));
	dev->ktp = bcm43xx_shm_read16(dev, BCM43xx_SHM_SHARED,
				      BCM43xx_SHM_SH_KTP);
	/* KTP is a word address, but we address SHM bytewise.
	 * So multiply by two.
	 */
	dev->ktp *= 2;
	if (dev->dev->id.revision >= 5) {
		/* Number of RCMTA address slots */
		bcm43xx_write16(dev, BCM43xx_MMIO_RCMTA_COUNT,
				dev->max_nr_keys - 8);
	}
	bcm43xx_clear_keys(dev);
}

static int bcm43xx_rng_read(struct hwrng *rng, u32 *data)
{
	struct bcm43xx_wl *wl = (struct bcm43xx_wl *)rng->priv;
	unsigned long flags;

	/* Don't take wl->mutex here, as it could deadlock with
	 * hwrng internal locking. It's not needed to take
	 * wl->mutex here, anyway. */

	spin_lock_irqsave(&wl->irq_lock, flags);
	*data = bcm43xx_read16(wl->current_dev, BCM43xx_MMIO_RNG);
	spin_unlock_irqrestore(&wl->irq_lock, flags);

	return (sizeof(u16));
}

static void bcm43xx_rng_exit(struct bcm43xx_wl *wl)
{
	if (wl->rng_initialized)
		hwrng_unregister(&wl->rng);
}

static int bcm43xx_rng_init(struct bcm43xx_wl *wl)
{
	int err;

	snprintf(wl->rng_name, ARRAY_SIZE(wl->rng_name),
		 "%s_%s", KBUILD_MODNAME, wiphy_name(wl->hw->wiphy));
	wl->rng.name = wl->rng_name;
	wl->rng.data_read = bcm43xx_rng_read;
	wl->rng.priv = (unsigned long)wl;
	wl->rng_initialized = 1;
	err = hwrng_register(&wl->rng);
	if (err) {
		wl->rng_initialized = 0;
		printk(KERN_ERR PFX "Failed to register the random "
		       "number generator (%d)\n", err);
	}

	return err;
}

static int bcm43xx_tx(struct ieee80211_hw *hw,
		      struct sk_buff *skb,
		      struct ieee80211_tx_control *ctl)
{
	struct bcm43xx_wl *wl = hw_to_bcm43xx_wl(hw);
	struct bcm43xx_wldev *dev = wl->current_dev;
	int err = -ENODEV;
	unsigned long flags;

	/* DMA-TX is done without a global lock. */
	if (unlikely(!dev))
		goto out;
	assert(bcm43xx_status(dev) == BCM43xx_STAT_INITIALIZED);
	assert(dev->started);
	if (bcm43xx_using_pio(dev)) {
		spin_lock_irqsave(&wl->irq_lock, flags);
		err = bcm43xx_pio_tx(dev, skb, ctl);
		spin_unlock_irqrestore(&wl->irq_lock, flags);
	} else
		err = bcm43xx_dma_tx(dev, skb, ctl);
out:
	if (unlikely(err))
		return NETDEV_TX_BUSY;
	return NETDEV_TX_OK;
}

static int bcm43xx_conf_tx(struct ieee80211_hw *hw,
			   int queue,
			   const struct ieee80211_tx_queue_params *params)
{
	return 0;
}

static int bcm43xx_get_tx_stats(struct ieee80211_hw *hw,
				struct ieee80211_tx_queue_stats *stats)
{
	struct bcm43xx_wl *wl = hw_to_bcm43xx_wl(hw);
	struct bcm43xx_wldev *dev = wl->current_dev;
	unsigned long flags;
	int err = -ENODEV;

	if (!dev)
		goto out;
	spin_lock_irqsave(&wl->irq_lock, flags);
	if (likely(bcm43xx_status(dev) == BCM43xx_STAT_INITIALIZED)) {
		if (bcm43xx_using_pio(dev))
			bcm43xx_pio_get_tx_stats(dev, stats);
		else
			bcm43xx_dma_get_tx_stats(dev, stats);
		err = 0;
	}
	spin_unlock_irqrestore(&wl->irq_lock, flags);
out:
	return err;
}

static int bcm43xx_get_stats(struct ieee80211_hw *hw,
			     struct ieee80211_low_level_stats *stats)
{
	struct bcm43xx_wl *wl = hw_to_bcm43xx_wl(hw);
	unsigned long flags;

	spin_lock_irqsave(&wl->irq_lock, flags);
	memcpy(stats, &wl->ieee_stats, sizeof(*stats));
	spin_unlock_irqrestore(&wl->irq_lock, flags);

	return 0;
}

static int bcm43xx_dev_reset(struct ieee80211_hw *hw)
{
	struct bcm43xx_wl *wl = hw_to_bcm43xx_wl(hw);
	struct bcm43xx_wldev *dev = wl->current_dev;
	unsigned long flags;

	if (!dev)
		return -ENODEV;
	spin_lock_irqsave(&wl->irq_lock, flags);
	bcm43xx_controller_restart(dev, "Reset by ieee80211 subsystem");
	spin_unlock_irqrestore(&wl->irq_lock, flags);

	return 0;
}

static const char * phymode_to_string(unsigned int phymode)
{
	switch (phymode) {
	case BCM43xx_PHYMODE_A:
		return "A";
	case BCM43xx_PHYMODE_B:
		return "B";
	case BCM43xx_PHYMODE_G:
		return "G";
	default:
		assert(0);
	}
	return "";
}

static int find_wldev_for_phymode(struct bcm43xx_wl *wl,
				  unsigned int phymode,
				  struct bcm43xx_wldev **dev,
				  int *gmode)
{
	struct bcm43xx_wldev *d;

	list_for_each_entry(d, &wl->devlist, list) {
		if (d->phy.possible_phymodes & phymode) {
			/* Ok, this device supports the PHY-mode.
			 * Now figure out how the gmode bit has to be
			 * set to support it. */
			if (phymode == BCM43xx_PHYMODE_A)
				*gmode = 0;
			else
				*gmode = 1;
			*dev = d;

			return 0;
		}
	}

	return -ESRCH;
}

static void bcm43xx_put_phy_into_reset(struct bcm43xx_wldev *dev)
{
	struct ssb_device *sdev = dev->dev;
	u32 tmslow;

	tmslow = ssb_read32(sdev, SSB_TMSLOW);
	tmslow &= ~BCM43xx_TMSLOW_GMODE;
	tmslow |= BCM43xx_TMSLOW_PHYRESET;
	tmslow |= SSB_TMSLOW_FGC;
	ssb_write32(sdev, SSB_TMSLOW, tmslow);
	msleep(1);

	tmslow = ssb_read32(sdev, SSB_TMSLOW);
	tmslow &= ~SSB_TMSLOW_FGC;
	tmslow |= BCM43xx_TMSLOW_PHYRESET;
	ssb_write32(sdev, SSB_TMSLOW, tmslow);
	msleep(1);
}

static int bcm43xx_switch_phymode(struct bcm43xx_wl *wl,
				  unsigned int new_mode)
{
	struct bcm43xx_wldev *up_dev;
	struct bcm43xx_wldev *down_dev;
	int err;
	int gmode = -1;
	int old_was_started = 0;
	int old_was_inited = 0;

	err = find_wldev_for_phymode(wl, new_mode, &up_dev, &gmode);
	if (err) {
		printk(KERN_INFO PFX "Could not find a device for %s-PHY mode\n",
		       phymode_to_string(new_mode));
		return err;
	}
	assert(gmode == 0 || gmode == 1);
	if ((up_dev == wl->current_dev) &&
	    (wl->current_dev->phy.gmode == gmode)) {
		/* This device is already running. */
		return 0;
	}
	dprintk(KERN_INFO PFX "Reconfiguring PHYmode to %s-PHY\n",
		phymode_to_string(new_mode));
	down_dev = wl->current_dev;

	/* Shutdown the currently running core. */
	if (down_dev->started) {
		old_was_started = 1;
		bcm43xx_wireless_core_stop(down_dev);
	}
	if (bcm43xx_status(down_dev) == BCM43xx_STAT_INITIALIZED) {
		old_was_inited = 1;
		bcm43xx_wireless_core_exit(down_dev);
	}

	if (down_dev != up_dev) {
		/* We switch to a different core, so we put PHY into
		 * RESET on the old core. */
		bcm43xx_put_phy_into_reset(down_dev);
	}

	/* Now start the new core. */
	up_dev->phy.gmode = gmode;
	if (old_was_inited) {
		err = bcm43xx_wireless_core_init(up_dev);
		if (err) {
			printk(KERN_INFO PFX "Fatal: Could not initialize device for "
			       "new selected %s-PHY mode\n",
			       phymode_to_string(new_mode));
			return err;
		}
	}
	if (old_was_started) {
		assert(old_was_inited);
		err = bcm43xx_wireless_core_start(up_dev);
		if (err) {
			printk(KERN_INFO PFX "Fatal: Coult not start device for "
			       "new selected %s-PHY mode\n",
			       phymode_to_string(new_mode));
			bcm43xx_wireless_core_exit(up_dev);
			return err;
		}
	}

	wl->current_dev = up_dev;

	return 0;
}

static int bcm43xx_antenna_from_ieee80211(u8 antenna)
{
	switch (antenna) {
	case 0: /* default/diversity */
		return BCM43xx_ANTENNA_DEFAULT;
	case 1: /* Antenna 0 */
		return BCM43xx_ANTENNA0;
	case 2: /* Antenna 1 */
		return BCM43xx_ANTENNA1;
	default:
		return BCM43xx_ANTENNA_DEFAULT;
	}
}

static int bcm43xx_dev_config(struct ieee80211_hw *hw,
			      struct ieee80211_conf *conf)
{
	struct bcm43xx_wl *wl = hw_to_bcm43xx_wl(hw);
	struct bcm43xx_wldev *dev;
	struct bcm43xx_phy *phy;
	unsigned long flags;
	unsigned int new_phymode = 0xFFFF;
	int antenna_tx;
	int antenna_rx;
	int err = 0;
	u32 savedirqs;

	antenna_tx = bcm43xx_antenna_from_ieee80211(conf->antenna_sel_tx);
	antenna_rx = bcm43xx_antenna_from_ieee80211(conf->antenna_sel_rx);

	mutex_lock(&wl->mutex);

	/* Switch the PHY mode (if necessary). */
	switch (conf->phymode) {
	case MODE_IEEE80211A:
		new_phymode = BCM43xx_PHYMODE_A;
		break;
	case MODE_IEEE80211B:
		new_phymode = BCM43xx_PHYMODE_B;
		break;
	case MODE_IEEE80211G:
		new_phymode = BCM43xx_PHYMODE_G;
		break;
	default:
		assert(0);
	}
	err = bcm43xx_switch_phymode(wl, new_phymode);
	if (err)
		goto out_unlock_mutex;
	dev = wl->current_dev;
	phy = &dev->phy;

	/* Disable IRQs while reconfiguring the device.
	 * This makes it possible to drop the spinlock throughout
	 * the reconfiguration process. */
	spin_lock_irqsave(&wl->irq_lock, flags);
	if ((bcm43xx_status(dev) != BCM43xx_STAT_INITIALIZED) ||
	    !dev->started) {
		spin_unlock_irqrestore(&wl->irq_lock, flags);
		goto out_unlock_mutex;
	}
	savedirqs = bcm43xx_interrupt_disable(dev, BCM43xx_IRQ_ALL);
	spin_unlock_irqrestore(&wl->irq_lock, flags);
	bcm43xx_synchronize_irq(dev);

	/* Switch to the requested channel.
	 * The firmware takes care of races with the TX handler. */
	if (conf->channel_val != phy->channel)
		bcm43xx_radio_selectchannel(dev, conf->channel_val, 0);

	/* Enable/Disable ShortSlot timing. */
	if ((!!(conf->flags & IEEE80211_CONF_SHORT_SLOT_TIME)) != dev->short_slot) {
		assert(phy->type == BCM43xx_PHYTYPE_G);
		if (conf->flags & IEEE80211_CONF_SHORT_SLOT_TIME)
			bcm43xx_short_slot_timing_enable(dev);
		else
			bcm43xx_short_slot_timing_disable(dev);
	}

	/* Adjust the desired TX power level. */
	if (conf->power_level != 0) {
		if (conf->power_level != phy->power_level) {
			phy->power_level = conf->power_level;
			bcm43xx_phy_xmitpower(dev);
		}
	}

	/* Hide/Show the SSID (AP mode only). */
	if (conf->flags & IEEE80211_CONF_SSID_HIDDEN) {
		bcm43xx_write32(dev, BCM43xx_MMIO_STATUS_BITFIELD,
				bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD)
				| BCM43xx_SBF_NO_SSID_BCAST);
	} else {
		bcm43xx_write32(dev, BCM43xx_MMIO_STATUS_BITFIELD,
				bcm43xx_read32(dev, BCM43xx_MMIO_STATUS_BITFIELD)
				& ~BCM43xx_SBF_NO_SSID_BCAST);
	}

	/* Antennas for RX and management frame TX. */
	bcm43xx_mgmtframe_txantenna(dev, antenna_tx);
	bcm43xx_set_rx_antenna(dev, antenna_rx);

	/* Update templates for AP mode. */
	if (bcm43xx_is_mode(wl, IEEE80211_IF_TYPE_AP))
		bcm43xx_set_beacon_int(dev, conf->beacon_int);


	spin_lock_irqsave(&wl->irq_lock, flags);
	bcm43xx_interrupt_enable(dev, savedirqs);
	mmiowb();
	spin_unlock_irqrestore(&wl->irq_lock, flags);
out_unlock_mutex:
	mutex_unlock(&wl->mutex);

	return err;
}

static int bcm43xx_dev_set_key(struct ieee80211_hw *hw,
			       set_key_cmd cmd,
			       u8 *addr,
			       struct ieee80211_key_conf *key,
			       int aid)
{
	struct bcm43xx_wl *wl = hw_to_bcm43xx_wl(hw);
	struct bcm43xx_wldev *dev = wl->current_dev;
	unsigned long flags;
	u8 algorithm;
	u8 index;
	int err = -EINVAL;

	if (!dev)
		return -ENODEV;
	switch (key->alg) {
	case ALG_NONE:
	case ALG_NULL:
		algorithm = BCM43xx_SEC_ALGO_NONE;
		break;
	case ALG_WEP:
		if (key->keylen == 5)
			algorithm = BCM43xx_SEC_ALGO_WEP40;
		else
			algorithm = BCM43xx_SEC_ALGO_WEP104;
		break;
	case ALG_TKIP:
		algorithm = BCM43xx_SEC_ALGO_TKIP;
		break;
	case ALG_CCMP:
		algorithm = BCM43xx_SEC_ALGO_AES;
		break;
	default:
		assert(0);
		goto out;
	}

	index = (u8)(key->keyidx);
	if (index > 3)
		goto out;

	mutex_lock(&wl->mutex);
	spin_lock_irqsave(&wl->irq_lock, flags);

	if (bcm43xx_status(dev) != BCM43xx_STAT_INITIALIZED) {
		err = -ENODEV;
		goto out_unlock;
	}

	switch (cmd) {
	case SET_KEY:
		key->flags &= ~IEEE80211_KEY_FORCE_SW_ENCRYPT;

		if (algorithm == BCM43xx_SEC_ALGO_TKIP) {
			/* FIXME: No TKIP hardware encryption for now. */
			key->flags |= IEEE80211_KEY_FORCE_SW_ENCRYPT;
		}

		if (is_broadcast_ether_addr(addr)) {
			/* addr is FF:FF:FF:FF:FF:FF for default keys */
			err = bcm43xx_key_write(dev, index, algorithm,
						key->key, key->keylen,
						NULL, key);
		} else {
			err = bcm43xx_key_write(dev, -1, algorithm,
						key->key, key->keylen,
						addr, key);
		}
		if (err) {
			key->flags |= IEEE80211_KEY_FORCE_SW_ENCRYPT;
			goto out_unlock;
		}
		dev->key[key->hw_key_idx].enabled = 1;

		if (algorithm == BCM43xx_SEC_ALGO_WEP40 ||
		    algorithm == BCM43xx_SEC_ALGO_WEP104) {
			bcm43xx_hf_write(dev,
					 bcm43xx_hf_read(dev) |
					 BCM43xx_HF_USEDEFKEYS);
		} else {
			bcm43xx_hf_write(dev,
					 bcm43xx_hf_read(dev) &
					 ~BCM43xx_HF_USEDEFKEYS);
		}
		break;
	case DISABLE_KEY: {
		static const u8 zero[BCM43xx_SEC_KEYSIZE] = { 0 };

		algorithm = BCM43xx_SEC_ALGO_NONE;
		if (is_broadcast_ether_addr(addr)) {
			err = bcm43xx_key_write(dev, index, algorithm,
						zero, BCM43xx_SEC_KEYSIZE,
						NULL, key);
		} else {
			err = bcm43xx_key_write(dev, -1, algorithm,
						zero, BCM43xx_SEC_KEYSIZE,
						addr, key);
		}
		dev->key[key->hw_key_idx].enabled = 0;
		break;
	}
	case REMOVE_ALL_KEYS:
		bcm43xx_clear_keys(dev);
		err = 0;
		break;
	default:
		assert(0);
	}
out_unlock:
	spin_unlock_irqrestore(&wl->irq_lock, flags);
	mutex_unlock(&wl->mutex);
out:
	if (!err) {
		dprintk(KERN_DEBUG PFX "Using %s based encryption for keyidx: %d, "
			"mac: " MAC_FMT "\n",
			(key->flags & IEEE80211_KEY_FORCE_SW_ENCRYPT) ?
			"software" : "hardware",
			key->keyidx, MAC_ARG(addr));
	}
	return err;
}

static void bcm43xx_set_multicast_list(struct ieee80211_hw *hw,
				       unsigned short netflags,
				       int mc_count)
{
	struct bcm43xx_wl *wl = hw_to_bcm43xx_wl(hw);
	struct bcm43xx_wldev *dev = wl->current_dev;
	unsigned long flags;

	if (!dev)
		return;
	spin_lock_irqsave(&wl->irq_lock, flags);
	if (wl->promisc != !!(netflags & IFF_PROMISC)) {
		wl->promisc = !!(netflags & IFF_PROMISC);
		if (bcm43xx_status(dev) == BCM43xx_STAT_INITIALIZED)
			bcm43xx_adjust_opmode(dev);
	}
	spin_unlock_irqrestore(&wl->irq_lock, flags);
}

static int bcm43xx_config_interface(struct ieee80211_hw *hw,
				    int if_id,
				    struct ieee80211_if_conf *conf)
{
	struct bcm43xx_wl *wl = hw_to_bcm43xx_wl(hw);
	struct bcm43xx_wldev *dev = wl->current_dev;
	unsigned long flags;

	if (!dev)
		return -ENODEV;
	mutex_lock(&wl->mutex);
	spin_lock_irqsave(&wl->irq_lock, flags);
	if (conf->type != IEEE80211_IF_TYPE_MNTR) {
		assert(wl->if_id == if_id);
		wl->bssid = conf->bssid;
		if (bcm43xx_is_mode(wl, IEEE80211_IF_TYPE_AP)) {
			assert(conf->type == IEEE80211_IF_TYPE_AP);
			bcm43xx_set_ssid(dev, conf->ssid, conf->ssid_len);
			if (conf->beacon)
				bcm43xx_refresh_templates(dev, conf->beacon);
		}
		bcm43xx_write_mac_bssid_templates(dev);
	}
	spin_unlock_irqrestore(&wl->irq_lock, flags);
	mutex_unlock(&wl->mutex);

	return 0;
}

/* Locking: wl->mutex */
static void bcm43xx_wireless_core_stop(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_wl *wl = dev->wl;
	unsigned long flags;

	if (!dev->started)
		return;
	dev->started = 0;

	mutex_unlock(&wl->mutex);
	/* Must unlock as it would otherwise deadlock. No races here. */
	bcm43xx_periodic_tasks_delete(dev);
	flush_workqueue(dev->wl->hw->workqueue);
	mutex_lock(&wl->mutex);

	ieee80211_stop_queues(wl->hw); //FIXME this could cause a deadlock, as mac80211 seems buggy.

	/* Disable and sync interrupts. */
	spin_lock_irqsave(&wl->irq_lock, flags);
	dev->irq_savedstate = bcm43xx_interrupt_disable(dev, BCM43xx_IRQ_ALL);
	bcm43xx_read32(dev, BCM43xx_MMIO_GEN_IRQ_MASK); /* flush */
	spin_unlock_irqrestore(&wl->irq_lock, flags);
	bcm43xx_synchronize_irq(dev);

	bcm43xx_mac_suspend(dev);
	free_irq(dev->dev->irq, dev);
	dprintk(KERN_INFO PFX "Wireless interface stopped\n");
}

/* Locking: wl->mutex */
static int bcm43xx_wireless_core_start(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_wl *wl = dev->wl;
	int err;

	assert(!dev->started);

	drain_txstatus_queue(dev);
	err = request_irq(dev->dev->irq, bcm43xx_interrupt_handler,
			  IRQF_SHARED, KBUILD_MODNAME, dev);
	if (err) {
		printk(KERN_ERR PFX "Cannot request IRQ-%d\n",
		       dev->dev->irq);
		goto out;
	}
	dev->started = 1;
	bcm43xx_interrupt_enable(dev, dev->irq_savedstate);
	bcm43xx_mac_enable(dev);

	ieee80211_start_queues(wl->hw);
	bcm43xx_periodic_tasks_setup(dev);
	dprintk(KERN_INFO PFX "Wireless interface started\n");
out:
	return err;
}

/* Get PHY and RADIO versioning numbers */
static int bcm43xx_phy_versioning(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;
	u32 tmp;
	u8 analog_type;
	u8 phy_type;
	u8 phy_rev;
	u16 radio_manuf;
	u16 radio_ver;
	u16 radio_rev;
	int unsupported = 0;

	/* Get PHY versioning */
	tmp = bcm43xx_read16(dev, BCM43xx_MMIO_PHY_VER);
	analog_type = (tmp & BCM43xx_PHYVER_ANALOG) >> BCM43xx_PHYVER_ANALOG_SHIFT;
	phy_type = (tmp & BCM43xx_PHYVER_TYPE) >> BCM43xx_PHYVER_TYPE_SHIFT;
	phy_rev = (tmp & BCM43xx_PHYVER_VERSION);
	switch (phy_type) {
	case BCM43xx_PHYTYPE_A:
		if (phy_rev >= 4)
			unsupported = 1;
		break;
	case BCM43xx_PHYTYPE_B:
		if (phy_rev != 2 && phy_rev != 4 && phy_rev != 6 && phy_rev != 7)
			unsupported = 1;
		break;
	case BCM43xx_PHYTYPE_G:
		if (phy_rev > 8)
			unsupported = 1;
		break;
	default:
		unsupported = 1;
	};
	if (unsupported) {
		printk(KERN_ERR PFX "FOUND UNSUPPORTED PHY "
		       "(Analog %u, Type %u, Revision %u)\n",
		       analog_type, phy_type, phy_rev);
		return -EOPNOTSUPP;
	}
	dprintk(KERN_INFO PFX "Found PHY: Analog %u, Type %u, Revision %u\n",
		analog_type, phy_type, phy_rev);


	/* Get RADIO versioning */
	if (dev->dev->bus->chip_id == 0x4317) {
		if (dev->dev->bus->chip_rev == 0)
			tmp = 0x3205017F;
		else if (dev->dev->bus->chip_rev == 1)
			tmp = 0x4205017F;
		else
			tmp = 0x5205017F;
	} else {
		bcm43xx_write16(dev, BCM43xx_MMIO_RADIO_CONTROL,
				BCM43xx_RADIOCTL_ID);
		tmp = bcm43xx_read16(dev, BCM43xx_MMIO_RADIO_DATA_HIGH);
		tmp <<= 16;
		bcm43xx_write16(dev, BCM43xx_MMIO_RADIO_CONTROL,
				BCM43xx_RADIOCTL_ID);
		tmp |= bcm43xx_read16(dev, BCM43xx_MMIO_RADIO_DATA_LOW);
	}
	radio_manuf = (tmp & 0x00000FFF);
	radio_ver = (tmp & 0x0FFFF000) >> 12;
	radio_rev = (tmp & 0xF0000000) >> 28;
	switch (phy_type) {
	case BCM43xx_PHYTYPE_A:
		if (radio_ver != 0x2060)
			unsupported = 1;
		if (radio_rev != 1)
			unsupported = 1;
		if (radio_manuf != 0x17F)
			unsupported = 1;
		break;
	case BCM43xx_PHYTYPE_B:
		if ((radio_ver & 0xFFF0) != 0x2050)
			unsupported = 1;
		break;
	case BCM43xx_PHYTYPE_G:
		if (radio_ver != 0x2050)
			unsupported = 1;
		break;
	default:
		assert(0);
	}
	if (unsupported) {
		printk(KERN_ERR PFX "FOUND UNSUPPORTED RADIO "
		       "(Manuf 0x%X, Version 0x%X, Revision %u)\n",
		       radio_manuf, radio_ver, radio_rev);
		return -EOPNOTSUPP;
	}
	dprintk(KERN_INFO PFX "Found Radio: Manuf 0x%X, Version 0x%X, Revision %u\n",
		radio_manuf, radio_ver, radio_rev);


	phy->radio_manuf = radio_manuf;
	phy->radio_ver = radio_ver;
	phy->radio_rev = radio_rev;

	phy->analog = analog_type;
	phy->type = phy_type;
	phy->rev = phy_rev;

	return 0;
}

static void setup_struct_phy_for_init(struct bcm43xx_wldev *dev,
				      struct bcm43xx_phy *phy)
{
	struct bcm43xx_txpower_lo_control *lo;
	int i;

	memset(phy->minlowsig, 0xFF, sizeof(phy->minlowsig));
	memset(phy->minlowsigpos, 0, sizeof(phy->minlowsigpos));

	/* Flags */
	phy->locked = 0;

	phy->aci_enable = 0;
	phy->aci_wlan_automatic = 0;
	phy->aci_hw_rssi = 0;

	lo = phy->lo_control;
	if (lo) {
		memset(lo, 0, sizeof(*(phy->lo_control)));
		lo->rebuild = 1;
		lo->tx_bias = 0xFF;
	}
	phy->max_lb_gain = 0;
	phy->trsw_rx_gain = 0;
	phy->txpwr_offset = 0;

	/* NRSSI */
	phy->nrssislope = 0;
	for (i = 0; i < ARRAY_SIZE(phy->nrssi); i++)
		phy->nrssi[i] = -1000;
	for (i = 0; i < ARRAY_SIZE(phy->nrssi_lt); i++)
		phy->nrssi_lt[i] = i;

	phy->lofcal = 0xFFFF;
	phy->initval = 0xFFFF;

	spin_lock_init(&phy->lock);
	phy->interfmode = BCM43xx_INTERFMODE_NONE;
	phy->channel = 0xFF;
}

static void setup_struct_wldev_for_init(struct bcm43xx_wldev *dev)
{
	/* Flags */
	dev->reg124_set_0x4 = 0;

	/* Stats */
	memset(&dev->stats, 0, sizeof(dev->stats));

	setup_struct_phy_for_init(dev, &dev->phy);

	/* IRQ related flags */
	dev->irq_reason = 0;
	memset(dev->dma_reason, 0, sizeof(dev->dma_reason));
	dev->irq_savedstate = BCM43xx_IRQ_MASKTEMPLATE;

	dev->mac_suspended = 1;

	/* Noise calculation context */
	memset(&dev->noisecalc, 0, sizeof(dev->noisecalc));
}

static void bcm43xx_bluetooth_coext_enable(struct bcm43xx_wldev *dev)
{
	struct ssb_sprom *sprom = &dev->dev->bus->sprom;
	u32 hf;

	if (!(sprom->r1.boardflags_lo & BCM43xx_BFL_BTCOEXIST))
		return;
	if (dev->phy.type != BCM43xx_PHYTYPE_B && !dev->phy.gmode)
		return;

	hf = bcm43xx_hf_read(dev);
	if (sprom->r1.boardflags_lo & BCM43xx_BFL_BTCMOD)
		hf |= BCM43xx_HF_BTCOEXALT;
	else
		hf |= BCM43xx_HF_BTCOEX;
	bcm43xx_hf_write(dev, hf);
	//TODO
}

static void bcm43xx_bluetooth_coext_disable(struct bcm43xx_wldev *dev)
{//TODO
}

static void bcm43xx_imcfglo_timeouts_workaround(struct bcm43xx_wldev *dev)
{
#ifdef CONFIG_SSB_DRIVER_PCICORE
	struct ssb_bus *bus = dev->dev->bus;
	u32 tmp;

	if (bus->pcicore.dev &&
	    bus->pcicore.dev->id.coreid == SSB_DEV_PCI &&
	    bus->pcicore.dev->id.revision <= 5) {
		/* IMCFGLO timeouts workaround. */
		tmp = ssb_read32(dev->dev, SSB_IMCFGLO);
		tmp &= ~SSB_IMCFGLO_REQTO;
		tmp &= ~SSB_IMCFGLO_SERTO;
		switch (bus->bustype) {
		case SSB_BUSTYPE_PCI:
		case SSB_BUSTYPE_PCMCIA:
			tmp |= 0x32;
			break;
		case SSB_BUSTYPE_SSB:
			tmp |= 0x53;
			break;
		}
		ssb_write32(dev->dev, SSB_IMCFGLO, tmp);
	}
#endif /* CONFIG_SSB_DRIVER_PCICORE */
}

/* Shutdown a wireless core */
static void bcm43xx_wireless_core_exit(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_phy *phy = &dev->phy;

	if (bcm43xx_status(dev) != BCM43xx_STAT_INITIALIZED)
		return;

	bcm43xx_rng_exit(dev->wl);
	bcm43xx_pio_free(dev);
	bcm43xx_dma_free(dev);
	bcm43xx_chip_exit(dev);
	bcm43xx_radio_turn_off(dev);
	bcm43xx_switch_analog(dev, 0);
	if (phy->dyn_tssi_tbl)
		kfree(phy->tssi2dbm);
	kfree(phy->lo_control);
	phy->lo_control = NULL;
	ssb_device_disable(dev->dev, 0);
	ssb_bus_may_powerdown(dev->dev->bus);
	bcm43xx_set_status(dev, BCM43xx_STAT_UNINIT);
}

/* Initialize a wireless core */
static int bcm43xx_wireless_core_init(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_wl *wl = dev->wl;
	struct ssb_bus *bus = dev->dev->bus;
	struct ssb_sprom *sprom = &bus->sprom;
	struct bcm43xx_phy *phy = &dev->phy;
	int err;
	u32 hf, tmp;

	assert(bcm43xx_status(dev) == BCM43xx_STAT_UNINIT);
	bcm43xx_set_status(dev, BCM43xx_STAT_INITIALIZING);

	err = ssb_bus_powerup(bus, 0);
	if (err)
		goto out;
	if (!ssb_device_is_enabled(dev->dev)) {
		tmp = phy->gmode ? BCM43xx_TMSLOW_GMODE : 0;
		bcm43xx_wireless_core_reset(dev, tmp);
	}

	if ((phy->type == BCM43xx_PHYTYPE_B) || (phy->type == BCM43xx_PHYTYPE_G)) {
		phy->lo_control = kzalloc(sizeof(*(phy->lo_control)), GFP_KERNEL);
		if (!phy->lo_control) {
			err = -ENOMEM;
			goto err_busdown;
		}
	}
	setup_struct_wldev_for_init(dev);

	err = bcm43xx_phy_init_tssi2dbm_table(dev);
	if (err)
		goto err_kfree_lo_control;

	/* Enable IRQ routing to this device. */
	ssb_pcicore_dev_irqvecs_enable(&bus->pcicore, dev->dev);

	bcm43xx_imcfglo_timeouts_workaround(dev);
	bcm43xx_bluetooth_coext_disable(dev);
	bcm43xx_phy_early_init(dev);
	err = bcm43xx_chip_init(dev);
	if (err)
		goto err_kfree_tssitbl;
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED,
			    BCM43xx_SHM_SH_WLCOREREV,
			    dev->dev->id.revision);
	hf = bcm43xx_hf_read(dev);
	if (phy->type == BCM43xx_PHYTYPE_G) {
		hf |= BCM43xx_HF_SYMW;
		if (phy->rev == 1)
			hf |= BCM43xx_HF_GDCW;
		if (sprom->r1.boardflags_lo & BCM43xx_BFL_PACTRL)
			hf |= BCM43xx_HF_OFDMPABOOST;
	} else if (phy->type == BCM43xx_PHYTYPE_B) {
		hf |= BCM43xx_HF_SYMW;
		if (phy->rev >= 2 && phy->radio_ver == 0x2050)
			hf &= ~BCM43xx_HF_GDCW;
	}
	bcm43xx_hf_write(dev, hf);

	/* Short/Long Retry Limit.
	 * The retry-limit is a 4-bit counter. Enforce this to avoid overflowing
	 * the chip-internal counter.
	 */
	tmp = limit_value(modparam_short_retry, 0, 0xF);
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SCRATCH,
			    BCM43xx_SHM_SC_SRLIMIT, tmp);
	tmp = limit_value(modparam_long_retry, 0, 0xF);
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SCRATCH,
			    BCM43xx_SHM_SC_LRLIMIT, tmp);

	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED,
			    BCM43xx_SHM_SH_SFFBLIM, 3);
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED,
			    BCM43xx_SHM_SH_LFFBLIM, 2);

	bcm43xx_rate_memory_init(dev);

	/* Minimum Contention Window */
	if (phy->type == BCM43xx_PHYTYPE_B) {
		bcm43xx_shm_write16(dev, BCM43xx_SHM_SCRATCH,
				    BCM43xx_SHM_SC_MINCONT, 0x1F);
	} else {
		bcm43xx_shm_write16(dev, BCM43xx_SHM_SCRATCH,
				    BCM43xx_SHM_SC_MINCONT, 0xF);
	}
	/* Maximum Contention Window */
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SCRATCH,
			    BCM43xx_SHM_SC_MAXCONT, 0x3FF);

	do {
		if (bcm43xx_using_pio(dev)) {
			err = bcm43xx_pio_init(dev);
		} else {
			err = bcm43xx_dma_init(dev);
			if (!err)
				bcm43xx_qos_init(dev);
		}
	} while (err == -EAGAIN);
	if (err)
		goto err_chip_exit;

//FIXME
#if 1
	bcm43xx_write16(dev, 0x0612, 0x0050);
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, 0x0416, 0x0050);
	bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED, 0x0414, 0x01F4);
#endif

	bcm43xx_bluetooth_coext_enable(dev);

	ssb_bus_powerup(bus, 1); /* Enable dynamic PCTL */
	wl->bssid = NULL;
	bcm43xx_upload_card_macaddress(dev, NULL);
	bcm43xx_security_init(dev);
	bcm43xx_rng_init(wl);

	bcm43xx_set_status(dev, BCM43xx_STAT_INITIALIZED);

out:
	return err;

err_chip_exit:
	bcm43xx_chip_exit(dev);
err_kfree_tssitbl:
	if (phy->dyn_tssi_tbl)
		kfree(phy->tssi2dbm);
err_kfree_lo_control:
	kfree(phy->lo_control);
	phy->lo_control = NULL;
err_busdown:
	ssb_bus_may_powerdown(bus);
	bcm43xx_set_status(dev, BCM43xx_STAT_UNINIT);
	return err;
}

static int bcm43xx_add_interface(struct ieee80211_hw *hw,
				 struct ieee80211_if_init_conf *conf)
{
	struct bcm43xx_wl *wl = hw_to_bcm43xx_wl(hw);
	struct bcm43xx_wldev *dev;
	unsigned long flags;
	int err = -EOPNOTSUPP;
	int did_init = 0;

	mutex_lock(&wl->mutex);
	if ((conf->type != IEEE80211_IF_TYPE_MNTR) &&
	    wl->operating)
		goto out_mutex_unlock;

	dprintk(KERN_INFO PFX "Adding Interface type %d\n", conf->type);

	dev = wl->current_dev;
	if (bcm43xx_status(dev) == BCM43xx_STAT_UNINIT) {
		err = bcm43xx_wireless_core_init(dev);
		if (err)
			goto out_mutex_unlock;
		did_init = 1;
	}
	if (!dev->started) {
		err = bcm43xx_wireless_core_start(dev);
		if (err) {
			if (did_init)
				bcm43xx_wireless_core_exit(dev);
			goto out_mutex_unlock;
		}
	}

	spin_lock_irqsave(&wl->irq_lock, flags);
	switch (conf->type) {
	case IEEE80211_IF_TYPE_MNTR:
		wl->monitor++;
		break;
	default:
		wl->operating = 1;
		wl->if_id = conf->if_id;
		wl->if_type = conf->type;
		bcm43xx_upload_card_macaddress(dev, conf->mac_addr);
	}
	bcm43xx_adjust_opmode(dev);
	spin_unlock_irqrestore(&wl->irq_lock, flags);

	err = 0;
out_mutex_unlock:
	mutex_unlock(&wl->mutex);

	return err;
}

static void bcm43xx_remove_interface(struct ieee80211_hw *hw,
				     struct ieee80211_if_init_conf *conf)
{
	struct bcm43xx_wl *wl = hw_to_bcm43xx_wl(hw);
	struct bcm43xx_wldev *dev;
	unsigned long flags;

	dprintk(KERN_INFO PFX "Removing Interface type %d\n", conf->type);

	mutex_lock(&wl->mutex);
	if (conf->type == IEEE80211_IF_TYPE_MNTR) {
		wl->monitor--;
		assert(wl->monitor >= 0);
	} else {
		assert(wl->operating);
		wl->operating = 0;
	}

	dev = wl->current_dev;
	if (!wl->operating && wl->monitor == 0) {
		/* No interface left. */
		if (dev->started)
			bcm43xx_wireless_core_stop(dev);
		bcm43xx_wireless_core_exit(dev);
	} else {
		/* Just monitor interfaces left. */
		spin_lock_irqsave(&wl->irq_lock, flags);
		bcm43xx_adjust_opmode(dev);
		if (!wl->operating)
			bcm43xx_upload_card_macaddress(dev, NULL);
		spin_unlock_irqrestore(&wl->irq_lock, flags);
	}
	mutex_unlock(&wl->mutex);
}


static const struct ieee80211_ops bcm43xx_hw_ops = {
	.tx = bcm43xx_tx,
	.conf_tx = bcm43xx_conf_tx,
	.add_interface = bcm43xx_add_interface,
	.remove_interface = bcm43xx_remove_interface,
	.reset = bcm43xx_dev_reset,
	.config = bcm43xx_dev_config,
	.config_interface = bcm43xx_config_interface,
	.set_multicast_list = bcm43xx_set_multicast_list,
	.set_key = bcm43xx_dev_set_key,
	.get_stats = bcm43xx_get_stats,
	.get_tx_stats = bcm43xx_get_tx_stats,
};

/* Hard-reset the chip. Do not call this directly.
 * Use bcm43xx_controller_restart()
 */
static void bcm43xx_chip_reset(struct work_struct *work)
{
	struct bcm43xx_wldev *dev =
		container_of(work, struct bcm43xx_wldev, restart_work);
	struct bcm43xx_wl *wl = dev->wl;
	int err;
	int was_started = 0;
	int was_inited = 0;

	mutex_lock(&wl->mutex);

	/* Bring the device down... */
	if (dev->started) {
		was_started = 1;
		bcm43xx_wireless_core_stop(dev);
	}
	if (bcm43xx_status(dev) == BCM43xx_STAT_INITIALIZED) {
		was_inited = 1;
		bcm43xx_wireless_core_exit(dev);
	}

	/* ...and up again. */
	if (was_inited) {
		err = bcm43xx_wireless_core_init(dev);
		if (err)
			goto out;
	}
	if (was_started) {
		assert(was_inited);
		err = bcm43xx_wireless_core_start(dev);
		if (err) {
			bcm43xx_wireless_core_exit(dev);
			goto out;
		}
	}
out:
	mutex_unlock(&wl->mutex);
	if (err)
		printk(KERN_ERR PFX "Controller restart FAILED\n");
	else
		printk(KERN_INFO PFX "Controller restarted\n");
}

static int bcm43xx_setup_modes(struct bcm43xx_wldev *dev,
			       int have_aphy,
			       int have_bphy,
			       int have_gphy)
{
	struct ieee80211_hw *hw = dev->wl->hw;
	struct ieee80211_hw_mode *mode;
	struct bcm43xx_phy *phy = &dev->phy;
	int cnt = 0;
	int err;

/*FIXME: Don't tell ieee80211 about an A-PHY, because we currently don't support A-PHY. */
have_aphy = 0;

	phy->possible_phymodes = 0;
	for ( ; 1; cnt++) {
		if (have_aphy) {
			assert(cnt < BCM43xx_MAX_PHYHWMODES);
			mode = &phy->hwmodes[cnt];

			mode->mode = MODE_IEEE80211A;
			mode->num_channels = bcm43xx_a_chantable_size;
			mode->channels = bcm43xx_a_chantable;
			mode->num_rates = bcm43xx_a_ratetable_size;
			mode->rates = bcm43xx_a_ratetable;
			err = ieee80211_register_hwmode(hw, mode);
			if (err)
				return err;

			phy->possible_phymodes |= BCM43xx_PHYMODE_A;
			have_aphy = 0;
			continue;
		}
		if (have_bphy) {
			assert(cnt < BCM43xx_MAX_PHYHWMODES);
			mode = &phy->hwmodes[cnt];

			mode->mode = MODE_IEEE80211B;
			mode->num_channels = bcm43xx_bg_chantable_size;
			mode->channels = bcm43xx_bg_chantable;
			mode->num_rates = bcm43xx_b_ratetable_size;
			mode->rates = bcm43xx_b_ratetable;
			err = ieee80211_register_hwmode(hw, mode);
			if (err)
				return err;

			phy->possible_phymodes |= BCM43xx_PHYMODE_B;
			have_bphy = 0;
			continue;
		}
		if (have_gphy) {
			assert(cnt < BCM43xx_MAX_PHYHWMODES);
			mode = &phy->hwmodes[cnt];

			mode->mode = MODE_IEEE80211G;
			mode->num_channels = bcm43xx_bg_chantable_size;
			mode->channels = bcm43xx_bg_chantable;
			mode->num_rates = bcm43xx_g_ratetable_size;
			mode->rates = bcm43xx_g_ratetable;
			err = ieee80211_register_hwmode(hw, mode);
			if (err)
				return err;

			phy->possible_phymodes |= BCM43xx_PHYMODE_G;
			have_gphy = 0;
			continue;
		}
		break;
	}

	return 0;
}

static void bcm43xx_wireless_core_detach(struct bcm43xx_wldev *dev)
{
	/* We release firmware that late to not be required to re-request
	 * is all the time when we reinit the core. */
	bcm43xx_release_firmware(dev);
}

static int bcm43xx_wireless_core_attach(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_wl *wl = dev->wl;
	struct ssb_bus *bus = dev->dev->bus;
	struct pci_dev *pdev = bus->host_pci;
	int err;
	int have_aphy = 0, have_bphy = 0, have_gphy = 0;
	u32 tmp;

	/* Do NOT do any device initialization here.
	 * Do it in wireless_core_init() instead.
	 * This function is for gathering basic information about the HW, only.
	 * Also some structs may be set up here. But most likely you want to have
	 * that in core_init(), too.
	 */

	/* Get the PHY type. */
	if (dev->dev->id.revision >= 5) {
		u32 tmshigh;

		tmshigh = ssb_read32(dev->dev, SSB_TMSHIGH);
		have_aphy = !!(tmshigh & BCM43xx_TMSHIGH_APHY);
		have_gphy = !!(tmshigh & BCM43xx_TMSHIGH_GPHY);
		if (!have_aphy && !have_gphy)
			have_bphy = 1;
	} else if (dev->dev->id.revision == 4) {
		have_gphy = 1;
		have_aphy = 1;
	} else
		have_bphy = 1;

	/* Initialize LEDs structs. */
	err = bcm43xx_leds_init(dev);
	if (err)
		goto out;

	dev->phy.gmode = (have_gphy || have_bphy);
	tmp = dev->phy.gmode ? BCM43xx_TMSLOW_GMODE : 0;
	bcm43xx_wireless_core_reset(dev, tmp);

	err = bcm43xx_phy_versioning(dev);
	if (err)
		goto err_leds_exit;
	/* Check if this device supports multiband. */
	if (!pdev ||
	    (pdev->device != 0x4312 &&
	     pdev->device != 0x4319 &&
	     pdev->device != 0x4324)) {
		/* No multiband support. */
		have_aphy = 0;
		have_bphy = 0;
		have_gphy = 0;
		switch (dev->phy.type) {
		case BCM43xx_PHYTYPE_A:
			have_aphy = 1;
			break;
		case BCM43xx_PHYTYPE_B:
			have_bphy = 1;
			break;
		case BCM43xx_PHYTYPE_G:
			have_gphy = 1;
			break;
		default:
			assert(0);
		}
	}
	dev->phy.gmode = (have_gphy || have_bphy);
	tmp = dev->phy.gmode ? BCM43xx_TMSLOW_GMODE : 0;
	bcm43xx_wireless_core_reset(dev, tmp);

	err = bcm43xx_validate_chipaccess(dev);
	if (err)
		goto err_leds_exit;
	err = bcm43xx_setup_modes(dev, have_aphy,
				  have_bphy, have_gphy);
	if (err)
		goto err_leds_exit;

	/* Now set some default "current_dev" */
	if (!wl->current_dev)
		wl->current_dev = dev;
	INIT_WORK(&dev->restart_work, bcm43xx_chip_reset);

	bcm43xx_radio_turn_off(dev);
	bcm43xx_switch_analog(dev, 0);
	ssb_device_disable(dev->dev, 0);
	ssb_bus_may_powerdown(bus);

out:
	return err;

err_leds_exit:
	bcm43xx_leds_exit(dev);
	return err;
}

static void bcm43xx_one_core_detach(struct ssb_device *dev)
{
	struct bcm43xx_wldev *wldev;
	struct bcm43xx_wl *wl;

	wldev = ssb_get_drvdata(dev);
	wl = wldev->wl;
	bcm43xx_debugfs_remove_device(wldev);
	bcm43xx_wireless_core_detach(wldev);
	list_del(&wldev->list);
	wl->nr_devs--;
	ssb_set_drvdata(dev, NULL);
	kfree(wldev);
}

static int bcm43xx_one_core_attach(struct ssb_device *dev,
				   struct bcm43xx_wl *wl)
{
	struct bcm43xx_wldev *wldev;
	struct pci_dev *pdev;
	int err = -ENOMEM;

	if (!list_empty(&wl->devlist)) {
		/* We are not the first core on this chip. */
		pdev = dev->bus->host_pci;
		/* Only special chips support more than one wireless
		 * core, although some of the other chips have more than
		 * one wireless core as well. Check for this and
		 * bail out early.
		 */
		if (!pdev ||
		    ((pdev->device != 0x4321) &&
		     (pdev->device != 0x4313) &&
		     (pdev->device != 0x431A))) {
			dprintk(KERN_INFO PFX "Ignoring unconnected 802.11 core\n");
			return -ENODEV;
		}
	}

	wldev = kzalloc(sizeof(*wldev), GFP_KERNEL);
	if (!wldev)
		goto out;

	wldev->dev = dev;
	wldev->wl = wl;
	bcm43xx_set_status(wldev, BCM43xx_STAT_UNINIT);
	wldev->bad_frames_preempt = modparam_bad_frames_preempt;
	tasklet_init(&wldev->isr_tasklet,
		     (void (*)(unsigned long))bcm43xx_interrupt_tasklet,
		     (unsigned long)wldev);
	if (modparam_pio)
		wldev->__using_pio = 1;
	INIT_LIST_HEAD(&wldev->list);

	err = bcm43xx_wireless_core_attach(wldev);
	if (err)
		goto err_kfree_wldev;

	list_add(&wldev->list, &wl->devlist);
	wl->nr_devs++;
	ssb_set_drvdata(dev, wldev);
	bcm43xx_debugfs_add_device(wldev);

out:
	return err;

err_kfree_wldev:
	kfree(wldev);
	return err;
}

static void bcm43xx_sprom_fixup(struct ssb_bus *bus)
{
	/* boardflags workarounds */
	if (bus->boardinfo.vendor == SSB_BOARDVENDOR_DELL &&
	    bus->chip_id == 0x4301 &&
	    bus->boardinfo.rev == 0x74)
		bus->sprom.r1.boardflags_lo |= BCM43xx_BFL_BTCOEXIST;
	if (bus->boardinfo.vendor == PCI_VENDOR_ID_APPLE &&
	    bus->boardinfo.type == 0x4E &&
	    bus->boardinfo.rev > 0x40)
		bus->sprom.r1.boardflags_lo |= BCM43xx_BFL_PACTRL;

	/* Convert Antennagain values to Q5.2 */
	bus->sprom.r1.antenna_gain_a <<= 2;
	bus->sprom.r1.antenna_gain_bg <<= 2;
}

static void bcm43xx_wireless_exit(struct ssb_device *dev,
				  struct bcm43xx_wl *wl)
{
	struct ieee80211_hw *hw = wl->hw;

	ssb_set_devtypedata(dev, NULL);
	ieee80211_free_hw(hw);
}

static int bcm43xx_wireless_init(struct ssb_device *dev)
{
	struct ssb_sprom *sprom = &dev->bus->sprom;
	struct ieee80211_hw *hw;
	struct bcm43xx_wl *wl;
	int err = -ENOMEM;

	bcm43xx_sprom_fixup(dev->bus);

	hw = ieee80211_alloc_hw(sizeof(*wl), &bcm43xx_hw_ops);
	if (!hw) {
		printk(KERN_ERR PFX "Could not allocate ieee80211 device\n");
		goto out;
	}

	/* fill hw info */
	hw->flags = IEEE80211_HW_HOST_GEN_BEACON_TEMPLATE |
		    IEEE80211_HW_MONITOR_DURING_OPER |
		    IEEE80211_HW_DEVICE_HIDES_WEP |
		    IEEE80211_HW_WEP_INCLUDE_IV;
	hw->max_signal = 100;
	hw->max_rssi = -110;
	hw->max_noise = -110;
	hw->queues = 1; /* FIXME: hardware has more queues */
	SET_IEEE80211_DEV(hw, dev->dev);
	if (is_valid_ether_addr(sprom->r1.et1mac))
		SET_IEEE80211_PERM_ADDR(hw, sprom->r1.et1mac);
	else
		SET_IEEE80211_PERM_ADDR(hw, sprom->r1.il0mac);

	/* Get and initialize struct bcm43xx_wl */
	wl = hw_to_bcm43xx_wl(hw);
	memset(wl, 0, sizeof(*wl));
	wl->hw = hw;
	spin_lock_init(&wl->irq_lock);
	spin_lock_init(&wl->leds_lock);
	mutex_init(&wl->mutex);
	INIT_LIST_HEAD(&wl->devlist);

	ssb_set_devtypedata(dev, wl);
	printk(KERN_INFO PFX "Broadcom %04X WLAN found\n", dev->bus->chip_id);
	err = 0;
out:
	return err;
}

static int bcm43xx_probe(struct ssb_device *dev,
			 const struct ssb_device_id *id)
{
	struct bcm43xx_wl *wl;
	int err;
	int first = 0;

	wl = ssb_get_devtypedata(dev);
	if (!wl) {
		/* Probing the first core. Must setup common struct bcm43xx_wl */
		first = 1;
		err = bcm43xx_wireless_init(dev);
		if (err)
			goto out;
		wl = ssb_get_devtypedata(dev);
		assert(wl);
	}
	err = bcm43xx_one_core_attach(dev, wl);
	if (err)
		goto err_wireless_exit;

	if (first) {
		err = ieee80211_register_hw(wl->hw);
		if (err)
			goto err_one_core_detach;
	}

out:
	return err;

err_one_core_detach:
	bcm43xx_one_core_detach(dev);
err_wireless_exit:
	if (first)
		bcm43xx_wireless_exit(dev, wl);
	return err;
}

static void bcm43xx_remove(struct ssb_device *dev)
{
	struct bcm43xx_wl *wl = ssb_get_devtypedata(dev);
	struct bcm43xx_wldev *wldev = ssb_get_drvdata(dev);

	assert(wl);
	if (wl->current_dev == wldev)
		ieee80211_unregister_hw(wl->hw);

	bcm43xx_one_core_detach(dev);

	if (list_empty(&wl->devlist)) {
		/* Last core on the chip unregistered.
		 * We can destroy common struct bcm43xx_wl.
		 */
		bcm43xx_wireless_exit(dev, wl);
	}
}

/* Hard-reset the chip.
 * This can be called from interrupt or process context.
 * dev->irq_lock must be locked.
 */
void bcm43xx_controller_restart(struct bcm43xx_wldev *dev, const char *reason)
{
	if (bcm43xx_status(dev) != BCM43xx_STAT_INITIALIZED)
		return;
	printk(KERN_ERR PFX "Controller RESET (%s) ...\n", reason);
	queue_work(dev->wl->hw->workqueue, &dev->restart_work);
}

#ifdef CONFIG_PM

static int bcm43xx_suspend(struct ssb_device *dev, pm_message_t state)
{
	struct bcm43xx_wldev *wldev = ssb_get_drvdata(dev);
	struct bcm43xx_wl *wl = wldev->wl;

	dprintk(KERN_INFO PFX "Suspending...\n");

	mutex_lock(&wl->mutex);
	wldev->was_started = !!wldev->started;
	wldev->was_initialized = (bcm43xx_status(wldev) == BCM43xx_STAT_INITIALIZED);
	if (wldev->started)
		bcm43xx_wireless_core_stop(wldev);
	if (bcm43xx_status(wldev) == BCM43xx_STAT_INITIALIZED)
		bcm43xx_wireless_core_exit(wldev);

	mutex_unlock(&wl->mutex);

	dprintk(KERN_INFO PFX "Device suspended.\n");

	return 0;
}

static int bcm43xx_resume(struct ssb_device *dev)
{
	struct bcm43xx_wldev *wldev = ssb_get_drvdata(dev);
	int err = 0;

	dprintk(KERN_INFO PFX "Resuming...\n");

	if (wldev->was_initialized) {
		err = bcm43xx_wireless_core_init(wldev);
		if (err) {
			printk(KERN_ERR PFX "Resume failed at core init\n");
			goto out;
		}
	}
	if (wldev->was_started) {
		assert(wldev->was_initialized);
		err = bcm43xx_wireless_core_start(wldev);
		if (err) {
			printk(KERN_ERR PFX "Resume failed at core start\n");
			goto out;
		}
	}

	dprintk(KERN_INFO PFX "Device resumed.\n");
out:
	return err;
}

#else	/* CONFIG_PM */
# define bcm43xx_suspend	NULL
# define bcm43xx_resume		NULL
#endif	/* CONFIG_PM */

static struct ssb_driver bcm43xx_ssb_driver = {
	.name		= KBUILD_MODNAME,
	.id_table	= bcm43xx_ssb_tbl,
	.probe		= bcm43xx_probe,
	.remove		= bcm43xx_remove,
	.suspend	= bcm43xx_suspend,
	.resume		= bcm43xx_resume,
};

#ifdef CONFIG_BCM43XX_MAC80211_PCI
/* The PCI frontend stub */
static const struct pci_device_id bcm43xx_pci_tbl[] = {
	{ PCI_DEVICE(PCI_VENDOR_ID_BROADCOM, 0x4307) },
	{ PCI_DEVICE(PCI_VENDOR_ID_BROADCOM, 0x4311) },
	{ PCI_DEVICE(PCI_VENDOR_ID_BROADCOM, 0x4312) },
	{ PCI_DEVICE(PCI_VENDOR_ID_BROADCOM, 0x4318) },
	{ PCI_DEVICE(PCI_VENDOR_ID_BROADCOM, 0x4319) },
	{ PCI_DEVICE(PCI_VENDOR_ID_BROADCOM, 0x4320) },
	{ PCI_DEVICE(PCI_VENDOR_ID_BROADCOM, 0x4321) },
	{ PCI_DEVICE(PCI_VENDOR_ID_BROADCOM, 0x4324) },
	{ PCI_DEVICE(PCI_VENDOR_ID_BROADCOM, 0x4325) },
	{ 0 },
};
MODULE_DEVICE_TABLE(pci, bcm43xx_pci_tbl);

static struct pci_driver bcm43xx_pci_driver = {
	.name		= "bcm43xx-pci",
	.id_table	= bcm43xx_pci_tbl,
};
#endif /* CONFIG_BCM43XX_MAC80211_PCI */

static int __init bcm43xx_init(void)
{
	int err;

	bcm43xx_debugfs_init();
#ifdef CONFIG_BCM43XX_MAC80211_PCI
	err = ssb_pcihost_register(&bcm43xx_pci_driver);
	if (err)
		goto err_dfs_exit;
#endif
	err = bcm43xx_pcmcia_init();
	if (err)
		goto err_pci_exit;
	err = ssb_driver_register(&bcm43xx_ssb_driver);
	if (err)
		goto err_pcmcia_exit;

	return err;

err_pcmcia_exit:
	bcm43xx_pcmcia_exit();
err_pci_exit:
#ifdef CONFIG_BCM43XX_MAC80211_PCI
	ssb_pcihost_unregister(&bcm43xx_pci_driver);
#endif
err_dfs_exit:
	bcm43xx_debugfs_exit();
	return err;
}

static void __exit bcm43xx_exit(void)
{
	ssb_driver_unregister(&bcm43xx_ssb_driver);
	bcm43xx_pcmcia_exit();
#ifdef CONFIG_BCM43XX_MAC80211_PCI
	ssb_pcihost_unregister(&bcm43xx_pci_driver);
#endif
	bcm43xx_debugfs_exit();
}

module_init(bcm43xx_init)
module_exit(bcm43xx_exit)
