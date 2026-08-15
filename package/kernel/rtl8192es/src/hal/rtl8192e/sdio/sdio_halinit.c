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
#define _SDIO_HALINIT_C_

#include <drv_types.h>
#include <rtl8192e_hal.h>
#include "hal_com_h2c.h"
#ifndef CONFIG_SDIO_HCI
	#error "CONFIG_SDIO_HCI shall be on!\n"
#endif

static void rtl8192es_interface_configure(PADAPTER padapter)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(padapter);
	struct dvobj_priv		*pdvobjpriv = adapter_to_dvobj(padapter);
	struct registry_priv	*pregistrypriv = &padapter->registrypriv;
	BOOLEAN		bWiFiConfig	= pregistrypriv->wifi_spec;


	pdvobjpriv->RtOutPipe[0] = WLAN_TX_HIQ_DEVICE_ID;
	pdvobjpriv->RtOutPipe[1] = WLAN_TX_MIQ_DEVICE_ID;
	pdvobjpriv->RtOutPipe[2] = WLAN_TX_LOQ_DEVICE_ID;

	if (bWiFiConfig)
		pHalData->OutEpNumber = 2;
	else
		pHalData->OutEpNumber = SDIO_MAX_TX_QUEUE;

	switch (pHalData->OutEpNumber) {
	case 3:
		pHalData->OutEpQueueSel = TX_SELE_HQ | TX_SELE_LQ | TX_SELE_NQ;
		break;
	case 2:
		pHalData->OutEpQueueSel = TX_SELE_HQ | TX_SELE_NQ;
		break;
	case 1:
		pHalData->OutEpQueueSel = TX_SELE_HQ;
		break;
	default:
		break;
	}

	Hal_MappingOutPipe(padapter, pHalData->OutEpNumber);
}

/*
 * Description:
 *	Call power on sequence to enable card
 *
 * Return:
 *	_SUCCESS	enable success
 *	_FAIL		enable fail
 */
static u8 _CardEnable(PADAPTER padapter)
{
	u8 bMacPwrCtrlOn;
	u8 ret;

	RTW_INFO("=>%s\n", __FUNCTION__);

	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (bMacPwrCtrlOn == _FALSE) {
#ifdef CONFIG_PLATFORM_SPRD
		u8 val8;
#endif /* CONFIG_PLATFORM_SPRD */

		/* RSV_CTRL 0x1C[7:0] = 0x00 */
		/* unlock ISO/CLK/Power control register */
		rtw_write8(padapter, REG_RSV_CTRL, 0x0);

#ifdef CONFIG_PLATFORM_SPRD
		val8 =  rtw_read8(padapter, 0x4);
		val8 = val8 & ~BIT(5);
		rtw_write8(padapter, 0x4, val8);
#endif /* CONFIG_PLATFORM_SPRD */

		ret = HalPwrSeqCmdParsing(padapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK, Rtl8192E_NIC_ENABLE_FLOW);
		if (ret == _SUCCESS) {
			u8 bMacPwrCtrlOn = _TRUE;
			rtw_hal_set_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
		} else {
			RTW_ERR("%s: run power on flow fail\n", __func__);
			return _FAIL;
		}

	} else {

		RTW_INFO("=>%s bMacPwrCtrlOn == _TRUE do nothing !!\n", __FUNCTION__);
		ret = _SUCCESS;
	}

	RTW_INFO("<=%s\n", __FUNCTION__);

	return ret;

}

static u32 _InitPowerOn_8192ES(PADAPTER padapter)
{
	u8 value8;
	u16 value16;
	u32 value32;
	u8 ret;

	RTW_INFO("=>%s\n", __FUNCTION__);

	/* all of these MUST be configured before power on */
#ifdef CONFIG_XTAL_26M
	/* Config PLL Reference CLK, */
	/* Change crystal to 26M, APLL_FREF_SEL = 4b'0101 */
	/* APLL_FREF_SEL[0]=1b'1 */
	value8 = rtw_read8(padapter, REG_AFE_PLL_CTRL);
	value8 |= BIT(2);
	rtw_write8(padapter, REG_AFE_PLL_CTRL, value8);
	/* APLL_FREF_SEL[2:1]=2b'10 */
	value8 = rtw_read8(padapter, REG_AFE_CTRL4_8192E+1);
	value8 &= ~(BIT(1) | BIT(0));
	value8 |= BIT(1);
	rtw_write16(padapter, REG_AFE_CTRL4_8192E+1, value8);
	/* APLL_FREF_SEL[3]=1b'0 */
	value8 = rtw_read8(padapter, REG_AFE_CTRL4_8192E);
	value8 &= ~BIT(7);
	rtw_write16(padapter, REG_AFE_CTRL4_8192E, value8);
#endif /* CONFIG_XTAL_26M */


	/*
		Ext XTAL : need clock request from platform.
		ex:platform Oscillator
		Internal XTAL :don't need extra clock request.
		ex:on board XTAL
	*/
#ifdef CONFIG_EXT_CLK
	/* Use external crystal(XTAL) */
	value8 = rtw_read8(padapter, REG_PAD_CTRL1_8192E+2);
	value8 |=  BIT(7);
	rtw_write8(padapter, REG_PAD_CTRL1_8192E+2, value8);

	/* CLK_REQ High active or Low Active */
	/* Request GPIO polarity: */
	/* 0: low active */
	/* 1: high active */
	/* need to discuss with customer */
	value8 = rtw_read8(padapter, REG_MULTI_FUNC_CTRL + 1);
	value8 |= BIT(5);
	rtw_write8(padapter, REG_MULTI_FUNC_CTRL + 1, value8);
#endif /* CONFIG_EXT_CLK */

	ret = _CardEnable(padapter);
	if (ret == _FAIL)
		return ret;

#if 0
	/*  Radio-Off Pin Trigger */
	value8 = rtw_read8(padapter, REG_GPIO_INTM + 1);
	value8 |= BIT(1); /*  Enable falling edge triggering interrupt */
	rtw_write8(padapter, REG_GPIO_INTM + 1, value8);
	value8 = rtw_read8(padapter, REG_GPIO_IO_SEL_2 + 1);
	value8 |= BIT(1);
	rtw_write8(padapter, REG_GPIO_IO_SEL_2 + 1, value8);
#endif

	/* Enable power down and GPIO interrupt */
	value16 = rtw_read16(padapter, REG_APS_FSMCO);
	value16 |= EnPDN; /* Enable HW power down and RF on */
	rtw_write16(padapter, REG_APS_FSMCO, value16);

	/* TODO: need to check */
	rtw_write16(padapter, REG_CR, 0x00);  /* suggseted by zhouzhou, by page, 20111230 */
	/* Enable MAC DMA/WMAC/SCHEDULE/SEC block */
	value16 = rtw_read16(padapter, REG_CR);
	value16 |= (HCI_TXDMA_EN | HCI_RXDMA_EN | TXDMA_EN | RXDMA_EN
		    | PROTOCOL_EN | SCHEDULE_EN | ENSEC | CALTMR_EN);
	/* for SDIO - Set CR bit10 to enable 32k calibration. Suggested by SD1 Gimmy. Added by tynli. 2011.08.31. */

	rtw_write16(padapter, REG_CR, value16);

	/* Enable CMD53 R/W Operation
	*	bMacPwrCtrlOn = TRUE;
	*	rtw_hal_set_hwreg(padapter, HW_VAR_APFM_ON_MAC, (pu8)(&bMacPwrCtrlOn)); */

	RTW_INFO("<=%s\n", __FUNCTION__);

	return _SUCCESS;

}

