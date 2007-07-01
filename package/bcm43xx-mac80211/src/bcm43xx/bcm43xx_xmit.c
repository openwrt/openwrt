/*

  Broadcom BCM43xx wireless driver

  Transmission (TX/RX) related functions.

  Copyright (C) 2005 Martin Langer <martin-langer@gmx.de>
  Copyright (C) 2005 Stefano Brivio <st3@riseup.net>
  Copyright (C) 2005, 2006 Michael Buesch <mb@bu3sch.de>
  Copyright (C) 2005 Danny van Dyk <kugelfang@gentoo.org>
  Copyright (C) 2005 Andreas Jaggi <andreas.jaggi@waterwave.ch>

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

#include "bcm43xx_xmit.h"
#include "bcm43xx_phy.h"
#include "bcm43xx_dma.h"
#include "bcm43xx_pio.h"


/* Extract the bitrate out of a CCK PLCP header. */
static u8 bcm43xx_plcp_get_bitrate_cck(struct bcm43xx_plcp_hdr6 *plcp)
{
	switch (plcp->raw[0]) {
	case 0x0A:
		return BCM43xx_CCK_RATE_1MB;
	case 0x14:
		return BCM43xx_CCK_RATE_2MB;
	case 0x37:
		return BCM43xx_CCK_RATE_5MB;
	case 0x6E:
		return BCM43xx_CCK_RATE_11MB;
	}
	assert(0);
	return 0;
}

/* Extract the bitrate out of an OFDM PLCP header. */
static u8 bcm43xx_plcp_get_bitrate_ofdm(struct bcm43xx_plcp_hdr6 *plcp)
{
	switch (plcp->raw[0] & 0xF) {
	case 0xB:
		return BCM43xx_OFDM_RATE_6MB;
	case 0xF:
		return BCM43xx_OFDM_RATE_9MB;
	case 0xA:
		return BCM43xx_OFDM_RATE_12MB;
	case 0xE:
		return BCM43xx_OFDM_RATE_18MB;
	case 0x9:
		return BCM43xx_OFDM_RATE_24MB;
	case 0xD:
		return BCM43xx_OFDM_RATE_36MB;
	case 0x8:
		return BCM43xx_OFDM_RATE_48MB;
	case 0xC:
		return BCM43xx_OFDM_RATE_54MB;
	}
	assert(0);
	return 0;
}

u8 bcm43xx_plcp_get_ratecode_cck(const u8 bitrate)
{
	switch (bitrate) {
	case BCM43xx_CCK_RATE_1MB:
		return 0x0A;
	case BCM43xx_CCK_RATE_2MB:
		return 0x14;
	case BCM43xx_CCK_RATE_5MB:
		return 0x37;
	case BCM43xx_CCK_RATE_11MB:
		return 0x6E;
	}
	assert(0);
	return 0;
}

u8 bcm43xx_plcp_get_ratecode_ofdm(const u8 bitrate)
{
	switch (bitrate) {
	case BCM43xx_OFDM_RATE_6MB:
		return 0xB;
	case BCM43xx_OFDM_RATE_9MB:
		return 0xF;
	case BCM43xx_OFDM_RATE_12MB:
		return 0xA;
	case BCM43xx_OFDM_RATE_18MB:
		return 0xE;
	case BCM43xx_OFDM_RATE_24MB:
		return 0x9;
	case BCM43xx_OFDM_RATE_36MB:
		return 0xD;
	case BCM43xx_OFDM_RATE_48MB:
		return 0x8;
	case BCM43xx_OFDM_RATE_54MB:
		return 0xC;
	}
	assert(0);
	return 0;
}

