/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
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
#define _RTW_VHT_C

#include <drv_types.h>

#ifdef CONFIG_80211AC_VHT
const u16 _vht_max_mpdu_len[] = {
	3895,
	7991,
	11454,
	0,
};

const u8 _vht_sup_ch_width_set_to_bw_cap[] = {
	BW_CAP_80M,
	BW_CAP_80M | BW_CAP_160M,
	BW_CAP_80M | BW_CAP_160M | BW_CAP_80_80M,
	0,
};

const char *const _vht_sup_ch_width_set_str[] = {
	"80MHz",
	"160MHz",
	"160MHz & 80+80MHz",
	"BW-RSVD",
};

void dump_vht_cap_ie_content(void *sel, const u8 *buf, u32 buf_len)
{
	if (buf_len != VHT_CAP_IE_LEN) {
		RTW_PRINT_SEL(sel, "Invalid VHT capability IE len:%d != %d\n", buf_len, VHT_CAP_IE_LEN);
		return;
	}

	RTW_PRINT_SEL(sel, "cap_info:%02x %02x %02x %02x: MAX_MPDU_LEN:%u %s%s%s%s%s RX-STBC:%u MAX_AMPDU_LEN:%u\n"
		, *(buf), *(buf + 1), *(buf + 2), *(buf + 3)
		, vht_max_mpdu_len(GET_VHT_CAPABILITY_ELE_MAX_MPDU_LENGTH(buf))
		, vht_sup_ch_width_set_str(GET_VHT_CAPABILITY_ELE_CHL_WIDTH(buf))
		, GET_VHT_CAPABILITY_ELE_RX_LDPC(buf) ? " RX-LDPC" : ""
		, GET_VHT_CAPABILITY_ELE_SHORT_GI80M(buf) ? " SGI-80" : ""
		, GET_VHT_CAPABILITY_ELE_SHORT_GI160M(buf) ? " SGI-160" : ""
		, GET_VHT_CAPABILITY_ELE_TX_STBC(buf) ? " TX-STBC" : ""
		, GET_VHT_CAPABILITY_ELE_RX_STBC(buf)
		, VHT_MAX_AMPDU_LEN(GET_VHT_CAPABILITY_ELE_MAX_RXAMPDU_FACTOR(buf))
	);
}

void dump_vht_cap_ie(void *sel, const u8 *ie, u32 ie_len)
{
	const u8 *vht_cap_ie;
	sint vht_cap_ielen;

	vht_cap_ie = rtw_get_ie(ie, WLAN_EID_VHT_CAPABILITY, &vht_cap_ielen, ie_len);
	if (!ie || vht_cap_ie != ie)
		return;

	dump_vht_cap_ie_content(sel, vht_cap_ie + 2, vht_cap_ielen);
}

const char *const _vht_op_ch_width_str[] = {
	"20 or 40MHz",
	"80MHz",
	"160MHz",
	"80+80MHz",
	"BW-RSVD",
};

void dump_vht_op_ie_content(void *sel, const u8 *buf, u32 buf_len)
{
	if (buf_len != VHT_OP_IE_LEN) {
		RTW_PRINT_SEL(sel, "Invalid VHT operation IE len:%d != %d\n", buf_len, VHT_OP_IE_LEN);
		return;
	}

	RTW_PRINT_SEL(sel, "\tVHT Operation Info:\n");
	RTW_PRINT_SEL(sel, "\tChannel Width: %u\n"
					, GET_VHT_OPERATION_ELE_CHL_WIDTH(buf));
	RTW_PRINT_SEL(sel, "\tCenter Frequency Channel for 80 and 160 MHz Operation: %u\n"
					, GET_VHT_OPERATION_ELE_CENTER_FREQ1(buf));
	RTW_PRINT_SEL(sel, "\tCenter Frequency Channel for 80+80 MHz Operation: %u\n"
					, GET_VHT_OPERATION_ELE_CENTER_FREQ2(buf));
	RTW_PRINT_SEL(sel, "\tVHT Basic MCS Set: 0x%04x\n"
					, GET_VHT_OPERATION_ELE_BASIC_MCS_SET(buf));
}

void dump_vht_op_ie(void *sel, const u8 *ie, u32 ie_len)
{
	const u8 *vht_op_ie;
	sint vht_op_ielen;

	vht_op_ie = rtw_get_ie(ie, WLAN_EID_VHT_OPERATION, &vht_op_ielen, ie_len);
	if (!ie || vht_op_ie != ie)
		return;

	dump_vht_op_ie_content(sel, vht_op_ie + 2, vht_op_ielen);
}

/*				20/40/80/160,	ShortGI,	MCS Rate  */
const u16 VHT_MCS_DATA_RATE[CHANNEL_WIDTH_MAX][2][30] = {
	{	{
			13, 26, 39, 52, 78, 104, 117, 130, 156, 156,
			26, 52, 78, 104, 156, 208, 234, 260, 312, 312,
			39, 78, 117, 156, 234, 312, 351, 390, 468, 520
		},			/* Long GI, 20MHz */
		{
			14, 29, 43, 58, 87, 116, 130, 144, 173, 173,
			29, 58, 87, 116, 173, 231, 260, 289, 347, 347,
			43,	87, 130, 173, 260, 347, 390,	433,	520, 578
		}
	},		/* Short GI, 20MHz */
	{	{
			27, 54, 81, 108, 162, 216, 243, 270, 324, 360,
			54, 108, 162, 216, 324, 432, 486, 540, 648, 720,
			81, 162, 243, 324, 486, 648, 729, 810, 972, 1080
		}, 		/* Long GI, 40MHz */
		{
			30, 60, 90, 120, 180, 240, 270, 300, 360, 400,
			60, 120, 180, 240, 360, 480, 540, 600, 720, 800,
			90, 180, 270, 360, 540, 720, 810, 900, 1080, 1200
		}
	},		/* Short GI, 40MHz */
	{	{
			59, 117,  176, 234, 351, 468, 527, 585, 702, 780,
			117, 234, 351, 468, 702, 936, 1053, 1170, 1404, 1560,
			176, 351, 527, 702, 1053, 1404, 1580, 1755, 2106, 2340
		},	/* Long GI, 80MHz */
		{
			65, 130, 195, 260, 390, 520, 585, 650, 780, 867,
			130, 260, 390, 520, 780, 1040, 1170, 1300, 1560, 1734,
			195, 390, 585, 780, 1170, 1560, 1755, 1950, 2340, 2600
		}
	},	/* Short GI, 80MHz */
	{	{
			117, 234, 351, 468, 702, 936, 1053, 1170, 1404, 1560,
			234, 468, 702, 936, 1404, 1872, 2106, 2340, 2808, 3120,
			351, 702, 1053, 1404, 2106, 2808, 3159, 3510, 4212, 4212
		},	/* Long GI, 160MHz */
		{
			130, 260, 390, 520, 780, 1040, 1170, 1300, 1560, 1734,
			260, 520, 780, 1040, 1560, 2080, 2340, 2600, 3120, 3466,
			390, 780, 1170, 1560, 2340, 3120, 3510, 3900, 4680, 4680
		}
	}	/* Short GI, 160MHz */
};

u8	rtw_get_vht_highest_rate(u8 *pvht_mcs_map)
{
	u8	i, j;
	u8	bit_map;
	u8	vht_mcs_rate = 0;

	for (i = 0; i < 2; i++) {
		if (pvht_mcs_map[i] != 0xff) {
			for (j = 0; j < 8; j += 2) {
				bit_map = (pvht_mcs_map[i] >> j) & 3;

				if (bit_map != 3)
					vht_mcs_rate = MGN_VHT1SS_MCS7 + 10 * j / 2 + i * 40 + bit_map; /* VHT rate indications begin from 0x90 */
			}
		}
	}

	/* RTW_INFO("HighestVHTMCSRate is %x\n", vht_mcs_rate); */
	return vht_mcs_rate;
}

u8	rtw_vht_mcsmap_to_nss(u8 *pvht_mcs_map)
{
	u8	i, j;
	u8	bit_map;
	u8	nss = 0;

	for (i = 0; i < 2; i++) {
		if (pvht_mcs_map[i] != 0xff) {
			for (j = 0; j < 8; j += 2) {
				bit_map = (pvht_mcs_map[i] >> j) & 3;

				if (bit_map != 3)
					nss++;
			}
		}
	}

	/* RTW_INFO("%s : %dSS\n", __FUNCTION__, nss); */
	return nss;
}

void rtw_vht_nss_to_mcsmap(u8 nss, u8 *target_mcs_map, u8 *cur_mcs_map)
{
	u8	i, j;
	u8	cur_rate, target_rate;

	for (i = 0; i < 2; i++) {
		target_mcs_map[i] = 0;
		for (j = 0; j < 8; j += 2) {
			cur_rate = (cur_mcs_map[i] >> j) & 3;
			if (cur_rate == 3) /* 0x3 indicates not supported that num of SS */
				target_rate = 3;
			else if (nss <= ((j / 2) + i * 4))
				target_rate = 3;
			else
				target_rate = cur_rate;

			target_mcs_map[i] |= (target_rate << j);
		}
	}

	/* RTW_INFO("%s : %dSS\n", __FUNCTION__, nss); */
}

u16	rtw_vht_mcs_to_data_rate(u8 bw, u8 short_GI, u8 vht_mcs_rate)
{
	if (vht_mcs_rate > MGN_VHT3SS_MCS9)
		vht_mcs_rate = MGN_VHT3SS_MCS9;
	/* RTW_INFO("bw=%d, short_GI=%d, ((vht_mcs_rate - MGN_VHT1SS_MCS0)&0x3f)=%d\n", bw, short_GI, ((vht_mcs_rate - MGN_VHT1SS_MCS0)&0x3f)); */
	return VHT_MCS_DATA_RATE[bw][short_GI][((vht_mcs_rate - MGN_VHT1SS_MCS0) & 0x3f)];
}

