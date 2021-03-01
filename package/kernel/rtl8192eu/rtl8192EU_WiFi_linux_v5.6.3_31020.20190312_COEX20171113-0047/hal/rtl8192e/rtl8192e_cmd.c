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
#define _RTL8192E_CMD_C_

/* #include <drv_types.h> */
#include <rtl8192e_hal.h>
#include "hal_com_h2c.h"
#include <hal_com.h>
#define CONFIG_H2C_EF

#define RTL8192E_MAX_H2C_BOX_NUMS	4
#define RTL8192E_MAX_CMD_LEN	7
#define RTL8192E_MESSAGE_BOX_SIZE		4
#define RTL8192E_EX_MESSAGE_BOX_SIZE	4


static u8 _is_fw_read_cmd_down(_adapter *padapter, u8 msgbox_num)
{
	u8	read_down = _FALSE;
	int	retry_cnts = 100;

	u8 valid;

	/* RTW_INFO(" _is_fw_read_cmd_down ,reg_1cc(%x),msg_box(%d)...\n",rtw_read8(padapter,REG_HMETFR),msgbox_num); */

	do {
		valid = rtw_read8(padapter, REG_HMETFR) & BIT(msgbox_num);
		if (0 == valid)
			read_down = _TRUE;
		else
			rtw_msleep_os(1);
	} while ((!read_down) && (retry_cnts--));

	return read_down;

}


/*****************************************
* H2C Msg format :
* 0x1DF - 0x1D0
*| 31 - 8	| 7-5	 4 - 0	|
*| h2c_msg	|Class_ID CMD_ID	|
*
* Extend 0x1FF - 0x1F0
*|31 - 0	  |
*|ext_msg|
******************************************/
s32 FillH2CCmd_8192E(PADAPTER padapter, u8 ElementID, u32 CmdLen, u8 *pCmdBuffer)
{
	u8 h2c_box_num = 0;
	u32	msgbox_addr = 0;
	u32 msgbox_ex_addr = 0;
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);
	u8 cmd_idx = 0;
	u32	h2c_cmd = 0;
	u32	h2c_cmd_ex = 0;
	s32 ret = _FAIL;


	padapter = GET_PRIMARY_ADAPTER(padapter);
	pHalData = GET_HAL_DATA(padapter);


	if (pHalData->bFWReady == _FALSE) {
		RTW_INFO("FillH2CCmd_8192E(): return H2C cmd because fw is not ready\n");
		return ret;
	}

	_enter_critical_mutex(&(adapter_to_dvobj(padapter)->h2c_fwcmd_mutex), NULL);

	if (!pCmdBuffer)
		goto exit;
	if (CmdLen > RTL8192E_MAX_CMD_LEN)
		goto exit;
	if (rtw_is_surprise_removed(padapter))
		goto exit;

	/* pay attention to if  race condition happened in  H2C cmd setting. */
	do {
		h2c_box_num = pHalData->LastHMEBoxNum;

		if (!_is_fw_read_cmd_down(padapter, h2c_box_num)) {
			RTW_INFO(" fw read cmd failed...\n");
			goto exit;
		}

		*(u8 *)(&h2c_cmd) = ElementID;

		if (CmdLen <= 3)
			_rtw_memcpy((u8 *)(&h2c_cmd) + 1, pCmdBuffer, CmdLen);
		else {
			_rtw_memcpy((u8 *)(&h2c_cmd) + 1, pCmdBuffer, 3);
			_rtw_memcpy((u8 *)(&h2c_cmd_ex), pCmdBuffer + 3, CmdLen - 3);
		}

		/* Write Ext command */
		msgbox_ex_addr = REG_HMEBOX_EXT0_8192E + (h2c_box_num * RTL8192E_EX_MESSAGE_BOX_SIZE);
#ifdef CONFIG_H2C_EF
		for (cmd_idx = 0; cmd_idx < RTL8192E_EX_MESSAGE_BOX_SIZE; cmd_idx++)
			rtw_write8(padapter, msgbox_ex_addr + cmd_idx, *((u8 *)(&h2c_cmd_ex) + cmd_idx));
#else
		h2c_cmd_ex = le32_to_cpu(h2c_cmd_ex);
		rtw_write32(padapter, msgbox_ex_addr, h2c_cmd_ex);
#endif
		
		/* Write command */
		msgbox_addr = REG_HMEBOX_0 + (h2c_box_num * RTL8192E_MESSAGE_BOX_SIZE);
#ifdef CONFIG_H2C_EF
		for (cmd_idx = 0; cmd_idx < RTL8192E_MESSAGE_BOX_SIZE; cmd_idx++)
			rtw_write8(padapter, msgbox_addr + cmd_idx, *((u8 *)(&h2c_cmd) + cmd_idx));
#else
		h2c_cmd = le32_to_cpu(h2c_cmd);
		rtw_write32(padapter, msgbox_addr, h2c_cmd);
#endif

		/*	RTW_INFO("MSG_BOX:%d,CmdLen(%d), reg:0x%x =>h2c_cmd:0x%.08x, reg:0x%x =>h2c_cmd_ex:0x%.08x ..\n" */
		/*	 	,pHalData->LastHMEBoxNum ,CmdLen,msgbox_addr,h2c_cmd,msgbox_ex_addr,h2c_cmd_ex); */

		pHalData->LastHMEBoxNum = (h2c_box_num + 1) % RTL8192E_MAX_H2C_BOX_NUMS;

	} while (0);

	ret = _SUCCESS;