void bcm43xx_generate_plcp_hdr(struct bcm43xx_plcp_hdr4 *plcp,
			       const u16 octets, const u8 bitrate)
{
	__le32 *data = &(plcp->data);
	__u8 *raw = plcp->raw;

	if (bcm43xx_is_ofdm_rate(bitrate)) {
		*data = bcm43xx_plcp_get_ratecode_ofdm(bitrate);
		assert(!(octets & 0xF000));
		*data |= (octets << 5);
		*data = cpu_to_le32(*data);
	} else {
		u32 plen;

		plen = octets * 16 / bitrate;
		if ((octets * 16 % bitrate) > 0) {
			plen++;
			if ((bitrate == BCM43xx_CCK_RATE_11MB)
			    && ((octets * 8 % 11) < 4)) {
				raw[1] = 0x84;
			} else
				raw[1] = 0x04;
		} else
			raw[1] = 0x04;
		*data |= cpu_to_le32(plen << 16);
		raw[0] = bcm43xx_plcp_get_ratecode_cck(bitrate);
	}
}

static u8 bcm43xx_calc_fallback_rate(u8 bitrate)
{
	switch (bitrate) {
	case BCM43xx_CCK_RATE_1MB:
		return BCM43xx_CCK_RATE_1MB;
	case BCM43xx_CCK_RATE_2MB:
		return BCM43xx_CCK_RATE_1MB;
	case BCM43xx_CCK_RATE_5MB:
		return BCM43xx_CCK_RATE_2MB;
	case BCM43xx_CCK_RATE_11MB:
		return BCM43xx_CCK_RATE_5MB;
	case BCM43xx_OFDM_RATE_6MB:
		return BCM43xx_CCK_RATE_5MB;
	case BCM43xx_OFDM_RATE_9MB:
		return BCM43xx_OFDM_RATE_6MB;
	case BCM43xx_OFDM_RATE_12MB:
		return BCM43xx_OFDM_RATE_9MB;
	case BCM43xx_OFDM_RATE_18MB:
		return BCM43xx_OFDM_RATE_12MB;
	case BCM43xx_OFDM_RATE_24MB:
		return BCM43xx_OFDM_RATE_18MB;
	case BCM43xx_OFDM_RATE_36MB:
		return BCM43xx_OFDM_RATE_24MB;
	case BCM43xx_OFDM_RATE_48MB:
		return BCM43xx_OFDM_RATE_36MB;
	case BCM43xx_OFDM_RATE_54MB:
		return BCM43xx_OFDM_RATE_48MB;
	}
	assert(0);
	return 0;
}