u32	rtw_get_dft_vht_cap_ie(_adapter *padapter, u8 *pbuf)
{
	u8 bw;
	u16 HighestRate;
	u8 *pcap, *pcap_mcs;
	u32 len = 0;
	struct registry_priv *pregistrypriv = &padapter->registrypriv;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct vht_priv *pvhtpriv = &pmlmepriv->dev_vhtpriv;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

	pcap = pvhtpriv->vht_cap;
	_rtw_memset(pcap, 0, 32);

	/*
	* VHT Capabilities Information field : B0 to B31
	*/

	/* B0 B1 Maximum MPDU Length */
	SET_VHT_CAPABILITY_ELE_MAX_MPDU_LENGTH(pcap, pvhtpriv->max_mpdu_len);

	/* B2 B3 Supported Channel Width Set */
	if (rtw_hw_chk_bw_cap(adapter_to_dvobj(padapter), BW_CAP_160M) && REGSTY_IS_BW_5G_SUPPORT(pregistrypriv, CHANNEL_WIDTH_160)) {
		if (rtw_hw_chk_bw_cap(adapter_to_dvobj(padapter), BW_CAP_80_80M) && REGSTY_IS_BW_5G_SUPPORT(pregistrypriv, CHANNEL_WIDTH_80_80)) {
			SET_VHT_CAPABILITY_ELE_CHL_WIDTH(pcap, 2);
			RTW_INFO("[VHT] Declare supporting 160MHz and 80+80MHz\n");
		} else {
			SET_VHT_CAPABILITY_ELE_CHL_WIDTH(pcap, 1);
			RTW_INFO("[VHT] Declare supporting 160MHz\n");
		}
	} else
		SET_VHT_CAPABILITY_ELE_CHL_WIDTH(pcap, 0);

	/* B4 Rx LDPC */
	if (TEST_FLAG(pvhtpriv->ldpc_cap, LDPC_VHT_ENABLE_RX)) {
		SET_VHT_CAPABILITY_ELE_RX_LDPC(pcap, 1);
		RTW_INFO("[VHT] Declare supporting RX LDPC\n");
	}

	/* B5 ShortGI for 80MHz */
	SET_VHT_CAPABILITY_ELE_SHORT_GI80M(pcap, pvhtpriv->sgi_80m ? 1 : 0);
	if (pvhtpriv->sgi_80m)
		RTW_INFO("[VHT] Declare supporting SGI 80MHz\n");

	/* B6 Short GI for 160 and 80+80 MHz */
	SET_VHT_CAPABILITY_ELE_SHORT_GI160M(pcap, pvhtpriv->sgi_160m ? 1 : 0);
	if (pvhtpriv->sgi_160m) {
		RTW_INFO("[VHT] Declare supporting SGI 160MHz and 80+80MHz\n");
	}

	/* B7 Tx STBC */
	if (TEST_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_TX)) {
		SET_VHT_CAPABILITY_ELE_TX_STBC(pcap, 1);
		RTW_INFO("[VHT] Declare supporting TX STBC\n");
	}

	/* B8 B9 B10 Rx STBC */
	if (TEST_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_RX)) {
		SET_VHT_CAPABILITY_ELE_RX_STBC(pcap, pvhtpriv->rx_stbc_nss);
		RTW_INFO("[VHT] Declare supporting RX STBC = %d\n", pvhtpriv->rx_stbc_nss);
	}

	#ifdef CONFIG_BEAMFORMING
	/* B11 SU Beamformer Capable */
	if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE)) {
		SET_VHT_CAPABILITY_ELE_SU_BFER(pcap, 1);
		RTW_INFO("[VHT] Declare supporting SU Beamformer\n");
		/* B16 17 18 Number of Sounding Dimensions */
		SET_VHT_CAPABILITY_ELE_SOUNDING_DIMENSIONS(pcap, pvhtpriv->num_snd_dim);
		/* B19 MU Beamformer Capable */
		if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_AP_ENABLE)) {
			SET_VHT_CAPABILITY_ELE_MU_BFER(pcap, 1);
			RTW_INFO("[VHT] Declare supporting MU Beamformer\n");
		}
	}

	/* B12 SU Beamformee Capable */
	if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE)) {
		u8 bfme_sts = pvhtpriv->bfme_sts;

		SET_VHT_CAPABILITY_ELE_SU_BFEE(pcap, 1);
		RTW_INFO("[VHT] Declare supporting SU Beamformee\n");

		/* IOT action suggested by Yu Chen 2017/3/3 */
		if ((pmlmeinfo->assoc_AP_vendor == HT_IOT_PEER_BROADCOM) &&
			!pvhtpriv->ap_bf_cap.is_mu_bfer &&
			pvhtpriv->ap_bf_cap.su_sound_dim == 2)

		/* B13 14 15 Beamformee STS Capability */
		SET_VHT_CAPABILITY_ELE_SU_BFEE_STS_CAP(pcap, bfme_sts);

		/* B20 MU Beamformee Capable */
		if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_STA_ENABLE)) {
			SET_VHT_CAPABILITY_ELE_MU_BFEE(pcap, 1);
			RTW_INFO("[VHT] Declare supporting MU Beamformee\n");
		}
	}
	#endif/*CONFIG_BEAMFORMING*/

	/* B21 VHT TXOP PS */
	if (pvhtpriv->txop_ps) {
		SET_VHT_CAPABILITY_ELE_TXOP_PS(pcap, 1);
		RTW_INFO("[VHT] Declare supporting VHT TXOP power save\n");
	}

	/* B22 +HTC-VHT Capable */
	if (pvhtpriv->htc_vht) {
		SET_VHT_CAPABILITY_ELE_HTC_VHT(pcap, 1);
		RTW_INFO("[VHT] Declare supporting VHT variant HT Control\n");
	}

	/* B23 24 25 Maximum A-MPDU Length Exponent */
	SET_VHT_CAPABILITY_ELE_MAX_RXAMPDU_FACTOR(pcap, pvhtpriv->ampdu_len);
	RTW_INFO("[VHT] Declare supporting RX A-MPDU Length Exponent = %u\n", pvhtpriv->ampdu_len);

	/* B26 27 VHT Link Adaptation Capable */
	if (pvhtpriv->link_adap_cap) {
		SET_VHT_CAPABILITY_ELE_LINK_ADAPTION(pcap, 1);
		RTW_INFO("[VHT] Declare supporting link adaptation using VHT variant HT Control\n");
	}

	/* B28 Rx Antenna Pattern Consistency */
	if (pvhtpriv->rx_ant_pattern) {
		SET_VHT_CAPABILITY_ELE_RX_ANT_PATTERN(pcap, 1);
		RTW_INFO("[VHT] Declare supporting RX Antenna Pattern Consistency\n");
	}

	/* B29 Tx Antenna Pattern Consistency */
	if (pvhtpriv->tx_ant_pattern) {
		SET_VHT_CAPABILITY_ELE_TX_ANT_PATTERN(pcap, 1);
		RTW_INFO("[VHT] Declare supporting TX Antenna Pattern Consistency\n");
	}

	/* B30 B31 Extended NSS BW Support */
	SET_VHT_CAPABILITY_ELE_EXT_NSS_BW(pcap, pvhtpriv->ext_nss_bw);

	/*
	* Supported VHT-MCS and NSS Set : 8 bytes
	*/
	pcap_mcs = GET_VHT_CAPABILITY_ELE_RX_MCS(pcap);
	_rtw_memcpy(pcap_mcs, pvhtpriv->vht_mcs_map, 2);

	pcap_mcs = GET_VHT_CAPABILITY_ELE_TX_MCS(pcap);
	_rtw_memcpy(pcap_mcs, pvhtpriv->vht_mcs_map, 2);

	/* find the largest bw supported by both registry and hal */
	bw = rtw_hw_largest_bw(adapter_to_dvobj(padapter), REGSTY_BW_5G(pregistrypriv));

	HighestRate = VHT_MCS_DATA_RATE[bw][pvhtpriv->sgi_80m][((pvhtpriv->vht_highest_rate - MGN_VHT1SS_MCS0) & 0x3f)];
	HighestRate = (HighestRate + 1) >> 1;

	SET_VHT_CAPABILITY_ELE_MCS_RX_HIGHEST_RATE(pcap, HighestRate); /* indicate we support highest rx rate is 600Mbps. */
	SET_VHT_CAPABILITY_ELE_MCS_TX_HIGHEST_RATE(pcap, HighestRate); /* indicate we support highest tx rate is 600Mbps. */
	if (rtw_hw_chk_bw_cap(adapter_to_dvobj(padapter), BW_CAP_160M) && REGSTY_IS_BW_5G_SUPPORT(pregistrypriv, CHANNEL_WIDTH_160))
		SET_VHT_CAPABILITY_ELE_MCS_TX_EXT_NSS_BW_CAP(pcap, 1); /*160M*/

	pbuf = rtw_set_ie(pbuf, EID_VHTCapability, 12, pcap, &len);

	return len;
}

/* Initialized vhtpriv by PHL default setting */
void rtw_vht_get_dft_setting(_adapter *padapter,
			struct protocol_cap_t *dft_proto_cap,
			struct role_link_cap_t *dft_cap)
{
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct vht_priv *pvhtpriv = &pmlmepriv->dev_vhtpriv;
	struct registry_priv *pregistrypriv = &padapter->registrypriv;
	BOOLEAN bHwLDPCSupport = _FALSE, bHwSTBCSupport = _FALSE;
	u8 stbc_rx = 0;
#ifdef CONFIG_BEAMFORMING
	BOOLEAN	bHwSupportBeamformer = _FALSE, bHwSupportBeamformee = _FALSE;
	u8 mu_bfer, mu_bfee;
#endif /* CONFIG_BEAMFORMING */
	u8 tx_nss, rx_nss;
	struct mlme_ext_priv *pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

	/* Short GI */
	pvhtpriv->sgi_80m = ((dft_proto_cap->sgi_80)
		&& rtw_hw_chk_bw_cap(adapter_to_dvobj(padapter), BW_CAP_80M)
		&& TEST_FLAG(pregistrypriv->short_gi, BIT2));

	pvhtpriv->sgi_160m = ((dft_proto_cap->sgi_160)
		&& rtw_hw_chk_bw_cap(adapter_to_dvobj(padapter), BW_CAP_160M)
		&& TEST_FLAG(pregistrypriv->short_gi, BIT3));

	/* LDPC */
	bHwLDPCSupport = (dft_proto_cap->vht_ldpc) ? _TRUE : _FALSE;
	if (bHwLDPCSupport) {
		if (TEST_FLAG(pregistrypriv->ldpc_cap, BIT0))
			SET_FLAG(pvhtpriv->ldpc_cap, LDPC_VHT_ENABLE_RX);
	}

	bHwLDPCSupport = (dft_cap->tx_vht_ldpc) ? _TRUE : _FALSE;
	if (bHwLDPCSupport) {
		if (TEST_FLAG(pregistrypriv->ldpc_cap, BIT1))
			SET_FLAG(pvhtpriv->ldpc_cap, LDPC_VHT_ENABLE_TX);
	}

	if (pvhtpriv->ldpc_cap)
		RTW_INFO("[VHT] Support LDPC = 0x%02X\n", pvhtpriv->ldpc_cap);

	/* STBC */
	if (dft_proto_cap->stbc_vht_tx)
		SET_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_TX);

	if (dft_proto_cap->stbc_vht_rx) {
		SET_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_RX);
		pvhtpriv->rx_stbc_nss = dft_proto_cap->stbc_vht_rx;
	}

	if (pvhtpriv->stbc_cap)
		RTW_INFO("[VHT] Support STBC = 0x%02X\n", pvhtpriv->stbc_cap);

	/* Beamforming setting */
#ifdef CONFIG_BEAMFORMING
	/* only enable beamforming in STA client mode */
	if (MLME_IS_STA(padapter) && !MLME_IS_GC(padapter)
				  && !MLME_IS_ADHOC(padapter)
				  && !MLME_IS_MESH(padapter))
	{

		bHwSupportBeamformer = (dft_proto_cap->vht_su_bfmr) ? _TRUE : _FALSE;
		bHwSupportBeamformee = (dft_proto_cap->vht_su_bfme) ? _TRUE : _FALSE;

		mu_bfer = (dft_proto_cap->vht_mu_bfmr) ? _TRUE : _FALSE;
		mu_bfee = (dft_proto_cap->vht_mu_bfme) ? _TRUE : _FALSE;

		if (TEST_FLAG(pregistrypriv->beamform_cap, BIT0) && bHwSupportBeamformer) {
#ifdef CONFIG_CONCURRENT_MODE
			if ((pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) {
				SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE);
				RTW_INFO("[VHT] CONCURRENT AP Support Beamformer\n");
				if (TEST_FLAG(pregistrypriv->beamform_cap, BIT(2))
				    && (_TRUE == mu_bfer)) {
					SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_AP_ENABLE);
					RTW_INFO("[VHT] Support MU-MIMO AP\n");
				}
				pvhtpriv->num_snd_dim = dft_proto_cap->num_snd_dim;
			} else
				RTW_INFO("[VHT] CONCURRENT not AP ;not allow  Support Beamformer\n");
#else
			SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE);
			RTW_INFO("[VHT] Support Beamformer\n");
			if (TEST_FLAG(pregistrypriv->beamform_cap, BIT(2))
			    && (_TRUE == mu_bfer)
			    && ((pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE)) {
				SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_AP_ENABLE);
				RTW_INFO("[VHT] Support MU-MIMO AP\n");
			}
