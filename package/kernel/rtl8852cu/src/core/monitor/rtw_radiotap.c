/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
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
#define _RTW_RADIOTAP_C_

#ifdef CONFIG_WIFI_MONITOR

#include <drv_types.h>
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0))
#include <linux/bitfield.h>
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0))
#define __bf_shf(x) (__builtin_ffsll(x) - 1)

/**
 * FIELD_PREP() - prepare a bitfield element
 * @_mask: shifted mask defining the field's length and position
 * @_val:  value to put in the field
 *
 * FIELD_PREP() masks and shifts up the value.  The result should
 * be combined with other fields of the bitfield using logical OR.
 */
#define FIELD_PREP(_mask, _val)						\
	({								\
		((typeof(_mask))(_val) << __bf_shf(_mask)) & (_mask);	\
	})
#endif

#define __encode_bits(w, v, field) FIELD_PREP((u##w)(field), v)
#define __u16_encode_bits(v, field)	__encode_bits(16, v, field)
#define __le16_encode_bits(v, field)	cpu_to_le16(__encode_bits(16, v, field))
#define __be16_encode_bits(v, field)	cpu_to_be16(__encode_bits(16, v, field))
#define __u32_encode_bits(v, field)	__encode_bits(32, v, field)
#define __le32_encode_bits(v, field)	cpu_to_le32(__encode_bits(32, v, field))
#define __be32_encode_bits(v, field)	cpu_to_be32(__encode_bits(32, v, field))
#define __u64_encode_bits(v, field)	__encode_bits(64, v, field)
#define __le64_encode_bits(v, field)	cpu_to_le64(__encode_bits(64, v, field))
#define __be64_encode_bits(v, field)	cpu_to_be64(__encode_bits(64, v, field))


#define CHAN2FREQ(a) ((a < 14) ? (2407+5*a) : (5000+5*a))

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 20, 0))
#define IEEE80211_RADIOTAP_ZERO_LEN_PSDU 26
#define IEEE80211_RADIOTAP_LSIG 27
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 9, 0))
#define IEEE80211_RADIOTAP_TIMESTAMP 22
/* For IEEE80211_RADIOTAP_TIMESTAMP */
#define IEEE80211_RADIOTAP_TIMESTAMP_UNIT_MASK			0x000F
#define IEEE80211_RADIOTAP_TIMESTAMP_UNIT_MS			0x0000
#define IEEE80211_RADIOTAP_TIMESTAMP_UNIT_US			0x0001
#define IEEE80211_RADIOTAP_TIMESTAMP_UNIT_NS			0x0003
#define IEEE80211_RADIOTAP_TIMESTAMP_SPOS_MASK			0x00F0
#define IEEE80211_RADIOTAP_TIMESTAMP_SPOS_BEGIN_MDPU		0x0000
#define IEEE80211_RADIOTAP_TIMESTAMP_SPOS_EO_MPDU		0x0010
#define IEEE80211_RADIOTAP_TIMESTAMP_SPOS_EO_PPDU		0x0020
#define IEEE80211_RADIOTAP_TIMESTAMP_SPOS_PLCP_SIG_ACQ		0x0030
#define IEEE80211_RADIOTAP_TIMESTAMP_SPOS_UNKNOWN		0x00F0

#define IEEE80211_RADIOTAP_TIMESTAMP_FLAG_64BIT			0x00
#define IEEE80211_RADIOTAP_TIMESTAMP_FLAG_32BIT			0x01
#define IEEE80211_RADIOTAP_TIMESTAMP_FLAG_ACCURACY		0x02
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 12, 0))
/* for IEEE80211_RADIOTAP_CHANNEL */
#define	IEEE80211_CHAN_GSM	0x1000	/* GSM (900 MHz) */
#define	IEEE80211_CHAN_STURBO	0x2000	/* Static Turbo */
#define	IEEE80211_CHAN_HALF	0x4000	/* Half channel (10 MHz wide) */
#define	IEEE80211_CHAN_QUARTER	0x8000	/* Quarter channel (5 MHz wide) */
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 19, 0))
#define IEEE80211_RADIOTAP_HE 23

struct ieee80211_radiotap_he {
	u16 data1, data2, data3, data4, data5, data6;
};

enum ieee80211_radiotap_he_bits {
	IEEE80211_RADIOTAP_HE_DATA1_FORMAT_MASK		= 3,
	IEEE80211_RADIOTAP_HE_DATA1_FORMAT_SU		= 0,
	IEEE80211_RADIOTAP_HE_DATA1_FORMAT_EXT_SU	= 1,
	IEEE80211_RADIOTAP_HE_DATA1_FORMAT_MU		= 2,
	IEEE80211_RADIOTAP_HE_DATA1_FORMAT_TRIG		= 3,