static void generate_txhdr_fw4(struct bcm43xx_wldev *dev,
			       struct bcm43xx_txhdr_fw4 *txhdr,
			       const unsigned char *fragment_data,
			       unsigned int fragment_len,
			       const struct ieee80211_tx_control *txctl,
			       u16 cookie)
{
	const struct bcm43xx_phy *phy = &dev->phy;
	const struct ieee80211_hdr *wlhdr = (const struct ieee80211_hdr *)fragment_data;
	int use_encryption = ((!(txctl->flags & IEEE80211_TXCTL_DO_NOT_ENCRYPT)) &&
			      (txctl->key_idx >= 0));
	u16 fctl = le16_to_cpu(wlhdr->frame_control);
	u8 rate, rate_fb;
	int rate_ofdm, rate_fb_ofdm;
	unsigned int plcp_fragment_len;
	u32 mac_ctl = 0;
	u16 phy_ctl = 0;
	u8 extra_ft = 0;

	memset(txhdr, 0, sizeof(*txhdr));

	rate = txctl->tx_rate;
	rate_ofdm = bcm43xx_is_ofdm_rate(rate);
	rate_fb = (txctl->alt_retry_rate == -1) ? rate : txctl->alt_retry_rate;
	rate_fb_ofdm = bcm43xx_is_ofdm_rate(rate_fb);

	if (rate_ofdm)
		txhdr->phy_rate = bcm43xx_plcp_get_ratecode_ofdm(rate);
	else
		txhdr->phy_rate = bcm43xx_plcp_get_ratecode_cck(rate);
	txhdr->mac_frame_ctl = wlhdr->frame_control;
	memcpy(txhdr->tx_receiver, wlhdr->addr1, 6);

	/* Calculate duration for fallback rate */
	if ((rate_fb == rate) ||
	    (wlhdr->duration_id & cpu_to_le16(0x8000)) ||
	    (wlhdr->duration_id == cpu_to_le16(0))) { 
		/* If the fallback rate equals the normal rate or the
		 * dur_id field contains an AID, CFP magic or 0,
		 * use the original dur_id field. */
		txhdr->dur_fb = wlhdr->duration_id;
	} else {
		int fbrate_base100kbps = BCM43xx_RATE_TO_BASE100KBPS(rate_fb);
		txhdr->dur_fb = ieee80211_generic_frame_duration(dev->wl->hw,
								 fragment_len,
								 fbrate_base100kbps);
	}

	plcp_fragment_len = fragment_len + FCS_LEN;
	if (use_encryption) {
		u8 key_idx = (u16)(txctl->key_idx);
		struct bcm43xx_key *key;
		int wlhdr_len;
		size_t iv_len;

		assert(key_idx < dev->max_nr_keys);
		key = &(dev->key[key_idx]);

		if (key->enabled) {
			/* Hardware appends ICV. */
			plcp_fragment_len += txctl->icv_len;

			key_idx = bcm43xx_kidx_to_fw(dev, key_idx);
			mac_ctl |= (key_idx << BCM43xx_TX4_MAC_KEYIDX_SHIFT) &
				   BCM43xx_TX4_MAC_KEYIDX;
			mac_ctl |= (key->algorithm << BCM43xx_TX4_MAC_KEYALG_SHIFT) &
				   BCM43xx_TX4_MAC_KEYALG;
			wlhdr_len = ieee80211_get_hdrlen(fctl);
			iv_len = min((size_t)txctl->iv_len,
				     ARRAY_SIZE(txhdr->iv));
			memcpy(txhdr->iv, ((u8 *)wlhdr) + wlhdr_len, iv_len);
		}
	}
	bcm43xx_generate_plcp_hdr((struct bcm43xx_plcp_hdr4 *)(&txhdr->plcp),
				  plcp_fragment_len, rate);
	bcm43xx_generate_plcp_hdr((struct bcm43xx_plcp_hdr4 *)(&txhdr->plcp_fb),
				  plcp_fragment_len, rate_fb);

	/* Extra Frame Types */
	if (rate_fb_ofdm)
		extra_ft |= BCM43xx_TX4_EFT_FBOFDM;

	/* Set channel radio code. Note that the micrcode ORs 0x100 to
	 * this value before comparing it to the value in SHM, if this
	 * is a 5Ghz packet.
	 */
	txhdr->chan_radio_code = phy->channel;

	/* PHY TX Control word */
	if (rate_ofdm)
		phy_ctl |= BCM43xx_TX4_PHY_OFDM;
	if (dev->short_preamble)
		phy_ctl |= BCM43xx_TX4_PHY_SHORTPRMBL;
	switch (txctl->antenna_sel_tx) {
	case 0:
		phy_ctl |= BCM43xx_TX4_PHY_ANTLAST;
		break;
	case 1:
		phy_ctl |= BCM43xx_TX4_PHY_ANT0;
		break;
	case 2:
		phy_ctl |= BCM43xx_TX4_PHY_ANT1;
		break;
	default:
		assert(0);
	}

	/* MAC control */
	if (!(txctl->flags & IEEE80211_TXCTL_NO_ACK))
		mac_ctl |= BCM43xx_TX4_MAC_ACK;
	if (!(((fctl & IEEE80211_FCTL_FTYPE) == IEEE80211_FTYPE_CTL) &&
	      ((fctl & IEEE80211_FCTL_STYPE) == IEEE80211_STYPE_PSPOLL)))
		mac_ctl |= BCM43xx_TX4_MAC_HWSEQ;
	if (txctl->flags & IEEE80211_TXCTL_FIRST_FRAGMENT)
		mac_ctl |= BCM43xx_TX4_MAC_STMSDU;
	if (phy->type == BCM43xx_PHYTYPE_A)
		mac_ctl |= BCM43xx_TX4_MAC_5GHZ;

	/* Generate the RTS or CTS-to-self frame */
	if ((txctl->flags & IEEE80211_TXCTL_USE_RTS_CTS) ||
	    (txctl->flags & IEEE80211_TXCTL_USE_CTS_PROTECT)) {
		unsigned int len;
		struct ieee80211_hdr *hdr;
		int rts_rate, rts_rate_fb;
		int rts_rate_ofdm, rts_rate_fb_ofdm;

		rts_rate = txctl->rts_cts_rate;
		rts_rate_ofdm = bcm43xx_is_ofdm_rate(rts_rate);
		rts_rate_fb = bcm43xx_calc_fallback_rate(rts_rate);
		rts_rate_fb_ofdm = bcm43xx_is_ofdm_rate(rts_rate_fb);

		if (txctl->flags & IEEE80211_TXCTL_USE_CTS_PROTECT) {
			ieee80211_ctstoself_get(dev->wl->hw,
						fragment_data, fragment_len, txctl,
						(struct ieee80211_cts *)(txhdr->rts_frame));
			mac_ctl |= BCM43xx_TX4_MAC_SENDCTS;
			len = sizeof(struct ieee80211_cts);
		} else {
			ieee80211_rts_get(dev->wl->hw,
					  fragment_data, fragment_len, txctl,
					  (struct ieee80211_rts *)(txhdr->rts_frame));
			mac_ctl |= BCM43xx_TX4_MAC_SENDRTS;
			len = sizeof(struct ieee80211_rts);
		}
		len += FCS_LEN;
		bcm43xx_generate_plcp_hdr((struct bcm43xx_plcp_hdr4 *)(&txhdr->rts_plcp),
					  len, rts_rate);
		bcm43xx_generate_plcp_hdr((struct bcm43xx_plcp_hdr4 *)(&txhdr->rts_plcp_fb),
					  len, rts_rate_fb);
		hdr = (struct ieee80211_hdr *)(&txhdr->rts_frame);
		txhdr->rts_dur_fb = hdr->duration_id;
		if (rts_rate_ofdm) {
			extra_ft |= BCM43xx_TX4_EFT_RTSOFDM;
			txhdr->phy_rate_rts = bcm43xx_plcp_get_ratecode_ofdm(rts_rate);
		} else
			txhdr->phy_rate_rts = bcm43xx_plcp_get_ratecode_cck(rts_rate);
		if (rts_rate_fb_ofdm)
			extra_ft |= BCM43xx_TX4_EFT_RTSFBOFDM;
		mac_ctl |= BCM43xx_TX4_MAC_LONGFRAME;
	}

	/* Magic cookie */
	txhdr->cookie = cpu_to_le16(cookie);

	/* Apply the bitfields */
	txhdr->mac_ctl = cpu_to_le32(mac_ctl);
	txhdr->phy_ctl = cpu_to_le16(phy_ctl);
	txhdr->extra_ft = extra_ft;
}