exit:

	_exit_critical_mutex(&(adapter_to_dvobj(padapter)->h2c_fwcmd_mutex), NULL);


	return ret;
}

u8 rtl8192e_h2c_msg_hdl(_adapter *padapter, unsigned char *pbuf)
{
	u8 ElementID, CmdLen;
	u8 *pCmdBuffer;
	struct cmd_msg_parm  *pcmdmsg;

	if (!pbuf)
		return H2C_PARAMETERS_ERROR;

	pcmdmsg = (struct cmd_msg_parm *)pbuf;
	ElementID = pcmdmsg->eid;
	CmdLen = pcmdmsg->sz;
	pCmdBuffer = pcmdmsg->buf;

	FillH2CCmd_8192E(padapter, ElementID, CmdLen, pCmdBuffer);

	return H2C_SUCCESS;
}

void rtl8192e_set_FwPwrMode_cmd(PADAPTER padapter, u8 PSMode)
{
	u8	u1H2CSetPwrMode[H2C_PWRMODE_LEN] = {0};
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	u8	Mode = 0, RLBM = 0, PowerState = 0, LPSAwakeIntvl = 2;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	u8 allQueueUAPSD = 0;


	RTW_INFO("%s: Mode=%d SmartPS=%d\n", __FUNCTION__, PSMode, pwrpriv->smart_ps);

	switch (PSMode) {
	case PS_MODE_ACTIVE:
		Mode = 0;
		break;
	case PS_MODE_MIN:
		Mode = 1;
		break;
	case PS_MODE_MAX:
		RLBM = 1;
		Mode = 1;
		break;
	case PS_MODE_DTIM:
		RLBM = 2;
		Mode = 1;
		break;
	case PS_MODE_UAPSD_WMM:
		Mode = 2;
		break;
	default:
		Mode = 0;
		break;
	}

	if (Mode > PS_MODE_ACTIVE) {
#ifdef CONFIG_BT_COEXIST
		if (rtw_btcoex_IsBtControlLps(padapter) == _TRUE && (pHalData->EEPROMBluetoothCoexist == 1))
			PowerState = rtw_btcoex_RpwmVal(padapter);
		else
#endif /* CONFIG_BT_COEXIST */
			PowerState = 0x00;/* AllON(0x0C), RFON(0x04), RFOFF(0x00) */
	} else
		PowerState = 0x0C;/* AllON(0x0C), RFON(0x04), RFOFF(0x00) */
	/*
		if(BT_1AntPowerSaveMode(Adapter) && (FW_PS_ACTIVE_MODE != Mode))
		{
			Mode = FW_PS_MIN_MODE;
		}
	*/
	/* 0: Active, 1: LPS, 2: WMMPS */
	SET_8192E_H2CCMD_PWRMODE_PARM_MODE(u1H2CSetPwrMode, Mode);

	/* 0:Min, 1:Max , 2:User define */
	SET_8192E_H2CCMD_PWRMODE_PARM_RLBM(u1H2CSetPwrMode, RLBM);

	/* (LPS) smart_ps:  0: PS_Poll, 1: PS_Poll , 2: NullData */
	/* (WMM)smart_ps: 0:PS_Poll, 1:NullData */
	SET_8192E_H2CCMD_PWRMODE_PARM_SMART_PS(u1H2CSetPwrMode, pwrpriv->smart_ps);

	/* AwakeInterval: Unit is beacon interval, this field is only valid in PS_DTIM mode */
	SET_8192E_H2CCMD_PWRMODE_PARM_BCN_PASS_TIME(u1H2CSetPwrMode, LPSAwakeIntvl);

	/* (WMM only)bAllQueueUAPSD */
	SET_8192E_H2CCMD_PWRMODE_PARM_ALL_QUEUE_UAPSD(u1H2CSetPwrMode, allQueueUAPSD);
	/*
		if(PSMode == FW_PS_ACTIVE_MODE)
		{
			PowerState |= FW_PWR_STATE_ACTIVE;
		}
		else
		{
			if(BT_1AntPowerSaveMode(Adapter))
			{
				PowerState = BT_1AntRpwmVal(Adapter);
				pwrModeByte5 = BT_1AntLpsVal(Adapter);
				SET_8192E_H2CCMD_PWRMODE_PARM_BYTE5(u1H2CSetPwrMode, pwrModeByte5);
			}
			else
				PowerState |= FW_PWR_STATE_RF_OFF;
		}

	*/
	/* AllON(0x0C), RFON(0x04), RFOFF(0x00) */
	SET_8192E_H2CCMD_PWRMODE_PARM_PWR_STATE(u1H2CSetPwrMode, PowerState);

	/*
		if(BT_1Ant(Adapter) && BT_IsBtCoexManualControl(Adapter))
			return;

		BT_RecordPwrMode(Adapter, &u1H2CSetPwrMode[0], 6);
	*/
#ifdef CONFIG_BT_COEXIST
	if (pHalData->EEPROMBluetoothCoexist == 1)
		rtw_btcoex_RecordPwrMode(padapter, u1H2CSetPwrMode, H2C_PWRMODE_LEN);
#endif /* CONFIG_BT_COEXIST */

	FillH2CCmd_8192E(padapter, H2C_8192E_SETPWRMODE, sizeof(u1H2CSetPwrMode), (u8 *)&u1H2CSetPwrMode);
	/* BT_DecExecPwrCmdCnt(Adapter);	 */

}