/* Tx Page FIFO threshold */
void _init_available_page_threshold(PADAPTER padapter, u8 numHQ, u8 numNQ, u8 numLQ, u8 numPubQ)
{
	u16	HQ_threshold, NQ_threshold, LQ_threshold;

	HQ_threshold = (numPubQ + numHQ + 1) >> 1;
	HQ_threshold |= (HQ_threshold << 8);

	NQ_threshold = (numPubQ + numNQ + 1) >> 1;
	NQ_threshold |= (NQ_threshold << 8);

	LQ_threshold = (numPubQ + numLQ + 1) >> 1;
	LQ_threshold |= (LQ_threshold << 8);

	rtw_write16(padapter, 0x218, HQ_threshold);
	rtw_write16(padapter, 0x21A, NQ_threshold);
	rtw_write16(padapter, 0x21C, LQ_threshold);
	RTW_INFO("%s(): Enable Tx FIFO Page Threshold H:0x%x,N:0x%x,L:0x%x\n", __FUNCTION__, HQ_threshold, NQ_threshold, LQ_threshold);
}

static void _InitQueueReservedPage_8192ESdio(PADAPTER padapter)
{
	_InitQueueReservedPage_8192E(padapter);
}

void sdio_AggSettingRxUpdate_8192ES(PADAPTER padapter)
{
#ifdef CONFIG_RX_AGGREGATION
	/* HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);	 */
	struct registry_priv *pregistrypriv = &padapter->registrypriv;
	u8	valueDMATimeout = 0;
	u8	valueDMAPageCount = 0;

	if (pregistrypriv->wifi_spec) {
		/* 2010.04.27 hpfan */
		/* Adjust RxAggrTimeout to close to zero disable RxAggr, suggested by designer */
		/* Timeout value is calculated by 34 / (2^n) */
		valueDMATimeout = 0x0f;
		valueDMAPageCount = 0x01;
	} else {
		valueDMATimeout = 0x10;/* th suggested by SD1-KY */
		valueDMAPageCount = 0x0d;
	}

	rtw_write8(padapter, REG_RXDMA_AGG_PG_TH + 1, valueDMATimeout);
	rtw_write8(padapter, REG_RXDMA_AGG_PG_TH, valueDMAPageCount);

	rtw_write8(padapter, REG_RXDMA_8192E, rtw_read8(padapter, REG_RXDMA_8192E) | BIT_DMA_MODE);
	rtw_write8(padapter, REG_TRXDMA_CTRL, (rtw_read8(padapter, REG_TRXDMA_CTRL) | RXDMA_AGG_EN));
#endif
}

void _initSdioAggregationSetting(PADAPTER padapter)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	/* Rx aggregation setting */
	sdio_AggSettingRxUpdate_8192ES(padapter);
}



void _InitInterrupt(PADAPTER padapter)
{

	/* HISR write one to clear */
	rtw_write32(padapter, REG_HISR0_8192E, 0xFFFFFFFF);
	rtw_write32(padapter, REG_HISR1_8192E, 0xFFFFFFFF);

	/* HIMR - turn all off */
	rtw_write32(padapter, REG_HIMR0_8192E, 0);
	rtw_write32(padapter, REG_HIMR1_8192E, 0);

	/*  */
	/* Initialize and enable SDIO Host Interrupt. */
	/*  */
	InitInterrupt8192ESdio(padapter);


	/*  */
	/* Initialize and enable system Host Interrupt. */
	/*  */
	/* InitSysInterrupt8192ESdio(Adapter); */ /* TODO: */

	/*  */
	/* Enable SDIO Host Interrupt. */
	/*  */
	/* EnableInterrupt8192ESdio(padapter); */ /* Move to sd_intf_start()/stop */

}





#if 0
static void _InitAntenna_Selection(PADAPTER padapter)
{
	rtw_write8(padapter, REG_LEDCFG2, 0x82);
}