void bcm43xx_generate_txhdr(struct bcm43xx_wldev *dev,
			    u8 *txhdr,
			    const unsigned char *fragment_data,
			    unsigned int fragment_len,
			    const struct ieee80211_tx_control *txctl,
			    u16 cookie)
{
	generate_txhdr_fw4(dev, (struct bcm43xx_txhdr_fw4 *)txhdr,
			   fragment_data, fragment_len,
			   txctl, cookie);
}

static s8 bcm43xx_rssi_postprocess(struct bcm43xx_wldev *dev,
				   u8 in_rssi, int ofdm,
				   int adjust_2053, int adjust_2050)
{
	struct bcm43xx_phy *phy = &dev->phy;
	s32 tmp;

	switch (phy->radio_ver) {
	case 0x2050:
		if (ofdm) {
			tmp = in_rssi;
			if (tmp > 127)
				tmp -= 256;
			tmp *= 73;
			tmp /= 64;
			if (adjust_2050)
				tmp += 25;
			else
				tmp -= 3;
		} else {
			if (dev->dev->bus->sprom.r1.boardflags_lo & BCM43xx_BFL_RSSI) {
				if (in_rssi > 63)
					in_rssi = 63;
				tmp = phy->nrssi_lt[in_rssi];
				tmp = 31 - tmp;
				tmp *= -131;
				tmp /= 128;
				tmp -= 57;
			} else {
				tmp = in_rssi;
				tmp = 31 - tmp;
				tmp *= -149;
				tmp /= 128;
				tmp -= 68;
			}
			if (phy->type == BCM43xx_PHYTYPE_G &&
			    adjust_2050)
				tmp += 25;
		}
		break;
	case 0x2060:
		if (in_rssi > 127)
			tmp = in_rssi - 256;
		else
			tmp = in_rssi;
		break;
	default:
		tmp = in_rssi;
		tmp -= 11;
		tmp *= 103;
		tmp /= 64;
		if (adjust_2053)
			tmp -= 109;
		else
			tmp -= 83;
	}

	return (s8)tmp;
}