	IEEE80211_RADIOTAP_HE_DATA1_BSS_COLOR_KNOWN	= 0x0004,
	IEEE80211_RADIOTAP_HE_DATA1_BEAM_CHANGE_KNOWN	= 0x0008,
	IEEE80211_RADIOTAP_HE_DATA1_UL_DL_KNOWN		= 0x0010,
	IEEE80211_RADIOTAP_HE_DATA1_DATA_MCS_KNOWN	= 0x0020,
	IEEE80211_RADIOTAP_HE_DATA1_DATA_DCM_KNOWN	= 0x0040,
	IEEE80211_RADIOTAP_HE_DATA1_CODING_KNOWN	= 0x0080,
	IEEE80211_RADIOTAP_HE_DATA1_LDPC_XSYMSEG_KNOWN	= 0x0100,
	IEEE80211_RADIOTAP_HE_DATA1_STBC_KNOWN		= 0x0200,
	IEEE80211_RADIOTAP_HE_DATA1_SPTL_REUSE_KNOWN	= 0x0400,
	IEEE80211_RADIOTAP_HE_DATA1_SPTL_REUSE2_KNOWN	= 0x0800,
	IEEE80211_RADIOTAP_HE_DATA1_SPTL_REUSE3_KNOWN	= 0x1000,
	IEEE80211_RADIOTAP_HE_DATA1_SPTL_REUSE4_KNOWN	= 0x2000,
	IEEE80211_RADIOTAP_HE_DATA1_BW_RU_ALLOC_KNOWN	= 0x4000,
	IEEE80211_RADIOTAP_HE_DATA1_DOPPLER_KNOWN	= 0x8000,

	IEEE80211_RADIOTAP_HE_DATA2_PRISEC_80_KNOWN	= 0x0001,
	IEEE80211_RADIOTAP_HE_DATA2_GI_KNOWN		= 0x0002,
	IEEE80211_RADIOTAP_HE_DATA2_NUM_LTF_SYMS_KNOWN	= 0x0004,
	IEEE80211_RADIOTAP_HE_DATA2_PRE_FEC_PAD_KNOWN	= 0x0008,
	IEEE80211_RADIOTAP_HE_DATA2_TXBF_KNOWN		= 0x0010,
	IEEE80211_RADIOTAP_HE_DATA2_PE_DISAMBIG_KNOWN	= 0x0020,
	IEEE80211_RADIOTAP_HE_DATA2_TXOP_KNOWN		= 0x0040,
	IEEE80211_RADIOTAP_HE_DATA2_MIDAMBLE_KNOWN	= 0x0080,
	IEEE80211_RADIOTAP_HE_DATA2_RU_OFFSET		= 0x3f00,
	IEEE80211_RADIOTAP_HE_DATA2_RU_OFFSET_KNOWN	= 0x4000,
	IEEE80211_RADIOTAP_HE_DATA2_PRISEC_80_SEC	= 0x8000,

	IEEE80211_RADIOTAP_HE_DATA3_BSS_COLOR		= 0x003f,
	IEEE80211_RADIOTAP_HE_DATA3_BEAM_CHANGE		= 0x0040,
	IEEE80211_RADIOTAP_HE_DATA3_UL_DL		= 0x0080,
	IEEE80211_RADIOTAP_HE_DATA3_DATA_MCS		= 0x0f00,
	IEEE80211_RADIOTAP_HE_DATA3_DATA_DCM		= 0x1000,
	IEEE80211_RADIOTAP_HE_DATA3_CODING		= 0x2000,
	IEEE80211_RADIOTAP_HE_DATA3_LDPC_XSYMSEG	= 0x4000,
	IEEE80211_RADIOTAP_HE_DATA3_STBC		= 0x8000,

	IEEE80211_RADIOTAP_HE_DATA4_SU_MU_SPTL_REUSE	= 0x000f,
	IEEE80211_RADIOTAP_HE_DATA4_MU_STA_ID		= 0x7ff0,
	IEEE80211_RADIOTAP_HE_DATA4_TB_SPTL_REUSE1	= 0x000f,
	IEEE80211_RADIOTAP_HE_DATA4_TB_SPTL_REUSE2	= 0x00f0,
	IEEE80211_RADIOTAP_HE_DATA4_TB_SPTL_REUSE3	= 0x0f00,
	IEEE80211_RADIOTAP_HE_DATA4_TB_SPTL_REUSE4	= 0xf000,

	IEEE80211_RADIOTAP_HE_DATA5_DATA_BW_RU_ALLOC	= 0x000f,
		IEEE80211_RADIOTAP_HE_DATA5_DATA_BW_RU_ALLOC_20MHZ	= 0,
		IEEE80211_RADIOTAP_HE_DATA5_DATA_BW_RU_ALLOC_40MHZ	= 1,
		IEEE80211_RADIOTAP_HE_DATA5_DATA_BW_RU_ALLOC_80MHZ	= 2,
		IEEE80211_RADIOTAP_HE_DATA5_DATA_BW_RU_ALLOC_160MHZ	= 3,
		IEEE80211_RADIOTAP_HE_DATA5_DATA_BW_RU_ALLOC_26T	= 4,
		IEEE80211_RADIOTAP_HE_DATA5_DATA_BW_RU_ALLOC_52T	= 5,
		IEEE80211_RADIOTAP_HE_DATA5_DATA_BW_RU_ALLOC_106T	= 6,
		IEEE80211_RADIOTAP_HE_DATA5_DATA_BW_RU_ALLOC_242T	= 7,
		IEEE80211_RADIOTAP_HE_DATA5_DATA_BW_RU_ALLOC_484T	= 8,
		IEEE80211_RADIOTAP_HE_DATA5_DATA_BW_RU_ALLOC_996T	= 9,
		IEEE80211_RADIOTAP_HE_DATA5_DATA_BW_RU_ALLOC_2x996T	= 10,

