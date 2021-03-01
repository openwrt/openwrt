/******************************************************************************
 *
 * Copyright(c) 2012 - 2017 Realtek Corporation.
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
#define _RTL8192E_XMIT_C_

/* #include <drv_types.h> */
#include <rtl8192e_hal.h>

s32	rtl8192e_init_xmit_priv(_adapter *padapter)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;

#ifdef CONFIG_TX_EARLY_MODE
	pHalData->bEarlyModeEnable = padapter->registrypriv.early_mode;
#endif
	pxmitpriv->hw_ssn_seq_no = rtw_get_hwseq_no(padapter);

	pxmitpriv->nqos_ssn = 0;
	return _SUCCESS;
}

void _dbg_dump_tx_info(_adapter	*padapter, int frame_tag, u8 *ptxdesc)
{
	u8 bDumpTxPkt;
	u8 bDumpTxDesc = _FALSE;
	rtw_hal_get_def_var(padapter, HAL_DEF_DBG_DUMP_TXPKT, &(bDumpTxPkt));

	if (bDumpTxPkt == 1) { /* dump txdesc for data frame */
		RTW_INFO("dump tx_desc for data frame\n");
		if ((frame_tag & 0x0f) == DATA_FRAMETAG)
			bDumpTxDesc = _TRUE;
	} else if (bDumpTxPkt == 2) { /* dump txdesc for mgnt frame */
		RTW_INFO("dump tx_desc for mgnt frame\n");
		if ((frame_tag & 0x0f) == MGNT_FRAMETAG)
			bDumpTxDesc = _TRUE;
	} else if (bDumpTxPkt == 3) { /* dump early info */
	}

	if (bDumpTxDesc) {
		/* ptxdesc->txdw4 = cpu_to_le32(0x00001006); */ /* RTS Rate=24M */
		/*	ptxdesc->txdw6 = 0x6666f800; */
		RTW_INFO("=====================================\n");
		RTW_INFO("Offset00(0x%08x)\n", *((u32 *)(ptxdesc)));
		RTW_INFO("Offset04(0x%08x)\n", *((u32 *)(ptxdesc + 4)));
		RTW_INFO("Offset08(0x%08x)\n", *((u32 *)(ptxdesc + 8)));
		RTW_INFO("Offset12(0x%08x)\n", *((u32 *)(ptxdesc + 12)));
		RTW_INFO("Offset16(0x%08x)\n", *((u32 *)(ptxdesc + 16)));
		RTW_INFO("Offset20(0x%08x)\n", *((u32 *)(ptxdesc + 20)));
		RTW_INFO("Offset24(0x%08x)\n", *((u32 *)(ptxdesc + 24)));
		RTW_INFO("Offset28(0x%08x)\n", *((u32 *)(ptxdesc + 28)));
		RTW_INFO("Offset32(0x%08x)\n", *((u32 *)(ptxdesc + 32)));
		RTW_INFO("Offset36(0x%08x)\n", *((u32 *)(ptxdesc + 36)));
		RTW_INFO("=====================================\n");
	}

}

/*
 * Description:
 *	Aggregation packets and send to hardware
 *
 * Return:
 *	0	Success
 *	-1	Hardware resource(TX FIFO) not ready
 *	-2	Software resource(xmitbuf) not ready
 */
#ifdef CONFIG_TX_EARLY_MODE

/* #define DBG_EMINFO */

#if RTL8188E_EARLY_MODE_PKT_NUM_10 == 1
	#define EARLY_MODE_MAX_PKT_NUM	10
#else
	#define EARLY_MODE_MAX_PKT_NUM	5
#endif


struct EMInfo {
	u8	EMPktNum;
	u16  EMPktLen[EARLY_MODE_MAX_PKT_NUM];
};