//TODO
#if 0
static s8 bcm43xx_rssinoise_postprocess(struct bcm43xx_wldev *dev,
					u8 in_rssi)
{
	struct bcm43xx_phy *phy = &dev->phy;
	s8 ret;

	if (phy->type == BCM43xx_PHYTYPE_A) {
		//TODO: Incomplete specs.
		ret = 0;
	} else
		ret = bcm43xx_rssi_postprocess(dev, in_rssi, 0, 1, 1);

	return ret;
}
#endif

void bcm43xx_rx(struct bcm43xx_wldev *dev,
		struct sk_buff *skb,
		const void *_rxhdr)
{
	struct ieee80211_rx_status status;
	struct bcm43xx_plcp_hdr6 *plcp;
	struct ieee80211_hdr *wlhdr;
	const struct bcm43xx_rxhdr_fw4 *rxhdr = _rxhdr;
	u16 fctl;
	u16 phystat0, phystat3, chanstat, mactime;
	u32 macstat;
	u16 chanid;
	u8 jssi;
	int padding;

	memset(&status, 0, sizeof(status));

	/* Get metadata about the frame from the header. */
	phystat0 = le16_to_cpu(rxhdr->phy_status0);
	phystat3 = le16_to_cpu(rxhdr->phy_status3);
	jssi = rxhdr->jssi;
	macstat = le32_to_cpu(rxhdr->mac_status);
	mactime = le16_to_cpu(rxhdr->mac_time);
	chanstat = le16_to_cpu(rxhdr->channel);

	if (macstat & BCM43xx_RX_MAC_FCSERR)
		dev->wl->ieee_stats.dot11FCSErrorCount++;

	/* Skip PLCP and padding */
	padding = (macstat & BCM43xx_RX_MAC_PADDING) ? 2 : 0;
	if (unlikely(skb->len < (sizeof(struct bcm43xx_plcp_hdr6) + padding))) {
		dprintkl(KERN_DEBUG PFX "RX: Packet size underrun (1)\n");
		goto drop;
	}
	plcp = (struct bcm43xx_plcp_hdr6 *)(skb->data + padding);
	skb_pull(skb, sizeof(struct bcm43xx_plcp_hdr6) + padding);
	/* The skb contains the Wireless Header + payload data now */
	if (unlikely(skb->len < (2+2+6/*minimum hdr*/ + FCS_LEN))) {
		dprintkl(KERN_DEBUG PFX "RX: Packet size underrun (2)\n");
		goto drop;
	}
	wlhdr = (struct ieee80211_hdr *)(skb->data);
	fctl = le16_to_cpu(wlhdr->frame_control);
	skb_trim(skb, skb->len - FCS_LEN);

	if ((macstat & BCM43xx_RX_MAC_DEC) &&
	    !(macstat & BCM43xx_RX_MAC_DECERR)) {
		unsigned int keyidx;
		int wlhdr_len;
		int iv_len;
		int icv_len;

		keyidx = ((macstat & BCM43xx_RX_MAC_KEYIDX)
			  >> BCM43xx_RX_MAC_KEYIDX_SHIFT);
		/* We must adjust the key index here. We want the "physical"
		 * key index, but the ucode passed it slightly different.
		 */
		keyidx = bcm43xx_kidx_to_raw(dev, keyidx);
		assert(keyidx < dev->max_nr_keys);

		if (dev->key[keyidx].algorithm != BCM43xx_SEC_ALGO_NONE) {
			/* Remove PROTECTED flag to mark it as decrypted. */
			assert(fctl & IEEE80211_FCTL_PROTECTED);
			fctl &= ~IEEE80211_FCTL_PROTECTED;
			wlhdr->frame_control = cpu_to_le16(fctl);

			wlhdr_len = ieee80211_get_hdrlen(fctl);
			if (unlikely(skb->len < (wlhdr_len + 3))) {
				dprintkl(KERN_DEBUG PFX
					 "RX: Packet size underrun (3)\n");
				goto drop;
			}
			if (skb->data[wlhdr_len + 3] & (1 << 5)) {
				/* The Ext-IV Bit is set in the "KeyID"
				 * octet of the IV.
				 */
				iv_len = 8;
				icv_len = 8;
			} else {
				iv_len = 4;
				icv_len = 4;
			}
			if (unlikely(skb->len < (wlhdr_len + iv_len + icv_len))) {
				dprintkl(KERN_DEBUG PFX
					 "RX: Packet size underrun (4)\n");
				goto drop;
			}
			/* Remove the IV */
			memmove(skb->data + iv_len, skb->data, wlhdr_len);
			skb_pull(skb, iv_len);
			/* Remove the ICV */
			skb_trim(skb, skb->len - icv_len);

			status.flag |= RX_FLAG_DECRYPTED;
		}
	}

	status.ssi = bcm43xx_rssi_postprocess(dev, jssi,
					      (phystat0 & BCM43xx_RX_PHYST0_OFDM),
					      (phystat0 & BCM43xx_RX_PHYST0_GAINCTL),
					      (phystat3 & BCM43xx_RX_PHYST3_TRSTATE));
	status.noise = dev->stats.link_noise;
	status.signal = jssi; /* this looks wrong, but is what mac80211 wants */
	if (phystat0 & BCM43xx_RX_PHYST0_OFDM)
		status.rate = bcm43xx_plcp_get_bitrate_ofdm(plcp);
	else
		status.rate = bcm43xx_plcp_get_bitrate_cck(plcp);
	status.antenna = !!(phystat0 & BCM43xx_RX_PHYST0_ANT);
	status.mactime = mactime;

	chanid = (chanstat & BCM43xx_RX_CHAN_ID) >> BCM43xx_RX_CHAN_ID_SHIFT;
	switch (chanstat & BCM43xx_RX_CHAN_PHYTYPE) {
	case BCM43xx_PHYTYPE_A:
		status.phymode = MODE_IEEE80211A;
		status.freq = chanid;
		status.channel = bcm43xx_freq_to_channel_a(chanid);
		break;
	case BCM43xx_PHYTYPE_B:
		status.phymode = MODE_IEEE80211B;
		status.freq = chanid + 2400;
		status.channel = bcm43xx_freq_to_channel_bg(chanid + 2400);
		break;
	case BCM43xx_PHYTYPE_G:
		status.phymode = MODE_IEEE80211G;
		status.freq = chanid + 2400;
		status.channel = bcm43xx_freq_to_channel_bg(chanid + 2400);
		break;
	default:
		assert(0);
	}

	dev->stats.last_rx = jiffies;
	ieee80211_rx_irqsafe(dev->wl->hw, skb, &status);

	return;
drop:
	dprintkl(KERN_DEBUG PFX "RX: Packet dropped\n");
	dev_kfree_skb_any(skb);
}