	IEEE80211_RADIOTAP_HE_DATA5_GI			= 0x0030,
		IEEE80211_RADIOTAP_HE_DATA5_GI_0_8			= 0,
		IEEE80211_RADIOTAP_HE_DATA5_GI_1_6			= 1,
		IEEE80211_RADIOTAP_HE_DATA5_GI_3_2			= 2,

	IEEE80211_RADIOTAP_HE_DATA5_LTF_SIZE		= 0x00c0,
		IEEE80211_RADIOTAP_HE_DATA5_LTF_SIZE_UNKNOWN		= 0,
		IEEE80211_RADIOTAP_HE_DATA5_LTF_SIZE_1X			= 1,
		IEEE80211_RADIOTAP_HE_DATA5_LTF_SIZE_2X			= 2,
		IEEE80211_RADIOTAP_HE_DATA5_LTF_SIZE_4X			= 3,
	IEEE80211_RADIOTAP_HE_DATA5_NUM_LTF_SYMS	= 0x0700,
	IEEE80211_RADIOTAP_HE_DATA5_PRE_FEC_PAD		= 0x3000,
	IEEE80211_RADIOTAP_HE_DATA5_TXBF		= 0x4000,
	IEEE80211_RADIOTAP_HE_DATA5_PE_DISAMBIG		= 0x8000,

	IEEE80211_RADIOTAP_HE_DATA6_NSTS		= 0x000f,
	IEEE80211_RADIOTAP_HE_DATA6_DOPPLER		= 0x0010,
	IEEE80211_RADIOTAP_HE_DATA6_TXOP		= 0x7f00,
	IEEE80211_RADIOTAP_HE_DATA6_MIDAMBLE_PDCTY	= 0x8000,
};

#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 8, 0))
#define IEEE80211_RADIOTAP_VHT 21
/* For IEEE80211_RADIOTAP_VHT */
#define IEEE80211_RADIOTAP_VHT_KNOWN_STBC			0x0001
#define IEEE80211_RADIOTAP_VHT_KNOWN_TXOP_PS_NA			0x0002
#define IEEE80211_RADIOTAP_VHT_KNOWN_GI				0x0004
#define IEEE80211_RADIOTAP_VHT_KNOWN_SGI_NSYM_DIS		0x0008
#define IEEE80211_RADIOTAP_VHT_KNOWN_LDPC_EXTRA_OFDM_SYM	0x0010
#define IEEE80211_RADIOTAP_VHT_KNOWN_BEAMFORMED			0x0020
#define IEEE80211_RADIOTAP_VHT_KNOWN_BANDWIDTH			0x0040
#define IEEE80211_RADIOTAP_VHT_KNOWN_GROUP_ID			0x0080
#define IEEE80211_RADIOTAP_VHT_KNOWN_PARTIAL_AID		0x0100

#define IEEE80211_RADIOTAP_VHT_FLAG_STBC			0x01
#define IEEE80211_RADIOTAP_VHT_FLAG_TXOP_PS_NA			0x02
#define IEEE80211_RADIOTAP_VHT_FLAG_SGI				0x04
#define IEEE80211_RADIOTAP_VHT_FLAG_SGI_NSYM_M10_9		0x08
#define IEEE80211_RADIOTAP_VHT_FLAG_LDPC_EXTRA_OFDM_SYM		0x10
#define IEEE80211_RADIOTAP_VHT_FLAG_BEAMFORMED			0x20
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(3, 15, 0))
#define IEEE80211_RADIOTAP_CODING_LDPC_USER0			0x01
#define IEEE80211_RADIOTAP_CODING_LDPC_USER1			0x02
#define IEEE80211_RADIOTAP_CODING_LDPC_USER2			0x04
#define IEEE80211_RADIOTAP_CODING_LDPC_USER3			0x08
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 7, 0))
#define IEEE80211_RADIOTAP_AMPDU_STATUS 20
/* For IEEE80211_RADIOTAP_AMPDU_STATUS */
#define IEEE80211_RADIOTAP_AMPDU_REPORT_ZEROLEN		0x0001
#define IEEE80211_RADIOTAP_AMPDU_IS_ZEROLEN		0x0002
#define IEEE80211_RADIOTAP_AMPDU_LAST_KNOWN		0x0004
#define IEEE80211_RADIOTAP_AMPDU_IS_LAST		0x0008
#define IEEE80211_RADIOTAP_AMPDU_DELIM_CRC_ERR		0x0010
#define IEEE80211_RADIOTAP_AMPDU_DELIM_CRC_KNOWN	0x0020
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(4, 17, 0))
#define IEEE80211_RADIOTAP_AMPDU_EOF			0x0040
#define IEEE80211_RADIOTAP_AMPDU_EOF_KNOWN		0x0080
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 39))
#define IEEE80211_RADIOTAP_MCS 19
/* For IEEE80211_RADIOTAP_MCS */
#define IEEE80211_RADIOTAP_MCS_HAVE_BW		0x01
#define IEEE80211_RADIOTAP_MCS_HAVE_MCS		0x02
#define IEEE80211_RADIOTAP_MCS_HAVE_GI		0x04
#define IEEE80211_RADIOTAP_MCS_HAVE_FMT		0x08
#define IEEE80211_RADIOTAP_MCS_HAVE_FEC		0x10