void
InsertEMContent_8192E(
	struct EMInfo *pEMInfo,
	IN pu1Byte	VirtualAddress)
{

#if RTL8188E_EARLY_MODE_PKT_NUM_10 == 1
	u1Byte index = 0;
	u4Byte	dwtmp = 0;
#endif

	_rtw_memset(VirtualAddress, 0, EARLY_MODE_INFO_SIZE);
	if (pEMInfo->EMPktNum == 0)
		return;

#ifdef DBG_EMINFO
	{
		int i;
		RTW_INFO("\n%s ==> pEMInfo->EMPktNum =%d\n", __FUNCTION__, pEMInfo->EMPktNum);
		for (i = 0; i < EARLY_MODE_MAX_PKT_NUM; i++)
			RTW_INFO("%s ==> pEMInfo->EMPktLen[%d] =%d\n", __FUNCTION__, i, pEMInfo->EMPktLen[i]);

	}
#endif

#if RTL8188E_EARLY_MODE_PKT_NUM_10 == 1
	SET_EARLYMODE_PKTNUM(VirtualAddress, pEMInfo->EMPktNum);

	if (pEMInfo->EMPktNum == 1)
		dwtmp = pEMInfo->EMPktLen[0];
	else {
		dwtmp = pEMInfo->EMPktLen[0];
		dwtmp += ((dwtmp % 4) ? (4 - dwtmp % 4) : 0) + 4;
		dwtmp += pEMInfo->EMPktLen[1];
	}
	SET_EARLYMODE_LEN0(VirtualAddress, dwtmp);
	if (pEMInfo->EMPktNum <= 3)
		dwtmp = pEMInfo->EMPktLen[2];
	else {
		dwtmp = pEMInfo->EMPktLen[2];
		dwtmp += ((dwtmp % 4) ? (4 - dwtmp % 4) : 0) + 4;
		dwtmp += pEMInfo->EMPktLen[3];
	}
	SET_EARLYMODE_LEN1(VirtualAddress, dwtmp);
	if (pEMInfo->EMPktNum <= 5)
		dwtmp = pEMInfo->EMPktLen[4];
	else {
		dwtmp = pEMInfo->EMPktLen[4];
		dwtmp += ((dwtmp % 4) ? (4 - dwtmp % 4) : 0) + 4;
		dwtmp += pEMInfo->EMPktLen[5];
	}
	SET_EARLYMODE_LEN2_1(VirtualAddress, dwtmp & 0xF);
	SET_EARLYMODE_LEN2_2(VirtualAddress, dwtmp >> 4);
	if (pEMInfo->EMPktNum <= 7)
		dwtmp = pEMInfo->EMPktLen[6];
	else {
		dwtmp = pEMInfo->EMPktLen[6];
		dwtmp += ((dwtmp % 4) ? (4 - dwtmp % 4) : 0) + 4;
		dwtmp += pEMInfo->EMPktLen[7];
	}
	SET_EARLYMODE_LEN3(VirtualAddress, dwtmp);
	if (pEMInfo->EMPktNum <= 9)
		dwtmp = pEMInfo->EMPktLen[8];
	else {
		dwtmp = pEMInfo->EMPktLen[8];
		dwtmp += ((dwtmp % 4) ? (4 - dwtmp % 4) : 0) + 4;
		dwtmp += pEMInfo->EMPktLen[9];
	}
	SET_EARLYMODE_LEN4(VirtualAddress, dwtmp);
#else
	SET_EARLYMODE_PKTNUM(VirtualAddress, pEMInfo->EMPktNum);
	SET_EARLYMODE_LEN0(VirtualAddress, pEMInfo->EMPktLen[0]);
	SET_EARLYMODE_LEN1(VirtualAddress, pEMInfo->EMPktLen[1]);
	SET_EARLYMODE_LEN2_1(VirtualAddress, pEMInfo->EMPktLen[2] & 0xF);
	SET_EARLYMODE_LEN2_2(VirtualAddress, pEMInfo->EMPktLen[2] >> 4);
	SET_EARLYMODE_LEN3(VirtualAddress, pEMInfo->EMPktLen[3]);
	SET_EARLYMODE_LEN4(VirtualAddress, pEMInfo->EMPktLen[4]);
#endif

}