#ifdef CONFIG_TDLS
#ifdef CONFIG_TDLS_CH_SW
void rtl8192e_set_BcnEarly_C2H_Rpt_cmd(PADAPTER padapter, u8 enable)
{
	u8	u1H2CSetPwrMode[H2C_PWRMODE_LEN] = {0};

	SET_8192E_H2CCMD_PWRMODE_PARM_MODE(u1H2CSetPwrMode, 1);
	SET_8192E_H2CCMD_PWRMODE_PARM_RLBM(u1H2CSetPwrMode, 1);
	SET_8192E_H2CCMD_PWRMODE_PARM_SMART_PS(u1H2CSetPwrMode, 0);
	SET_8192E_H2CCMD_PWRMODE_PARM_BCN_PASS_TIME(u1H2CSetPwrMode, 0);
	SET_8192E_H2CCMD_PWRMODE_PARM_ALL_QUEUE_UAPSD(u1H2CSetPwrMode, 0);
	SET_8192E_H2CCMD_PWRMODE_PARM_BCN_EARLY_C2H_RPT(u1H2CSetPwrMode, enable);
	SET_8192E_H2CCMD_PWRMODE_PARM_PWR_STATE(u1H2CSetPwrMode, 0x0C);
	FillH2CCmd_8192E(padapter, H2C_8192E_SETPWRMODE, sizeof(u1H2CSetPwrMode), u1H2CSetPwrMode);
}
#endif
#endif

/*
 * Description: Get the reserved page number in Tx packet buffer.
 * Retrun value: the page number.
 * 2012.08.09, by tynli.
 *   */