#define IEEE80211_RADIOTAP_MCS_BW_MASK		0x03
#define		IEEE80211_RADIOTAP_MCS_BW_20		0
#define		IEEE80211_RADIOTAP_MCS_BW_40		1
#define		IEEE80211_RADIOTAP_MCS_BW_20L		2
#define		IEEE80211_RADIOTAP_MCS_BW_20U		3
#define IEEE80211_RADIOTAP_MCS_SGI		0x04
#define IEEE80211_RADIOTAP_MCS_FMT_GF		0x08
#define IEEE80211_RADIOTAP_MCS_FEC_LDPC		0x10
#elif (LINUX_VERSION_CODE < KERNEL_VERSION(3, 11, 0))
#define IEEE80211_RADIOTAP_MCS_HAVE_STBC	0x20

#define IEEE80211_RADIOTAP_MCS_STBC_MASK	0x60
#define		IEEE80211_RADIOTAP_MCS_STBC_1	1
#define		IEEE80211_RADIOTAP_MCS_STBC_2	2
#define		IEEE80211_RADIOTAP_MCS_STBC_3	3
#define IEEE80211_RADIOTAP_MCS_STBC_SHIFT	5
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 34))
#define IEEE80211_RADIOTAP_RADIOTAP_NAMESPACE 29
#define IEEE80211_RADIOTAP_VENDOR_NAMESPACE 30
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 30))
#define IEEE80211_RADIOTAP_F_BADFCS 0x40
#endif

static inline void _rtw_radiotap_fill_flags(struct rx_pkt_attrib *a, u8 *flags)
{
	struct moinfo *moif = (struct moinfo *)&a->moif;

	if (0)
		*flags |= IEEE80211_RADIOTAP_F_CFP;

	if (0)
		*flags |= IEEE80211_RADIOTAP_F_SHORTPRE;

	if ((a->encrypt == 1) || (a->encrypt == 5))
		*flags |= IEEE80211_RADIOTAP_F_WEP;

	if (a->mfrag)
		*flags |= IEEE80211_RADIOTAP_F_FRAG;

	if (1)
		*flags |= IEEE80211_RADIOTAP_F_FCS;

	if (0)
		*flags |= IEEE80211_RADIOTAP_F_DATAPAD;

	if (a->crc_err)
		*flags |= IEEE80211_RADIOTAP_F_BADFCS;

	/* Currently unspecified but used
	   for short guard interval (HT) */
	if (moif->u.snif_info.sgi || a->sgi)
		*flags |= 0x80;

}

sint rtw_fill_radiotap_hdr(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
		struct rx_pkt_attrib *a, struct rtw_recv_pkt *rx_req, u8 *buf)
{
#define RTAP_HDR_MAX 64

	sint ret = _SUCCESS;
	struct moinfo *moif = (struct moinfo *)&a->moif;

	u8 rx_cnt = 0;

	int i = 0;
	u8 tmp_8bit = 0;
	u16 tmp_16bit = 0;
	u32 tmp_32bit = 0;
	u64 tmp_64bit = 0;

	struct sk_buff *pskb = NULL;

	struct ieee80211_radiotap_header *rtap_hdr = NULL;
	u8 *ptr = NULL;

	/*
	  radiotap length (include header 8)
	  11G length: 36 (0x0040002f)
	  11N length:
	  11AC length: 60 (0x0070002b)
	 */
	u8 hdr_buf[RTAP_HDR_MAX] = { 0 };
	u16 rt_len = 8;

	/* create header */
	rtap_hdr = (struct ieee80211_radiotap_header *)&hdr_buf[0];
	rtap_hdr->it_version = PKTHDR_RADIOTAP_VERSION;

	/* each antenna information */
	rx_cnt = GET_HAL_RFPATH_NUM(padapter->dvobj);
#if 0
	if (rx_cnt > 1) {
		rtap_hdr->it_present |= BIT(IEEE80211_RADIOTAP_RADIOTAP_NAMESPACE) |
		BIT(IEEE80211_RADIOTAP_EXT);

		for (i = 1; i < rx_cnt; i++) {
			tmp_32bit = (BIT(IEEE80211_RADIOTAP_DBM_ANTSIGNAL) |
				     BIT(IEEE80211_RADIOTAP_LOCK_QUALITY) |
				     BIT(IEEE80211_RADIOTAP_ANTENNA) |
				     BIT(IEEE80211_RADIOTAP_RADIOTAP_NAMESPACE) |
				     BIT(IEEE80211_RADIOTAP_EXT));
			_rtw_memcpy(&hdr_buf[rt_len], &tmp_32bit, 4);
			rt_len += 4;
		}

		tmp_32bit = (BIT(IEEE80211_RADIOTAP_DBM_ANTSIGNAL) |
			     BIT(IEEE80211_RADIOTAP_LOCK_QUALITY) |
			     BIT(IEEE80211_RADIOTAP_ANTENNA));
		_rtw_memcpy(&hdr_buf[rt_len], &tmp_32bit, 4);
		rt_len += 4;
	}
#endif

	/* tsft, Required Alignment: 8 bytes */
	if (0) { //(a->free_cnt) {
		/* TSFT + free_cnt */
		rtap_hdr->it_present |= BIT(IEEE80211_RADIOTAP_TSFT);
		if (!IS_ALIGNED(rt_len, 8))
			rt_len = ((rt_len + 7) & 0xFFF8); /* Alignment */

		tmp_64bit = cpu_to_le64(rx_req->mdata.freerun_cnt);
		_rtw_memcpy(&hdr_buf[rt_len], &tmp_64bit, 8);
		rt_len += 8;
	}