void UpdateEarlyModeInfo8192E(struct xmit_priv *pxmitpriv, struct xmit_buf *pxmitbuf)
{
	/* _adapter *padapter, struct xmit_frame *pxmitframe,struct tx_servq	*ptxservq */
	int index, j;
	u16 offset, pktlen;
	PTXDESC_8192E ptxdesc;

	u8 *pmem, *pEMInfo_mem;
	s8 node_num_0 = 0, node_num_1 = 0;
	struct EMInfo eminfo;
	struct agg_pkt_info *paggpkt;
	struct xmit_frame *pframe = (struct xmit_frame *)pxmitbuf->priv_data;
	pmem = pframe->buf_addr;

#ifdef DBG_EMINFO
	RTW_INFO("\n%s ==> agg_num:%d\n", __FUNCTION__, pframe->agg_num);
	for (index = 0; index < pframe->agg_num; index++) {
		offset =	pxmitpriv->agg_pkt[index].offset;
		pktlen = pxmitpriv->agg_pkt[index].pkt_len;
		RTW_INFO("%s ==> agg_pkt[%d].offset=%d\n", __FUNCTION__, index, offset);
		RTW_INFO("%s ==> agg_pkt[%d].pkt_len=%d\n", __FUNCTION__, index, pktlen);
	}
#endif

	if (pframe->agg_num > EARLY_MODE_MAX_PKT_NUM) {
		node_num_0 = pframe->agg_num;
		node_num_1 = EARLY_MODE_MAX_PKT_NUM - 1;
	}

	for (index = 0; index < pframe->agg_num; index++) {

		offset = pxmitpriv->agg_pkt[index].offset;
		pktlen = pxmitpriv->agg_pkt[index].pkt_len;

		_rtw_memset(&eminfo, 0, sizeof(struct EMInfo));
		if (pframe->agg_num > EARLY_MODE_MAX_PKT_NUM) {
			if (node_num_0 > EARLY_MODE_MAX_PKT_NUM) {
				eminfo.EMPktNum = EARLY_MODE_MAX_PKT_NUM;
				node_num_0--;
			} else {
				eminfo.EMPktNum = node_num_1;
				node_num_1--;
			}
		} else
			eminfo.EMPktNum = pframe->agg_num - (index + 1);
		for (j = 0; j < eminfo.EMPktNum ; j++) {
			eminfo.EMPktLen[j] = pxmitpriv->agg_pkt[index + 1 + j].pkt_len + 4; /* 4 bytes CRC */
		}

		if (pmem) {
			if (index == 0) {
				ptxdesc = (PTXDESC_8192E)(pmem);
				pEMInfo_mem = ((u8 *)ptxdesc) + TXDESC_SIZE;
			} else {
				pmem = pmem + pxmitpriv->agg_pkt[index - 1].offset;
				ptxdesc = (PTXDESC_8192E)(pmem);
				pEMInfo_mem = ((u8 *)ptxdesc) + TXDESC_SIZE;
			}

#ifdef DBG_EMINFO
			RTW_INFO("%s ==> desc.pkt_len=%d\n", __FUNCTION__, ptxdesc->pktlen);
#endif
			InsertEMContent_8192E(&eminfo, pEMInfo_mem);
		}


	}
	_rtw_memset(pxmitpriv->agg_pkt, 0, sizeof(struct agg_pkt_info) * MAX_AGG_PKT_NUM);

}
#endif

#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
void rtl8192e_cal_txdesc_chksum(u8 *ptxdesc)
{
	u16	*usPtr = (u16 *)ptxdesc;
	u32 count = 16;	/* (32 bytes / 2 bytes per XOR) => 16 times */
	u32 index;
	u16 checksum = 0;

	/* Clear first */
	SET_TX_DESC_TX_DESC_CHECKSUM_92E(ptxdesc, 0);

	for (index = 0 ; index < count ; index++)
		checksum = checksum ^ le16_to_cpu(*(usPtr + index));

	SET_TX_DESC_TX_DESC_CHECKSUM_92E(ptxdesc, checksum);
}
#endif