u8
GetTxBufferRsvdPageNum8192E(_adapter *padapter, bool wowlan)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	u8	RsvdPageNum = 0;
	/* default reseved 1 page for the IC type which is undefined. */
	u8	TxPageBndy = LAST_ENTRY_OF_TX_PKT_BUFFER_8192C;

	rtw_hal_get_def_var(padapter, HAL_DEF_TX_PAGE_BOUNDARY, (u8 *)&TxPageBndy);

	RsvdPageNum = LAST_ENTRY_OF_TX_PKT_BUFFER_8192C - TxPageBndy + 1;

	return RsvdPageNum;
}
void rtl8192e_download_rsvd_page(PADAPTER padapter, u8 mstatus)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	BOOLEAN		bSendBeacon = _FALSE;
	BOOLEAN		bcn_valid = _FALSE;
	u8	DLBcnCount = 0;
	u32 poll = 0;
	u8 RegFwHwTxQCtrl;

	RTW_INFO("+" FUNC_ADPT_FMT ": hw_port=%d mstatus(%x)\n",
		 FUNC_ADPT_ARG(padapter), get_hw_port(padapter), mstatus);

	if (mstatus == 1) {
		u8 bcn_ctrl = rtw_read8(padapter, REG_BCN_CTRL);

		/* We should set AID, correct TSF, HW seq enable before set JoinBssReport to Fw in 88/92C. */
		/* Suggested by filen. Added by tynli. */
		rtw_write16(padapter, REG_BCN_PSR_RPT, (0xC000 | pmlmeinfo->aid));
		/* Hw sequende enable by dedault. 2010.06.23. by tynli. */
		/* rtw_write16(padapter, REG_NQOS_SEQ, ((pmlmeext->mgnt_seq+100)&0xFFF)); */
		/* rtw_write8(padapter, REG_HWSEQ_CTRL, 0xFF); */

		/* Set REG_CR bit 8. DMA beacon by SW. */
		rtw_write8(padapter,  REG_CR + 1,
			rtw_read8(padapter,  REG_CR + 1) | BIT0);

		/* Disable Hw protection for a time which revserd for Hw sending beacon. */
		/* Fix download reserved page packet fail that access collision with the protection time. */
		/* 2010.05.11. Added by tynli. */
		rtw_write8(padapter, REG_BCN_CTRL, (bcn_ctrl & (~EN_BCN_FUNCTION)) | DIS_TSF_UDT);

		RegFwHwTxQCtrl = rtw_read8(padapter, REG_FWHW_TXQ_CTRL + 2);
		if (RegFwHwTxQCtrl & BIT6) {
			RTW_INFO("HalDownloadRSVDPage(): There is an Adapter is sending beacon.\n");
			bSendBeacon = _TRUE;
		}

		/* Set FWHW_TXQ_CTRL 0x422[6]=0 to tell Hw the packet is not a real beacon frame. */
		RegFwHwTxQCtrl &= (~BIT6);
		rtw_write8(padapter, REG_FWHW_TXQ_CTRL + 2, RegFwHwTxQCtrl);

		/* Clear beacon valid check bit. */
		rtw_hal_set_hwreg(padapter, HW_VAR_BCN_VALID, NULL);
		rtw_hal_set_hwreg(padapter, HW_VAR_DL_BCN_SEL, NULL);
		DLBcnCount = 0;
		poll = 0;
		do {
			rtw_hal_set_fw_rsvd_page(padapter, _FALSE);
			DLBcnCount++;
			do {
				rtw_yield_os();
				/* rtw_mdelay_os(10); */
				/* check rsvd page download OK. */
				rtw_hal_get_hwreg(padapter, HW_VAR_BCN_VALID, (u8 *)(&bcn_valid));
				poll++;
			} while (!bcn_valid && (poll % 10) != 0 && !RTW_CANNOT_RUN(padapter));

		} while (!bcn_valid && DLBcnCount <= 100 && !RTW_CANNOT_RUN(padapter));

		/* RT_ASSERT(bcn_valid, ("HalDownloadRSVDPage88ES(): 1 Download RSVD page failed!\n")); */
		if (RTW_CANNOT_RUN(padapter))
			;
		else if (!bcn_valid)
			RTW_ERR(ADPT_FMT": 1 DL RSVD page failed! DLBcnCount:%u, poll:%u\n",
				 ADPT_ARG(padapter) , DLBcnCount, poll);
		else {
			struct pwrctrl_priv *pwrctl = adapter_to_pwrctl(padapter);

			pwrctl->fw_psmode_iface_id = padapter->iface_id;
			rtw_hal_set_fw_rsvd_page(padapter, _TRUE);
			RTW_INFO(ADPT_FMT": 1 DL RSVD page success! DLBcnCount:%u, poll:%u\n",
				 ADPT_ARG(padapter), DLBcnCount, poll);
		}

		/* restore bcn_ctrl */
		rtw_write8(padapter, REG_BCN_CTRL, bcn_ctrl);

		/* To make sure that if there exists an adapter which would like to send beacon. */
		/* If exists, the origianl value of 0x422[6] will be 1, we should check this to */
		/* prevent from setting 0x422[6] to 0 after download reserved page, or it will cause */
		/* the beacon cannot be sent by HW. */
		/* 2010.06.23. Added by tynli. */
		if (bSendBeacon) {
			RegFwHwTxQCtrl |= BIT6;
			rtw_write8(padapter, REG_FWHW_TXQ_CTRL + 2, RegFwHwTxQCtrl);
		}

		/*  */
		/* Update RSVD page location H2C to Fw. */
		/*  */
		if (bcn_valid) {
			rtw_hal_set_hwreg(padapter, HW_VAR_BCN_VALID, NULL);
			RTW_INFO("Set RSVD page location to Fw.\n");
			/* FillH2CCmd88E(Adapter, H2C_88E_RSVDPAGE, H2C_RSVDPAGE_LOC_LENGTH, pMgntInfo->u1RsvdPageLoc); */
		}

		/* Do not enable HW DMA BCN or it will cause Pcie interface hang by timing issue. 2011.11.24. by tynli. */
		/* if(!padapter->bEnterPnpSleep) */
#ifndef CONFIG_PCI_HCI
		{
			/* Clear CR[8] or beacon packet will not be send to TxBuf anymore. */
			rtw_write8(padapter,  REG_CR + 1,
				rtw_read8(padapter,  REG_CR + 1) & (~BIT0));
		}
#endif /* !CONFIG_PCI_HCI */
	}
}
void rtl8192e_set_FwJoinBssReport_cmd(PADAPTER padapter, u8 mstatus)
{
	if (mstatus == 1)
		rtl8192e_download_rsvd_page(padapter, RT_MEDIA_CONNECT);
}