	/* flags */
	rtap_hdr->it_present |= BIT(IEEE80211_RADIOTAP_FLAGS);
	_rtw_radiotap_fill_flags(a, &hdr_buf[rt_len]);
	rt_len += 1;

	/* rate */
	if (rx_req->mdata.rx_rate <= RTW_DATA_RATE_OFDM54) {
		rtap_hdr->it_present |= BIT(IEEE80211_RADIOTAP_RATE);
		hdr_buf[rt_len] = hwrate_to_mrate(rx_req->mdata.rx_rate);
		rt_len += 1;
	}

	/* channel & flags, Required Alignment: 2 bytes */
	if (1) {
		rtap_hdr->it_present |= BIT(IEEE80211_RADIOTAP_CHANNEL);
		rt_len += (rt_len % 2); /* Alignment */

		tmp_16bit = CHAN2FREQ(rtw_get_oper_ch(padapter, padapter_link));
		_rtw_memcpy(&hdr_buf[rt_len], &tmp_16bit, 2);
		rt_len += 2;

		/* channel flags */
		tmp_16bit = 0;
		if (WIFI_ROLE_LINK_IS_ON_24G(padapter_link))
			tmp_16bit |= cpu_to_le16(IEEE80211_CHAN_2GHZ);
		else
			tmp_16bit |= cpu_to_le16(IEEE80211_CHAN_5GHZ);

		if (rx_req->mdata.rx_rate <= RTW_DATA_RATE_CCK11) {
			/* CCK */
			tmp_16bit |= cpu_to_le16(IEEE80211_CHAN_CCK);
		} else {
			/* OFDM */
			tmp_16bit |= cpu_to_le16(IEEE80211_CHAN_OFDM);
		}

		if (rtw_get_oper_bw(padapter, padapter_link) == CHANNEL_WIDTH_10) {
			/* 10Mhz Channel Width */
			tmp_16bit |= cpu_to_le16(IEEE80211_CHAN_HALF);
		}

		if (rtw_get_oper_bw(padapter, padapter_link) == CHANNEL_WIDTH_5) {
			/* 5Mhz Channel Width */
			tmp_16bit |= cpu_to_le16(IEEE80211_CHAN_QUARTER);
		}
		_rtw_memcpy(&hdr_buf[rt_len], &tmp_16bit, 2);
		rt_len += 2;
	}

	/* dBm Antenna Signal */
	rtap_hdr->it_present |= BIT(IEEE80211_RADIOTAP_DBM_ANTSIGNAL);
	hdr_buf[rt_len] = a->phy_info.recv_signal_power;
	rt_len += 1;

#if 0
	/* dBm Antenna Noise */
	rtap_hdr->it_present |= BIT(IEEE80211_RADIOTAP_DBM_ANTNOISE);
	hdr_buf[rt_len] = 0;
	rt_len += 1;
#endif
#if 0
	/* Signal Quality, Required Alignment: 2 bytes */
	rtap_hdr->it_present |= BIT(IEEE80211_RADIOTAP_LOCK_QUALITY);
	if (!IS_ALIGNED(rt_len, 2))
	rt_len++;
	hdr_buf[rt_len] = a->phy_info.signal_quality;
	rt_len += 2;

#endif

#if 0
	/* Antenna */
	rtap_hdr->it_present |= BIT(IEEE80211_RADIOTAP_ANTENNA);
	hdr_buf[rt_len] = 0; /* pHalData->rf_type; */
	rt_len += 1;
#endif
#if 0
	/* RX flags, Required Alignment: 2 bytes */
	rtap_hdr->it_present |= BIT(IEEE80211_RADIOTAP_RX_FLAGS);
	tmp_16bit = 0;
	_rtw_memcpy(&hdr_buf[rt_len], &tmp_16bit, 2);
	rt_len += 2;
#endif

	/* MCS information, Required Alignment: 1 bytes */
	if (rx_req->mdata.rx_rate >= RTW_DATA_RATE_MCS0 && rx_req->mdata.rx_rate <= RTW_DATA_RATE_MCS31) {
		rtap_hdr->it_present |= BIT(IEEE80211_RADIOTAP_MCS);
		/* Structure u8 known, u8 flags, u8 mcs */

		/* known.bandwidth */
		hdr_buf[rt_len] |= IEEE80211_RADIOTAP_MCS_HAVE_BW;
		if (moif->u.snif_info.ofdm_bw)
			hdr_buf[rt_len + 1] |= IEEE80211_RADIOTAP_MCS_BW_40;
		if (rx_req->mdata.bw == CHANNEL_WIDTH_40)
			hdr_buf[rt_len + 1] |= IEEE80211_RADIOTAP_MCS_BW_40;
		else
			hdr_buf[rt_len + 1] |= IEEE80211_RADIOTAP_MCS_BW_20;


		/* known.guard interval */
		hdr_buf[rt_len] |= IEEE80211_RADIOTAP_MCS_HAVE_GI;
		if (moif->u.snif_info.sgi) {
			hdr_buf[rt_len + 1] |= IEEE80211_RADIOTAP_MCS_SGI;
		} else {
			hdr_buf[rt_len + 1] |= ((a->sgi & 0x01) << 2);
		}

		/* FEC Type */
		hdr_buf[rt_len] |= IEEE80211_RADIOTAP_MCS_HAVE_FEC;
		if (moif->u.snif_info.ldpc) {
			hdr_buf[rt_len + 1] |= ((moif->u.snif_info.ldpc & 0x01) << 4);
		} else {
			hdr_buf[rt_len + 1] |= ((a->ldpc & 0x01) << 4);
		}

		/* STBC */
		hdr_buf[rt_len] |= IEEE80211_RADIOTAP_MCS_HAVE_STBC;
		if (moif->u.snif_info.stbc) {
			hdr_buf[rt_len + 1] |= ((moif->u.snif_info.stbc & 0x03) << 5);
		} else {
			hdr_buf[rt_len + 1] |= ((a->stbc & 0x03) << 5);
		}

		/* known.MCS index */
		hdr_buf[rt_len] |= IEEE80211_RADIOTAP_MCS_HAVE_MCS;

		/* u8 mcs */
		hdr_buf[rt_len + 2] = rx_req->mdata.rx_rate - RTW_DATA_RATE_MCS0;

		rt_len += 3;
	}