#endif
		}
		if (TEST_FLAG(pregistrypriv->beamform_cap, BIT1) && bHwSupportBeamformee) {
			SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE);
			RTW_INFO("[VHT] Support Beamformee\n");
			pvhtpriv->bfme_sts = dft_proto_cap->bfme_sts;
			if (TEST_FLAG(pregistrypriv->beamform_cap, BIT(3))
			    && (_TRUE == mu_bfee)
			    && ((pmlmeinfo->state & 0x03) != WIFI_FW_AP_STATE)) {
				SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_STA_ENABLE);
				RTW_INFO("[VHT] Support MU-MIMO STA\n");
			}
		}
	}
#endif /* CONFIG_BEAMFORMING */

	/* We don't support these features, and PHL doesn't define */
	pvhtpriv->txop_ps = 0;
	pvhtpriv->htc_vht = 1;
	pvhtpriv->link_adap_cap = 0;
	pvhtpriv->tx_ant_pattern= 0;
	pvhtpriv->rx_ant_pattern= 0;
	pvhtpriv->ext_nss_bw = 0;

	pvhtpriv->ampdu_len = pregistrypriv->ampdu_factor;
	pvhtpriv->max_mpdu_len = dft_proto_cap->max_amsdu_len;

	tx_nss = GET_PHY_TX_NSS_BY_BAND(padapter, HW_BAND_0);
	rx_nss = GET_PHY_RX_NSS_BY_BAND(padapter, HW_BAND_0);

	/* for now, vhtpriv.vht_mcs_map comes from RX NSS */
	rtw_vht_nss_to_mcsmap(rx_nss, pvhtpriv->vht_mcs_map, pregistrypriv->vht_rx_mcs_map);
	pvhtpriv->vht_highest_rate = rtw_get_vht_highest_rate(pvhtpriv->vht_mcs_map);
}

/* Initialized vhtpriv by adapter real setting */
void rtw_vht_get_real_setting(_adapter *padapter, struct _ADAPTER_LINK *padapter_link)
{
#ifdef CONFIG_BEAMFORMING
	BOOLEAN bHwSupportBeamformer = _FALSE, bHwSupportBeamformee = _FALSE;
	u8 mu_bfer, mu_bfee;
#endif /* CONFIG_BEAMFORMING */
	u8 tx_nss, rx_nss;
	u8 rf_type = 0;
	struct rtw_wifi_role_t *wrole = padapter->phl_role;
	struct registry_priv *pregistrypriv = &(padapter->registrypriv);
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv *pmlmeext = &(padapter_link->mlmeextpriv);
	struct protocol_cap_t *proto_cap = &(padapter_link->wrlink->protocol_cap);
	struct role_link_cap_t *cap = &(padapter_link->wrlink->cap);
	struct vht_priv *pvhtpriv = &pmlmepriv->vhtpriv;

	/* Short GI */
	pvhtpriv->sgi_80m = ((proto_cap->sgi_80)
		&& rtw_hw_chk_bw_cap(adapter_to_dvobj(padapter), BW_CAP_80M)
		&& TEST_FLAG(pregistrypriv->short_gi, BIT2));

	pvhtpriv->sgi_160m = ((proto_cap->sgi_160)
		&& rtw_hw_chk_bw_cap(adapter_to_dvobj(padapter), BW_CAP_160M)
		&& TEST_FLAG(pregistrypriv->short_gi, BIT3));

	/* LDPC support */
	if (proto_cap->vht_ldpc)
		SET_FLAG(pvhtpriv->ldpc_cap, LDPC_VHT_ENABLE_RX);

	if (cap->tx_vht_ldpc)
		SET_FLAG(pvhtpriv->ldpc_cap, LDPC_VHT_ENABLE_TX);

	if (pvhtpriv->ldpc_cap)
		RTW_INFO("[VHT] Support LDPC = 0x%02X\n", pvhtpriv->ldpc_cap);

	/* STBC */
	if (proto_cap->stbc_vht_tx)
		SET_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_TX);

	if (proto_cap->stbc_vht_rx) {
		SET_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_RX);
		pvhtpriv->rx_stbc_nss = proto_cap->stbc_vht_rx;
	}

	if (pvhtpriv->stbc_cap)
		RTW_INFO("[VHT] Support STBC = 0x%02X\n", pvhtpriv->stbc_cap);

	/* Beamforming setting */
	CLEAR_FLAGS(pvhtpriv->beamform_cap);
#ifdef CONFIG_BEAMFORMING
	if (proto_cap->vht_su_bfmr) {
		SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE);
		RTW_INFO("[VHT] HAL Support Beamformer\n");
		if (proto_cap->vht_mu_bfmr) {
			SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_AP_ENABLE);
			RTW_INFO("[VHT] Support MU-MIMO AP\n");
		}
	}

	if (proto_cap->vht_su_bfme) {
		u8 bfme_sts = 0;

		rtw_hal_get_def_var(padapter, padapter_link, HAL_DEF_BEAMFORMEE_CAP, (u8 *)&bfme_sts);
		pvhtpriv->bfme_sts = bfme_sts;
		SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE);
		RTW_INFO("[VHT] HAL Support Beamformee\n");
		if (proto_cap->vht_mu_bfme) {
			SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_STA_ENABLE);
			RTW_INFO("[VHT] Support MU-MIMO STA\n");
		}
	}
#endif

/* ToDo: check bfee flow will move change role type and modify wifi_role->protocol_cap */
#if 0
#ifdef CONFIG_BEAMFORMING
	/* only enable beamforming in STA client mode */
	if (MLME_IS_STA(padapter) && !MLME_IS_GC(padapter)
				  && !MLME_IS_ADHOC(padapter)
				  && !MLME_IS_MESH(padapter))
	{
		rtw_hal_get_def_var(padapter, HAL_DEF_EXPLICIT_BEAMFORMER,
			(u8 *)&bHwSupportBeamformer);
		rtw_hal_get_def_var(padapter, HAL_DEF_EXPLICIT_BEAMFORMEE,
			(u8 *)&bHwSupportBeamformee);
		mu_bfer = _FALSE;
		mu_bfee = _FALSE;
		rtw_hal_get_def_var(padapter, HAL_DEF_VHT_MU_BEAMFORMER, &mu_bfer);
		rtw_hal_get_def_var(padapter, HAL_DEF_VHT_MU_BEAMFORMEE, &mu_bfee);
		if (TEST_FLAG(pregistrypriv->beamform_cap, BIT0) && bHwSupportBeamformer) {
#ifdef CONFIG_CONCURRENT_MODE
			if ((pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE) {
				u8 num_snd_dim = 0;

				SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE);
				RTW_INFO("[VHT] CONCURRENT AP Support Beamformer\n");
				if (TEST_FLAG(pregistrypriv->beamform_cap, BIT(2))
				    && (_TRUE == mu_bfer)) {
					SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_AP_ENABLE);
					RTW_INFO("[VHT] Support MU-MIMO AP\n");
				}
				rtw_hal_get_def_var(padapter, HAL_DEF_BEAMFORMER_CAP, (u8 *)&num_snd_dim);
				pvhtpriv->num_snd_dim = num_snd_dim;
			} else
				RTW_INFO("[VHT] CONCURRENT not AP ;not allow  Support Beamformer\n");
#else
			SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE);
			RTW_INFO("[VHT] Support Beamformer\n");
			if (TEST_FLAG(pregistrypriv->beamform_cap, BIT(2))
			    && (_TRUE == mu_bfer)
			    && ((pmlmeinfo->state & 0x03) == WIFI_FW_AP_STATE)) {
				SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_AP_ENABLE);
				RTW_INFO("[VHT] Support MU-MIMO AP\n");
			}
#endif
		}
		if (TEST_FLAG(pregistrypriv->beamform_cap, BIT1) && bHwSupportBeamformee) {
			u8 bfme_sts = 0;

			SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE);
			RTW_INFO("[VHT] Support Beamformee\n");
			rtw_hal_get_def_var(padapter, HAL_DEF_BEAMFORMEE_CAP, (u8 *)&bfme_sts);
			pvhtpriv->bfme_sts = bfme_sts;
			if (TEST_FLAG(pregistrypriv->beamform_cap, BIT(3))
			    && (_TRUE == mu_bfee)
			    && ((pmlmeinfo->state & 0x03) != WIFI_FW_AP_STATE)) {
				SET_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_STA_ENABLE);
				RTW_INFO("[VHT] Support MU-MIMO STA\n");
			}
		}
	}
#endif /* CONFIG_BEAMFORMING */
#endif

	/* We don't support these features, and PHL doesn't define */
	pvhtpriv->txop_ps = 0;
	pvhtpriv->htc_vht = 1;
	pvhtpriv->link_adap_cap = 0;
	pvhtpriv->tx_ant_pattern= 0;
	pvhtpriv->rx_ant_pattern= 0;
	pvhtpriv->ext_nss_bw = 0;

	pvhtpriv->ampdu_len = pregistrypriv->ampdu_factor;
	pvhtpriv->max_mpdu_len = proto_cap->max_amsdu_len;

	tx_nss = get_phy_tx_nss(padapter, padapter_link);
	rx_nss = get_phy_rx_nss(padapter, padapter_link);

	/* for now, vhtpriv.vht_mcs_map comes from RX NSS */
	rtw_vht_nss_to_mcsmap(rx_nss, pvhtpriv->vht_mcs_map, pregistrypriv->vht_rx_mcs_map);
	pvhtpriv->vht_highest_rate = rtw_get_vht_highest_rate(pvhtpriv->vht_mcs_map);
}

u64	rtw_vht_mcs_map_to_bitmap(u8 *mcs_map, u8 nss)
{
	u8 i, j, tmp;
	u64 bitmap = 0;
	u8 bits_nss = nss * 2;

	for (i = j = 0; i < bits_nss; i += 2, j += 10) {
		/* every two bits means single sptial stream */
		tmp = (mcs_map[i / 8] >> i) & 3;

		switch (tmp) {
		case 2:
			bitmap = bitmap | (0x03ff << j);
			break;
		case 1:
			bitmap = bitmap | (0x01ff << j);
			break;
		case 0:
			bitmap = bitmap | (0x00ff << j);
			break;
		default:
			break;
		}
	}

	RTW_INFO("vht_mcs_map=%02x %02x, nss=%u => bitmap=%016llx\n"
		, mcs_map[0], mcs_map[1], nss, bitmap);

	return bitmap;
}

#ifdef CONFIG_BEAMFORMING
void update_sta_vht_info_apmode_bf_cap(_adapter *padapter, struct sta_info *psta)
{
	struct _ADAPTER_LINK *padapter_link = psta->padapter_link;
	struct link_mlme_priv	*pmlmepriv = &(padapter_link->mlmepriv);
	struct vht_priv	*pvhtpriv_ap = &pmlmepriv->vhtpriv;
	struct vht_priv	*pvhtpriv_sta = &psta->vhtpriv;
	u16	cur_beamform_cap = 0;

	/* B11 SU Beamformer Capable, the target supports Beamformer and we are Beamformee */
	if (TEST_FLAG(pvhtpriv_ap->beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE) &&
	    GET_VHT_CAPABILITY_ELE_SU_BFEE(pvhtpriv_sta->vht_cap)) {
		SET_FLAG(cur_beamform_cap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE);
		/*Shift to BEAMFORMING_VHT_BEAMFORMER_STS_CAP*/
		SET_FLAG(cur_beamform_cap, GET_VHT_CAPABILITY_ELE_SU_BFEE_STS_CAP(pvhtpriv_sta->vht_cap) << 8);
	}

	/* B12 SU Beamformee Capable, the target supports Beamformee and we are Beamformer */
	if (TEST_FLAG(pvhtpriv_ap->beamform_cap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE) &&
	    GET_VHT_CAPABILITY_ELE_SU_BFER(pvhtpriv_sta->vht_cap)) {
		SET_FLAG(cur_beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE);
		/*Shit to BEAMFORMING_VHT_BEAMFORMEE_SOUND_DIM*/
		SET_FLAG(cur_beamform_cap, GET_VHT_CAPABILITY_ELE_SU_BFER_SOUND_DIM_NUM(pvhtpriv_sta->vht_cap) << 12);
	}

	if (cur_beamform_cap)
		RTW_INFO("Current STA(%d) VHT Beamforming Setting = %02X\n", psta->phl_sta->aid, cur_beamform_cap);

	pvhtpriv_sta->beamform_cap = cur_beamform_cap;

	/* ToDo: need to API to inform hal_sta->bf_info.vht_beamform_cap  */
	#if 0
	psta->phl_sta->bf_info.vht_beamform_cap = cur_beamform_cap;
	#endif
}
#endif