static void _InitPABias(PADAPTER padapter)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(padapter);
	u8			pa_setting;
	BOOLEAN		is92C = IS_92C_SERIAL(pHalData->version_id);

	/* FIXED PA current issue */
	/* efuse_one_byte_read(padapter, 0x1FA, &pa_setting); */
	efuse_OneByteRead(padapter, 0x1FA, &pa_setting, _FALSE);


	if (!(pa_setting & BIT0)) {
		phy_set_rf_reg(padapter, RF_PATH_A, 0x15, 0x0FFFFF, 0x0F406);
		phy_set_rf_reg(padapter, RF_PATH_A, 0x15, 0x0FFFFF, 0x4F406);
		phy_set_rf_reg(padapter, RF_PATH_A, 0x15, 0x0FFFFF, 0x8F406);
		phy_set_rf_reg(padapter, RF_PATH_A, 0x15, 0x0FFFFF, 0xCF406);
	}

	if (!(pa_setting & BIT1) && is92C) {
		phy_set_rf_reg(padapter, RF_PATH_B, 0x15, 0x0FFFFF, 0x0F406);
		phy_set_rf_reg(padapter, RF_PATH_B, 0x15, 0x0FFFFF, 0x4F406);
		phy_set_rf_reg(padapter, RF_PATH_B, 0x15, 0x0FFFFF, 0x8F406);
		phy_set_rf_reg(padapter, RF_PATH_B, 0x15, 0x0FFFFF, 0xCF406);
	}

	if (!(pa_setting & BIT4)) {
		pa_setting = rtw_read8(padapter, 0x16);
		pa_setting &= 0x0F;
		rtw_write8(padapter, 0x16, pa_setting | 0x80);
		rtw_write8(padapter, 0x16, pa_setting | 0x90);
	}
}

VOID
_InitRDGSetting_8188E(
	IN	PADAPTER Adapter
)
{
	PlatformEFIOWrite1Byte(Adapter, REG_RD_CTRL, 0xFF);
	PlatformEFIOWrite2Byte(Adapter, REG_RD_NAV_NXT, 0x200);
	PlatformEFIOWrite1Byte(Adapter, REG_RD_RESP_PKT_TH, 0x05);
}
#endif

static u32 rtl8192es_hal_init(PADAPTER padapter)
{
	s32 ret;
	u8	txpktbuf_bndy;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(padapter);
	struct pwrctrl_priv		*pwrctrlpriv = adapter_to_pwrctl(padapter);
	struct registry_priv	*pregistrypriv = &padapter->registrypriv;
	rt_rf_power_state	eRfPowerStateToSet;
	u8 value8;
	u16 value16;

	systime init_start_time = rtw_get_current_time();

#ifdef DBG_HAL_INIT_PROFILING
	enum HAL_INIT_STAGES {
		HAL_INIT_STAGES_BEGIN = 0,
		HAL_INIT_STAGES_INIT_PW_ON,
		HAL_INIT_STAGES_MISC01,
		HAL_INIT_STAGES_DOWNLOAD_FW,
		HAL_INIT_STAGES_MAC,
		HAL_INIT_STAGES_BB,
		HAL_INIT_STAGES_RF,
		HAL_INIT_STAGES_EFUSE_PATCH,
		HAL_INIT_STAGES_INIT_LLTT,

		HAL_INIT_STAGES_MISC02,
		HAL_INIT_STAGES_TURN_ON_BLOCK,
		HAL_INIT_STAGES_INIT_SECURITY,
		HAL_INIT_STAGES_MISC11,
		HAL_INIT_STAGES_INIT_HAL_DM,
		/* HAL_INIT_STAGES_RF_PS, */
		HAL_INIT_STAGES_IQK,
		HAL_INIT_STAGES_PW_TRACK,
		HAL_INIT_STAGES_LCK,
		/* HAL_INIT_STAGES_MISC21, */
		HAL_INIT_STAGES_INIT_PABIAS,
		/* HAL_INIT_STAGES_ANTENNA_SEL, */
		HAL_INIT_STAGES_MISC31,
		HAL_INIT_STAGES_END,
		HAL_INIT_STAGES_NUM
	};

	char *hal_init_stages_str[] = {
		"HAL_INIT_STAGES_BEGIN",
		"HAL_INIT_STAGES_INIT_PW_ON",
		"HAL_INIT_STAGES_MISC01",
		"HAL_INIT_STAGES_DOWNLOAD_FW",
		"HAL_INIT_STAGES_MAC",
		"HAL_INIT_STAGES_BB",
		"HAL_INIT_STAGES_RF",
		"HAL_INIT_STAGES_EFUSE_PATCH",
		"HAL_INIT_STAGES_INIT_LLTT",
		"HAL_INIT_STAGES_MISC02",
		"HAL_INIT_STAGES_TURN_ON_BLOCK",
		"HAL_INIT_STAGES_INIT_SECURITY",
		"HAL_INIT_STAGES_MISC11",
		"HAL_INIT_STAGES_INIT_HAL_DM",
		/* "HAL_INIT_STAGES_RF_PS", */
		"HAL_INIT_STAGES_IQK",
		"HAL_INIT_STAGES_PW_TRACK",
		"HAL_INIT_STAGES_LCK",
		/* "HAL_INIT_STAGES_MISC21", */
		"HAL_INIT_STAGES_INIT_PABIAS"
		/* "HAL_INIT_STAGES_ANTENNA_SEL", */
		"HAL_INIT_STAGES_MISC31",
		"HAL_INIT_STAGES_END",
	};


	int hal_init_profiling_i;
	systime hal_init_stages_timestamp[HAL_INIT_STAGES_NUM]; /* used to record the time of each stage's starting point */

	for (hal_init_profiling_i = 0; hal_init_profiling_i < HAL_INIT_STAGES_NUM; hal_init_profiling_i++)
		hal_init_stages_timestamp[hal_init_profiling_i] = 0;

#define HAL_INIT_PROFILE_TAG(stage) do { hal_init_stages_timestamp[(stage)] = rtw_get_current_time(); } while (0)
#else
#define HAL_INIT_PROFILE_TAG(stage) do {} while (0)
#endif /* DBG_HAL_INIT_PROFILING */

	RTW_INFO("+%s\n", __FUNCTION__);

	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_BEGIN);
	/* Disable Interrupt first.
	*	rtw_hal_disable_interrupt(padapter);
	*	DisableInterrupt8188ESdio(padapter); */


	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_INIT_PW_ON);
	ret = rtw_hal_power_on(padapter);
	if (_FAIL == ret) {
		goto exit;
	}

	ret = sdio_power_on_check(padapter);
	if (_FAIL == ret) {
		RTW_INFO("Power on Fail! do it again\n");
		ret = rtw_hal_power_on(padapter);
		if (_FAIL == ret) {
			RTW_INFO("Failed to init Power On!\n");
			goto exit;
		}
	}
	RTW_INFO("Power on ok!\n");


	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_MISC01);
	txpktbuf_bndy = TX_PAGE_BOUNDARY_8192E;

	_InitQueueReservedPage_8192ESdio(padapter);
	_InitQueuePriority_8192E(padapter);
	_InitPageBoundary_8192E(padapter);

	/*  */
	/* Configure SDIO TxRx Control to enable Rx DMA timer masking. */
	/* 2010.02.24. */
	/*  */
	value8 = SdioLocalCmd52Read1Byte(padapter, SDIO_REG_TX_CTRL);
	SdioLocalCmd52Write1Byte(padapter, SDIO_REG_TX_CTRL, 0x02);

	rtw_write8(padapter, SDIO_LOCAL_BASE | SDIO_REG_HRPWM1, 0);
	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_DOWNLOAD_FW);
	if (padapter->registrypriv.mp_mode == 0) {
		ret = FirmwareDownload8192E(padapter, _FALSE);
		if (ret != _SUCCESS) {
			RTW_INFO("%s: Download Firmware failed!!\n", __FUNCTION__);
			pHalData->bFWReady = _FALSE;
			pHalData->fw_ractrl = _FALSE;
			goto exit;
		} else {
			pHalData->bFWReady = _TRUE;
			pHalData->fw_ractrl = _TRUE;
		}
	}

	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_MAC);