	/* AMPDU, Required Alignment: 4 bytes */
	if (rx_req->mdata.ampdu) {
		static u32 ref_num = 0x10000000;
		static u8 ppdu_cnt = 0;

		/* Structure u32 reference number, u16 flags, u8 delimiter CRC value, u8 reserved */
		rtap_hdr->it_present |= BIT(IEEE80211_RADIOTAP_AMPDU_STATUS);
		if (!IS_ALIGNED(rt_len, 4))
			rt_len = ((rt_len + 3) & 0xFFFC); /* Alignment */

		/* u32 reference number */
		if (rx_req->mdata.ppdu_cnt != ppdu_cnt) {
			ppdu_cnt = rx_req->mdata.ppdu_cnt;
			ref_num += 1;
		}
		tmp_32bit = cpu_to_le32(ref_num);
		_rtw_memcpy(&hdr_buf[rt_len], &tmp_32bit, 4);
		rt_len += 4;

		/* u16 flags */
		tmp_16bit = 0;
		if (0) {
			tmp_16bit |= cpu_to_le16(IEEE80211_RADIOTAP_AMPDU_REPORT_ZEROLEN);
			tmp_16bit |= cpu_to_le16(IEEE80211_RADIOTAP_AMPDU_IS_ZEROLEN);
		}

		if (0) {
			tmp_16bit |= cpu_to_le16(IEEE80211_RADIOTAP_AMPDU_IS_LAST);
			tmp_16bit |= cpu_to_le16(IEEE80211_RADIOTAP_AMPDU_LAST_KNOWN);
		}

		if (0) {
			tmp_16bit |= cpu_to_le16(IEEE80211_RADIOTAP_AMPDU_DELIM_CRC_ERR);
			tmp_16bit |= cpu_to_le16(IEEE80211_RADIOTAP_AMPDU_DELIM_CRC_KNOWN);
		}

		if (rx_req->mdata.ampdu_end_pkt) {
			tmp_16bit |= cpu_to_le16(IEEE80211_RADIOTAP_AMPDU_EOF_KNOWN);
			tmp_16bit |= cpu_to_le16(IEEE80211_RADIOTAP_AMPDU_EOF);
		}

		_rtw_memcpy(&hdr_buf[rt_len], &tmp_16bit, 2);
		rt_len += 2;

		/* u8 delimiter CRC value, u8 reserved */
		rt_len += 2;
	}