void bcm43xx_handle_txstatus(struct bcm43xx_wldev *dev,
			     const struct bcm43xx_txstatus *status)
{
	bcm43xx_debugfs_log_txstat(dev, status);

	if (status->intermediate)
		return;
	if (status->for_ampdu)
		return;
	if (!status->acked)
		dev->wl->ieee_stats.dot11ACKFailureCount++;
	if (status->rts_count) {
		if (status->rts_count == 0xF) //FIXME
			dev->wl->ieee_stats.dot11RTSFailureCount++;
		else
			dev->wl->ieee_stats.dot11RTSSuccessCount++;
	}

	if (bcm43xx_using_pio(dev))
		bcm43xx_pio_handle_txstatus(dev, status);
	else
		bcm43xx_dma_handle_txstatus(dev, status);
}

/* Handle TX status report as received through DMA/PIO queues */
void bcm43xx_handle_hwtxstatus(struct bcm43xx_wldev *dev,
			       const struct bcm43xx_hwtxstatus *hw)
{
	struct bcm43xx_txstatus status;
	u8 tmp;

	status.cookie = le16_to_cpu(hw->cookie);
	status.seq = le16_to_cpu(hw->seq);
	status.phy_stat = hw->phy_stat;
	tmp = hw->count;
	status.frame_count = (tmp >> 4);
	status.rts_count = (tmp & 0x0F);
	tmp = hw->flags;
	status.supp_reason = ((tmp & 0x1C) >> 2);
	status.pm_indicated = !!(tmp & 0x80);
	status.intermediate = !!(tmp & 0x40);
	status.for_ampdu = !!(tmp & 0x20);
	status.acked = !!(tmp & 0x02);

	bcm43xx_handle_txstatus(dev, &status);
}