#if (HAL_MAC_ENABLE == 1)
	ret = PHY_MACConfig8192E(padapter);
	if (ret != _SUCCESS) {
		goto exit;
	}
#endif

	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_BB);
	/*  */
	/* d. Initialize BB related configurations. */
	/*  */
#if (HAL_BB_ENABLE == 1)
	ret = PHY_BBConfig8192E(padapter);
	if (ret != _SUCCESS) {
		goto exit;
	}
#endif


	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_RF);

#if (HAL_RF_ENABLE == 1)
	ret = PHY_RFConfig8192E(padapter);

	if (ret != _SUCCESS) {
		goto exit;
	}
	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_EFUSE_PATCH);
	_InitTxBufferBoundary_8192E(padapter, txpktbuf_bndy);

	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_INIT_LLTT);
	ret = InitLLTTable8192E(padapter, txpktbuf_bndy);
	if (_SUCCESS != ret) {
		goto exit;
	}

	if (pHalData->bRDGEnable)
		_InitRDGSetting_8192E(padapter);

#ifdef CONFIG_TX_EARLY_MODE

	if (pHalData->AMPDUBurstMode) {

		value8 = rtw_read8(padapter, REG_EARLY_MODE_CONTROL_8192E);
#if RTL8192E_EARLY_MODE_PKT_NUM_10 == 1
		value8 = value8 | 0x1f;
#else
		value8 = value8 | 0xf;
#endif
		rtw_write8(padapter, REG_EARLY_MODE_CONTROL_8192E, value8);

		rtw_write8(padapter, REG_EARLY_MODE_CONTROL_8192E+3, 0x80);

		value8 = rtw_read8(padapter, REG_TCR + 1);
		value8 = value8 | 0x40;
		rtw_write8(padapter, REG_TCR + 1, value8);
	} else {
		/* rtw_write8(Adapter, REG_EARLY_MODE_CONTROL, 0); */
	}
#endif /* CONFIG_TX_EARLY_MODE */


	if (pwrctrlpriv->reg_rfoff == _TRUE)
		pwrctrlpriv->rf_pwrstate = rf_off;

	/* 2010/08/09 MH We need to check if we need to turnon or off RF after detecting */
	/* HW GPIO pin. Before PHY_RFConfig8192C. */
	HalDetectPwrDownMode8192E(padapter);

	/* Save target channel */
	/* <Roger_Notes> Current Channel will be updated again later. */
	pHalData->current_channel = 1;



	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_MISC02);
	/* Get Rx PHY status in order to report RSSI and others. */
	_InitDriverInfoSize_8192E(padapter, DRVINFO_SZ);
	_InitNetworkType_8192E(padapter);
	_InitWMACSetting_8192E(padapter);
	_InitAdaptiveCtrl_8192E(padapter);
	_InitEDCA_8192E(padapter);
	_InitRetryFunction_8192E(padapter);
	_initSdioAggregationSetting(padapter);

	_InitBeaconParameters_8192E(padapter);
	_InitBeaconMaxError_8192E(padapter, _TRUE);
	_InitInterrupt(padapter);
	/* Enable MACTXEN/MACRXEN block */
	value16 = rtw_read16(padapter, REG_CR);
	value16 |= (MACTXEN | MACRXEN);
	rtw_write8(padapter, REG_CR, value16);

#if defined(CONFIG_CONCURRENT_MODE) || defined(CONFIG_TX_MCAST2UNI)

#ifdef CONFIG_CHECK_AC_LIFETIME
	/* Enable lifetime check for the four ACs */
	rtw_write8(padapter, REG_LIFETIME_CTRL, rtw_read8(padapter, REG_LIFETIME_CTRL) | 0x0f);
#endif /* CONFIG_CHECK_AC_LIFETIME	 */

#ifdef CONFIG_TX_MCAST2UNI
	rtw_write16(padapter, REG_PKT_VO_VI_LIFE_TIME, 0x0400);	/* unit: 256us. 256ms */
	rtw_write16(padapter, REG_PKT_BE_BK_LIFE_TIME, 0x0400);	/* unit: 256us. 256ms */
#else	/* CONFIG_TX_MCAST2UNI */
	rtw_write16(padapter, REG_PKT_VO_VI_LIFE_TIME, 0x3000);	/* unit: 256us. 3s */
	rtw_write16(padapter, REG_PKT_BE_BK_LIFE_TIME, 0x3000);	/* unit: 256us. 3s */
#endif /* CONFIG_TX_MCAST2UNI */
#endif /* CONFIG_CONCURRENT_MODE || CONFIG_TX_MCAST2UNI */

#endif /* HAL_RF_ENABLE == 1 */


	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_TURN_ON_BLOCK);
	_BBTurnOnBlock_8192E(padapter);

	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_INIT_SECURITY);
#if 1
	invalidate_cam_all(padapter);