void update_sta_vht_info_apmode(_adapter *padapter, void *sta)
{
	struct sta_info	*psta = (struct sta_info *)sta;
	struct rtw_phl_stainfo_t *phl_sta = psta->phl_sta;
	struct rtw_wifi_role_t *wrole = padapter->phl_role;
	struct _ADAPTER_LINK *padapter_link = psta->padapter_link;
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct protocol_cap_t *protocol_cap = &(padapter_link->wrlink->protocol_cap);
	struct vht_priv *pvhtpriv_ap = &pmlmepriv->vhtpriv;
	struct vht_priv *pvhtpriv_sta = &psta->vhtpriv;

	u8	cur_ldpc_cap = 0, cur_stbc_cap = 0;
	s8 bw_mode = -1;
	u8	*pcap_mcs;

	if (pvhtpriv_sta->vht_option == _FALSE)
		return;

	if (pvhtpriv_sta->op_present) {
		if (GET_VHT_OPERATION_ELE_CHL_WIDTH(pvhtpriv_sta->vht_op) >= CH_WIDTH_80_160M)
			bw_mode = CHANNEL_WIDTH_80;
	}

	if (pvhtpriv_sta->notify_present)
		bw_mode = GET_VHT_OPERATING_MODE_FIELD_CHNL_WIDTH(&pvhtpriv_sta->vht_op_mode_notify);
	else if (MLME_IS_AP(padapter)) {
		/* for VHT client without Operating Mode Notify IE; minimal 80MHz */
		if (bw_mode < CHANNEL_WIDTH_80)
			bw_mode = CHANNEL_WIDTH_80;
	}

	if (bw_mode != -1)
		psta->phl_sta->chandef.bw = bw_mode; /* update bw_mode only if get value from VHT IEs */

	/* ToDo: need to API to inform hal_sta->ra_info.is_vht_enable  */
	/* psta->phl_sta->ra_info.is_vht_enable = _TRUE; */

	/* B4 Rx LDPC */
	if (TEST_FLAG(pvhtpriv_ap->ldpc_cap, LDPC_VHT_ENABLE_TX) &&
	    GET_VHT_CAPABILITY_ELE_RX_LDPC(pvhtpriv_sta->vht_cap)) {
		SET_FLAG(cur_ldpc_cap, (LDPC_VHT_ENABLE_TX | LDPC_VHT_CAP_TX));
		RTW_INFO("Current STA(%d) VHT LDPC = %02X\n", psta->phl_sta->aid, cur_ldpc_cap);
	}
	pvhtpriv_sta->ldpc_cap = cur_ldpc_cap;

	if (psta->phl_sta->chandef.bw > pmlmeext->chandef.bw)
		psta->phl_sta->chandef.bw = pmlmeext->chandef.bw;

	if (psta->phl_sta->chandef.bw == CHANNEL_WIDTH_80) {
		/* B5 Short GI for 80 MHz */
		pvhtpriv_sta->sgi_80m = (GET_VHT_CAPABILITY_ELE_SHORT_GI80M(pvhtpriv_sta->vht_cap) & pvhtpriv_ap->sgi_80m) ? _TRUE : _FALSE;
		/* RTW_INFO("Current STA ShortGI80MHz = %d\n", pvhtpriv_sta->sgi_80m); */
	} else if (psta->phl_sta->chandef.bw >= CHANNEL_WIDTH_160) {
		/* B6 Short GI for 160 MHz */
		pvhtpriv_sta->sgi_160m = (GET_VHT_CAPABILITY_ELE_SHORT_GI160M(pvhtpriv_sta->vht_cap) & pvhtpriv_ap->sgi_160m) ? _TRUE : _FALSE;
		/* RTW_INFO("Current STA ShortGI160MHz = %d\n", pvhtpriv_sta->sgi_160m); */
	}

	/* B8 B9 B10 Rx STBC */
	if (TEST_FLAG(pvhtpriv_ap->stbc_cap, STBC_VHT_ENABLE_TX) &&
	    GET_VHT_CAPABILITY_ELE_RX_STBC(pvhtpriv_sta->vht_cap)) {
		SET_FLAG(cur_stbc_cap, (STBC_VHT_ENABLE_TX | STBC_VHT_CAP_TX));
		RTW_INFO("Current STA(%d) VHT STBC = %02X\n", psta->phl_sta->aid, cur_stbc_cap);
		phl_sta->asoc_cap.stbc_vht_rx =
			protocol_cap->stbc_vht_tx ?
			GET_VHT_CAPABILITY_ELE_RX_STBC(pvhtpriv_sta->vht_cap) : 0;
	}
	pvhtpriv_sta->stbc_cap = cur_stbc_cap;
	phl_sta->asoc_cap.stbc_vht_tx =
		GET_VHT_CAPABILITY_ELE_TX_STBC(pvhtpriv_sta->vht_cap);

#ifdef CONFIG_BEAMFORMING
	update_sta_vht_info_apmode_bf_cap(padapter, psta);
#endif

	/* B23 B24 B25 Maximum A-MPDU Length Exponent */
	pvhtpriv_sta->ampdu_len = GET_VHT_CAPABILITY_ELE_MAX_RXAMPDU_FACTOR(pvhtpriv_sta->vht_cap);

	pcap_mcs = GET_VHT_CAPABILITY_ELE_RX_MCS(pvhtpriv_sta->vht_cap);
	_rtw_memcpy(pvhtpriv_sta->vht_mcs_map, pcap_mcs, 2);
	pvhtpriv_sta->vht_highest_rate = rtw_get_vht_highest_rate(pvhtpriv_sta->vht_mcs_map);
}

#ifdef PRIVATE_R
u8 VHT_get_ss_from_map(u8 *vht_mcs_map)
{
	u8 i, j;
	u8 ss = 0;

	for (i = 0; i < 2; i++) {
		if (vht_mcs_map[i] != 0xff) {
			for (j = 0; j < 8; j += 2) {
				if (((vht_mcs_map[i] >> j) & 0x03) == 0x03)
					break;
				ss++;
			}
		}

	}

return ss;
}

void VHT_caps_handler_infra_ap(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			PNDIS_802_11_VARIABLE_IEs pIE)
{
	struct link_mlme_priv		*pmlmepriv = &padapter_link->mlmepriv;
	struct vht_priv_infra_ap	*pvhtpriv = &pmlmepriv->vhtpriv_infra_ap;
	u8      cur_stbc_cap_infra_ap = 0;
	u16	cur_beamform_cap_infra_ap = 0;
	u8	*pcap_mcs;
	u8	*pcap_mcs_tx;
	u8	Rx_ss = 0, Tx_ss = 0;

	struct link_mlme_ext_priv		*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info		*pmlmeinfo = &(pmlmeext->mlmext_info);

	if (pIE == NULL)
		return;

	pmlmeinfo->ht_vht_received |= BIT(1);

	pvhtpriv->ldpc_cap_infra_ap = GET_VHT_CAPABILITY_ELE_RX_LDPC(pIE->data);

	if (GET_VHT_CAPABILITY_ELE_RX_STBC(pIE->data))
		SET_FLAG(cur_stbc_cap_infra_ap, STBC_VHT_ENABLE_RX);
	if (GET_VHT_CAPABILITY_ELE_TX_STBC(pIE->data))
		SET_FLAG(cur_stbc_cap_infra_ap, STBC_VHT_ENABLE_TX);
	pvhtpriv->stbc_cap_infra_ap = cur_stbc_cap_infra_ap;

	/*store ap info for channel bandwidth*/
	pvhtpriv->channel_width_infra_ap = GET_VHT_CAPABILITY_ELE_CHL_WIDTH(pIE->data);

	/*check B11: SU Beamformer Capable and B12: SU Beamformee B19: MU Beamformer B20:MU Beamformee*/
	if (GET_VHT_CAPABILITY_ELE_SU_BFER(pIE->data))
		SET_FLAG(cur_beamform_cap_infra_ap, BEAMFORMING_VHT_BEAMFORMER_ENABLE);
	if (GET_VHT_CAPABILITY_ELE_SU_BFEE(pIE->data))
		SET_FLAG(cur_beamform_cap_infra_ap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE);
	if (GET_VHT_CAPABILITY_ELE_MU_BFER(pIE->data))
		SET_FLAG(cur_beamform_cap_infra_ap, BEAMFORMING_VHT_MU_MIMO_AP_ENABLE);
	if (GET_VHT_CAPABILITY_ELE_MU_BFEE(pIE->data))
		SET_FLAG(cur_beamform_cap_infra_ap, BEAMFORMING_VHT_MU_MIMO_STA_ENABLE);
	pvhtpriv->beamform_cap_infra_ap = cur_beamform_cap_infra_ap;

	/*store information about vht_mcs_set*/
	pcap_mcs = GET_VHT_CAPABILITY_ELE_RX_MCS(pIE->data);
	pcap_mcs_tx = GET_VHT_CAPABILITY_ELE_TX_MCS(pIE->data);
	_rtw_memcpy(pvhtpriv->vht_mcs_map_infra_ap, pcap_mcs, 2);
	_rtw_memcpy(pvhtpriv->vht_mcs_map_tx_infra_ap, pcap_mcs_tx, 2);

	Rx_ss = VHT_get_ss_from_map(pvhtpriv->vht_mcs_map_infra_ap);
	Tx_ss = VHT_get_ss_from_map(pvhtpriv->vht_mcs_map_tx_infra_ap);
	if (Rx_ss >= Tx_ss) {
		pvhtpriv->number_of_streams_infra_ap = Rx_ss;
	} else{
		pvhtpriv->number_of_streams_infra_ap = Tx_ss;
	}

}
#endif /* PRIVATE_R */