/* Stop any TX operation on the device (suspend the hardware queues) */
void bcm43xx_tx_suspend(struct bcm43xx_wldev *dev)
{
	if (bcm43xx_using_pio(dev))
		bcm43xx_pio_freeze_txqueues(dev);
	else
		bcm43xx_dma_tx_suspend(dev);
}

/* Resume any TX operation on the device (resume the hardware queues) */
void bcm43xx_tx_resume(struct bcm43xx_wldev *dev)
{
	if (bcm43xx_using_pio(dev))
		bcm43xx_pio_thaw_txqueues(dev);
	else
		bcm43xx_dma_tx_resume(dev);
}

#if 0
static void upload_qos_parms(struct bcm43xx_wldev *dev,
			     const u16 *parms,
			     u16 offset)
{
	int i;

	for (i = 0; i < BCM43xx_NR_QOSPARMS; i++) {
		bcm43xx_shm_write16(dev, BCM43xx_SHM_SHARED,
				    offset + (i * 2), parms[i]);
	}
}
#endif

/* Initialize the QoS parameters */
void bcm43xx_qos_init(struct bcm43xx_wldev *dev)
{
	/* FIXME: This function must probably be called from the mac80211
	 * config callback. */
return;

	bcm43xx_hf_write(dev, bcm43xx_hf_read(dev) | BCM43xx_HF_EDCF);
	//FIXME kill magic
	bcm43xx_write16(dev, 0x688,
			bcm43xx_read16(dev, 0x688) | 0x4);


	/*TODO: We might need some stack support here to get the values. */
}