#else
	CamResetAllEntry(padapter);
	padapter->hal_func.EnableHWSecCfgHandler(padapter);
#endif
	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_MISC11);
	/* 2010/12/17 MH We need to set TX power according to EFUSE content at first. */
	rtw_hal_set_chnl_bw(padapter, padapter->registrypriv.channel,
		CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE, HAL_PRIME_CHNL_OFFSET_DONT_CARE);
	/*  */
	/* Disable BAR, suggested by Scott */
	/* 2010.04.09 add by hpfan */
	/*  */
	rtw_write32(padapter, REG_BAR_MODE_CTRL, 0x0201ffff);

	/* HW SEQ CTRL */
	/* set 0x0 to 0xFF by tynli. Default enable HW SEQ NUM. */
	rtw_write8(padapter, REG_HWSEQ_CTRL, 0xFF);

	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_INIT_HAL_DM);
	rtl8192e_InitHalDm(padapter);

#ifdef CONFIG_BT_COEXIST
	if (pHalData->EEPROMBluetoothCoexist == 1) {
		/* Init BT hw config.*/
		rtw_btcoex_HAL_Initialize(padapter, _FALSE);
	}
#endif

#if (MP_DRIVER == 1)
	if (padapter->registrypriv.mp_mode == 1) {
		padapter->mppriv.channel = pHalData->current_channel;
		MPT_InitializeAdapter(padapter, padapter->mppriv.channel);
	} else
#endif /* (MP_DRIVER == 1) */
	{
		/*  */
		/* 2010/08/11 MH Merge from 8192SE for Minicard init. We need to confirm current radio status */
		/* and then decide to enable RF or not.!!!??? For Selective suspend mode. We may not */
		/* call init_adapter. May cause some problem?? */
		/*  */
		/* Fix the bug that Hw/Sw radio off before S3/S4, the RF off action will not be executed */
		/* in MgntActSet_RF_State() after wake up, because the value of pHalData->eRFPowerState */
		/* is the same as eRfOff, we should change it to eRfOn after we config RF parameters. */
		/* Added by tynli. 2010.03.30. */
		pwrctrlpriv->rf_pwrstate = rf_on;
		RT_CLEAR_PS_LEVEL(pwrctrlpriv, RT_RF_OFF_LEVL_HALT_NIC);

		HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_IQK);
		if (pwrctrlpriv->rf_pwrstate == rf_on) {
			pHalData->bNeedIQK = _TRUE;
			if (pHalData->bIQKInitialized)
				halrf_iqk_trigger(&pHalData->odmpriv, _TRUE);
				/*phy_iq_calibrate_8192e(padapter, _TRUE);*/
			else {
				/*phy_iq_calibrate_8192e(padapter, _FALSE);*/
				halrf_iqk_trigger(&pHalData->odmpriv, _FALSE);
				pHalData->bIQKInitialized = _TRUE;
			}
			HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_PW_TRACK);

			odm_txpowertracking_check(&pHalData->odmpriv);


			HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_LCK);
			/* phy_lc_calibrate_8192e((GET_HAL_DATA(Adapter)->odmpriv));		 */
		}

	}
#ifdef CONFIG_HIGH_CHAN_SUPER_CALIBRATION
	rtw_hal_set_chnl_bw(padapter, 13,
		CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE, HAL_PRIME_CHNL_OFFSET_DONT_CARE);

	PHY_SpurCalibration_8192E(padapter);

	rtw_hal_set_chnl_bw(padapter, padapter->registrypriv.channel,
		CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE, HAL_PRIME_CHNL_OFFSET_DONT_CARE);
#endif

	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_INIT_PABIAS);

	/* Init BT hw config. */
	/* HALBT_InitHwConfig(padapter); */


	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_MISC31);
	/* 2010/05/20 MH We need to init timer after update setting. Otherwise, we can not get correct inf setting. */
	/* 2010/05/18 MH For SE series only now. Init GPIO detect time */
#if 0
	if (pDevice->RegUsbSS) {
		GpioDetectTimerStart(padapter);	/* Disable temporarily */
	}
#endif

	/* 2010/08/23 MH According to Alfred's suggestion, we need to to prevent HW enter */
	/* suspend mode automatically. */
	/* HwSuspendModeEnable92Cu(padapter, FALSE); */

	/* 2010/12/17 MH For TX power level OID modification from UI.
	*	padapter->HalFunc.GetTxPowerLevelHandler( padapter, &pHalData->DefaultTxPwrDbm ); */
	/* dbg_print("pHalData->DefaultTxPwrDbm = %d\n", pHalData->DefaultTxPwrDbm); */

	/* if(pHalData->SwBeaconType < HAL92CSDIO_DEFAULT_BEACON_TYPE) */ /* The lowest Beacon Type that HW can support */
	/*		pHalData->SwBeaconType = HAL92CSDIO_DEFAULT_BEACON_TYPE; */

	/*  */
	/* Update current Tx FIFO page status. */
	/*  */
	HalQueryTxBufferStatus8192ESdio(padapter);
	HalQueryTxOQTBufferStatus8192ESdio(padapter);
	pHalData->SdioTxOQTMaxFreeSpace = pHalData->SdioTxOQTFreeSpace;
	PHY_SetRFEReg_8192E(padapter);
	if (pregistrypriv->wifi_spec) {
		rtw_write16(padapter, REG_FAST_EDCA_CTRL , 0);

		/*Nav limit , suggest by SD1-Pisa,disable NAV_UPPER function when wifi_spec=1 for Test item: 5.2.3*/
		rtw_write8(padapter, REG_NAV_UPPER, 0x0);
	}

	/* TODO:Setting HW_VAR_NAV_UPPER !!!!!!!!!!!!!!!!!!!! */
	/* rtw_hal_set_hwreg(Adapter, HW_VAR_NAV_UPPER, ((pu1Byte)&NavUpper)); */

	/* pHalData->PreRpwmVal = PlatformEFSdioLocalCmd52Read1Byte(Adapter, SDIO_REG_HRPWM1)&0x80; */


	/* enable Tx report. */
	/* rtw_write8(padapter,  REG_FWHW_TXQ_CTRL+1, 0x0F); */