void VHT_caps_handler(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			PNDIS_802_11_VARIABLE_IEs pIE)
{
	struct rtw_wifi_role_t 	*wrole = padapter->phl_role;
	struct link_mlme_priv	*pmlmepriv = &padapter_link->mlmepriv;
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct protocol_cap_t *protocol_cap = &padapter_link->wrlink->protocol_cap;
	struct vht_priv		*pvhtpriv = &pmlmepriv->vhtpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	WLAN_BSSID_EX           *cur_network = &(pmlmeinfo->network);
	struct sta_priv 	*pstapriv = &padapter->stapriv;
	struct sta_info		*psta = NULL;
	struct rtw_phl_stainfo_t *phl_sta = NULL;
	u8	cur_ldpc_cap = 0, cur_stbc_cap = 0, rx_nss = 0;
	u16	cur_beamform_cap = 0;
	u8	*pcap_mcs;

	if (pIE == NULL)
		return;

	if (pvhtpriv->vht_option == _FALSE)
		return;

	psta = rtw_get_stainfo(pstapriv, cur_network->MacAddress);
	if (psta == NULL)
		return;
	phl_sta = psta->phl_sta;

	pmlmeinfo->VHT_enable = 1;

	/* B4 Rx LDPC */
	if (TEST_FLAG(pvhtpriv->ldpc_cap, LDPC_VHT_ENABLE_TX) &&
	    GET_VHT_CAPABILITY_ELE_RX_LDPC(pIE->data)) {
		SET_FLAG(cur_ldpc_cap, (LDPC_VHT_ENABLE_TX | LDPC_VHT_CAP_TX));
		RTW_INFO("Current VHT LDPC Setting = %02X\n", cur_ldpc_cap);
		phl_sta->asoc_cap.vht_ldpc = 1;
	}
	pvhtpriv->ldpc_cap = cur_ldpc_cap;

	/* B5 Short GI for 80 MHz */
	pvhtpriv->sgi_80m = (GET_VHT_CAPABILITY_ELE_SHORT_GI80M(pIE->data) & pvhtpriv->sgi_80m) ? _TRUE : _FALSE;
	/* RTW_INFO("Current ShortGI80MHz = %d\n", pvhtpriv->sgi_80m); */

	/* B6 Short GI for 160 MHz */
	pvhtpriv->sgi_160m = (GET_VHT_CAPABILITY_ELE_SHORT_GI160M(pIE->data) & pvhtpriv->sgi_160m) ? _TRUE : _FALSE;
	/* RTW_INFO("Current ShortGI160MHz = %d\n", pvhtpriv->sgi_160m); */

	/* B8 B9 B10 Rx STBC */
	if (TEST_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_TX) &&
	    GET_VHT_CAPABILITY_ELE_RX_STBC(pIE->data)) {
		SET_FLAG(cur_stbc_cap, (STBC_VHT_ENABLE_TX | STBC_VHT_CAP_TX));
		RTW_INFO("Current VHT STBC Setting = %02X\n", cur_stbc_cap);
		phl_sta->asoc_cap.stbc_vht_rx =
			protocol_cap->stbc_vht_tx ?
			GET_VHT_CAPABILITY_ELE_RX_STBC(pIE->data) : 0;
	}
	pvhtpriv->stbc_cap = cur_stbc_cap;
	phl_sta->asoc_cap.stbc_vht_tx = GET_VHT_CAPABILITY_ELE_TX_STBC(pIE->data);

	phl_sta->asoc_cap.vht_su_bfmr = GET_VHT_CAPABILITY_ELE_SU_BFER(pIE->data);
	phl_sta->asoc_cap.vht_su_bfme = GET_VHT_CAPABILITY_ELE_SU_BFEE(pIE->data);
	phl_sta->asoc_cap.vht_mu_bfmr = GET_VHT_CAPABILITY_ELE_MU_BFER(pIE->data);
	phl_sta->asoc_cap.vht_mu_bfme = GET_VHT_CAPABILITY_ELE_MU_BFEE(pIE->data);
	phl_sta->asoc_cap.bfme_sts = GET_VHT_CAPABILITY_ELE_SU_BFEE_STS_CAP(pIE->data);
	phl_sta->asoc_cap.num_snd_dim = GET_VHT_CAPABILITY_ELE_SU_BFER_SOUND_DIM_NUM(pIE->data);

	RTW_INFO("%s: VHT STA assoc_cap:\n", __func__);
	RTW_INFO("- SU BFer: %d\n", phl_sta->asoc_cap.vht_su_bfmr);
	RTW_INFO("- SU BFee: %d\n", phl_sta->asoc_cap.vht_su_bfme);
	RTW_INFO("- MU BFer: %d\n", phl_sta->asoc_cap.vht_mu_bfmr);
	RTW_INFO("- MU BFee: %d\n", phl_sta->asoc_cap.vht_mu_bfme);
	RTW_INFO("- BFee STS: %d\n", phl_sta->asoc_cap.bfme_sts);
	RTW_INFO("- BFer SND DIM number: %d\n", phl_sta->asoc_cap.num_snd_dim);

#ifdef CONFIG_BEAMFORMING
	/*
	 * B11 SU Beamformer Capable,
	 * the target supports Beamformer and we are Beamformee
	 */
	if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE)
	    && GET_VHT_CAPABILITY_ELE_SU_BFER(pIE->data)) {
		SET_FLAG(cur_beamform_cap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE);

		/* Shift to BEAMFORMING_VHT_BEAMFORMEE_STS_CAP */
		SET_FLAG(cur_beamform_cap, GET_VHT_CAPABILITY_ELE_SU_BFEE_STS_CAP(pIE->data) << 8);

		/*
		 * B19 MU Beamformer Capable,
		 * the target supports Beamformer and we are Beamformee
		 */
		if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_STA_ENABLE)
		    && GET_VHT_CAPABILITY_ELE_MU_BFER(pIE->data))
			SET_FLAG(cur_beamform_cap, BEAMFORMING_VHT_MU_MIMO_STA_ENABLE);
	}

	/*
	 * B12 SU Beamformee Capable,
	 * the target supports Beamformee and we are Beamformer
	 */
	if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE)
	    && GET_VHT_CAPABILITY_ELE_SU_BFEE(pIE->data)) {
		SET_FLAG(cur_beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE);

		/* Shit to BEAMFORMING_VHT_BEAMFORMER_SOUND_DIM */
		SET_FLAG(cur_beamform_cap, GET_VHT_CAPABILITY_ELE_SU_BFER_SOUND_DIM_NUM(pIE->data) << 12);

		/*
		 * B20 MU Beamformee Capable,
		 * the target supports Beamformee and we are Beamformer
		 */
		if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_AP_ENABLE)
		    && GET_VHT_CAPABILITY_ELE_MU_BFEE(pIE->data))
			SET_FLAG(cur_beamform_cap, BEAMFORMING_VHT_MU_MIMO_AP_ENABLE);
	}

	pvhtpriv->beamform_cap = cur_beamform_cap;
	RTW_INFO("Current VHT Beamforming Setting=0x%04X\n", cur_beamform_cap);
#endif /* CONFIG_BEAMFORMING */

	/* B0 B1 Maximum MPDU Length */
	pvhtpriv->max_mpdu_len = GET_VHT_CAPABILITY_ELE_MAX_MPDU_LENGTH(pIE->data);
	/* B23 B24 B25 Maximum A-MPDU Length Exponent */
	pvhtpriv->ampdu_len = GET_VHT_CAPABILITY_ELE_MAX_RXAMPDU_FACTOR(pIE->data);

	pcap_mcs = GET_VHT_CAPABILITY_ELE_RX_MCS(pIE->data);
	rx_nss = get_phy_rx_nss(padapter, padapter_link);
	rtw_vht_nss_to_mcsmap(rx_nss, pvhtpriv->vht_mcs_map, pcap_mcs);
	pvhtpriv->vht_highest_rate = rtw_get_vht_highest_rate(pvhtpriv->vht_mcs_map);
}

void VHT_operation_handler(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
				PNDIS_802_11_VARIABLE_IEs pIE)
{
	struct link_mlme_priv		*pmlmepriv = &padapter_link->mlmepriv;
	struct vht_priv		*pvhtpriv = &pmlmepriv->vhtpriv;

	if (pIE == NULL)
		return;

	if (pvhtpriv->vht_option == _FALSE)
		return;

	if (pIE->Length != VHT_OP_IE_LEN) {
		RTW_WARN("%s: unexpected IE length(%u)!\n",
			 __func__, pIE->Length);
	}

	pvhtpriv->op_present = 1;
	_rtw_memcpy(pvhtpriv->vht_op, pIE->data,
		    pIE->Length > VHT_OP_IE_LEN ? VHT_OP_IE_LEN : pIE->Length);
}

void rtw_process_vht_op_mode_notify(_adapter *padapter, u8 *pframe, void *sta)
{
	struct registry_priv	*regsty = &padapter->registrypriv;
	struct sta_info		*psta = (struct sta_info *)sta;
	struct _ADAPTER_LINK	*padapter_link = psta->padapter_link;
	struct link_mlme_priv	*pmlmepriv = &padapter_link->mlmepriv;
	struct vht_priv		*pvhtpriv = &pmlmepriv->vhtpriv;
	u8	target_bw;
	u8	target_rxss, current_rxss;
	u8	update_ra = _FALSE;
	u8 tx_nss = 0;

	if (pvhtpriv->vht_option == _FALSE)
		return;

	target_bw = GET_VHT_OPERATING_MODE_FIELD_CHNL_WIDTH(pframe);
	tx_nss = get_phy_tx_nss(padapter, padapter_link);
	target_rxss = rtw_min(tx_nss, (GET_VHT_OPERATING_MODE_FIELD_RX_NSS(pframe) + 1));

	if (target_bw != psta->phl_sta->chandef.bw) {
		if (rtw_hw_is_bw_support(adapter_to_dvobj(padapter), target_bw)
		    && REGSTY_IS_BW_5G_SUPPORT(regsty, target_bw)
		   ) {
			update_ra = _TRUE;
			psta->phl_sta->chandef.bw = target_bw;
		}
	}

	current_rxss = rtw_vht_mcsmap_to_nss(psta->vhtpriv.vht_mcs_map);
	if (target_rxss != current_rxss) {
		u8	vht_mcs_map[2] = {};

		update_ra = _TRUE;

		rtw_vht_nss_to_mcsmap(target_rxss, vht_mcs_map, psta->vhtpriv.vht_mcs_map);
		_rtw_memcpy(psta->vhtpriv.vht_mcs_map, vht_mcs_map, 2);

		update_sta_ra_info(padapter, psta);
	}

	if (update_ra) {
		rtw_phl_cmd_change_stainfo(adapter_to_dvobj(padapter)->phl,
					   psta->phl_sta,
					   STA_CHG_RAMASK,
					   NULL,
					   0,
					   PHL_CMD_NO_WAIT,
					   0);
	}
}

u32	rtw_build_vht_operation_ie(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			u8 *pbuf, u8 channel)
{
	struct registry_priv	*pregistrypriv = &padapter->registrypriv;
	struct link_mlme_priv		*pmlmepriv = &padapter_link->mlmepriv;
	struct vht_priv			*pvhtpriv = &pmlmepriv->vhtpriv;
	/* struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv; */

	u8	ChnlWidth, center_freq, bw_mode;
	u32	len = 0;
	u8	operation[5];
	struct rtw_chan_def chdef = {0};

	_rtw_memset(operation, 0, 5);

	bw_mode = REGSTY_BW_5G(pregistrypriv); /* TODO: control op bw with other info */

	if (rtw_hw_chk_bw_cap(adapter_to_dvobj(padapter), BW_CAP_80M | BW_CAP_160M)
		&& REGSTY_BW_5G(pregistrypriv) >= CHANNEL_WIDTH_80
		) {
		chdef.chan = channel;
		chdef.bw = bw_mode;
		chdef.offset = CHAN_OFFSET_UPPER;
		chdef.band = rtw_get_band_type(channel);

		center_freq = rtw_phl_get_center_ch(&chdef);
		ChnlWidth = CH_WIDTH_80_160M;
	} else {
		center_freq = 0;
		ChnlWidth = CH_WIDTH_20_40M;
	}

	SET_VHT_OPERATION_ELE_CHL_WIDTH(operation, ChnlWidth);
	/* center frequency */
	SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ1(operation, center_freq);/* Todo: need to set correct center channel */
	SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ2(operation, 0);

	_rtw_memcpy(operation + 3, pvhtpriv->vht_mcs_map, 2);

	rtw_set_ie(pbuf, EID_VHTOperation, 5, operation, &len);

	return len;
}

