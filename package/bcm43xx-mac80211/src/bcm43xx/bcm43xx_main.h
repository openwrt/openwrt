/*

  Broadcom BCM43xx wireless driver

  Copyright (c) 2005 Martin Langer <martin-langer@gmx.de>,
                     Stefano Brivio <st3@riseup.net>
                     Michael Buesch <mb@bu3sch.de>
                     Danny van Dyk <kugelfang@gentoo.org>
                     Andreas Jaggi <andreas.jaggi@waterwave.ch>

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

#ifndef BCM43xx_MAIN_H_
#define BCM43xx_MAIN_H_

#include "bcm43xx.h"


#define P4D_BYT3S(magic, nr_bytes)	u8 __p4dding##magic[nr_bytes]
#define P4D_BYTES(line, nr_bytes)	P4D_BYT3S(line, nr_bytes)
/* Magic helper macro to pad structures. Ignore those above. It's magic. */
#define PAD_BYTES(nr_bytes)		P4D_BYTES( __LINE__ , (nr_bytes))


/* Lightweight function to convert a frequency (in Mhz) to a channel number. */
static inline
u8 bcm43xx_freq_to_channel_a(int freq)
{
	return ((freq - 5000) / 5);
}
static inline
u8 bcm43xx_freq_to_channel_bg(int freq)
{
	u8 channel;

	if (freq == 2484)
		channel = 14;
	else
		channel = (freq - 2407) / 5;

	return channel;
}
static inline
u8 bcm43xx_freq_to_channel(struct bcm43xx_wldev *dev,
			   int freq)
{
	if (dev->phy.type == BCM43xx_PHYTYPE_A)
		return bcm43xx_freq_to_channel_a(freq);
	return bcm43xx_freq_to_channel_bg(freq);
}

/* Lightweight function to convert a channel number to a frequency (in Mhz). */
static inline
int bcm43xx_channel_to_freq_a(u8 channel)
{
	return (5000 + (5 * channel));
}
static inline
int bcm43xx_channel_to_freq_bg(u8 channel)
{
	int freq;

	if (channel == 14)
		freq = 2484;
	else
		freq = 2407 + (5 * channel);

	return freq;
}
static inline
int bcm43xx_channel_to_freq(struct bcm43xx_wldev *dev,
			    u8 channel)
{
	if (dev->phy.type == BCM43xx_PHYTYPE_A)
		return bcm43xx_channel_to_freq_a(channel);
	return bcm43xx_channel_to_freq_bg(channel);
}

static inline
int bcm43xx_is_cck_rate(int rate)
{
	return (rate == BCM43xx_CCK_RATE_1MB ||
		rate == BCM43xx_CCK_RATE_2MB ||
		rate == BCM43xx_CCK_RATE_5MB ||
		rate == BCM43xx_CCK_RATE_11MB);
}

static inline
int bcm43xx_is_ofdm_rate(int rate)
{
	return !bcm43xx_is_cck_rate(rate);
}

static inline
int bcm43xx_is_hw_radio_enabled(struct bcm43xx_wldev *dev)
{
	/* function to return state of hardware enable of radio
	 * returns 0 if radio disabled, 1 if radio enabled
	 */
	struct bcm43xx_phy *phy = &dev->phy;

	if (phy->rev >= 3)
		return ((bcm43xx_read32(dev, BCM43xx_MMIO_RADIO_HWENABLED_HI)
					& BCM43xx_MMIO_RADIO_HWENABLED_HI_MASK)
					== 0) ? 1 : 0;
	else
		return ((bcm43xx_read16(dev, BCM43xx_MMIO_RADIO_HWENABLED_LO)
					& BCM43xx_MMIO_RADIO_HWENABLED_LO_MASK)
					== 0) ? 0 : 1;
}

void bcm43xx_tsf_read(struct bcm43xx_wldev *dev, u64 *tsf);
void bcm43xx_tsf_write(struct bcm43xx_wldev *dev, u64 tsf);

u32 bcm43xx_shm_read32(struct bcm43xx_wldev *dev,
		       u16 routing, u16 offset);
u16 bcm43xx_shm_read16(struct bcm43xx_wldev *dev,
		       u16 routing, u16 offset);
void bcm43xx_shm_write32(struct bcm43xx_wldev *dev,
			 u16 routing, u16 offset,
			 u32 value);
void bcm43xx_shm_write16(struct bcm43xx_wldev *dev,
			 u16 routing, u16 offset,
			 u16 value);

u32 bcm43xx_hf_read(struct bcm43xx_wldev *dev);
void bcm43xx_hf_write(struct bcm43xx_wldev *dev, u32 value);

void bcm43xx_dummy_transmission(struct bcm43xx_wldev *dev);

void bcm43xx_wireless_core_reset(struct bcm43xx_wldev *dev, u32 flags);

void bcm43xx_mac_suspend(struct bcm43xx_wldev *dev);
void bcm43xx_mac_enable(struct bcm43xx_wldev *dev);

void bcm43xx_controller_restart(struct bcm43xx_wldev *dev, const char *reason);

#endif /* BCM43xx_MAIN_H_ */