#ifdef CONFIG_P2P_PS
void rtl8192e_set_p2p_ctw_period_cmd(_adapter *padapter, u8 ctwindow)
{
	struct P2P_PS_CTWPeriod_t p2p_ps_ctw;

	p2p_ps_ctw.CTWPeriod = ctwindow;

	FillH2CCmd_8192E(padapter, H2C_8192E_P2P_PS_OFFLOAD, 1, (u8 *)(&p2p_ps_ctw));

}

void rtl8192e_set_p2p_ps_offload_cmd(_adapter *padapter, u8 p2p_ps_state)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct pwrctrl_priv		*pwrpriv = adapter_to_pwrctl(padapter);
	struct wifidirect_info	*pwdinfo = &(padapter->wdinfo);
	u8	*p2p_ps_offload = &pHalData->p2p_ps_offload;

	u8	i;


#if 1
	switch (p2p_ps_state) {
	case P2P_PS_DISABLE:
		RTW_INFO("P2P_PS_DISABLE\n");
		_rtw_memset(p2p_ps_offload, 0, 1);
		break;
	case P2P_PS_ENABLE:
		RTW_INFO("P2P_PS_ENABLE\n");
		/* update CTWindow value. */
		if (pwdinfo->ctwindow > 0) {
			SET_8192E_H2CCMD_P2P_PS_OFFLOAD_CTWINDOW_EN(p2p_ps_offload, 1);
			rtw_write8(padapter, REG_P2P_CTWIN, pwdinfo->ctwindow);
		}

		/* hw only support 2 set of NoA */
		for (i = 0 ; i < pwdinfo->noa_num ; i++) {
			/* To control the register setting for which NOA */
			rtw_write8(padapter, REG_NOA_DESC_SEL, (i << 4));
			if (i == 0)
				SET_8192E_H2CCMD_P2P_PS_OFFLOAD_NOA0_EN(p2p_ps_offload, 1);
			else
				SET_8192E_H2CCMD_P2P_PS_OFFLOAD_NOA1_EN(p2p_ps_offload, 1);

			/* config P2P NoA Descriptor Register */
			/* RTW_INFO("%s(): noa_duration = %x\n",__FUNCTION__,pwdinfo->noa_duration[i]); */
			rtw_write32(padapter, REG_NOA_DESC_DURATION, pwdinfo->noa_duration[i]);

			/* RTW_INFO("%s(): noa_interval = %x\n",__FUNCTION__,pwdinfo->noa_interval[i]); */
			rtw_write32(padapter, REG_NOA_DESC_INTERVAL, pwdinfo->noa_interval[i]);

			/* RTW_INFO("%s(): start_time = %x\n",__FUNCTION__,pwdinfo->noa_start_time[i]); */
			rtw_write32(padapter, REG_NOA_DESC_START, pwdinfo->noa_start_time[i]);

			/* RTW_INFO("%s(): noa_count = %x\n",__FUNCTION__,pwdinfo->noa_count[i]); */
			rtw_write8(padapter, REG_NOA_DESC_COUNT, pwdinfo->noa_count[i]);
		}

		if ((pwdinfo->opp_ps == 1) || (pwdinfo->noa_num > 0)) {
			/* rst p2p circuit */
			rtw_write8(padapter, REG_DUAL_TSF_RST, BIT(4));

			SET_8192E_H2CCMD_P2P_PS_OFFLOAD_ENABLE(p2p_ps_offload, 1);

			if (pwdinfo->role == P2P_ROLE_GO) {
				/* 1: Owner, 0: Client */
				SET_8192E_H2CCMD_P2P_PS_OFFLOAD_ROLE(p2p_ps_offload, 1);
				SET_8192E_H2CCMD_P2P_PS_OFFLOAD_ALLSTASLEEP(p2p_ps_offload, 0);
			} else {
				/* 1: Owner, 0: Client */
				SET_8192E_H2CCMD_P2P_PS_OFFLOAD_ROLE(p2p_ps_offload, 0);
			}

			/* SET_8192E_H2CCMD_P2P_PS_OFFLOAD_DISCOVERY(p2p_ps_offload, 0); */
		}
		break;
	case P2P_PS_SCAN:
		RTW_INFO("P2P_PS_SCAN\n");
		/* SET_8192E_H2CCMD_P2P_PS_OFFLOAD_DISCOVERY(p2p_ps_offload, 1); */
		break;
	case P2P_PS_SCAN_DONE:
		RTW_INFO("P2P_PS_SCAN_DONE\n");
		/* SET_8192E_H2CCMD_P2P_PS_OFFLOAD_DISCOVERY(p2p_ps_offload, 0); */
		pwdinfo->p2p_ps_state = P2P_PS_ENABLE;
		break;
	default:
		break;
	}

	RTW_INFO("P2P_PS_OFFLOAD : %x\n", p2p_ps_offload[0]);
	FillH2CCmd_8192E(padapter, H2C_8192E_P2P_PS_OFFLOAD, 1, p2p_ps_offload);