#if defined(CONFIG_CONCURRENT_MODE)
void fill_txdesc_force_bmc_camid(struct pkt_attrib *pattrib, u8 *ptxdesc)
{
	if ((pattrib->encrypt > 0) && (!pattrib->bswenc)
	    && (pattrib->bmc_camid != INVALID_SEC_MAC_CAM_ID)) {

		SET_TX_DESC_EN_DESC_ID_92E(ptxdesc, 1);
		SET_TX_DESC_MACID_92E(ptxdesc, pattrib->bmc_camid);
	}
}
#endif
void fill_txdesc_bmc_tx_rate(struct pkt_attrib *pattrib, u8 *ptxdesc)
{
	SET_TX_DESC_USE_RATE_92E(ptxdesc, 1);
	SET_TX_DESC_TX_RATE_92E(ptxdesc, MRateToHwRate(pattrib->rate));
	SET_TX_DESC_DISABLE_FB_92E(ptxdesc, 1);
}

void fill_txdesc_sectype(struct pkt_attrib *pattrib, u8 *ptxdesc)
{
	if ((pattrib->encrypt > 0) && !pattrib->bswenc) {
		switch (pattrib->encrypt) {
		/* SEC_TYPE : 0:NO_ENC,1:WEP40/TKIP,2:WAPI,3:AES */
		case _WEP40_:
		case _WEP104_:
		case _TKIP_:
		case _TKIP_WTMIC_:
			SET_TX_DESC_SEC_TYPE_92E(ptxdesc, 0x1);
			break;
#ifdef CONFIG_WAPI_SUPPORT
		case _SMS4_:
			SET_TX_DESC_SEC_TYPE_92E(ptxdesc, 0x2);
			break;
#endif
		case _AES_:
			SET_TX_DESC_SEC_TYPE_92E(ptxdesc, 0x3);
			break;
		case _NO_PRIVACY_:
		default:
			SET_TX_DESC_SEC_TYPE_92E(ptxdesc, 0x0);
			break;

		}

	}

}
void fill_txdesc_vcs(struct pkt_attrib *pattrib, u8 *ptxdesc)
{
	/* RTW_INFO("cvs_mode=%d\n", pattrib->vcs_mode);	 */

	SET_TX_DESC_HW_RTS_ENABLE_92E(ptxdesc, 0);

	switch (pattrib->vcs_mode) {
	case RTS_CTS:
		SET_TX_DESC_RTS_ENABLE_92E(ptxdesc, 1);
		break;
	case CTS_TO_SELF:
		SET_TX_DESC_CTS2SELF_92E(ptxdesc, 1);
		break;
	case NONE_VCS:
	default:
		break;
	}
#if 0 /* to do */
	/* Protection mode related */
	if (pTcb->bRTSEnable || pTcb->bCTSEnable) {
		SET_TX_DESC_CCA_RTS_92E(pDesc, pTcb->RTSCCA);

		/* SET_TX_DESC_RTS_ENABLE_92E(pDesc, ((pTcb->bRTSEnable && !pTcb->bCTSEnable) ? 1 : 0)); */
		/* SET_TX_DESC_CTS2SELF_92E(pDesc, ((pTcb->bCTSEnable) ? 1 : 0)); */

		SET_TX_DESC_CTROL_STBC_92E(pDesc, ((pTcb->bRTSSTBC) ? 1 : 0));
		SET_TX_DESC_RTS_SHORT_92E(pDesc, pTcb->bRTSShort);
		SET_TX_DESC_RTS_RATE_92E(pDesc, MRateToHwRate((u1Byte)pTcb->RTSRate));

		if (pMgntInfo->ForcedProtectionMode == PROTECTION_MODE_FORCE_ENABLE)
			SET_TX_DESC_RTS_RATE_FB_LIMIT_92E(pDesc, 1);
		else
			SET_TX_DESC_RTS_RATE_FB_LIMIT_92E(pDesc, 0xF);
	}
#endif
}