#if 0
	/*  Suggested by SD1 pisa. Added by tynli. 2011.10.21. */
	PlatformEFIOWrite1Byte(Adapter, REG_EARLY_MODE_CONTROL + 3, 0x01);

#endif
	/* tynli_test_tx_report. */
	/* rtw_write16(padapter, REG_TX_RPT_TIME, 0x3DF0); */


	/* enable tx DMA to drop the redundate data of packet */
	rtw_write16(padapter, REG_TXDMA_OFFSET_CHK, (rtw_read16(padapter, REG_TXDMA_OFFSET_CHK) | DROP_DATA_EN));

#ifdef CONFIG_PLATFORM_SPRD
	/* For Power Consumption, set all GPIO pin to ouput mode */
	/* 0x44~0x47 (GPIO 0~7), Note:GPIO5 is enabled for controlling external 26MHz request */
	rtw_write8(padapter, GPIO_IO_SEL, 0xFF);/* Reg0x46, set to o/p mode */

	/* 0x42~0x43 (GPIO 8~11) */
	value8 = rtw_read8(padapter, REG_GPIO_IO_SEL);
	rtw_write8(padapter, REG_GPIO_IO_SEL, (value8 << 4) | value8);
	value8 = rtw_read8(padapter, REG_GPIO_IO_SEL + 1);
	rtw_write8(padapter, REG_GPIO_IO_SEL + 1, value8 | 0x0F); /* Reg0x43 */
#endif /* CONFIG_PLATFORM_SPRD */


#ifdef CONFIG_XMIT_ACK
	/* ack for xmit mgmt frames. */
	rtw_write32(padapter, REG_FWHW_TXQ_CTRL, rtw_read32(padapter, REG_FWHW_TXQ_CTRL) | BIT(12));
#endif /* CONFIG_XMIT_ACK */

	RTW_INFO("-%s\n", __FUNCTION__);

exit:
	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_END);

	RTW_INFO("%s in %dms\n", __FUNCTION__, rtw_get_passing_time_ms(init_start_time));

#ifdef DBG_HAL_INIT_PROFILING
	hal_init_stages_timestamp[HAL_INIT_STAGES_END] = rtw_get_current_time();

	for (hal_init_profiling_i = 0; hal_init_profiling_i < HAL_INIT_STAGES_NUM - 1; hal_init_profiling_i++) {
		RTW_INFO("DBG_HAL_INIT_PROFILING: %35s, %u, %5u, %5u\n"
			 , hal_init_stages_str[hal_init_profiling_i]
			 , hal_init_stages_timestamp[hal_init_profiling_i]
			, (hal_init_stages_timestamp[hal_init_profiling_i + 1] - hal_init_stages_timestamp[hal_init_profiling_i])
			, rtw_get_time_interval_ms(hal_init_stages_timestamp[hal_init_profiling_i], hal_init_stages_timestamp[hal_init_profiling_i + 1])
			);
	}
#endif

	return ret;
}

static void hal_poweroff_8192es(PADAPTER padapter)
{
	u8		u1bTmp;
	u16		u2bTmp;
	u32		u4bTmp;
	u8		bMacPwrCtrlOn = _FALSE;
	u8		ret;

#ifdef CONFIG_PLATFORM_SPRD
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
#endif /* CONFIG_PLATFORM_SPRD	 */

	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (bMacPwrCtrlOn == _FALSE) {
		RTW_INFO("=>%s bMacPwrCtrlOn == _FALSE return !!\n", __FUNCTION__);
		return;
	}
	RTW_INFO("=>%s\n", __FUNCTION__);


	/* Stop Tx Report Timer. 0x4EC[Bit1]=b'0 */
	u1bTmp = rtw_read8(padapter, REG_TX_RPT_CTRL);
	rtw_write8(padapter, REG_TX_RPT_CTRL, u1bTmp & (~BIT1));

	/* stop rx */
	rtw_write8(padapter, REG_CR, 0x0);

#if 1
	/* For Power Consumption. */
	u1bTmp = rtw_read8(padapter, GPIO_IN);
	rtw_write8(padapter, GPIO_OUT, u1bTmp);
	rtw_write8(padapter, GPIO_IO_SEL, 0xFF);/* Reg0x46 */

	u1bTmp = rtw_read8(padapter, REG_GPIO_IO_SEL);
	rtw_write8(padapter, REG_GPIO_IO_SEL, (u1bTmp << 4) | u1bTmp);
	u1bTmp = rtw_read8(padapter, REG_GPIO_IO_SEL + 1);
	rtw_write8(padapter, REG_GPIO_IO_SEL + 1, u1bTmp | 0x0F); /* Reg0x43 */
#endif


	/* Run LPS WL RFOFF flow	 */
	ret = HalPwrSeqCmdParsing(padapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK, Rtl8192E_NIC_LPS_ENTER_FLOW);
	if (ret == _FALSE)
		RTW_INFO("%s: run RF OFF flow fail!\n", __func__);

	/*	==== Reset digital sequence   ====== */

	u1bTmp = rtw_read8(padapter, REG_MCUFWDL);
	if ((u1bTmp & RAM_DL_SEL) && GET_HAL_DATA(padapter)->bFWReady) { /* 8051 RAM code */
		/* Reset MCU 0x2[10]=0. */
		u1bTmp = rtw_read8(padapter, REG_SYS_FUNC_EN + 1);
		u1bTmp &= ~BIT(2);	/* 0x2[10], FEN_CPUEN */
		rtw_write8(padapter, REG_SYS_FUNC_EN + 1, u1bTmp);
	}

	/* u1bTmp = rtw_read8(padapter, REG_SYS_FUNC_EN+1); */
	/* u1bTmp &= ~BIT(2);	 */ /* 0x2[10], FEN_CPUEN */
	/* rtw_write8(padapter, REG_SYS_FUNC_EN+1, u1bTmp); */

	/* MCUFWDL 0x80[1:0]=0 */
	/* reset MCU ready status */
	rtw_write8(padapter, REG_MCUFWDL, 0);

	/* ==== Reset digital sequence end ====== */


	bMacPwrCtrlOn = _FALSE;	/* Disable CMD53 R/W */
	rtw_hal_set_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);