#endif


}

#endif /* CONFIG_P2P */


#if 0
u1Byte
hw_rate_to_m_rate(
	IN	u1Byte		rate
)
{
	u1Byte	ret_rate = MGN_1M;

	switch (rate) {

	case DESC_RATE1M:
		ret_rate = MGN_1M;
		break;
	case DESC_RATE2M:
		ret_rate = MGN_2M;
		break;
	case DESC_RATE5_5M:
		ret_rate = MGN_5_5M;
		break;
	case DESC_RATE11M:
		ret_rate = MGN_11M;
		break;
	case DESC_RATE6M:
		ret_rate = MGN_6M;
		break;
	case DESC_RATE9M:
		ret_rate = MGN_9M;
		break;
	case DESC_RATE12M:
		ret_rate = MGN_12M;
		break;
	case DESC_RATE18M:
		ret_rate = MGN_18M;
		break;
	case DESC_RATE24M:
		ret_rate = MGN_24M;
		break;
	case DESC_RATE36M:
		ret_rate = MGN_36M;
		break;
	case DESC_RATE48M:
		ret_rate = MGN_48M;
		break;
	case DESC_RATE54M:
		ret_rate = MGN_54M;
		break;
	case DESC_RATEMCS0:
		ret_rate = MGN_MCS0;
		break;
	case DESC_RATEMCS1:
		ret_rate = MGN_MCS1;
		break;
	case DESC_RATEMCS2:
		ret_rate = MGN_MCS2;
		break;
	case DESC_RATEMCS3:
		ret_rate = MGN_MCS3;
		break;
	case DESC_RATEMCS4:
		ret_rate = MGN_MCS4;
		break;
	case DESC_RATEMCS5:
		ret_rate = MGN_MCS5;
		break;
	case DESC_RATEMCS6:
		ret_rate = MGN_MCS6;
		break;
	case DESC_RATEMCS7:
		ret_rate = MGN_MCS7;
		break;
	case DESC_RATEMCS8:
		ret_rate = MGN_MCS8;
		break;
	case DESC_RATEMCS9:
		ret_rate = MGN_MCS9;
		break;
	case DESC_RATEMCS10:
		ret_rate = MGN_MCS10;
		break;
	case DESC_RATEMCS11:
		ret_rate = MGN_MCS11;
		break;
	case DESC_RATEMCS12:
		ret_rate = MGN_MCS12;
		break;
	case DESC_RATEMCS13:
		ret_rate = MGN_MCS13;
		break;
	case DESC_RATEMCS14:
		ret_rate = MGN_MCS14;
		break;
	case DESC_RATEMCS15:
		ret_rate = MGN_MCS15;
		break;
	case DESC_RATEMCS16:
		ret_rate = MGN_MCS16;
		break;
	case DESC_RATEMCS17:
		ret_rate = MGN_MCS17;
		break;
	case DESC_RATEMCS18:
		ret_rate = MGN_MCS18;
		break;
	case DESC_RATEMCS19:
		ret_rate = MGN_MCS19;
		break;
	case DESC_RATEMCS20:
		ret_rate = MGN_MCS20;
		break;
	case DESC_RATEMCS21:
		ret_rate = MGN_MCS21;
		break;
	case DESC_RATEMCS22:
		ret_rate = MGN_MCS22;
		break;
	case DESC_RATEMCS23:
		ret_rate = MGN_MCS23;
		break;
	case DESC_RATEMCS24:
		ret_rate = MGN_MCS24;
		break;
	case DESC_RATEMCS25:
		ret_rate = MGN_MCS25;
		break;
	case DESC_RATEMCS26:
		ret_rate = MGN_MCS26;
		break;
	case DESC_RATEMCS27:
		ret_rate = MGN_MCS27;
		break;
	case DESC_RATEMCS28:
		ret_rate = MGN_MCS28;
		break;
	case DESC_RATEMCS29:
		ret_rate = MGN_MCS29;
		break;
	case DESC_RATEMCS30:
		ret_rate = MGN_MCS30;
		break;
	case DESC_RATEMCS31:
		ret_rate = MGN_MCS31;
		break;
	case DESC_RATEVHTSS1MCS0:
		ret_rate = MGN_VHT1SS_MCS0;
		break;
	case DESC_RATEVHTSS1MCS1:
		ret_rate = MGN_VHT1SS_MCS1;
		break;
	case DESC_RATEVHTSS1MCS2:
		ret_rate = MGN_VHT1SS_MCS2;
		break;
	case DESC_RATEVHTSS1MCS3:
		ret_rate = MGN_VHT1SS_MCS3;
		break;
	case DESC_RATEVHTSS1MCS4:
		ret_rate = MGN_VHT1SS_MCS4;
		break;
	case DESC_RATEVHTSS1MCS5:
		ret_rate = MGN_VHT1SS_MCS5;
		break;
	case DESC_RATEVHTSS1MCS6:
		ret_rate = MGN_VHT1SS_MCS6;
		break;
	case DESC_RATEVHTSS1MCS7:
		ret_rate = MGN_VHT1SS_MCS7;
		break;
	case DESC_RATEVHTSS1MCS8:
		ret_rate = MGN_VHT1SS_MCS8;
		break;
	case DESC_RATEVHTSS1MCS9:
		ret_rate = MGN_VHT1SS_MCS9;
		break;
	case DESC_RATEVHTSS2MCS0:
		ret_rate = MGN_VHT2SS_MCS0;
		break;
	case DESC_RATEVHTSS2MCS1:
		ret_rate = MGN_VHT2SS_MCS1;
		break;
	case DESC_RATEVHTSS2MCS2:
		ret_rate = MGN_VHT2SS_MCS2;
		break;
	case DESC_RATEVHTSS2MCS3:
		ret_rate = MGN_VHT2SS_MCS3;
		break;
	case DESC_RATEVHTSS2MCS4:
		ret_rate = MGN_VHT2SS_MCS4;
		break;
	case DESC_RATEVHTSS2MCS5:
		ret_rate = MGN_VHT2SS_MCS5;
		break;
	case DESC_RATEVHTSS2MCS6:
		ret_rate = MGN_VHT2SS_MCS6;
		break;
	case DESC_RATEVHTSS2MCS7:
		ret_rate = MGN_VHT2SS_MCS7;
		break;
	case DESC_RATEVHTSS2MCS8:
		ret_rate = MGN_VHT2SS_MCS8;
		break;
	case DESC_RATEVHTSS2MCS9:
		ret_rate = MGN_VHT2SS_MCS9;
		break;
	case DESC_RATEVHTSS3MCS0:
		ret_rate = MGN_VHT3SS_MCS0;
		break;
	case DESC_RATEVHTSS3MCS1:
		ret_rate = MGN_VHT3SS_MCS1;
		break;
	case DESC_RATEVHTSS3MCS2:
		ret_rate = MGN_VHT3SS_MCS2;
		break;
	case DESC_RATEVHTSS3MCS3:
		ret_rate = MGN_VHT3SS_MCS3;
		break;
	case DESC_RATEVHTSS3MCS4:
		ret_rate = MGN_VHT3SS_MCS4;
		break;
	case DESC_RATEVHTSS3MCS5:
		ret_rate = MGN_VHT3SS_MCS5;
		break;
	case DESC_RATEVHTSS3MCS6:
		ret_rate = MGN_VHT3SS_MCS6;
		break;
	case DESC_RATEVHTSS3MCS7:
		ret_rate = MGN_VHT3SS_MCS7;
		break;
	case DESC_RATEVHTSS3MCS8:
		ret_rate = MGN_VHT3SS_MCS8;
		break;
	case DESC_RATEVHTSS3MCS9:
		ret_rate = MGN_VHT3SS_MCS9;
		break;
	case DESC_RATEVHTSS4MCS0:
		ret_rate = MGN_VHT4SS_MCS0;
		break;
	case DESC_RATEVHTSS4MCS1:
		ret_rate = MGN_VHT4SS_MCS1;
		break;
	case DESC_RATEVHTSS4MCS2:
		ret_rate = MGN_VHT4SS_MCS2;
		break;
	case DESC_RATEVHTSS4MCS3:
		ret_rate = MGN_VHT4SS_MCS3;
		break;
	case DESC_RATEVHTSS4MCS4:
		ret_rate = MGN_VHT4SS_MCS4;
		break;
	case DESC_RATEVHTSS4MCS5:
		ret_rate = MGN_VHT4SS_MCS5;
		break;
	case DESC_RATEVHTSS4MCS6:
		ret_rate = MGN_VHT4SS_MCS6;
		break;
	case DESC_RATEVHTSS4MCS7:
		ret_rate = MGN_VHT4SS_MCS7;
		break;
	case DESC_RATEVHTSS4MCS8:
		ret_rate = MGN_VHT4SS_MCS8;
		break;
	case DESC_RATEVHTSS4MCS9:
		ret_rate = MGN_VHT4SS_MCS9;
		break;

	default:
		break;
	}
	return ret_rate;
}