u32	rtw_build_vht_op_mode_notify_ie(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
						u8 *pbuf, u8 bw)
{
	/* struct registry_priv *pregistrypriv = &padapter->registrypriv; */
	struct link_mlme_priv *pmlmepriv = &padapter_link->mlmepriv;
	struct vht_priv	*pvhtpriv = &pmlmepriv->vhtpriv;
	u32	len = 0;
	u8	opmode = 0;
	u8	chnl_width, rx_nss;

	chnl_width = bw;
	rx_nss = rtw_vht_mcsmap_to_nss(pvhtpriv->vht_mcs_map);


	if (chnl_width == CHANNEL_WIDTH_160) {
		SET_VHT_OPERATING_MODE_FIELD_CHNL_WIDTH(&opmode, 2);
		SET_VHT_OPERATING_MODE_FIELD_80_80_160_BW(&opmode, 1);
	} else
		SET_VHT_OPERATING_MODE_FIELD_CHNL_WIDTH(&opmode, chnl_width);
	SET_VHT_OPERATING_MODE_FIELD_RX_NSS(&opmode, (rx_nss - 1));
	SET_VHT_OPERATING_MODE_FIELD_RX_NSS_TYPE(&opmode, 0); /* Todo */

	pvhtpriv->vht_op_mode_notify = opmode;

	pbuf = rtw_set_ie(pbuf, EID_OpModeNotification, 1, &opmode, &len);

	return len;
}

u32	rtw_build_vht_cap_ie(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, u8 *pbuf)
{
	u8 bw;
	u16 HighestRate;
	u8 *pcap, *pcap_mcs;
	u32 len = 0;
	struct registry_priv *pregistrypriv = &padapter->registrypriv;
	struct link_mlme_priv *pmlmepriv = &padapter_link->mlmepriv;
	struct vht_priv *pvhtpriv = &pmlmepriv->vhtpriv;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);

	pcap = pvhtpriv->vht_cap;
	_rtw_memset(pcap, 0, 32);

	/*
	* VHT Capabilities Information field : B0 to B31
	*/

	/* B0 B1 Maximum MPDU Length */
	SET_VHT_CAPABILITY_ELE_MAX_MPDU_LENGTH(pcap, pvhtpriv->max_mpdu_len);

	/* B2 B3 Supported Channel Width Set */
	if (rtw_hw_chk_bw_cap(adapter_to_dvobj(padapter), BW_CAP_160M) && REGSTY_IS_BW_5G_SUPPORT(pregistrypriv, CHANNEL_WIDTH_160)) {
		if (rtw_hw_chk_bw_cap(adapter_to_dvobj(padapter), BW_CAP_80_80M) && REGSTY_IS_BW_5G_SUPPORT(pregistrypriv, CHANNEL_WIDTH_80_80)) {
			SET_VHT_CAPABILITY_ELE_CHL_WIDTH(pcap, 2);
			/* RTW_INFO("[VHT] Declare supporting 160MHz and 80+80MHz\n"); */
		} else {
			SET_VHT_CAPABILITY_ELE_CHL_WIDTH(pcap, 1);
			/* RTW_INFO("[VHT] Declare supporting 160MHz\n"); */
		}
	} else
		SET_VHT_CAPABILITY_ELE_CHL_WIDTH(pcap, 0);

	/* B4 Rx LDPC */
	if (TEST_FLAG(pvhtpriv->ldpc_cap, LDPC_VHT_ENABLE_RX)) {
		SET_VHT_CAPABILITY_ELE_RX_LDPC(pcap, 1);
		/* RTW_INFO("[VHT] Declare supporting RX LDPC\n"); */
	}

	/* B5 ShortGI for 80MHz */
	SET_VHT_CAPABILITY_ELE_SHORT_GI80M(pcap, pvhtpriv->sgi_80m ? 1 : 0);
	/*
	if (pvhtpriv->sgi_80m)
		RTW_INFO("[VHT] Declare supporting SGI 80MHz\n");
	*/

	/* B6 Short GI for 160 and 80+80 MHz */
	SET_VHT_CAPABILITY_ELE_SHORT_GI160M(pcap, pvhtpriv->sgi_160m ? 1 : 0);
	/*
	if (pvhtpriv->sgi_160m) {
		RTW_INFO("[VHT] Declare supporting SGI 160MHz and 80+80MHz\n");
	}
	*/

	/* B7 Tx STBC */
	if (TEST_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_TX)) {
		SET_VHT_CAPABILITY_ELE_TX_STBC(pcap, 1);
		/* RTW_INFO("[VHT] Declare supporting TX STBC\n"); */
	}

	/* B8 B9 B10 Rx STBC */
	if (TEST_FLAG(pvhtpriv->stbc_cap, STBC_VHT_ENABLE_RX)) {
		SET_VHT_CAPABILITY_ELE_RX_STBC(pcap, pvhtpriv->rx_stbc_nss);
		/* RTW_INFO("[VHT] Declare supporting RX STBC = %d\n", pvhtpriv->rx_stbc_nss); */
	}

	#ifdef CONFIG_BEAMFORMING
	/* B11 SU Beamformer Capable */
	if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMER_ENABLE)) {
		SET_VHT_CAPABILITY_ELE_SU_BFER(pcap, 1);
		/* RTW_INFO("[VHT] Declare supporting SU Beamformer\n"); */

		/* B16 17 18 Number of Sounding Dimensions */
		SET_VHT_CAPABILITY_ELE_SOUNDING_DIMENSIONS(pcap, pvhtpriv->num_snd_dim);

		/* B19 MU Beamformer Capable */
		if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_AP_ENABLE)) {
			SET_VHT_CAPABILITY_ELE_MU_BFER(pcap, 1);
			/* RTW_INFO("[VHT] Declare supporting MU Beamformer\n"); */
		}
	}

	/* B12 SU Beamformee Capable */
	if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_BEAMFORMEE_ENABLE)) {
		u8 bfme_sts = pvhtpriv->bfme_sts;

		SET_VHT_CAPABILITY_ELE_SU_BFEE(pcap, 1);
		/* RTW_INFO("[VHT] Declare supporting SU Beamformee\n"); */

		/* IOT action suggested by Yu Chen 2017/3/3 */
		if ((pmlmeinfo->assoc_AP_vendor == HT_IOT_PEER_BROADCOM) &&
			!pvhtpriv->ap_bf_cap.is_mu_bfer &&
			pvhtpriv->ap_bf_cap.su_sound_dim == 2)
			bfme_sts = (bfme_sts >= 2 ? 2 : bfme_sts);

		/* B13 14 15 Beamformee STS Capability */
		SET_VHT_CAPABILITY_ELE_SU_BFEE_STS_CAP(pcap, bfme_sts);

		/* B20 MU Beamformee Capable */
		if (TEST_FLAG(pvhtpriv->beamform_cap, BEAMFORMING_VHT_MU_MIMO_STA_ENABLE)) {
			SET_VHT_CAPABILITY_ELE_MU_BFEE(pcap, 1);
			/* RTW_INFO("[VHT] Declare supporting MU Beamformee\n"); */
		}
	}
	#endif/*CONFIG_BEAMFORMING*/

	/* B21 VHT TXOP PS */
	if (pvhtpriv->txop_ps) {
		SET_VHT_CAPABILITY_ELE_TXOP_PS(pcap, 1);
		/* RTW_INFO("[VHT] Declare supporting VHT TXOP power save\n"); */
	}

	/* B22 +HTC-VHT Capable */
	if (pvhtpriv->htc_vht) {
		SET_VHT_CAPABILITY_ELE_HTC_VHT(pcap, 1);
		/* RTW_INFO("[VHT] Declare supporting VHT variant HT Control\n"); */
	}

	/* B23 24 25 Maximum A-MPDU Length Exponent */
	SET_VHT_CAPABILITY_ELE_MAX_RXAMPDU_FACTOR(pcap, pvhtpriv->ampdu_len);
	/* RTW_INFO("[VHT] Declare supporting RX A-MPDU Length Exponent = %u\n", pvhtpriv->ampdu_len); */

	/* B26 27 VHT Link Adaptation Capable */
	if (pvhtpriv->link_adap_cap) {
		SET_VHT_CAPABILITY_ELE_LINK_ADAPTION(pcap, 1);
		/* RTW_INFO("[VHT] Declare supporting link adaptation using VHT variant HT Control\n"); */
	}

	/* B28 Rx Antenna Pattern Consistency */
	if (pvhtpriv->rx_ant_pattern) {
		SET_VHT_CAPABILITY_ELE_RX_ANT_PATTERN(pcap, 1);
		/* RTW_INFO("[VHT] Declare supporting RX Antenna Pattern Consistency\n"); */
	}

	/* B29 Tx Antenna Pattern Consistency */
	if (pvhtpriv->tx_ant_pattern) {
		SET_VHT_CAPABILITY_ELE_TX_ANT_PATTERN(pcap, 1);
		/* RTW_INFO("[VHT] Declare supporting TX Antenna Pattern Consistency\n"); */
	}

	/* B30 B31 Extended NSS BW Support */
	SET_VHT_CAPABILITY_ELE_EXT_NSS_BW(pcap, pvhtpriv->ext_nss_bw);

	/*
	* Supported VHT-MCS and NSS Set : 8 bytes
	*/
	pcap_mcs = GET_VHT_CAPABILITY_ELE_RX_MCS(pcap);
	_rtw_memcpy(pcap_mcs, pvhtpriv->vht_mcs_map, 2);

	pcap_mcs = GET_VHT_CAPABILITY_ELE_TX_MCS(pcap);
	_rtw_memcpy(pcap_mcs, pvhtpriv->vht_mcs_map, 2);

	/* find the largest bw supported by both registry and hal */
	bw = rtw_hw_largest_bw(adapter_to_dvobj(padapter), REGSTY_BW_5G(pregistrypriv));

	if(bw >= ARRAY_SIZE(VHT_MCS_DATA_RATE)){
		RTW_WARN("BW parameter value is out of range:%u\n", bw);
		bw = ARRAY_SIZE(VHT_MCS_DATA_RATE) - 1;
	}
	HighestRate = VHT_MCS_DATA_RATE[bw][0][((pvhtpriv->vht_highest_rate - MGN_VHT1SS_MCS0) & 0x3f)];
	HighestRate = (HighestRate + 1) >> 1;

	SET_VHT_CAPABILITY_ELE_MCS_RX_HIGHEST_RATE(pcap, HighestRate); /* indicate we support highest rx rate is 600Mbps. */
	SET_VHT_CAPABILITY_ELE_MCS_TX_HIGHEST_RATE(pcap, HighestRate); /* indicate we support highest tx rate is 600Mbps. */
	if (rtw_hw_chk_bw_cap(adapter_to_dvobj(padapter), BW_CAP_160M) && REGSTY_IS_BW_5G_SUPPORT(pregistrypriv, CHANNEL_WIDTH_160))
		SET_VHT_CAPABILITY_ELE_MCS_TX_EXT_NSS_BW_CAP(pcap, 1); /*160M*/

	pbuf = rtw_set_ie(pbuf, EID_VHTCapability, 12, pcap, &len);

	return len;
}