#if 0
	if ((pMgntInfo->RfOffReason & RF_CHANGE_BY_HW) && pHalData->pwrdown) {
		/*  Power Down */

		/*  Card disable power action flow */
		ret = HalPwrSeqCmdParsing(Adapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK, Rtl8192E_NIC_PDN_FLOW);
	} else
#endif

	{ /* Non-Power Down */

		/* Card disable power action flow */
		ret = HalPwrSeqCmdParsing(padapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_SDIO_MSK, Rtl8192E_NIC_DISABLE_FLOW);


		if (ret == _FALSE)
			RTW_INFO("%s: run CARD DISABLE flow fail!\n", __func__);
	}


#if 0
	/*  Reset MCU IO Wrapper, added by Roger, 2011.08.30 */
	u1bTmp = rtw_read8(padapter, REG_RSV_CTRL + 1);
	u1bTmp &= ~BIT(0);
	rtw_write8(padapter, REG_RSV_CTRL + 1, u1bTmp);
	u1bTmp = rtw_read8(padapter, REG_RSV_CTRL + 1);
	u1bTmp |= BIT(0);
	rtw_write8(padapter, REG_RSV_CTRL + 1, u1bTmp);
#endif


	/* RSV_CTRL 0x1C[7:0]=0x0E */
	/* lock ISO/CLK/Power control register */
	rtw_write8(padapter, REG_RSV_CTRL, 0x0E);

	GET_HAL_DATA(padapter)->bFWReady = _FALSE;
	bMacPwrCtrlOn = _FALSE;
	rtw_hal_set_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);

	RTW_INFO("<=%s\n", __FUNCTION__);

}

static u32 rtl8192es_hal_deinit(PADAPTER padapter)
{
	RTW_INFO("=>%s\n", __FUNCTION__);

	if (rtw_is_hw_init_completed(padapter))
		rtw_hal_power_off(padapter);

	RTW_INFO("<=%s\n", __FUNCTION__);

	return _SUCCESS;
}

static void rtl8192es_init_default_value(PADAPTER padapter)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(padapter);
	rtl8192e_init_default_value(padapter);

	/* interface related variable */
	pHalData->SdioRxFIFOCnt = 0;
}

/*
 *	Description:
 *		We should set Efuse cell selection to WiFi cell in default.
 *
 *	Assumption:
 *		PASSIVE_LEVEL
 *
 *	Added by Roger, 2010.11.23.
 *   */
static void _EfuseCellSel(
	IN	PADAPTER	padapter
)
{
	/* HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter); */

	u32			value32;

	/* if(INCLUDE_MULTI_FUNC_BT(padapter)) */
	{
		value32 = rtw_read32(padapter, EFUSE_TEST);
		value32 = (value32 & ~EFUSE_SEL_MASK) | EFUSE_SEL(EFUSE_WIFI_SEL_0);
		rtw_write32(padapter, EFUSE_TEST, value32);
	}
}

static void
Hal_EfuseParsePIDVID_8192ES(
	IN	PADAPTER		pAdapter,
	IN	u8			*hwinfo,
	IN	BOOLEAN			AutoLoadFail
)
{
	/*	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter); */

	/*  */
	/* <Roger_Notes> The PID/VID info was parsed from CISTPL_MANFID Tuple in CIS area before. */
	/* VID is parsed from Manufacture code field and PID is parsed from Manufacture information field. */
	/* 2011.04.01. */
	/*  */

}