u8
BWMapping_92E(
	IN	PADAPTER		Adapter,
	IN	struct pkt_attrib	*pattrib
)
{
	u8	BWSettingOfDesc = 0;
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(Adapter);

	/* RTW_INFO("BWMapping pHalData->current_channel_bw %d, pattrib->bwmode %d\n",pHalData->current_channel_bw,pattrib->bwmode); */

	if (pHalData->current_channel_bw == CHANNEL_WIDTH_40) {
		if (pattrib->bwmode == CHANNEL_WIDTH_40)
			BWSettingOfDesc = 1;
		else
			BWSettingOfDesc = 0;
	} else
		BWSettingOfDesc = 0;

	/* if(pTcb->bBTTxPacket) */
	/*	BWSettingOfDesc = 0; */

	return BWSettingOfDesc;
}

u8
SCMapping_92E(
	IN	PADAPTER		Adapter,
	IN	struct pkt_attrib	*pattrib
)
{
	u8	SCSettingOfDesc = 0;
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(Adapter);
	/* RTW_INFO("SCMapping: pHalData->current_channel_bw %d, pHalData->nCur80MhzPrimeSC %d, pHalData->nCur40MhzPrimeSC %d\n",pHalData->current_channel_bw,pHalData->nCur80MhzPrimeSC,pHalData->nCur40MhzPrimeSC); */

	if (pHalData->current_channel_bw == CHANNEL_WIDTH_80) {
		if (pattrib->bwmode == CHANNEL_WIDTH_80)
			SCSettingOfDesc = VHT_DATA_SC_DONOT_CARE;
		else if (pattrib->bwmode == CHANNEL_WIDTH_40) {
			if (pHalData->nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER)
				SCSettingOfDesc = VHT_DATA_SC_40_LOWER_OF_80MHZ;
			else if (pHalData->nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER)
				SCSettingOfDesc = VHT_DATA_SC_40_UPPER_OF_80MHZ;
			else
				RTW_INFO("%s- current_channel_bw:%d, SCMapping: DONOT CARE Mode Setting\n", __func__, pHalData->current_channel_bw);
		} else {
			if ((pHalData->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER) && (pHalData->nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER))
				SCSettingOfDesc = VHT_DATA_SC_20_LOWEST_OF_80MHZ;
			else if ((pHalData->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER) && (pHalData->nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER))
				SCSettingOfDesc = VHT_DATA_SC_20_LOWER_OF_80MHZ;
			else if ((pHalData->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER) && (pHalData->nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER))
				SCSettingOfDesc = VHT_DATA_SC_20_UPPER_OF_80MHZ;
			else if ((pHalData->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER) && (pHalData->nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER))
				SCSettingOfDesc = VHT_DATA_SC_20_UPPERST_OF_80MHZ;
			else
				RTW_INFO("%s- current_channel_bw:%d, SCMapping: DONOT CARE Mode Setting\n", __func__, pHalData->current_channel_bw);
		}
	} else if (pHalData->current_channel_bw == CHANNEL_WIDTH_40) {
		/* RTW_INFO("SCMapping: HT Case: pHalData->current_channel_bw %d, pHalData->nCur40MhzPrimeSC %d\n",pHalData->current_channel_bw,pHalData->nCur40MhzPrimeSC); */

		if (pattrib->bwmode == CHANNEL_WIDTH_40)
			SCSettingOfDesc = VHT_DATA_SC_DONOT_CARE;
		else if (pattrib->bwmode == CHANNEL_WIDTH_20) {
			if (pHalData->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER)
				SCSettingOfDesc = VHT_DATA_SC_20_UPPER_OF_80MHZ;
			else if (pHalData->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER)
				SCSettingOfDesc = VHT_DATA_SC_20_LOWER_OF_80MHZ;
			else
				SCSettingOfDesc = VHT_DATA_SC_DONOT_CARE;

		}
	} else
		SCSettingOfDesc = VHT_DATA_SC_DONOT_CARE;

	return SCSettingOfDesc;
}