u8 rtw_vht_get_oper_bw(u8 *ht_op_ie, u8 *vht_cap_ie, u8 *vht_op_ie, u8 oper_bw)
{
	u8 ccfs0 = 0;
	u8 ccfs1 = 0;
	u8 ccfs2 = 0;

	if (GET_VHT_OPERATION_ELE_CHL_WIDTH(vht_op_ie + 2) >= CH_WIDTH_80_160M)
		oper_bw = CHANNEL_WIDTH_80;

	/*Ref 802.11-2020.pdf
	  * Table 9-272 Setting of the Supported Channel Width Set subfield and Extended NSS BW
	  *	Support subfield at a STA transmitting the VHT Capabilities Information field
	  * Table 11-23 VHT BSS bandwidth
	  * Table 11-25 Extended NSS channel width
	  */
	if (GET_VHT_CAPABILITY_ELE_CHL_WIDTH(vht_cap_ie + 2) == 1 &&  GET_VHT_CAPABILITY_ELE_EXT_NSS_BW(vht_cap_ie + 2) == 0) {
		if (oper_bw == CHANNEL_WIDTH_80) {
			ccfs0 = GET_VHT_OPERATION_ELE_CENTER_FREQ1(vht_op_ie + 2);
			ccfs1 = GET_VHT_OPERATION_ELE_CENTER_FREQ2(vht_op_ie + 2);
			if (ccfs1 > 0 && (ccfs1 - ccfs0 == 8 || ccfs0 - ccfs1 == 8) )
				oper_bw = CHANNEL_WIDTH_160;
		}
	} else if (GET_VHT_CAPABILITY_ELE_CHL_WIDTH(vht_cap_ie + 2) == 0 &&  GET_VHT_CAPABILITY_ELE_EXT_NSS_BW(vht_cap_ie + 2) == 1) {
		if (oper_bw == CHANNEL_WIDTH_80) {
			ccfs0 = GET_VHT_OPERATION_ELE_CENTER_FREQ1(vht_op_ie + 2);
			ccfs1 = GET_VHT_OPERATION_ELE_CENTER_FREQ2(vht_op_ie + 2);
			if (ht_op_ie != NULL)
				ccfs2 = GET_HT_OP_ELE_CHL_CENTER_FREQ_SEGMENT2(ht_op_ie + 2);
			else
				return oper_bw;
			if (ccfs1 == 0 &&  (ccfs2 - ccfs0 == 8 || ccfs0 - ccfs2 == 8))
				oper_bw = CHANNEL_WIDTH_160;
		}
	}

	return oper_bw;
}


u32 rtw_restructure_vht_ie(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
					u8 *in_ie, u8 *out_ie, uint in_len, uint *pout_len, u8 channel)
{
	u32	ielen;
	u8 max_bw;
	u8 oper_ch, oper_bw = CHANNEL_WIDTH_20, oper_offset = CHAN_OFFSET_NO_EXT;
	u8 *out_vht_op_ie, *ht_op_ie, *vht_cap_ie, *vht_op_ie;
	enum band_type band = rtw_is_2g_ch(channel) ? BAND_ON_24G : BAND_ON_5G;
	struct registry_priv *pregistrypriv = &padapter->registrypriv;
	struct link_mlme_priv	*pmlmepriv = &padapter_link->mlmepriv;
	struct vht_priv	*pvhtpriv = &pmlmepriv->vhtpriv;

	rtw_vht_get_real_setting(padapter, padapter_link);

	ht_op_ie = rtw_get_ie(in_ie + 12, WLAN_EID_HT_OPERATION, &ielen, in_len - 12);
	if (!ht_op_ie || ielen != HT_OP_IE_LEN)
		goto exit;
	vht_cap_ie = rtw_get_ie(in_ie + 12, EID_VHTCapability, &ielen, in_len - 12);
	if (!vht_cap_ie || ielen != VHT_CAP_IE_LEN)
		goto exit;
	vht_op_ie = rtw_get_ie(in_ie + 12, EID_VHTOperation, &ielen, in_len - 12);
	if (!vht_op_ie || ielen != VHT_OP_IE_LEN)
		goto exit;

	/* VHT Capabilities element */
	*pout_len += rtw_build_vht_cap_ie(padapter, padapter_link, out_ie + *pout_len);


	/* VHT Operation element */
	out_vht_op_ie = out_ie + *pout_len;
	rtw_set_ie(out_vht_op_ie, EID_VHTOperation, VHT_OP_IE_LEN, vht_op_ie + 2 , pout_len);

	/* get primary channel from HT_OP_IE */
	oper_ch = GET_HT_OP_ELE_PRI_CHL(ht_op_ie + 2);

	/* find the largest bw supported by both registry and hal */
	if (band == BAND_ON_5G)
		max_bw = rtw_hw_largest_bw(adapter_to_dvobj(padapter), REGSTY_BW_5G(pregistrypriv));
	else if (band == BAND_ON_24G)
		max_bw = rtw_hw_largest_bw(adapter_to_dvobj(padapter), REGSTY_BW_2G(pregistrypriv));

	if (max_bw >= CHANNEL_WIDTH_40) {
		/* get bw offset form HT_OP_IE */
		if (GET_HT_OP_ELE_STA_CHL_WIDTH(ht_op_ie + 2)) {
			switch (GET_HT_OP_ELE_2ND_CHL_OFFSET(ht_op_ie + 2)) {
			case IEEE80211_SCA:
				oper_bw = CHANNEL_WIDTH_40;
				oper_offset = CHAN_OFFSET_UPPER;
				break;
			case IEEE80211_SCB:
				oper_bw = CHANNEL_WIDTH_40;
				oper_offset = CHAN_OFFSET_LOWER;
				break;
			}
		}

		if (oper_bw == CHANNEL_WIDTH_40) {
			oper_bw = rtw_vht_get_oper_bw(ht_op_ie, vht_cap_ie,
						      vht_op_ie, oper_bw);
			oper_bw = rtw_min(oper_bw, max_bw);
		}
	}

	/* try downgrage bw to fit in channel plan setting */
	oper_bw = alink_adjust_linking_bw_by_regd(padapter_link, band, oper_ch, oper_bw, oper_offset);
	if (oper_bw == CHANNEL_WIDTH_20)
		oper_offset = CHAN_OFFSET_NO_EXT;

	/* update VHT_OP_IE */
	if (oper_bw < CHANNEL_WIDTH_80) {
		SET_VHT_OPERATION_ELE_CHL_WIDTH(out_vht_op_ie + 2, CH_WIDTH_20_40M);
		SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ1(out_vht_op_ie + 2, 0);
		SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ2(out_vht_op_ie + 2, 0);
	} else if (oper_bw == CHANNEL_WIDTH_80) {
		struct rtw_chan_def chdef = {0};
		u8 cch;

		chdef.chan = oper_ch;
		chdef.bw = oper_bw;
		chdef.offset = oper_offset;
		chdef.band = rtw_get_band_type(oper_ch);

		cch = rtw_phl_get_center_ch(&chdef);

		SET_VHT_OPERATION_ELE_CHL_WIDTH(out_vht_op_ie + 2, CH_WIDTH_80_160M);
		SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ1(out_vht_op_ie + 2, cch);
		SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ2(out_vht_op_ie + 2, 0);

	} else if (oper_bw == CHANNEL_WIDTH_160) {
		/* Need add VHT operation setting for 160M ? */
	} else {
		RTW_ERR(FUNC_ADPT_FMT" unsupported BW:%u\n", FUNC_ADPT_ARG(padapter), oper_bw);
		rtw_warn_on(1);
	}

	/* Operating Mode Notification element */
	*pout_len += rtw_build_vht_op_mode_notify_ie(padapter, padapter_link, out_ie + *pout_len, oper_bw);

	pvhtpriv->vht_option = _TRUE;

exit:
	return pvhtpriv->vht_option;

}

void VHTOnAssocRsp(_adapter *padapter)
{
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_priv		*pmlmepriv = &padapter_link->mlmepriv;
	struct vht_priv		*pvhtpriv = &pmlmepriv->vhtpriv;
	struct link_mlme_ext_priv	*pmlmeext = &padapter_link->mlmeextpriv;
	struct link_mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	u8	ht_AMPDU_len;

	RTW_INFO("%s\n", __FUNCTION__);

	if (!pmlmeinfo->HT_enable)
		return;

	if (!pmlmeinfo->VHT_enable)
		return;

	ht_AMPDU_len = pmlmeinfo->HT_caps.u.HT_cap_element.AMPDU_para & 0x03;

	if (pvhtpriv->ampdu_len > ht_AMPDU_len)
		rtw_hal_set_hwreg(padapter, HW_VAR_AMPDU_FACTOR, (u8 *)(&pvhtpriv->ampdu_len));

	rtw_hal_set_hwreg(padapter, HW_VAR_AMPDU_MAX_TIME, (u8 *)(&pvhtpriv->vht_highest_rate));
}

void rtw_vht_ies_attach(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			WLAN_BSSID_EX *pnetwork)
{
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	u8 cap_len, operation_len;
	uint len = 0;
	sint ie_len = 0;
	u8 *p = NULL;

	p = rtw_get_ie(pnetwork->IEs + _BEACON_IE_OFFSET_, EID_VHTCapability, &ie_len,
			(pnetwork->IELength - _BEACON_IE_OFFSET_));
	if (p && ie_len > 0)
		return;

	rtw_vht_get_real_setting(padapter, padapter_link);

	/* VHT Operation mode notifiy bit in Extended IE (127) */
	rtw_add_ext_cap_info(pmlmepriv->ext_capab_ie_data, &(pmlmepriv->ext_capab_ie_len), OP_MODE_NOTIFICATION);
	rtw_update_ext_cap_ie(pmlmepriv->ext_capab_ie_data, pmlmepriv->ext_capab_ie_len, pnetwork->IEs \
		, &(pnetwork->IELength), _BEACON_IE_OFFSET_);

	/* VHT Capabilities element */
	cap_len = rtw_build_vht_cap_ie(padapter, padapter_link, pnetwork->IEs + pnetwork->IELength);
	pnetwork->IELength += cap_len;

	/* VHT Operation element */
	operation_len = rtw_build_vht_operation_ie(padapter, padapter_link, pnetwork->IEs + pnetwork->IELength,
										pnetwork->Configuration.DSConfig);
	pnetwork->IELength += operation_len;

	rtw_check_for_vht20(padapter, pnetwork->IEs + _BEACON_IE_OFFSET_, pnetwork->IELength - _BEACON_IE_OFFSET_);

	pmlmepriv->vhtpriv.vht_option = _TRUE;
}

void rtw_vht_ies_detach(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			WLAN_BSSID_EX *pnetwork)
{
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);

	rtw_remove_ext_cap_info(pmlmepriv->ext_capab_ie_data, &(pmlmepriv->ext_capab_ie_len), OP_MODE_NOTIFICATION);
	rtw_update_ext_cap_ie(pmlmepriv->ext_capab_ie_data, pmlmepriv->ext_capab_ie_len, pnetwork->IEs \
		, &(pnetwork->IELength), _BEACON_IE_OFFSET_);

	rtw_remove_bcn_ie(padapter, pnetwork, EID_VHTCapability);
	rtw_remove_bcn_ie(padapter, pnetwork, EID_VHTOperation);
	rtw_remove_bcn_ie(padapter, pnetwork, EID_VHTTransmitPower);

	pmlmepriv->vhtpriv.vht_option = _FALSE;
}

void rtw_check_for_vht20(_adapter *adapter, u8 *ies, int ies_len)
{
	u8 ht_ch, ht_bw, ht_offset;
	u8 vht_ch, vht_bw, vht_offset;

	rtw_ies_get_chbw(ies, ies_len, &ht_ch, &ht_bw, &ht_offset, 1, 0);
	rtw_ies_get_chbw(ies, ies_len, &vht_ch, &vht_bw, &vht_offset, 1, 1);

	if (ht_bw == CHANNEL_WIDTH_20 && vht_bw >= CHANNEL_WIDTH_80) {
		u8 *vht_op_ie;
		int vht_op_ielen;

		RTW_INFO(FUNC_ADPT_FMT" vht80 is not allowed without ht40\n", FUNC_ADPT_ARG(adapter));
		vht_op_ie = rtw_get_ie(ies, EID_VHTOperation, &vht_op_ielen, ies_len);
		if (vht_op_ie && vht_op_ielen) {
			RTW_INFO(FUNC_ADPT_FMT" switch to vht20\n", FUNC_ADPT_ARG(adapter));
			SET_VHT_OPERATION_ELE_CHL_WIDTH(vht_op_ie + 2, CH_WIDTH_20_40M);
			SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ1(vht_op_ie + 2, 0);
			SET_VHT_OPERATION_ELE_CHL_CENTER_FREQ2(vht_op_ie + 2, 0);
		}
	}
}