	/* VHT, Required Alignment: 2 bytes */
	if (rx_req->mdata.rx_rate >= RTW_DATA_RATE_VHT_NSS1_MCS0 && rx_req->mdata.rx_rate <= RTW_DATA_RATE_VHT_NSS4_MCS9) {

		rtap_hdr->it_present |= BIT(IEEE80211_RADIOTAP_VHT);

		rt_len += (rt_len % 2); /* Alignment */

		/* Structure
		   u16 known, u8 flags, u8 bandwidth, u8 mcs_nss[4],
		   u8 coding, u8 group_id, u16 partial_aid */

		tmp_16bit = 0;

		/* STBC */
		tmp_16bit |= cpu_to_le16(IEEE80211_RADIOTAP_VHT_KNOWN_STBC);
		if (moif->u.snif_info.stbc) {
			hdr_buf[rt_len + 2] |= IEEE80211_RADIOTAP_VHT_FLAG_STBC;
		} else {
			hdr_buf[rt_len + 2] |= (a->stbc & 0x01);
		}

		/* TXOP_PS_NOT_ALLOWED */
		tmp_16bit |= cpu_to_le16(IEEE80211_RADIOTAP_VHT_KNOWN_TXOP_PS_NA);
		if (moif->u.snif_info.vht_txop_not_allow) {
			hdr_buf[rt_len + 2] |= IEEE80211_RADIOTAP_VHT_FLAG_TXOP_PS_NA;
		}


		/* Guard interval */
		tmp_16bit |= cpu_to_le16(IEEE80211_RADIOTAP_VHT_KNOWN_GI);
		if (moif->u.snif_info.sgi) {
			hdr_buf[rt_len + 2] |= IEEE80211_RADIOTAP_VHT_FLAG_SGI;
		} else {
			hdr_buf[rt_len + 2] |= ((a->sgi & 0x01) << 2);
		}

		/* Short GI NSYM */
		tmp_16bit |= cpu_to_le16(IEEE80211_RADIOTAP_VHT_KNOWN_SGI_NSYM_DIS);
		if (moif->u.snif_info.vht_nsym_dis) {
			hdr_buf[rt_len + 2] |= IEEE80211_RADIOTAP_VHT_FLAG_SGI_NSYM_M10_9;
		}

		/* LDPC extra OFDM symbol */
		tmp_16bit |= cpu_to_le16(IEEE80211_RADIOTAP_VHT_KNOWN_LDPC_EXTRA_OFDM_SYM);
		if (moif->u.snif_info.vht_ldpc_extra) {
			hdr_buf[rt_len + 2] |= IEEE80211_RADIOTAP_VHT_FLAG_LDPC_EXTRA_OFDM_SYM;
		} else {
			hdr_buf[rt_len + 2] |= ((a->ldpc & 0x01) << 4);
		}

		/* Short GI NSYM */
		tmp_16bit |= cpu_to_le16(IEEE80211_RADIOTAP_VHT_KNOWN_BEAMFORMED);
		if (moif->u.snif_info.vht_beamformed) {
			hdr_buf[rt_len + 2] |= IEEE80211_RADIOTAP_VHT_FLAG_BEAMFORMED;
		}

		/* know.Bandwidth */
		tmp_16bit |= cpu_to_le16(IEEE80211_RADIOTAP_VHT_KNOWN_BANDWIDTH);

		/* Group ID */
		tmp_16bit |= cpu_to_le16(IEEE80211_RADIOTAP_VHT_KNOWN_GROUP_ID);

		/* Partial AID */
		tmp_16bit |= cpu_to_le16(IEEE80211_RADIOTAP_VHT_KNOWN_PARTIAL_AID);

		_rtw_memcpy(&hdr_buf[rt_len], &tmp_16bit, 2);
		rt_len += 3;

		/* u8 bandwidth */
		if (moif->u.snif_info.ofdm_bw)
			tmp_8bit = moif->u.snif_info.ofdm_bw;
		else
			tmp_8bit = rx_req->mdata.bw;

		switch (tmp_8bit) {
		case CHANNEL_WIDTH_20:
			hdr_buf[rt_len] |= 0;
			break;
		case CHANNEL_WIDTH_40:
			hdr_buf[rt_len] |= 1;
			break;
		case CHANNEL_WIDTH_80:
			hdr_buf[rt_len] |= 4;
			break;
		case CHANNEL_WIDTH_160:
			hdr_buf[rt_len] |= 11;
			break;
		default:
			hdr_buf[rt_len] |= 0;
		}
		rt_len += 1;

		/* u8 mcs_nss[4] */
		if ((RTW_DATA_RATE_VHT_NSS1_MCS0 <= rx_req->mdata.rx_rate) &&
			(rx_req->mdata.rx_rate <= RTW_DATA_RATE_VHT_NSS4_MCS9)) {
			/* User 0 */
			/* MCS */
			hdr_buf[rt_len] = cpu_to_le16 (((rx_req->mdata.rx_rate - RTW_DATA_RATE_VHT_NSS1_MCS0) % 10) << 4);
			/* NSS */
			hdr_buf[rt_len] |= cpu_to_le16((((rx_req->mdata.rx_rate - RTW_DATA_RATE_VHT_NSS1_MCS0) / 10) + 1));
		}
		rt_len += 4;

		/* u8 coding, phystat? */
		hdr_buf[rt_len] = 0;
		rt_len += 1;

		/* u8 group_id */
		hdr_buf[rt_len] = moif->u.snif_info.vht_group_id;
		rt_len += 1;

		/* u16 partial_aid */
		tmp_16bit = cpu_to_le16(moif->u.snif_info.vht_nsts_aid);
		_rtw_memcpy(&hdr_buf[rt_len], &tmp_16bit, 2);
		rt_len += 2;
	}