void fill_txdesc_phy(PADAPTER padapter, struct pkt_attrib *pattrib, u8 *ptxdesc)
{
	/* RTW_INFO("bwmode=%d, ch_off=%d\n", pattrib->bwmode, pattrib->ch_offset); */

	if (pattrib->ht_en) {
		/* Set Bandwidth and sub-channel settings. */
		SET_TX_DESC_DATA_BW_92E(ptxdesc, BWMapping_92E(padapter, pattrib));

		SET_TX_DESC_DATA_SC_92E(ptxdesc, SCMapping_92E(padapter, pattrib));
	}
}

void rtl8192e_fixed_rate(_adapter *padapter, u8 *ptxdesc)
{
	if (padapter->fix_rate != 0xFF) {

		SET_TX_DESC_USE_RATE_92E(ptxdesc, 1);
		/* if(pHalData->INIDATA_RATE[pattrib->mac_id] & BIT(7)) */
		{
			if (padapter->fix_rate & BIT(7))
				SET_TX_DESC_DATA_SHORT_92E(ptxdesc, 1);
		}
		SET_TX_DESC_TX_RATE_92E(ptxdesc, (padapter->fix_rate & 0x7F));
		/* SET_TX_DESC_DISABLE_FB_92E(ptxdesc,1); */
		if (!padapter->data_fb)
			SET_TX_DESC_DISABLE_FB_92E(ptxdesc, 1);

		/* ptxdesc->datarate = padapter->fix_rate; */
	}

}

/*
 * Description: In normal chip, we should send some packet to Hw which will be used by Fw
 *		in FW LPS mode. The function is to fill the Tx descriptor of this packets, then
 *		Fw can tell Hw to send these packet derectly.
 *   */
void rtl8192e_fill_fake_txdesc(
	PADAPTER	padapter,
	u8			*pDesc,
	u32			BufferLen,
	u8			IsPsPoll,
	u8			IsBTQosNull,
	u8			bDataFrame)
{
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct xmit_priv		*pxmitpriv = &padapter->xmitpriv;

	/* Clear all status */
	_rtw_memset(pDesc, 0, TXDESC_SIZE);

	SET_TX_DESC_OFFSET_92E(pDesc, (TXDESC_SIZE + OFFSET_SZ));

	SET_TX_DESC_PKT_SIZE_92E(pDesc, BufferLen);

	if (pmlmeext->cur_wireless_mode & WIRELESS_11B)
		SET_TX_DESC_RATE_ID_92E(pDesc, RATEID_IDX_B);
	else
		SET_TX_DESC_RATE_ID_92E(pDesc, RATEID_IDX_G);

	SET_TX_DESC_QUEUE_SEL_92E(pDesc,  QSLT_MGNT);

	/* Set NAVUSEHDR to prevent Ps-poll AId filed to be changed to error vlaue by Hw. */
	if (IsPsPoll)
		SET_TX_DESC_NAV_USE_HDR_92E(pDesc, 1);
	else {
		SET_TX_DESC_EN_HWSEQ_92E(pDesc, 1); /* Hw set sequence number */
		SET_TX_DESC_HWSEQ_SEL_92E(pDesc, pxmitpriv->hw_ssn_seq_no);
	}

	SET_TX_DESC_USE_RATE_92E(pDesc, 1);
	SET_TX_DESC_TX_RATE_92E(pDesc, MRateToHwRate(pmlmeext->tx_rate));

	/*  */
	/* Encrypt the data frame if under security mode excepct null data. Suggested by CCW. */
	/*  */
	if (_TRUE == bDataFrame) {
		struct pkt_attrib pattrib;
		pattrib.encrypt = padapter->securitypriv.dot11PrivacyAlgrthm;
		pattrib.bswenc = _FALSE;
		fill_txdesc_sectype(&pattrib, pDesc);
	}

#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
	/* USB interface drop packet if the checksum of descriptor isn't correct. */
	/* Using this checksum can let hardware recovery from packet bulk out error (e.g. Cancel URC, Bulk out error.). */
	rtl8192e_cal_txdesc_chksum(pDesc);
#endif
}