/* We need to update the (mlmepriv->vhtpriv) */
void rtw_update_drv_vht_cap(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			u8 *vht_cap_ie)
{
	/* Initialize VHT capability element */
	rtw_vht_get_real_setting(padapter, padapter_link);

	RTW_INFO("Don't setting VHT capability IE from hostap, builded by driver temporarily\n");
	rtw_build_vht_cap_ie(padapter, padapter_link, vht_cap_ie);
}

void rtw_check_vht_ies(_adapter *padapter, struct _ADAPTER_LINK *padapter_link,
			WLAN_BSSID_EX *pnetwork)
{
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct vht_priv *pvhtpriv = &(pmlmepriv->vhtpriv);
	u32 ie_len;
	u32 ies_len = pnetwork->IELength - _BEACON_IE_OFFSET_;
	u8 *ies = pnetwork->IEs + _BEACON_IE_OFFSET_;
	u8 *vht_cap_ie, *vht_op_ie;

	vht_cap_ie = rtw_get_ie(ies, EID_VHTCapability, &ie_len, ies_len);

	vht_op_ie = rtw_get_ie(ies, EID_VHTOperation, &ie_len, ies_len);

	rtw_update_drv_vht_cap(padapter, padapter_link, vht_cap_ie);

	rtw_add_ext_cap_info(pmlmepriv->ext_capab_ie_data, &(pmlmepriv->ext_capab_ie_len), OP_MODE_NOTIFICATION);
	rtw_update_ext_cap_ie(pmlmepriv->ext_capab_ie_data, pmlmepriv->ext_capab_ie_len, pnetwork->IEs \
		, &(pnetwork->IELength), _BEACON_IE_OFFSET_);

	/* Backup these two VHT IEs from hostapd/wpa_supplicant for restore usage */
	if (vht_cap_ie != NULL)
		_rtw_memcpy(pvhtpriv->vht_cap_ie_backup, vht_cap_ie + 2, VHT_CAP_IE_LEN);

	if (vht_op_ie != NULL)
		_rtw_memcpy(pvhtpriv->vht_op_ie_backup, vht_op_ie + 2, VHT_OP_IE_LEN);

	/* TODO : We don't handle this IE like before, so remove it */
	rtw_remove_bcn_ie(padapter, pnetwork, EID_VHTTransmitPower);
}

void rtw_update_probe_rsp_vht_cap(struct _ADAPTER *a, u8 *ies, sint ies_len)
{
	struct _ADAPTER_LINK *a_link = GET_PRIMARY_LINK(a);
	u8 *vht_cap_ie;
	sint ie_len;

	vht_cap_ie = rtw_get_ie(ies, WLAN_EID_VHT_CAPABILITY, &ie_len, ies_len);
	if (vht_cap_ie)
		rtw_build_vht_cap_ie(a, a_link, vht_cap_ie);
}

void rtw_reattach_vht_ies(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, WLAN_BSSID_EX *pnetwork)
{
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct vht_priv *pvhtpriv = &(pmlmepriv->vhtpriv);
	u8 *vht_cap_ie = pnetwork->IEs + pnetwork->IELength;
	u8 *vht_op_ie;

	RTW_INFO(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(padapter));

	if (pnetwork->IEs != NULL) {
		vht_op_ie = rtw_set_ie(vht_cap_ie, EID_VHTCapability, VHT_CAP_IE_LEN,
			pvhtpriv->vht_cap_ie_backup, &(pnetwork->IELength));

		rtw_set_ie(vht_op_ie, EID_VHTOperation, VHT_OP_IE_LEN,
			pvhtpriv->vht_op_ie_backup, &(pnetwork->IELength));

		rtw_add_ext_cap_info(pmlmepriv->ext_capab_ie_data, &(pmlmepriv->ext_capab_ie_len), OP_MODE_NOTIFICATION);
		rtw_update_ext_cap_ie(pmlmepriv->ext_capab_ie_data, pmlmepriv->ext_capab_ie_len, pnetwork->IEs \
		, &(pnetwork->IELength), _BEACON_IE_OFFSET_);
	}

	pmlmepriv->vhtpriv.vht_option = _TRUE;
}

/* rx_nss: number of rx spatial stream to be used  */
u8 _issue_op_mode_notify_frame(_adapter *a, struct _ADAPTER_LINK *a_link,
		u8 *ra, u8 rx_nss, enum channel_width bw, u8 wait_ack)
{
	int ret = _FAIL;
	u8 category = RTW_WLAN_CATEGORY_VHT;
	u8 action = RTW_WLAN_ACTION_VHT_OPMODE_NOTIFICATION;
	struct xmit_frame *mgntframe;
	struct pkt_attrib *attrib;
	u8 *frame;
	struct rtw_ieee80211_hdr *wlanhdr;
	u16 *fctrl;
	struct link_mlme_ext_info *mlmeinfo = &(a_link->mlmeextpriv.mlmext_info);
	struct xmit_priv *xmitpriv = &(a->xmitpriv);
	struct mlme_ext_priv *mlmeext = &(a->mlmeextpriv);
	u8 opmode = 0, op_bw, op_80_80_bw;
	u32 ie_len;

	if (alink_is_tx_blocked_by_ch_waiting(a_link)) {
		ret = _FALSE;
		goto exit;
	}

	mgntframe = alloc_mgtxmitframe(xmitpriv);
	if (mgntframe == NULL) {
		ret = _FALSE;
		goto exit;
	}

	/* update attribute */
	attrib = &mgntframe->attrib;
	update_mgntframe_attrib(a, a_link, attrib);

	_rtw_memset(mgntframe->buf_addr, 0, WLANHDR_OFFSET + TXDESC_OFFSET);

	frame = (u8 *)(mgntframe->buf_addr) + TXDESC_OFFSET;
	wlanhdr = (struct rtw_ieee80211_hdr *)frame;

	fctrl = &(wlanhdr->frame_ctl);
	*(fctrl) = 0;

	/*RA*/
	_rtw_memcpy(wlanhdr->addr1, ra, ETH_ALEN);
	/*TA*/
	_rtw_memcpy(wlanhdr->addr2, a_link->mac_addr, ETH_ALEN);
	/*BSSID*/
	_rtw_memcpy(wlanhdr->addr3, get_my_bssid(&(mlmeinfo->network)), ETH_ALEN);

	SetSeqNum(wlanhdr, mlmeext->mgnt_seq);
	mlmeext->mgnt_seq++;
	set_frame_sub_type(frame, WIFI_ACTION);

	frame += sizeof(struct rtw_ieee80211_hdr_3addr);
	attrib->pktlen = sizeof(struct rtw_ieee80211_hdr_3addr);

	/* category, action */
	frame = rtw_set_fixed_ie(frame, 1, &category, &attrib->pktlen);
	frame = rtw_set_fixed_ie(frame, 1, &action, &attrib->pktlen);

	/* Operating Mode field */
	switch (bw) {
	case CHANNEL_WIDTH_20:
		op_bw = 0;
		op_80_80_bw = 0;
		break;
	case CHANNEL_WIDTH_40:
		op_bw = 1;
		op_80_80_bw = 0;
		break;
	case CHANNEL_WIDTH_80:
		op_bw = 2;
		op_80_80_bw = 0;
		break;
	case CHANNEL_WIDTH_160:
	case CHANNEL_WIDTH_80_80:
		op_bw = 2;
		op_80_80_bw = 1;
		break;
	default:
		RTW_INFO("%s bw(%d) not support\n", __func__, bw);
		ret = _FAIL;
		goto exit;
	}

	RTW_INFO(FUNC_ADPT_FMT ": op_bw:%d, op_80_80_bw=%d, rx_nss=%d\n",
		FUNC_ADPT_ARG(a), op_bw, op_80_80_bw, rx_nss);
	SET_VHT_OPERATING_MODE_FIELD_CHNL_WIDTH(&opmode, op_bw);
	SET_VHT_OPERATING_MODE_FIELD_80_80_160_BW(&opmode, op_80_80_bw);
	SET_VHT_OPERATING_MODE_FIELD_RX_NSS(&opmode, (rx_nss - 1));
	SET_VHT_OPERATING_MODE_FIELD_RX_NSS_TYPE(&opmode, 0);
	frame = rtw_set_fixed_ie(frame, 1, &opmode, &attrib->pktlen);

	attrib->last_txcmdsz = attrib->pktlen;

	if (wait_ack)
		ret = dump_mgntframe_and_wait_ack(a, mgntframe);
	else {
		dump_mgntframe(a, mgntframe);
		ret = _SUCCESS;
	}

	if (ret != _SUCCESS)
		RTW_INFO("%s, ack to\n", __func__);

exit:
	return ret;
}

/* rx_nss: number of rx spatial stream to be used  */
u8 rtw_issue_op_mode_notify_frame(_adapter *a, struct _ADAPTER_LINK *a_link,
		u8 *ra, u8 rx_nss, enum channel_width bw, u8 try_cnt, u8 wait_ms)
{
	int ret = _FAIL;
	int i = 0;
	systime start = rtw_get_current_time();

	if (alink_is_tx_blocked_by_ch_waiting(a_link))
		goto exit;

	do {
		ret = _issue_op_mode_notify_frame(a, a_link, ra, rx_nss, bw, wait_ms > 0 ? _TRUE : _FALSE);

		i++;

		if (RTW_CANNOT_RUN(adapter_to_dvobj(a)))
			break;

		if (i < try_cnt && wait_ms > 0 && ret == _FAIL)
			rtw_msleep_os(wait_ms);

	} while ((i < try_cnt) && ((ret == _FAIL) || (wait_ms == 0)));

	if (ret != _FAIL) {
		ret = _SUCCESS;
#ifndef DBG_XMIT_ACK
		goto exit;
#endif
	}

	if (try_cnt && wait_ms) {
		if (ra)
			RTW_INFO(FUNC_ADPT_FMT" to "MAC_FMT", ch:%u%s, %d/%d in %u ms\n",
				FUNC_ADPT_ARG(a), MAC_ARG(ra),
				rtw_get_oper_ch(a, a_link),
				ret == _SUCCESS ? ", acked" : "", i, try_cnt, rtw_get_passing_time_ms(start));
		else
			RTW_INFO(FUNC_ADPT_FMT", ch:%u%s, %d/%d in %u ms\n",
				FUNC_ADPT_ARG(a),
				rtw_get_oper_ch(a, a_link),
				ret == _SUCCESS ? ", acked" : "", i, try_cnt, rtw_get_passing_time_ms(start));
	}

exit:
	return ret;
}

void rtw_vht_op_mode_ctrl_rx_nss(_adapter *adapter, struct _ADAPTER_LINK *a_link,
									struct sta_info *sta, u8 final_rx_nss, bool need_update_ra)
{
	struct link_mlme_ext_info *a_mlmeinfo = &(a_link->mlmeextpriv.mlmext_info);
	struct rtw_chan_def *a_ch_def = &a_link->mlmeextpriv.chandef;

	rtw_issue_op_mode_notify_frame(adapter, a_link, get_my_bssid(&(a_mlmeinfo->network)),
		                                          final_rx_nss, a_ch_def->bw, 3, 10);
	if (need_update_ra)
		rtw_phl_cmd_change_stainfo(adapter_to_dvobj(adapter)->phl,
					   sta->phl_sta,
					   STA_CHG_RAMASK,
					   NULL,
					   0,
					   PHL_CMD_DIRECTLY,
					   0);

}

#endif /* CONFIG_80211AC_VHT */