#endif

void dump_txrpt_ccx_92e(IN	u8 *CmdBuf)
{
	u8 MacID, Unicast, LifeTimeOver, RetryOver, DataRetryCount, QueueTimeUs, FinalDataRateIndex;

	RTW_INFO("============= %s ===========\n", __FUNCTION__);
	switch (GET_8192E_C2H_TX_RPT_QUEUE_SELECT(CmdBuf)) {
	/* AC Queue ------------------- */
	case 0x01:
	case 0x02:
		RTW_INFO("QSEL: BK_QUEUE ");
		break;
	case 0x00:
	case 0x03:
		RTW_INFO("QSEL: BE_QUEUE ");
		break;
	case 0x04:
	case 0x05:
		RTW_INFO("QSEL:VI_QUEUE ");
		break;
	case 0x06:
	case 0x07:
		RTW_INFO("QSEL:VO_QUEUE ");
		break;
	/* --------------------------- */
	case QSLT_BEACON:
		RTW_INFO("QSEL:BEACON_QUEUE ");
		break;
	case QSLT_HIGH:
		RTW_INFO("QSEL: HIGH_QUEUE ");
		break;
	case QSLT_MGNT:
		RTW_INFO("QSEL:MGNT_QUEUE ");
		break;
	case QSLT_CMD:
		RTW_INFO("QSEL:TXCMD_QUEUE ");
		break;

	default:
		RTW_INFO("QSEL:Invalid Queue Select ID !");
	}

	MacID = GET_8192E_C2H_TX_RPT_MAC_ID(CmdBuf);
	Unicast = GET_8192E_C2H_TX_RPT_PKT_BROCAST(CmdBuf);
	LifeTimeOver = GET_8192E_C2H_TX_RPT_LIFE_TIME_OVER(CmdBuf);
	RetryOver = GET_8192E_C2H_TX_RPT_RETRY_OVER(CmdBuf);
	DataRetryCount  = GET_8192E_C2H_TX_RPT_DATA_RETRY_CNT(CmdBuf);
	QueueTimeUs = GET_8192E_C2H_TX_RPT_QUEUE_TIME(CmdBuf) * USEC_UNIT_FOR_8192E_C2H_TX_RPT_QUEUE_TIME;
	FinalDataRateIndex = GET_8192E_C2H_TX_RPT_FINAL_DATA_RATE(CmdBuf);

	RTW_INFO("MacID:%u,Unicast:%u,LifeTimeOver:%u,RetryOver:%u,DataRetryCount:%u,QueueTimeUs:%u,FinalDataRateIndex:%u"
		, MacID, Unicast, LifeTimeOver, RetryOver, DataRetryCount, QueueTimeUs, FinalDataRateIndex);


}