static VOID
readAdapterInfo_8192ES(
	IN PADAPTER			padapter
)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(padapter);

	/* parse the eeprom/efuse content */
	Hal_EfuseParseIDCode8192E(padapter, pHalData->efuse_eeprom_data);
	Hal_EfuseParsePIDVID_8192ES(padapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	hal_config_macaddr(padapter, pHalData->bautoload_fail_flag);
	Hal_ReadPowerSavingMode8192E(padapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadTxPowerInfo8192E(padapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadPROMVersion8192E(padapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadChannelPlan8192E(padapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_EfuseParseXtal_8192E(padapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	/* Hal_EfuseParseCustomerID88E(padapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag); */
	Hal_ReadAntennaDiversity8192E(padapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadPAType_8192E(padapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadAmplifierType_8192E(padapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadRFEType_8192E(padapter, pHalData->efuse_eeprom_data,  pHalData->bautoload_fail_flag);
	Hal_ReadBoardType8192E(padapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadThermalMeter_8192E(padapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_EfuseParseBTCoexistInfo8192E(padapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);

#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
	Hal_DetectWoWMode(padapter);
#endif
	Hal_EfuseParseKFreeData_8192E(padapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);

	/* set coex. ant info once efuse parsing is done */
	rtw_btcoex_set_ant_info(padapter);
}

static void _ReadPROMContent(
	IN PADAPTER		padapter
)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(padapter);
	u8			eeValue;

	/* check system boot selection */
	eeValue = rtw_read8(padapter, REG_9346CR);
	pHalData->EepromOrEfuse = (eeValue & BOOT_FROM_EEPROM) ? _TRUE : _FALSE;
	pHalData->bautoload_fail_flag = (eeValue & EEPROM_EN) ? _FALSE : _TRUE;

	RTW_INFO("%s: 9346CR=0x%02X, Boot from %s, Autoload %s\n",
		 __FUNCTION__, eeValue,
		 (pHalData->EepromOrEfuse ? "EEPROM" : "EFUSE"),
		 (pHalData->bautoload_fail_flag ? "Fail" : "OK"));

	/*	pHalData->EEType = IS_BOOT_FROM_EEPROM(Adapter) ? EEPROM_93C46 : EEPROM_BOOT_EFUSE; */

	hal_InitPGData_8192E(padapter, pHalData->efuse_eeprom_data);
	readAdapterInfo_8192ES(padapter);
}
/*
 *	Description:
 *		Read HW adapter information by E-Fuse or EEPROM according CR9346 reported.
 *
 *	Assumption:
 *		PASSIVE_LEVEL (SDIO interface)
 *
 *   */
static u8 ReadAdapterInfo8192ES(PADAPTER padapter)
{
	/* Read EEPROM size before call any EEPROM function */
	padapter->EepromAddressSize = GetEEPROMSize8192E(padapter);

	/*	Efuse_InitSomeVar(Adapter);
	 *	_EfuseCellSel(padapter); */

	/* Read all content in Efuse/EEPROM. */
	_ReadPROMContent(padapter);

	/* We need to define the RF type after all PROM value is recognized. */
	hal_ReadRFType_8192E(padapter);

	return _SUCCESS;
}

static u8 SetHwReg8192ES(PADAPTER Adapter, u8 variable, u8 *val)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8 ret = _SUCCESS;

	switch (variable) {
	case HW_VAR_RXDMA_AGG_PG_TH: {
		u8	threshold = *((u8 *)val);
		if (threshold != 0)
			rtw_write8(Adapter, REG_RXDMA_AGG_PG_TH, threshold);
	}
	break;

	case HW_VAR_SET_RPWM:
#ifdef CONFIG_LPS_LCLK
		{
			u8	ps_state = *((u8 *)val);
			/* rpwm value only use BIT0(clock bit) ,BIT6(Ack bit), and BIT7(Toggle bit) for 88e. */
			/* BIT0 value - 1: 32k, 0:40MHz. */
			/* BIT6 value - 1: report cpwm value after success set, 0:do not report. */
			/* BIT7 value - Toggle bit change. */
			/* modify by Thomas. 2012/4/2. */
			ps_state = ps_state & 0xC1;

			/* RTW_INFO("##### Change RPWM value to = %x for switch clk #####\n",ps_state); */
			rtw_write8(Adapter, SDIO_LOCAL_BASE | SDIO_REG_HRPWM1, ps_state);
		}
#endif
		break;
	default:
		ret = SetHwReg8192E(Adapter, variable, val);
		break;
	}

	return ret;
}

static void GetHwReg8192ES(PADAPTER padapter, u8 variable, u8 *val)
{
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(padapter);

	switch (variable) {
	case HW_VAR_CPWM:
		*val = rtw_read8(padapter, SDIO_LOCAL_BASE | SDIO_REG_HCPWM1);
		break;
	default:
		GetHwReg8192E(padapter, variable, val);
		break;
	}

}

/*
 *	Description:
 *		Query setting of specified variable.
 *   */
u8
GetHalDefVar8192ESDIO(
	IN	PADAPTER				Adapter,
	IN	HAL_DEF_VARIABLE		eVariable,
	IN	PVOID					pValue
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8			bResult = _SUCCESS;

	switch (eVariable) {
	case HW_VAR_MAX_RX_AMPDU_FACTOR:
		*((u32 *)pValue) = MAX_AMPDU_FACTOR_16K;
		break;
	default:
		bResult = GetHalDefVar8192E(Adapter, eVariable, pValue);
		break;
	}

	return bResult;
}




/*
 *	Description:
 *		Change default setting of specified variable.
 *   */
u8
SetHalDefVar8192ESDIO(
	IN	PADAPTER				Adapter,
	IN	HAL_DEF_VARIABLE		eVariable,
	IN	PVOID					pValue
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8			bResult = _TRUE;

	switch (eVariable) {
	default:
		bResult = SetHalDefVar(Adapter, eVariable, pValue);
		break;
	}

	return bResult;
}




void rtl8192es_set_hal_ops(PADAPTER padapter)
{
	struct hal_ops *pHalFunc = &padapter->hal_func;


	pHalFunc->hal_power_on = _InitPowerOn_8192ES;
	pHalFunc->hal_power_off = hal_poweroff_8192es;

	pHalFunc->hal_init = &rtl8192es_hal_init;
	pHalFunc->hal_deinit = &rtl8192es_hal_deinit;

	pHalFunc->init_xmit_priv = &rtl8192es_init_xmit_priv;
	pHalFunc->free_xmit_priv = &rtl8192es_free_xmit_priv;

	pHalFunc->init_recv_priv = &rtl8192es_init_recv_priv;
	pHalFunc->free_recv_priv = &rtl8192es_free_recv_priv;
#ifdef CONFIG_RECV_THREAD_MODE
        pHalFunc->recv_hdl = rtl8192es_recv_hdl;
#endif /* CONFIG_RECV_THREAD_MODE */
#ifdef CONFIG_RTW_SW_LED
	pHalFunc->InitSwLeds = &rtl8192es_InitSwLeds;
	pHalFunc->DeInitSwLeds = &rtl8192es_DeInitSwLeds;
#endif/* CONFIG_RTW_SW_LED */

	pHalFunc->init_default_value = &rtl8192es_init_default_value;

	pHalFunc->intf_chip_configure = &rtl8192es_interface_configure;
	pHalFunc->read_adapter_info = &ReadAdapterInfo8192ES;

	pHalFunc->enable_interrupt = &EnableInterrupt8192ESdio;
	pHalFunc->disable_interrupt = &DisableInterrupt8192ESdio;
	pHalFunc->check_ips_status = &CheckIPSStatus;

#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
	pHalFunc->clear_interrupt = &ClearInterrupt8192ESdio;
#endif
	pHalFunc->set_hw_reg_handler = &SetHwReg8192ES;
	pHalFunc->GetHwRegHandler = &GetHwReg8192ES;

	pHalFunc->get_hal_def_var_handler = &GetHalDefVar8192ESDIO;
	pHalFunc->SetHalDefVarHandler = &SetHalDefVar8192ESDIO;

	pHalFunc->hal_xmit = &rtl8192es_hal_xmit;
	pHalFunc->mgnt_xmit = &rtl8192es_mgnt_xmit;
	pHalFunc->hal_xmitframe_enqueue = &rtl8192es_hal_xmitframe_enqueue;

#ifdef CONFIG_HOSTAPD_MLME
	pHalFunc->hostap_mgnt_xmit_entry = NULL;
#endif
#ifdef CONFIG_XMIT_THREAD_MODE
	pHalFunc->xmit_thread_handler = &rtl8192es_xmit_buf_handler;
#endif
	rtl8192e_set_hal_ops(pHalFunc);

}