	/* HE, Required Alignment: 2 bytes */
	if (rx_req->mdata.rx_rate >= RTW_DATA_RATE_HE_NSS1_MCS0 && rx_req->mdata.rx_rate <= RTW_DATA_RATE_HE_NSS4_MCS11) {

		struct ieee80211_radiotap_he he_hdr;
		u8 ltf, he_gi, ss;

		_rtw_memset(&he_hdr, 0, sizeof(struct ieee80211_radiotap_he));
		rtap_hdr->it_present |= BIT(IEEE80211_RADIOTAP_HE);

		rt_len += (rt_len % 2); /* Alignment */
		/* Structure
		   u16 data1, data2, data3, data4, data5, data6 */

		/* HE PPDU Format: 0=HE_SU, 1=HE_EXT_SU, 2=HE_MU, 3=HE_TRIG */
		he_hdr.data1 |= 0x0000;
		he_hdr.data1 |= cpu_to_le16(IEEE80211_RADIOTAP_HE_DATA1_DATA_MCS_KNOWN);
		he_hdr.data1 |= cpu_to_le16(IEEE80211_RADIOTAP_HE_DATA1_BW_RU_ALLOC_KNOWN);

		he_hdr.data2 |= cpu_to_le16(IEEE80211_RADIOTAP_HE_DATA2_GI_KNOWN);
		he_hdr.data2 |= cpu_to_le16(IEEE80211_RADIOTAP_HE_DATA2_NUM_LTF_SYMS_KNOWN);

		he_hdr.data3 |= __le16_encode_bits((rx_req->mdata.rx_rate & 0xf), IEEE80211_RADIOTAP_HE_DATA3_DATA_MCS);

		he_hdr.data5 |= cpu_to_le16((rx_req->mdata.bw));

		switch (rx_req->mdata.rx_gi_ltf) {
		case 0x0:
			he_gi = IEEE80211_RADIOTAP_HE_DATA5_GI_3_2;
			ltf = IEEE80211_RADIOTAP_HE_DATA5_LTF_SIZE_4X;
			break;
		case 0x1:
			he_gi = IEEE80211_RADIOTAP_HE_DATA5_GI_0_8;
			ltf = IEEE80211_RADIOTAP_HE_DATA5_LTF_SIZE_4X;
			break;
		case 0x2:
			he_gi = IEEE80211_RADIOTAP_HE_DATA5_GI_1_6;
			ltf = IEEE80211_RADIOTAP_HE_DATA5_LTF_SIZE_2X;
			break;
		case 0x3:
			he_gi = IEEE80211_RADIOTAP_HE_DATA5_GI_0_8;
			ltf = IEEE80211_RADIOTAP_HE_DATA5_LTF_SIZE_2X;
			break;
		case 0x4:
			he_gi = IEEE80211_RADIOTAP_HE_DATA5_GI_1_6;
			ltf = IEEE80211_RADIOTAP_HE_DATA5_LTF_SIZE_1X;
			break;
		case 0x5:
			he_gi = IEEE80211_RADIOTAP_HE_DATA5_GI_0_8;
			ltf = IEEE80211_RADIOTAP_HE_DATA5_LTF_SIZE_1X;
			break;
		default:
			he_gi = IEEE80211_RADIOTAP_HE_DATA5_GI_0_8;
			ltf = IEEE80211_RADIOTAP_HE_DATA5_LTF_SIZE_1X;
			break;
		}

		he_hdr.data5 |= __le16_encode_bits(he_gi, IEEE80211_RADIOTAP_HE_DATA5_GI);
		he_hdr.data5 |= __le16_encode_bits(ltf, IEEE80211_RADIOTAP_HE_DATA5_LTF_SIZE);

		ss = cpu_to_le16(((0x70 & rx_req->mdata.rx_rate) >> 4) + 1);
		he_hdr.data6 |= __le16_encode_bits(ss, IEEE80211_RADIOTAP_HE_DATA6_NSTS);

		_rtw_memcpy(&hdr_buf[rt_len], &he_hdr, sizeof(struct ieee80211_radiotap_he));

		rt_len += sizeof(struct ieee80211_radiotap_he);
	}

	/* frame timestamp, Required Alignment: 8 bytes */
	if (0) { //(a->free_cnt) {
		rtap_hdr->it_present |= BIT(IEEE80211_RADIOTAP_TIMESTAMP);
		if (!IS_ALIGNED(rt_len, 8))
			rt_len = ((rt_len + 7) & 0xFFF8); /* Alignment */

		/* u64 timestamp */
		tmp_64bit = cpu_to_le64(rx_req->mdata.freerun_cnt);
		_rtw_memcpy(&hdr_buf[rt_len], &tmp_64bit, 8);
		rt_len += 8;

		/* u16 accuracy */
		tmp_16bit = cpu_to_le16(22);
		_rtw_memcpy(&hdr_buf[rt_len], &tmp_16bit, 2);
		rt_len += 2;

		/* u8 unit/position */
		hdr_buf[rt_len] |= IEEE80211_RADIOTAP_TIMESTAMP_UNIT_US;
		rt_len += 1;

		/* u8 flags */
		hdr_buf[rt_len] |= IEEE80211_RADIOTAP_TIMESTAMP_FLAG_32BIT;
		hdr_buf[rt_len] |= IEEE80211_RADIOTAP_TIMESTAMP_FLAG_ACCURACY;
		rt_len += 1;
	}

	/* each antenna information */
#if 0
	if (rx_cnt > 1) {
		for (i = 0; i <= rx_cnt; i++) {
			/* dBm Antenna Signal */
			hdr_buf[rt_len] = a->phy_info.rx_mimo_signal_strength[i];
			rt_len += 1;

			/* Signal Quality */
			if (!IS_ALIGNED(rt_len, 2))
				rt_len++;
			hdr_buf[rt_len] = cpu_to_le16(a->phy_info.rx_mimo_signal_quality[i]);
			rt_len += 2;

			/* Antenna */
			hdr_buf[rt_len] = i; /* pHalData->rf_type; */
			rt_len += 1;
		}
	}
#endif

	/* push to skb */
	pskb = (struct sk_buff *)buf;
	if (skb_headroom(pskb) < rt_len) {
		RTW_INFO("%s:%d %s headroom is too small.\n", __FILE__, __LINE__, __func__);
		ret = _FAIL;
		return ret;
	}

	ptr = skb_push(pskb, rt_len);
	if (ptr) {
		rtap_hdr->it_len = cpu_to_le16(rt_len);
		rtap_hdr->it_present = cpu_to_le32(rtap_hdr->it_present);
		memcpy(ptr, rtap_hdr, rt_len);
	} else
		ret = _FAIL;

	return ret;

}

void rx_query_moinfo(struct rx_pkt_attrib *a, u8 *desc)
{
	switch (a->drvinfo_sz) {
	case 40:
		_rtw_memcpy(a->moif, &desc[32], 8);
		break;
	case 48:
		_rtw_memcpy(a->moif, &desc[32], 12);
		break;
	case 32:
		/* passthrough */
	default:
		break;
	}
}

#endif /* CONFIG_WIFI_MONITOR */