static VOID
C2HTxFeedbackHandler_8192E(
	IN	PADAPTER	Adapter,
	IN	u8			*CmdBuf,
	IN	u8			CmdLen
)
{
#ifdef CONFIG_XMIT_ACK
	if (GET_8192E_C2H_TX_RPT_RETRY_OVER(CmdBuf) | GET_8192E_C2H_TX_RPT_LIFE_TIME_OVER(CmdBuf))
		rtw_ack_tx_done(&Adapter->xmitpriv, RTW_SCTX_DONE_CCX_PKT_FAIL);
	else
		rtw_ack_tx_done(&Adapter->xmitpriv, RTW_SCTX_DONE_SUCCESS);
#endif
#ifdef DBG_CCX
	dump_txrpt_ccx_92e(CmdBuf);
#endif

}

s32 c2h_handler_8192e(_adapter *adapter, u8 id, u8 seq, u8 plen, u8 *payload)
{
	s32 ret = _SUCCESS;

	switch (id) {
	case C2H_CCX_TX_RPT:
		C2HTxFeedbackHandler_8192E(adapter, payload, plen);
		break;
	default:
		ret = _FAIL;
		break;
	}

exit:
	return ret;
}

#ifdef CONFIG_BT_COEXIST
void rtl8192e_download_BTCoex_AP_mode_rsvd_page(PADAPTER padapter)
{
	rtl8192e_download_rsvd_page(padapter, RT_MEDIA_CONNECT);
}

#endif /* CONFIG_BT_COEXIST */
