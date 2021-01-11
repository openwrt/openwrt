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
#define _HCI_HAL_INIT_C_

/* #include <drv_types.h> */
#include <rtl8192e_hal.h>


#ifndef CONFIG_USB_HCI

	#error "CONFIG_USB_HCI shall be on!\n"

#endif


static void _dbg_dump_macreg(_adapter *padapter)
{
	u32 offset = 0;
	u32 val32 = 0;
	u32 index = 0 ;
	for (index = 0; index < 64; index++) {
		offset = index * 4;
		val32 = rtw_read32(padapter, offset);
		RTW_INFO("offset : 0x%02x ,val:0x%08x\n", offset, val32);
	}
}

static VOID
_ConfigChipOutEP_8192E(
	IN	PADAPTER	pAdapter,
	IN	u8		NumOutPipe
)
{
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(pAdapter);


	pHalData->OutEpQueueSel = 0;
	pHalData->OutEpNumber = 0;

	switch (NumOutPipe) {
	case	4:
		pHalData->OutEpQueueSel = TX_SELE_HQ | TX_SELE_LQ | TX_SELE_NQ | TX_SELE_EQ;
		pHalData->OutEpNumber = 4;
		break;
	case	3:
		pHalData->OutEpQueueSel = TX_SELE_HQ | TX_SELE_LQ | TX_SELE_NQ;
		pHalData->OutEpNumber = 3;
		break;
	case	2:
		pHalData->OutEpQueueSel = TX_SELE_HQ | TX_SELE_NQ;
		pHalData->OutEpNumber = 2;
		break;
	case	1:
		pHalData->OutEpQueueSel = TX_SELE_HQ;
		pHalData->OutEpNumber = 1;
		break;
	default:
		break;

	}
	RTW_INFO("%s OutEpQueueSel(0x%02x), OutEpNumber(%d)\n", __FUNCTION__, pHalData->OutEpQueueSel, pHalData->OutEpNumber);

}

static BOOLEAN HalUsbSetQueuePipeMapping8192EUsb(
	IN	PADAPTER	pAdapter,
	IN	u8		NumInPipe,
	IN	u8		NumOutPipe
)
{
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(pAdapter);
	BOOLEAN			result		= _FALSE;

	_ConfigChipOutEP_8192E(pAdapter, NumOutPipe);

	/* Normal chip with one IN and one OUT doesn't have interrupt IN EP. */
	if (1 == pHalData->OutEpNumber) {
		if (1 != NumInPipe)
			return result;
	}

	/* All config other than above support one Bulk IN and one Interrupt IN. */
	/* if(2 != NumInPipe){ */
	/*	return result; */
	/* } */

	result = Hal_MappingOutPipe(pAdapter, NumOutPipe);

	return result;

}

void rtl8192eu_interface_configure(_adapter *padapter)
{
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(padapter);
	struct dvobj_priv	*pdvobjpriv = adapter_to_dvobj(padapter);
	struct registry_priv  *registry_par = &padapter->registrypriv;

	if (IS_HIGH_SPEED_USB(padapter)) {
		pHalData->UsbBulkOutSize = USB_HIGH_SPEED_BULK_SIZE;/* 512 bytes */
	} else {
		pHalData->UsbBulkOutSize = USB_FULL_SPEED_BULK_SIZE;/* 64 bytes */
	}

	pHalData->interfaceIndex = pdvobjpriv->InterfaceNumber;

#ifdef CONFIG_USB_TX_AGGREGATION
	pHalData->UsbTxAggMode		= 1;
	pHalData->UsbTxAggDescNum	= 3;	/* only 4 bits */
#endif

#ifdef CONFIG_USB_RX_AGGREGATION

	pHalData->rxagg_mode = registry_par->usb_rxagg_mode;

	if ((pHalData->rxagg_mode != RX_AGG_DMA) && (pHalData->rxagg_mode != RX_AGG_USB))
		pHalData->rxagg_mode = RX_AGG_DMA;

	/* pHalData->UsbRxAggBlockCount	= 8; */ /* unit : 512b */
	/* pHalData->UsbRxAggBlockTimeout	= 0x6; */

	/* pHalData->UsbRxAggPageCount	= 16;  */ /* uint :128 b */ /* 0x0A;	 */ /* 10 = MAX_RX_DMA_BUFFER_SIZE/2/pHalData->UsbBulkOutSize */
	/* pHalData->UsbRxAggPageTimeout = 0x6; */ /* 6, absolute time = 34ms/(2^6) */
	if (pHalData->rxagg_mode == RX_AGG_DMA) {
		pHalData->rxagg_dma_size = 8;/* unit 1k for Rx DMA aggregation mode */
		pHalData->rxagg_dma_timeout = 8;/* unit 32us */
	} else if (pHalData->rxagg_mode == RX_AGG_USB) {
#ifdef CONFIG_PREALLOC_RX_SKB_BUFFER
		u32 remainder = 0;
		u8 quotient = 0;

		remainder = MAX_RECVBUF_SZ % (4 * 1024);
		quotient = (u8)(MAX_RECVBUF_SZ >> 12);

		if (quotient > 5) {
			pHalData->rxagg_usb_size = 0x5;
			pHalData->rxagg_usb_timeout = 0x20;
		} else {
			if (remainder >= 2048) {
				pHalData->rxagg_usb_size = quotient;
				pHalData->rxagg_usb_timeout = 0x10;
			} else {
				pHalData->rxagg_usb_size = (quotient - 1);
				pHalData->rxagg_usb_timeout = 0x10;
			}
		}

#elif defined(CONFIG_PLATFORM_HISILICON)
		pHalData->rxagg_usb_size = 3; /*unit 4k for USB aggregation mode */
		pHalData->rxagg_usb_timeout = 8; /*unit 32us*/
#elif defined(CONFIG_PLATFORM_NOVATEK_NT72668)
		pHalData->rxagg_usb_size = 0x03;
		pHalData->rxagg_usb_timeout = 0x20;
#else
		pHalData->rxagg_usb_size = 6; /* unit 4k for USB aggregation mode */
		pHalData->rxagg_usb_timeout = 0x20; /* unit 32us */
#endif	/* CONFIG_PREALLOC_RX_SKB_BUFFER */
	}
#endif

	HalUsbSetQueuePipeMapping8192EUsb(padapter,
			  pdvobjpriv->RtNumInPipes, pdvobjpriv->RtNumOutPipes);

}

static VOID
_InitBurstPktLen_8192EU(IN PADAPTER Adapter)
{
#if 0
	u1Byte speedvalue, provalue, temp;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);


	/* PlatformEFIOWrite2Byte(Adapter, REG_TRXDMA_CTRL_8195, 0xf5b0); */
	/* PlatformEFIOWrite2Byte(Adapter, REG_TRXDMA_CTRL_8812, 0xf5b4); */
	PlatformEFIOWrite2Byte(Adapter, REG_RXDMA_STATUS_8192E, 0x7400);  /* burset lenght=4, set 0x3400 for burset length=2 */
	PlatformEFIOWrite1Byte(Adapter, 0x289, 0xf5);				/* for rxdma control */
	/* PlatformEFIOWrite1Byte(Adapter, 0x3a, 0x46); */

	/* 0x456 = 0x70, sugguested by Zhilin */
	/* PlatformEFIOWrite1Byte(Adapter, REG_AMPDU_MAX_TIME_8192E, 0x70); */

	/* Suggention by SD1 Jong and Pisa, by Maddest 20130107. */
	PlatformEFIOWrite2Byte(Adapter, REG_MAX_AGGR_NUM_8192E, 0x0e0e);
	PlatformEFIOWrite1Byte(Adapter, REG_FWHW_TXQ_CTRL_8192E, 0x80);/* EN_AMPDU_RTY_NEW */
	PlatformEFIOWrite1Byte(Adapter, REG_AMPDU_MAX_TIME_8192E, 0x5e);
	PlatformEFIOWrite4Byte(Adapter, REG_FAST_EDCA_CTRL_8192E, 0x03087777);


	/* PlatformEFIOWrite4Byte(Adapter, 0x458, 0xffffffff); */
	PlatformEFIOWrite1Byte(Adapter, REG_USTIME_TSF_8192E, 0x50);
	PlatformEFIOWrite1Byte(Adapter, REG_USTIME_EDCA_8192E, 0x50);

	if (IS_HARDWARE_TYPE_8821U(Adapter) || IS_HARDWARE_TYPE_8192EU(Adapter))
		speedvalue = BIT7;
	else
		speedvalue = PlatformEFIORead1Byte(Adapter, 0xff); /* check device operation speed: SS 0xff bit7 */

	if (speedvalue & BIT7) { /* USB2/1.1 Mode */
		temp = PlatformEFIORead1Byte(Adapter, REG_USB_INFO);
		if (((temp >> 4) & 0x03) == 0) {
			/* pHalData->UsbBulkOutSize = 512; */
			provalue = PlatformEFIORead1Byte(Adapter, REG_RXDMA_PRO_8192E);
			PlatformEFIOWrite1Byte(Adapter, REG_RXDMA_PRO_8192E, (provalue | BIT(4) & (~BIT(5)))); /* set burst pkt len=512B */
			PlatformEFIOWrite2Byte(Adapter, REG_RXDMA_PRO_8192E, 0x1e);
		} else {
			/* pHalData->UsbBulkOutSize = 64; */
			provalue = PlatformEFIORead1Byte(Adapter, REG_RXDMA_PRO_8192E);
			PlatformEFIOWrite1Byte(Adapter, REG_RXDMA_PRO_8192E, ((provalue | BIT(5)) & (~BIT(4)))); /* set burst pkt len=64B */
		}

		PlatformEFIOWrite2Byte(Adapter, REG_RXDMA_AGG_PG_TH_8192E, 0x2005); /* dmc agg th 20K */

		pHalData->bSupportUSB3 = FALSE;
	}

	PlatformEFIOWrite1Byte(Adapter, REG_DWBCN0_CTRL_8192E, 0x10);

	PlatformEFIOWrite1Byte(Adapter, 0x4c7, PlatformEFIORead1Byte(Adapter, 0x4c7) | BIT(7)); /* enable single pkt ampdu */
	PlatformEFIOWrite1Byte(Adapter, REG_RX_PKT_LIMIT_8192E, 0x18);		/* for VHT packet length 11K */

	/* PlatformEFIOWrite1Byte(Adapter, REG_MAX_AGGR_NUM_8192E, 0x1f); */
	PlatformEFIOWrite1Byte(Adapter, REG_PIFS_8192E, 0x00);
	/* PlatformEFIOWrite1Byte(Adapter, REG_FWHW_TXQ_CTRL_8192E, PlatformEFIORead1Byte(Adapter, REG_FWHW_TXQ_CTRL)&(~BIT(7))); */

#ifdef CONFIG_TX_EARLY_MODE
	if (pHalData->AMPDUBurstMode)
		PlatformEFIOWrite1Byte(Adapter, REG_SW_AMPDU_BURST_MODE_CTRL_8192E,  0x5F);
#endif

	PlatformEFIOWrite1Byte(Adapter, 0x1c, PlatformEFIORead1Byte(Adapter, 0x1c) | BIT(5) | BIT(6)); /* to prevent mac is reseted by bus. 20111208, by Page */
#endif
}

static u32 _InitPowerOn_8192EU(_adapter *padapter)
{
	u16 value16;
	u32 value32;
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(padapter);
	/* HW Power on sequence */
	u8 bMacPwrCtrlOn = _FALSE;

	rtw_hal_get_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (bMacPwrCtrlOn == _TRUE)
		return _SUCCESS;

	RTW_INFO("==>%s\n", __FUNCTION__);
	value32 = rtw_read32(padapter, REG_SYS_CFG1_8192E);

	if (value32 & BIT_SPSLDO_SEL) {
		/* LDO */
		rtw_write8(padapter, REG_LDO_SWR_CTRL, 0xC3);
	} else	{
		/* SPS */
		/* 0x7C [6] = 1¡¦b0 (IC default, 0x83) */


		/* 0x14[23:20]=b¡¦0101 (raise 1.2V voltage)
		   u1Byte	tmp1Byte = PlatformEFIORead1Byte(Adapter,0x16);
		   PlatformEFIOWrite1Byte(Adapter,0x16,tmp1Byte |BIT4|BIT6); */
		u32 voltage = rtw_read32(padapter , REG_SYS_SWR_CTRL2_8192E);

		if (((voltage & 0x00F00000) >> 20) == 0x4) {
			voltage = (voltage & 0xFF0FFFFF) | (0x05 << 20);
			rtw_write32(padapter , REG_SYS_SWR_CTRL2_8192E , voltage);
		}

		rtw_write8(padapter, REG_LDO_SWR_CTRL, 0x83);
	}

	/* adjust xtal/afe before enable PLL, suggest by SD1-Scott */
	Hal_CrystalAFEAdjust(padapter);

	if (!HalPwrSeqCmdParsing(padapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_PCI_MSK, Rtl8192E_NIC_ENABLE_FLOW)) {
		RTW_INFO("%s: HalPwrSeqCmdParsing fail\n", __func__);
		return _FAIL;
	}

	/* Enable MAC DMA/WMAC/SCHEDULE/SEC block */
	/* Set CR bit10 to enable 32k calibration. Suggested by SD1 Gimmy. Added by tynli. 2011.08.31. */
	rtw_write16(padapter, REG_CR, 0x00);  /* suggseted by zhouzhou, by page, 20111230 */
	value16 = rtw_read16(padapter, REG_CR);
	value16 |= (HCI_TXDMA_EN | HCI_RXDMA_EN | TXDMA_EN | RXDMA_EN
		    | PROTOCOL_EN | SCHEDULE_EN | ENSEC | CALTMR_EN);
	rtw_write16(padapter, REG_CR, value16);

	bMacPwrCtrlOn = _TRUE;
	rtw_hal_set_hwreg(padapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);

	return _SUCCESS;
}

/* ---------------------------------------------------------------
 *
 *	MAC init functions
 *
 * --------------------------------------------------------------- */

/* Shall USB interface init this? */
static VOID
_InitInterrupt_8192EU(
	IN  PADAPTER Adapter
)
{
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(Adapter);

	/*HISR write one to clear*/
	rtw_write32(Adapter, REG_HISR0_8192E, 0xFFFFFFFF);
	rtw_write32(Adapter, REG_HISR1_8192E, 0xFFFFFFFF);

	/* HIMR */
	rtw_write32(Adapter, REG_HIMR0_8192E, pHalData->IntrMask[0] & 0xFFFFFFFF);
	rtw_write32(Adapter, REG_HIMR1_8192E, pHalData->IntrMask[1] & 0xFFFFFFFF);
}


static VOID
_InitQueueReservedPage_8192EUsb(
	IN  PADAPTER Adapter
)
{
	_InitQueueReservedPage_8192E(Adapter);
}

static VOID
_InitHardwareDropIncorrectBulkOut_8192E(
	IN  PADAPTER Adapter
)
{
#ifdef ENABLE_USB_DROP_INCORRECT_OUT
	u32	value32 = rtw_read32(Adapter, REG_TXDMA_OFFSET_CHK);
	value32 |= DROP_DATA_EN;
	rtw_write32(Adapter, REG_TXDMA_OFFSET_CHK, value32);
#endif
}



#ifdef CONFIG_RTW_LED
static void _InitHWLed(PADAPTER Adapter)
{
	struct led_priv *pledpriv = adapter_to_led(Adapter);

	if (pledpriv->LedStrategy != HW_LED)
		return;

	rtw_write8(Adapter, REG_LEDCFG1, 0x02);
	/* HW led control
	 * to do ....
	 * must consider cases of antenna diversity/ commbo card/solo card/mini card */

}
#endif /* CONFIG_RTW_LED */


static VOID
_init_UsbBusSetting_8192EU(
	IN  PADAPTER Adapter
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8 usb_bus_setting = 0;
	usb_bus_setting = rtw_read8(Adapter, REG_RXDMA_8192E);
	usb_bus_setting = usb_bus_setting | BIT_DMA_MODE;
	usb_bus_setting = usb_bus_setting | BIT(3) | BIT(2); /* Burst number in dma mode,DD suggest 0x11		 */

	/* IS_SUPER_SPEED_USB(Adapter) */ /* for USB 3.0 */

	if (IS_HIGH_SPEED_USB(Adapter))
		rtw_write8(Adapter, REG_RXDMA_8192E, ((usb_bus_setting | BIT(4)) & (~BIT(5))));
	else if (IS_FULL_SPEED_USB(Adapter))
		rtw_write8(Adapter, REG_RXDMA_8192E, ((usb_bus_setting | BIT(5)) & (~BIT(4))));

}


/*-----------------------------------------------------------------------------
 * Function:	usb_AggSettingTxUpdate()
 *
 * Overview:	Seperate TX/RX parameters update independent for TP detection and
 *			dynamic TX/RX aggreagtion parameters update.
 *
 * Input:			PADAPTER
 *
 * Output/Return:	NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	12/10/2010	MHC		Seperate to smaller function.
 *
 *---------------------------------------------------------------------------*/
static VOID
usb_AggSettingTxUpdate_8192EU(
	IN	PADAPTER			Adapter
)
{
#ifdef CONFIG_USB_TX_AGGREGATION
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u32			value32;

	if (Adapter->registrypriv.wifi_spec)
		pHalData->UsbTxAggMode = _FALSE;

	if (pHalData->UsbTxAggMode) {
		value32 = rtw_read32(Adapter, REG_DWBCN0_CTRL_8192E);
		value32 = value32 & ~(BLK_DESC_NUM_MASK << BLK_DESC_NUM_SHIFT);
		value32 |= ((pHalData->UsbTxAggDescNum & BLK_DESC_NUM_MASK) << BLK_DESC_NUM_SHIFT);

		rtw_write32(Adapter, REG_DWBCN0_CTRL_8192E, value32);
		rtw_write8(Adapter, REG_DWBCN1_CTRL_8192E, pHalData->UsbTxAggDescNum << 1);
	}

#endif
}	/* usb_AggSettingTxUpdate */


/*-----------------------------------------------------------------------------
 * Function:	usb_AggSettingRxUpdate()
 *
 * Overview:	Seperate TX/RX parameters update independent for TP detection and
 *			dynamic TX/RX aggreagtion parameters update.
 *
 * Input:			PADAPTER
 *
 * Output/Return:	NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	12/10/2010	MHC		Seperate to smaller function.
 *
 *---------------------------------------------------------------------------*/

static VOID
usb_AggSettingRxUpdate_8192EU(
	IN	PADAPTER			Adapter
)
{
#ifdef CONFIG_USB_RX_AGGREGATION
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8			usb_agg_setting;
	u32			usb_agg_th = 0;

	usb_agg_setting = rtw_read8(Adapter, REG_TRXDMA_CTRL);
	rtw_write8(Adapter, REG_TRXDMA_CTRL, usb_agg_setting | RXDMA_AGG_EN);

	switch (pHalData->rxagg_mode) {
	case RX_AGG_DMA:
		usb_agg_th = (pHalData->rxagg_dma_size & 0x0F) | (pHalData->rxagg_dma_timeout << 8);
		usb_agg_th |= BIT_USB_RXDMA_AGG_EN;
		break;
	case RX_AGG_USB:
	case RX_AGG_MIX:
		usb_agg_th = (pHalData->rxagg_usb_size & 0x0F) | (pHalData->rxagg_usb_timeout << 8);
		break;
	case RX_AGG_DISABLE:
	default:
		/* TODO: */
		break;
	}

	rtw_write32(Adapter, REG_RXDMA_AGG_PG_TH, usb_agg_th);
	/*2010/12/10 MH Add for USB agg mode dynamic switch.*/
	pHalData->UsbRxHighSpeedMode = _FALSE;
#endif


}	/* usb_AggSettingRxUpdate */

static VOID
_init_UsbAggregationSetting_8192EU(
	IN  PADAPTER Adapter
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	/* Tx aggregation setting */
	usb_AggSettingTxUpdate_8192EU(Adapter);

	/* Rx aggregation setting */
	usb_AggSettingRxUpdate_8192EU(Adapter);
}

/*-----------------------------------------------------------------------------
 * Function:	USB_AggModeSwitch()
 *
 * Overview:	When RX traffic is more than 40M, we need to adjust some parameters to increase
 *			RX speed by increasing batch indication size. This will decrease TCP ACK speed, we
 *			need to monitor the influence of FTP/network share.
 *			For TX mode, we are still ubder investigation.
 *
 * Input:		PADAPTER
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	12/10/2010	MHC		Create Version 0.
 *
 *---------------------------------------------------------------------------*/
VOID
USB_AggModeSwitch(
	IN	PADAPTER			Adapter
)
{
#if 0
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PMGNT_INFO		pMgntInfo = &(Adapter->MgntInfo);

	/* pHalData->UsbRxHighSpeedMode = FALSE; */
	/* How to measure the RX speed? We assume that when traffic is more than */
	if (pMgntInfo->bRegAggDMEnable == FALSE) {
		return;	/* Inf not support. */
	}


	if (pMgntInfo->LinkDetectInfo.bHigherBusyRxTraffic == TRUE &&
	    pHalData->UsbRxHighSpeedMode == FALSE) {
		pHalData->UsbRxHighSpeedMode = TRUE;
	} else if (pMgntInfo->LinkDetectInfo.bHigherBusyRxTraffic == FALSE &&
		   pHalData->UsbRxHighSpeedMode == TRUE) {
		pHalData->UsbRxHighSpeedMode = FALSE;
	} else
		return;


#if USB_RX_AGGREGATION_92C
	if (pHalData->UsbRxHighSpeedMode == TRUE) {
		/* 2010/12/10 MH The parameter is tested by SD1 engineer and SD3 channel emulator. */
		/* USB mode */
#if (RT_PLATFORM == PLATFORM_LINUX)
		if (pMgntInfo->LinkDetectInfo.bTxBusyTraffic) {
			pHalData->RxAggBlockCount	= 16;
			pHalData->RxAggBlockTimeout	= 7;
		} else
#endif
		{
			pHalData->RxAggBlockCount	= 40;
			pHalData->RxAggBlockTimeout	= 5;
		}
		/* Mix mode */
		pHalData->RxAggPageCount	= 72;
		pHalData->RxAggPageTimeout	= 6;
	} else {
		/* USB mode */
		pHalData->RxAggBlockCount	= pMgntInfo->RegRxAggBlockCount;
		pHalData->RxAggBlockTimeout	= pMgntInfo->RegRxAggBlockTimeout;
		/* Mix mode */
		pHalData->RxAggPageCount		= pMgntInfo->RegRxAggPageCount;
		pHalData->RxAggPageTimeout	= pMgntInfo->RegRxAggPageTimeout;
	}

	if (pHalData->RxAggBlockCount > MAX_RX_AGG_BLKCNT)
		pHalData->RxAggBlockCount = MAX_RX_AGG_BLKCNT;
#if (OS_WIN_FROM_VISTA(OS_VERSION)) || (RT_PLATFORM == PLATFORM_LINUX)	/* do not support WINXP to prevent usbehci.sys BSOD */
	if (IS_WIRELESS_MODE_N_24G(Adapter) || IS_WIRELESS_MODE_N_5G(Adapter)) {
		/*  */
		/* 2010/12/24 MH According to V1012 QC IOT test, XP BSOD happen when running chariot test */
		/* with the aggregation dynamic change!! We need to disable the function to prevent it is broken */
		/* in usbehci.sys. */
		/*  */
		usb_AggSettingRxUpdate_8188E(Adapter);

		/* 2010/12/27 MH According to designer's suggstion, we can only modify Timeout value. Otheriwse */
		/* there might many HW incorrect behavior, the XP BSOD at usbehci.sys may be relative to the */
		/* issue. Base on the newest test, we can not enable block cnt > 30, otherwise XP usbehci.sys may */
		/* BSOD. */
	}
#endif

#endif
#endif
}	/* USB_AggModeSwitch */

static VOID _RfPowerSave(
	IN	PADAPTER		Adapter
)
{
#if 0
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(Adapter);
	PMGNT_INFO		pMgntInfo	= &(Adapter->MgntInfo);
	enum rf_path			eRFPath;

#if (DISABLE_BB_RF)
	return;
#endif

	if (pMgntInfo->RegRfOff == TRUE) { /* User disable RF via registry. */
		MgntActSet_RF_State(Adapter, eRfOff, RF_CHANGE_BY_SW);
		/* Those action will be discard in MgntActSet_RF_State because off the same state */
		for (eRFPath = 0; eRFPath < pHalData->NumTotalRFPath; eRFPath++)
			phy_set_rf_reg(Adapter, eRFPath, 0x4, 0xC00, 0x0);
	} else if (pMgntInfo->RfOffReason > RF_CHANGE_BY_PS) { /* H/W or S/W RF OFF before sleep. */
		MgntActSet_RF_State(Adapter, eRfOff, pMgntInfo->RfOffReason);
	} else {
		pHalData->eRFPowerState = eRfOn;
		pMgntInfo->RfOffReason = 0;
		if (Adapter->bInSetPower || Adapter->bResetInProgress)
			PlatformUsbEnableInPipes(Adapter);
	}
#endif
}

enum {
	Antenna_Lfet = 1,
	Antenna_Right = 2,
};

/*
 * 2010/08/26 MH Add for selective suspend mode check.
 * If Efuse 0x0e bit1 is not enabled, we can not support selective suspend for Minicard and
 * slim card.
 *   */
static VOID
HalDetectSelectiveSuspendMode(
	IN PADAPTER				Adapter
)
{
#if 0
	u8	tmpvalue;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct dvobj_priv	*pdvobjpriv = adapter_to_dvobj(Adapter);

	/* If support HW radio detect, we need to enable WOL ability, otherwise, we */
	/* can not use FW to notify host the power state switch. */

	EFUSE_ShadowRead(Adapter, 1, EEPROM_USB_OPTIONAL1, (u32 *)&tmpvalue);

	RTW_INFO("HalDetectSelectiveSuspendMode(): SS ");
	if (tmpvalue & BIT1)
		RTW_INFO("Enable\n");
	else {
		RTW_INFO("Disable\n");
		pdvobjpriv->RegUsbSS = _FALSE;
	}

	/* 2010/09/01 MH According to Dongle Selective Suspend INF. We can switch SS mode. */
	if (pdvobjpriv->RegUsbSS && !SUPPORT_HW_RADIO_DETECT(pHalData)) {
		/* PMGNT_INFO				pMgntInfo = &(Adapter->MgntInfo); */

		/* if (!pMgntInfo->bRegDongleSS)	 */
		/* { */
		pdvobjpriv->RegUsbSS = _FALSE;
		/* } */
	}
#endif
}	/* HalDetectSelectiveSuspendMode */

#if 0
/*-----------------------------------------------------------------------------
 * Function:	HwSuspendModeEnable92Cu()
 *
 * Overview:	HW suspend mode switch.
 *
 * Input:		NONE
 *
 * Output:	NONE
 *
 * Return:	NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	08/23/2010	MHC		HW suspend mode switch test..
 *---------------------------------------------------------------------------*/
static VOID
HwSuspendModeEnable_8192EU(
	IN	PADAPTER	pAdapter,
	IN	u8			Type
)
{
	/* PRT_USB_DEVICE 		pDevice = GET_RT_USB_DEVICE(pAdapter); */
	u16	reg = rtw_read16(pAdapter, REG_GPIO_MUXCFG);

	/* if (!pDevice->RegUsbSS) */
	{
		return;
	}

	/*  */
	/* 2010/08/23 MH According to Alfred's suggestion, we need to to prevent HW */
	/* to enter suspend mode automatically. Otherwise, it will shut down major power */
	/* domain and 8051 will stop. When we try to enter selective suspend mode, we */
	/* need to prevent HW to enter D2 mode aumotmatically. Another way, Host will */
	/* issue a S10 signal to power domain. Then it will cleat SIC setting(from Yngli). */
	/* We need to enable HW suspend mode when enter S3/S4 or disable. We need */
	/* to disable HW suspend mode for IPS/radio_off. */
	/*  */
	if (Type == _FALSE) {
		reg |= BIT14;
		rtw_write16(pAdapter, REG_GPIO_MUXCFG, reg);
		reg |= BIT12;
		rtw_write16(pAdapter, REG_GPIO_MUXCFG, reg);
	} else {
		reg &= (~BIT12);
		rtw_write16(pAdapter, REG_GPIO_MUXCFG, reg);
		reg &= (~BIT14);
		rtw_write16(pAdapter, REG_GPIO_MUXCFG, reg);
	}

}	/* HwSuspendModeEnable92Cu */
#endif

rt_rf_power_state RfOnOffDetect(IN	PADAPTER pAdapter)
{
	struct pwrctrl_priv *pwrctl = adapter_to_pwrctl(pAdapter);
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(pAdapter);
	u8	val8;
	rt_rf_power_state rfpowerstate = rf_off;

	if (pwrctl->bHWPowerdown) {
		val8 = rtw_read8(pAdapter, REG_HSISR);
		RTW_INFO("pwrdown, 0x5c(BIT7)=%02x\n", val8);
		rfpowerstate = (val8 & BIT7) ? rf_off : rf_on;
	} else { /* rf on/off */
		rtw_write8(pAdapter, REG_MAC_PINMUX_CFG, rtw_read8(pAdapter, REG_MAC_PINMUX_CFG) & ~(BIT3));
		val8 = rtw_read8(pAdapter, REG_GPIO_IO_SEL);
		RTW_INFO("GPIO_IN=%02x\n", val8);
		rfpowerstate = (val8 & BIT3) ? rf_on : rf_off;
	}
	return rfpowerstate;
}	/* HalDetectPwrDownMode */

void _ps_open_RF(_adapter *padapter)
{
	/* here call with bRegSSPwrLvl 1, bRegSSPwrLvl 2 needs to be verified */
	/* phy_SsPwrSwitch92CU(padapter, rf_on, 1); */
}

void _ps_close_RF(_adapter *padapter)
{
	/* here call with bRegSSPwrLvl 1, bRegSSPwrLvl 2 needs to be verified */
	/* phy_SsPwrSwitch92CU(padapter, rf_off, 1); */
}
/* page added for usb2 phy reg access. 20120514 */
VOID WriteUSB2PHYReg_8192EU(PADAPTER Adapter, u8 Offset, u8 Value)
{
	Offset -= 0x20;
	rtw_write8(Adapter, 0xFE41, Value);
	rtw_write8(Adapter, 0xFE40, Offset);
	rtw_write8(Adapter, 0xFE42, 0x81);
}

u1Byte ReadUSB2PHYReg_8192EU(PADAPTER Adapter, u8 Offset)
{
	u8 value;
	rtw_write8(Adapter, 0xFE40, Offset);
	rtw_write8(Adapter, 0xFE42, 0x81);
	value = rtw_read8(Adapter, 0xFE43);

	return value;
}
u32 rtl8192eu_hal_init(PADAPTER Adapter)
{
	u8	value8 = 0;
	u16  value16;
	u8	txpktbuf_bndy;
	u32	status = _SUCCESS;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	struct pwrctrl_priv		*pwrctrlpriv = adapter_to_pwrctl(Adapter);
	struct registry_priv	*pregistrypriv = &Adapter->registrypriv;

	rt_rf_power_state		eRfPowerStateToSet;


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
		HAL_INIT_STAGES_MISC21,
		/* HAL_INIT_STAGES_INIT_PABIAS, */
#ifdef CONFIG_BT_COEXIST
		HAL_INIT_STAGES_BT_COEXIST,
#endif
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
		"HAL_INIT_STAGES_MISC21",
#ifdef CONFIG_BT_COEXIST
		"HAL_INIT_STAGES_BT_COEXIST",
#endif
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




	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_BEGIN);
	/*
		if(0)
		{
			WriteUSB2PHYReg_8192EU(Adapter, 0xE1, ReadUSB2PHYReg_8192EU(Adapter, 0xE1)&0x7f);
			WriteUSB2PHYReg_8192EU(Adapter, 0xE2, ReadUSB2PHYReg_8192EU(Adapter, 0xE2)&0x0f);
			delay_us(50);
			WriteUSB2PHYReg_8192EU(Adapter, 0xE1, ReadUSB2PHYReg_8192EU(Adapter, 0xE1)|0x80);
			WriteUSB2PHYReg_8192EU(Adapter, 0xE2, ReadUSB2PHYReg_8192EU(Adapter, 0xE1)|0x90);
		}
	*/

#ifdef CONFIG_WOWLAN

	pwrctrlpriv->wowlan_wake_reason = rtw_read8(Adapter, REG_WOWLAN_WAKE_REASON);
	RTW_INFO("%s wowlan_wake_reason: 0x%02x\n",
		 __func__, pwrctrlpriv->wowlan_wake_reason);

	if (rtw_read8(Adapter, REG_MCUFWDL) & BIT7) {
		/*&&
			(pwrctrlpriv->wowlan_wake_reason & FW_DECISION_DISCONNECT)) {*/
		u8 reg_val = 0;
		RTW_INFO("+Reset Entry+\n");
		rtw_write8(Adapter, REG_MCUFWDL, 0x00);
		_8051Reset8192E(Adapter);
		/* reset BB */
		reg_val = rtw_read8(Adapter, REG_SYS_FUNC_EN);
		reg_val &= ~(BIT(0) | BIT(1));
		rtw_write8(Adapter, REG_SYS_FUNC_EN, reg_val);
		/* reset RF */
		rtw_write8(Adapter, REG_RF_CTRL, 0);
		/* reset TRX path */
		rtw_write16(Adapter, REG_CR, 0);
		/* reset MAC, Digital Core */
		reg_val = rtw_read8(Adapter, REG_SYS_FUNC_EN + 1);
		reg_val &= ~(BIT(4) | BIT(7));
		rtw_write8(Adapter, REG_SYS_FUNC_EN + 1, reg_val);
		reg_val = rtw_read8(Adapter, REG_SYS_FUNC_EN + 1);
		reg_val |= BIT(4) | BIT(7);
		rtw_write8(Adapter, REG_SYS_FUNC_EN + 1, reg_val);
		RTW_INFO("-Reset Entry-\n");
	}
#endif /* CONFIG_WOWLAN */


	if (pwrctrlpriv->bkeepfwalive) {
		_ps_open_RF(Adapter);

		if (pHalData->bIQKInitialized) {
			/*			PHY_IQCalibrate(padapter, _TRUE);		 */
		} else {
			/*			PHY_IQCalibrate(padapter, _FALSE);			 */
			pHalData->bIQKInitialized = _TRUE;
		}

		/*		dm_check_txpowertracking(padapter);
		 *		phy_lc_calibrate(padapter); */
		odm_txpowertracking_check(&pHalData->odmpriv);
		/* phy_lc_calibrate_8188e(Adapter); */

		goto exit;
	}



	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_INIT_PW_ON);
	status = rtw_hal_power_on(Adapter);
	if (status == _FAIL) {
		goto exit;
	}

	txpktbuf_bndy = TX_PAGE_BOUNDARY_8192E;

	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_MISC01);
	_InitQueueReservedPage_8192EUsb(Adapter);
	_InitQueuePriority_8192E(Adapter);
	_InitPageBoundary_8192E(Adapter);

	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_DOWNLOAD_FW);
	if (Adapter->registrypriv.mp_mode == 0) {
		status = FirmwareDownload8192E(Adapter, _FALSE);
		if (status != _SUCCESS) {
			RTW_INFO("%s: Download Firmware failed!!\n", __FUNCTION__);
			pHalData->bFWReady = _FALSE;
			pHalData->fw_ractrl = _FALSE;
			return status;
		} else {
			pHalData->bFWReady = _TRUE;
			pHalData->fw_ractrl = _TRUE;
		}
	}

	if (pwrctrlpriv->reg_rfoff == _TRUE)
		pwrctrlpriv->rf_pwrstate = rf_off;

	/* Save target channel	 */
	pHalData->current_channel = 6;/* default set to 6 */

	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_MAC);
#if (HAL_MAC_ENABLE == 1)
	status = PHY_MACConfig8192E(Adapter);
	if (status == _FAIL)
		goto exit;
#endif

	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_BB);
#if (HAL_BB_ENABLE == 1)
	status = PHY_BBConfig8192E(Adapter);
	if (status == _FAIL)
		goto exit;
#endif

	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_RF);
#if (HAL_RF_ENABLE == 1)
	status = PHY_RFConfig8192E(Adapter);
	if (status == _FAIL)
		goto exit;


	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_EFUSE_PATCH);

	_InitTxBufferBoundary_8192E(Adapter, txpktbuf_bndy);


	status =  InitLLTTable8192E(Adapter, txpktbuf_bndy);
	if (status == _FAIL) {
		goto exit;
	}

	_InitHardwareDropIncorrectBulkOut_8192E(Adapter);

	if (pHalData->bRDGEnable)
		_InitRDGSetting_8192E(Adapter);

#ifdef CONFIG_TX_EARLY_MODE

	if (pHalData->AMPDUBurstMode) {

		value8 = rtw_read8(Adapter, REG_EARLY_MODE_CONTROL_8192E);
#if RTL8192E_EARLY_MODE_PKT_NUM_10 == 1
		value8 = value8 | 0x1f;
#else
		value8 = value8 | 0xf;
#endif
		rtw_write8(Adapter, REG_EARLY_MODE_CONTROL_8192E, value8);

		rtw_write8(Adapter, REG_EARLY_MODE_CONTROL_8192E+3, 0x80);

		value8 = rtw_read8(Adapter, REG_TCR + 1);
		value8 = value8 | 0x40;
		rtw_write8(Adapter, REG_TCR + 1, value8);
	} else {
		/* rtw_write8(Adapter, REG_EARLY_MODE_CONTROL, 0); */
	}
#endif /* CONFIG_TX_EARLY_MODE */

	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_MISC02);
	/* Get Rx PHY status in order to report RSSI and others. */
	_InitDriverInfoSize_8192E(Adapter, DRVINFO_SZ);

	_InitInterrupt_8192EU(Adapter);

	_InitNetworkType_8192E(Adapter);/* set msr	 */
	_InitWMACSetting_8192E(Adapter);
	_InitAdaptiveCtrl_8192E(Adapter);
	_InitEDCA_8192E(Adapter);
	_InitRetryFunction_8192E(Adapter);
	_init_UsbBusSetting_8192EU(Adapter);
	_init_UsbAggregationSetting_8192EU(Adapter);

	_InitBeaconParameters_8192E(Adapter);
	_InitBeaconMaxError_8192E(Adapter, _TRUE);

	_InitBurstPktLen_8192EU(Adapter);  /* added by page. 20110919 */

	/*  */
	/* Init CR MACTXEN, MACRXEN after setting RxFF boundary REG_TRXFF_BNDY to patch */
	/* Hw bug which Hw initials RxFF boundry size to a value which is larger than the real Rx buffer size in 88E. */
	/* 2011.08.05. by tynli. */
	/*  */
	value8 = rtw_read8(Adapter, REG_CR);
	rtw_write8(Adapter, REG_CR, (value8 | MACTXEN | MACRXEN));

#ifdef CONFIG_CHECK_AC_LIFETIME
	/* Enable lifetime check for the four ACs */
	rtw_write8(Adapter, REG_LIFETIME_CTRL, rtw_read8(Adapter, REG_LIFETIME_CTRL) | 0x0f);
#endif /* CONFIG_CHECK_AC_LIFETIME */

#if defined(CONFIG_CONCURRENT_MODE) || defined(CONFIG_TX_MCAST2UNI)
#ifdef CONFIG_TX_MCAST2UNI
	rtw_write16(Adapter, REG_PKT_VO_VI_LIFE_TIME, 0x0400);	/* unit: 256us. 256ms */
	rtw_write16(Adapter, REG_PKT_BE_BK_LIFE_TIME, 0x0400);	/* unit: 256us. 256ms */
#else	/* CONFIG_TX_MCAST2UNI */
	rtw_write16(Adapter, REG_PKT_VO_VI_LIFE_TIME, 0x3000);	/* unit: 256us. 3s */
	rtw_write16(Adapter, REG_PKT_BE_BK_LIFE_TIME, 0x3000);	/* unit: 256us. 3s */
#endif /* CONFIG_TX_MCAST2UNI */
#endif /* CONFIG_CONCURRENT_MODE || CONFIG_TX_MCAST2UNI */


#ifdef CONFIG_RTW_LED
	_InitHWLed(Adapter);
#endif /* CONFIG_RTW_LED */

	_BBTurnOnBlock_8192E(Adapter);
#endif

	/*  */
	/* Joseph Note: Keep RfRegChnlVal for later use. */
	/*  */
	pHalData->RfRegChnlVal[0] = phy_query_rf_reg(Adapter, 0, RF_CHNLBW, bRFRegOffsetMask);
	pHalData->RfRegChnlVal[1] = phy_query_rf_reg(Adapter, 1, RF_CHNLBW, bRFRegOffsetMask);


	rtw_hal_set_chnl_bw(Adapter, Adapter->registrypriv.channel,
		CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE, HAL_PRIME_CHNL_OFFSET_DONT_CARE);

	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_TURN_ON_BLOCK);

	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_INIT_SECURITY);
	invalidate_cam_all(Adapter);

	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_MISC11);

	/* HW SEQ CTRL */
	/* set 0x0 to 0xFF by tynli. Default enable HW SEQ NUM. */
	rtw_write8(Adapter, REG_HWSEQ_CTRL, 0xFF);

	/*  */
	/* Disable BAR, suggested by Scott */
	/* 2010.04.09 add by hpfan */
	/*  */
	rtw_write32(Adapter, REG_BAR_MODE_CTRL, 0x0201ffff);
	PHY_SetRFEReg_8192E(Adapter);

	if (pregistrypriv->wifi_spec) {
		rtw_write16(Adapter, REG_FAST_EDCA_CTRL , 0);

		/* Nav limit , suggest by SD1-Pisa,disable NAV_UPPER function when wifi_spec=1 for Test item: 5.2.3 */
		rtw_write8(Adapter, REG_NAV_UPPER, 0x0);
	}

	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_INIT_HAL_DM);
	rtl8192e_InitHalDm(Adapter);

#if (MP_DRIVER == 1)
	if (Adapter->registrypriv.mp_mode == 1) {
		Adapter->mppriv.channel = pHalData->current_channel;
		MPT_InitializeAdapter(Adapter, Adapter->mppriv.channel);
	} else
#endif  /* #if (MP_DRIVER == 1) */
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


		/* 0x4c6[3] 1: RTS BW = Data BW */
		/* 0: RTS BW depends on CCA / secondary CCA result. */
		rtw_write8(Adapter, REG_QUEUE_CTRL, rtw_read8(Adapter, REG_QUEUE_CTRL) & 0xF7);

		/* enable Tx report. */
		/* rtw_write8(Adapter,  REG_FWHW_TXQ_CTRL+1, 0x0F); */

		/* Suggested by SD1 pisa. Added by tynli. 2011.10.21. */
		/* rtw_write8(Adapter, REG_EARLY_MODE_CONTROL_8192E+3, 0x01); */ /* Pretx_en, for WEP/TKIP SEC */

		/* tynli_test_tx_report. */
		/* rtw_write16(Adapter, REG_TX_RPT_TIME, 0x3DF0); */

		/* Reset USB mode switch setting	 */
		rtw_write8(Adapter, REG_ACLK_MON, 0x0);


		HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_IQK);
		if (pwrctrlpriv->rf_pwrstate == rf_on) {

			pHalData->neediqk_24g  = _TRUE;
			HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_PW_TRACK);

			odm_txpowertracking_check(&pHalData->odmpriv);


			HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_LCK);
			/* phy_lc_calibrate_8192e((GET_HAL_DATA(Adapter)->odmpriv));		 */
		}
	}
#ifdef CONFIG_HIGH_CHAN_SUPER_CALIBRATION
	rtw_hal_set_chnl_bw(Adapter, 13,
		CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE, HAL_PRIME_CHNL_OFFSET_DONT_CARE);
	PHY_SpurCalibration_8192E(Adapter);
	rtw_hal_set_chnl_bw(Adapter, Adapter->registrypriv.channel,
		CHANNEL_WIDTH_20, HAL_PRIME_CHNL_OFFSET_DONT_CARE, HAL_PRIME_CHNL_OFFSET_DONT_CARE);
#endif


	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_MISC21);


	/* HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_INIT_PABIAS);
	 *	_InitPABias(Adapter); */

#ifdef CONFIG_BT_COEXIST
	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_BT_COEXIST);

	if (pHalData->EEPROMBluetoothCoexist == 1) {
		/* YiWei 20140624 , Fix 8192eu mailbox BT info no response issue reduce I2C clock rate to 156KHz (default 1.25Mhz) */
		rtw_write8(Adapter, rPMAC_TxPacketNum, rtw_read8(Adapter, rPMAC_TxPacketNum) | 0x7);

		/* Init BT hw config.*/
		rtw_btcoex_HAL_Initialize(Adapter, _FALSE);
	}
#if 0
	/* else */
	{
		/*  In combo card run wifi only , must setting some hardware reg. */
		rtl8192e_combo_card_WifiOnlyHwInit(Adapter);
	}
#endif
#endif

	/* 2010/08/23 MH According to Alfred's suggestion, we need to to prevent HW enter */
	/* suspend mode automatically. */
	/* HwSuspendModeEnable92Cu(Adapter, _FALSE); */

	HAL_INIT_PROFILE_TAG(HAL_INIT_STAGES_MISC31);

	rtw_write8(Adapter, REG_USB_HRPWM, 0);
#ifdef CONFIG_XMIT_ACK
	/* ack for xmit mgmt frames. */
	rtw_write32(Adapter, REG_FWHW_TXQ_CTRL, rtw_read32(Adapter, REG_FWHW_TXQ_CTRL) | BIT(12));
#endif /* CONFIG_XMIT_ACK */
	/* Fixed LDPC rx hang issue. */
	{
		u4Byte	tmp4Byte = PlatformEFIORead4Byte(Adapter, REG_SYS_SWR_CTRL1_8192E);
		PlatformEFIOWrite1Byte(Adapter, REG_SYS_SWR_CTRL2_8192E, 0x75);
		tmp4Byte = (tmp4Byte & 0xfff00fff) | (0x7E << 12);
		PlatformEFIOWrite4Byte(Adapter, REG_SYS_SWR_CTRL1_8192E, tmp4Byte);
	}

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



	return status;
}

VOID
hal_poweroff_8192eu(
	IN	PADAPTER			Adapter
)
{
	u8	u1bTmp;
	u8 bMacPwrCtrlOn = _FALSE;

	rtw_hal_get_hwreg(Adapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);
	if (bMacPwrCtrlOn == _FALSE)
		return ;

	RTW_INFO(" %s\n", __FUNCTION__);

	/* Stop Tx Report Timer. 0x4EC[Bit1]=b'0 */
	u1bTmp = rtw_read8(Adapter, REG_TX_RPT_CTRL);
	rtw_write8(Adapter, REG_TX_RPT_CTRL, u1bTmp & (~BIT1));

	/* stop rx */
	rtw_write8(Adapter, REG_CR, 0x0);

	HalPwrSeqCmdParsing(Adapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK, Rtl8192E_NIC_LPS_ENTER_FLOW);

	/* MCUFWDL 0x80[1:0]=0				 */ /* reset MCU ready status */
	rtw_write8(Adapter, REG_MCUFWDL, 0x00);
#if 0
	if ((rtw_read8(Adapter, REG_MCUFWDL) & RAM_DL_SEL) &&
	    GET_HAL_DATA(Adapter)->bFWReady) /* 8051 RAM code */
		_8051Reset8192E(Adapter);
#else
	/* Reset MCU IO Wrapper */
	u1bTmp = rtw_read8(Adapter, REG_RSV_CTRL + 1);
	rtw_write8(Adapter, REG_RSV_CTRL + 1, (u1bTmp & (~BIT0)));

	/* Reset MCU. Suggested by Filen. 2011.01.26. by tynli. */
	u1bTmp = rtw_read8(Adapter, REG_SYS_FUNC_EN + 1);
	rtw_write8(Adapter, REG_SYS_FUNC_EN + 1, (u1bTmp & (~BIT2)));

	/* Enable MCU IO Wrapper , for IPS flow */
	u1bTmp = rtw_read8(Adapter, REG_RSV_CTRL + 1);
	rtw_write8(Adapter, REG_RSV_CTRL + 1, u1bTmp | BIT0);
#endif


	/* Card disable power action flow */
	HalPwrSeqCmdParsing(Adapter, PWR_CUT_ALL_MSK, PWR_FAB_ALL_MSK, PWR_INTF_USB_MSK, Rtl8192E_NIC_DISABLE_FLOW);

	bMacPwrCtrlOn = _FALSE;
	rtw_hal_set_hwreg(Adapter, HW_VAR_APFM_ON_MAC, &bMacPwrCtrlOn);

	GET_HAL_DATA(Adapter)->bFWReady = _FALSE;
}

static void rtl8192e_hw_power_down(_adapter *padapter)
{
	/* 2010/-8/09 MH For power down module, we need to enable register block contrl reg at 0x1c. */
	/* Then enable power down control bit of register 0x04 BIT4 and BIT15 as 1. */

	/* Enable register area 0x0-0xc. */
	rtw_write8(padapter, REG_RSV_CTRL, 0x0);
	rtw_write16(padapter, REG_APS_FSMCO, 0x8812);
}

u32 rtl8192eu_hal_deinit(PADAPTER Adapter)
{
	struct pwrctrl_priv *pwrctl = adapter_to_pwrctl(Adapter);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	RTW_INFO("==> %s\n", __FUNCTION__);


	rtw_write16(Adapter, REG_GPIO_MUXCFG, rtw_read16(Adapter, REG_GPIO_MUXCFG) & (~BIT12));


	if (pHalData->bSupportUSB3 == _TRUE) {
		/* set Reg 0xf008[3:4] to 2'11 to eable U1/U2 Mode in USB3.0. added by page, 20120712 */
		rtw_write8(Adapter, 0xf008, rtw_read8(Adapter, 0xf008) | 0x18);
	}

	rtw_write32(Adapter, REG_HISR0_8192E, 0xFFFFFFFF);
	rtw_write32(Adapter, REG_HISR1_8192E, 0xFFFFFFFF);
	rtw_write32(Adapter, REG_HIMR0_8192E, IMR_DISABLED_8192E);
	rtw_write32(Adapter, REG_HIMR1_8192E, IMR_DISABLED_8192E);

#ifdef SUPPORT_HW_RFOFF_DETECTED
	RTW_INFO("bkeepfwalive(%x)\n", pwrctl->bkeepfwalive);
	if (pwrctl->bkeepfwalive) {
		_ps_close_RF(Adapter);
		if ((pwrctl->bHWPwrPindetect) && (pwrctl->bHWPowerdown))
			rtl8192e_hw_power_down(Adapter);
	} else
#endif
	{
		if (rtw_is_hw_init_completed(Adapter)) {
			rtw_hal_power_off(Adapter);

			if ((pwrctl->bHWPwrPindetect) && (pwrctl->bHWPowerdown))
				rtl8192e_hw_power_down(Adapter);
		}
		pHalData->bMacPwrCtrlOn = _FALSE;

	}
	return _SUCCESS;
}


unsigned int rtl8192eu_inirp_init(PADAPTER Adapter)
{
	u8 i;
	struct recv_buf *precvbuf;
	uint	status;
	struct dvobj_priv *pdev = adapter_to_dvobj(Adapter);
	struct intf_hdl *pintfhdl = &Adapter->iopriv.intf;
	struct recv_priv *precvpriv = &(Adapter->recvpriv);
	u32(*_read_port)(struct intf_hdl *pintfhdl, u32 addr, u32 cnt, u8 *pmem);
#ifdef CONFIG_USB_INTERRUPT_IN_PIPE
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Adapter);
	u32(*_read_interrupt)(struct intf_hdl *pintfhdl, u32 addr);
#endif


	_read_port = pintfhdl->io_ops._read_port;

	status = _SUCCESS;


	precvpriv->ff_hwaddr = RECV_BULK_IN_ADDR;

	/* issue Rx irp to receive data	 */
	precvbuf = (struct recv_buf *)precvpriv->precv_buf;
	for (i = 0; i < NR_RECVBUFF; i++) {
		if (_read_port(pintfhdl, precvpriv->ff_hwaddr, 0, (unsigned char *)precvbuf) == _FALSE) {
			status = _FAIL;
			goto exit;
		}

		precvbuf++;
		precvpriv->free_recv_buf_queue_cnt--;
	}

#ifdef CONFIG_USB_INTERRUPT_IN_PIPE
	if (pdev->RtInPipe[REALTEK_USB_IN_INT_EP_IDX] != 0x07) {
		status = _FAIL;
		RTW_INFO("%s =>Warning !! Have not USB Int-IN pipe, RtIntInPipe(%d)!!!\n", __func__, pdev->RtInPipe[REALTEK_USB_IN_INT_EP_IDX]);
		goto exit;
	}
	_read_interrupt = pintfhdl->io_ops._read_interrupt;
	if (_read_interrupt(pintfhdl, RECV_INT_IN_ADDR) == _FALSE) {
		status = _FAIL;
	}
#endif

exit:



	return status;

}

unsigned int rtl8192eu_inirp_deinit(PADAPTER Adapter)
{

	rtw_read_port_cancel(Adapter);


	return _SUCCESS;
}

/* -------------------------------------------------------------------
 *
 *	EEPROM/EFUSE Content Parsing
 *
 * ------------------------------------------------------------------- */
VOID
hal_ReadIDs_8192EU(
	IN	PADAPTER	Adapter,
	IN	pu1Byte		PROMContent,
	IN	BOOLEAN		AutoloadFail
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	if (!AutoloadFail) {
		/* VID, PID */

		pHalData->EEPROMVID = ReadLE2Byte(&PROMContent[EEPROM_VID_8192EU]);
		pHalData->EEPROMPID = ReadLE2Byte(&PROMContent[EEPROM_PID_8192EU]);



		/* Customer ID, 0x00 and 0xff are reserved for Realtek.		 */
		pHalData->EEPROMCustomerID = *(u8 *)&PROMContent[EEPROM_CustomID_8192E];
		pHalData->EEPROMSubCustomerID = EEPROM_Default_SubCustomerID;

	} else {
		pHalData->EEPROMVID			= EEPROM_Default_VID;
		pHalData->EEPROMPID			= EEPROM_Default_PID;

		/* Customer ID, 0x00 and 0xff are reserved for Realtek.		 */
		pHalData->EEPROMCustomerID		= EEPROM_Default_CustomerID;
		pHalData->EEPROMSubCustomerID	= EEPROM_Default_SubCustomerID;

	}

	if ((pHalData->EEPROMVID == 0x050D) && (pHalData->EEPROMPID == 0x1106)) /* SerComm for Belkin. */
		pHalData->CustomerID = RT_CID_819x_Sercomm_Belkin;
	else if ((pHalData->EEPROMVID == 0x0846) && (pHalData->EEPROMPID == 0x9051)) /* SerComm for Netgear. */
		pHalData->CustomerID = RT_CID_819x_Sercomm_Netgear;
	else if ((pHalData->EEPROMVID == 0x2001) && (pHalData->EEPROMPID == 0x330e)) /* add by ylb 20121012 for customer led for alpha */
		pHalData->CustomerID = RT_CID_819x_ALPHA_Dlink;

	RTW_INFO("VID = 0x%04X, PID = 0x%04X\n", pHalData->EEPROMVID, pHalData->EEPROMPID);
	RTW_INFO("Customer ID: 0x%02X, SubCustomer ID: 0x%02X\n", pHalData->EEPROMCustomerID, pHalData->EEPROMSubCustomerID);
}

VOID
hal_CustomizedBehavior_8192EU(
	IN	PADAPTER	Adapter
)
{
#ifdef CONFIG_RTW_SW_LED

	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct led_priv	*pledpriv = adapter_to_led(Adapter);


	/* Led mode */
	switch (pHalData->CustomerID) {
	case RT_CID_DEFAULT:
		pledpriv->LedStrategy = SW_LED_MODE1;
		pledpriv->bRegUseLed = _TRUE;
		break;

	case RT_CID_819x_HP:
		pledpriv->LedStrategy = SW_LED_MODE6; /* Customize Led mode	 */
		break;

	case RT_CID_819x_Sercomm_Belkin:
		pledpriv->LedStrategy = SW_LED_MODE9;
		break;

	case RT_CID_819x_Sercomm_Netgear:
		pledpriv->LedStrategy = SW_LED_MODE10;
		break;

	case RT_CID_819x_ALPHA_Dlink: /* add by ylb 20121012 for customer led for alpha */
		pledpriv->LedStrategy = SW_LED_MODE1;
		break;

	default:
		pledpriv->LedStrategy = SW_LED_MODE9;
		break;
	}

	/* 2010.04.28 for 88CU minicard led control */
	/* if(pHalData->InterfaceSel == INTF_SEL2_MINICARD) */
	/* { */
	/*	pHalData->LedStrategy = SW_LED_MODE6; */
	/* } */
	pHalData->bLedOpenDrain = _TRUE;/* Support Open-drain arrangement for controlling the LED. Added by Roger, 2009.10.16. */
#endif
}

static void
hal_CustomizeByCustomerID_8192EU(
	IN	PADAPTER		padapter
)
{
#if 0
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);

	/* For customized behavior. */
	if ((pHalData->EEPROMVID == 0x103C) && (pHalData->EEPROMPID == 0x1629)) /* HP Lite-On for RTL8188CUS Slim Combo. */
		pHalData->CustomerID = RT_CID_819x_HP;
	else if ((pHalData->EEPROMVID == 0x9846) && (pHalData->EEPROMPID == 0x9041))
		pHalData->CustomerID = RT_CID_NETGEAR;
	else if ((pHalData->EEPROMVID == 0x2019) && (pHalData->EEPROMPID == 0x1201))
		pHalData->CustomerID = RT_CID_PLANEX;
	else if ((pHalData->EEPROMVID == 0x0BDA) && (pHalData->EEPROMPID == 0x5088))
		pHalData->CustomerID = RT_CID_CC_C;

	RTW_INFO("PID= 0x%x, VID=  %x\n", pHalData->EEPROMPID, pHalData->EEPROMVID);

	/*	Decide CustomerID according to VID/DID or EEPROM */
	switch (pHalData->EEPROMCustomerID) {
	case EEPROM_CID_DEFAULT:
		if ((pHalData->EEPROMVID == 0x2001) && (pHalData->EEPROMPID == 0x3308))
			pHalData->CustomerID = RT_CID_DLINK;
		else if ((pHalData->EEPROMVID == 0x2001) && (pHalData->EEPROMPID == 0x3309))
			pHalData->CustomerID = RT_CID_DLINK;
		else if ((pHalData->EEPROMVID == 0x2001) && (pHalData->EEPROMPID == 0x330a))
			pHalData->CustomerID = RT_CID_DLINK;
		else if ((pHalData->EEPROMVID == 0x0BFF) && (pHalData->EEPROMPID == 0x8160)) {
			pHalData->bAutoConnectEnable = _FALSE;
			pHalData->CustomerID = RT_CID_CHINA_MOBILE;
		} else if ((pHalData->EEPROMVID == 0x0BDA) &&	(pHalData->EEPROMPID == 0x5088))
			pHalData->CustomerID = RT_CID_CC_C;

		RTW_INFO("PID= 0x%x, VID=  %x\n", pHalData->EEPROMPID, pHalData->EEPROMVID);
		break;
	case EEPROM_CID_WHQL:
		/* padapter->bInHctTest = TRUE; */

		/* pMgntInfo->bSupportTurboMode = FALSE; */
		/* pMgntInfo->bAutoTurboBy8186 = FALSE; */

		/* pMgntInfo->PowerSaveControl.bInactivePs = FALSE; */
		/* pMgntInfo->PowerSaveControl.bIPSModeBackup = FALSE; */
		/* pMgntInfo->PowerSaveControl.bLeisurePs = FALSE; */
		/* pMgntInfo->PowerSaveControl.bLeisurePsModeBackup = FALSE; */
		/* pMgntInfo->keepAliveLevel = 0; */

		/* padapter->bUnloadDriverwhenS3S4 = FALSE; */
		break;
	default:
		pHalData->CustomerID = RT_CID_DEFAULT;
		break;

	}
	RTW_INFO("MGNT Customer ID: 0x%2x\n", pHalData->CustomerID);

	hal_CustomizedBehavior_8192EU(padapter);
#endif
}


static VOID
ReadLEDSetting_8192EU(
	IN	PADAPTER	Adapter,
	IN	u8		*PROMContent,
	IN	BOOLEAN		AutoloadFail
)
{
#ifdef CONFIG_RTW_LED
	struct led_priv *pledpriv = adapter_to_led(Adapter);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

#ifdef CONFIG_RTW_SW_LED
	pledpriv->bRegUseLed = _TRUE;

	switch (pHalData->CustomerID) {
	default:
		pledpriv->LedStrategy = SW_LED_MODE1;
		break;
	}
	pHalData->bLedOpenDrain = _TRUE;/* Support Open-drain arrangement for controlling the LED. Added by Roger, 2009.10.16. */
#else /* HW LED */
	pledpriv->LedStrategy = HW_LED;
#endif /* CONFIG_RTW_SW_LED */
#endif
}

VOID
InitAdapterVariablesByPROM_8192EU(
	IN	PADAPTER	Adapter
)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Adapter);

	hal_InitPGData_8192E(Adapter, pHalData->efuse_eeprom_data);

	Hal_EfuseParseIDCode8192E(Adapter, pHalData->efuse_eeprom_data);

	Hal_ReadPROMVersion8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	hal_ReadIDs_8192EU(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	hal_config_macaddr(Adapter, pHalData->bautoload_fail_flag);
	Hal_ReadPowerSavingMode8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadTxPowerInfo8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadBoardType8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);

	/*  */
	/* Read Bluetooth co-exist and initialize */
	/*  */
	Hal_EfuseParseBTCoexistInfo8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);

	Hal_ReadChannelPlan8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_EfuseParseXtal_8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadThermalMeter_8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadAntennaDiversity8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadPAType_8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadAmplifierType_8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);
	Hal_ReadRFEType_8192E(Adapter, pHalData->efuse_eeprom_data,  pHalData->bautoload_fail_flag);
	hal_CustomizeByCustomerID_8192EU(Adapter);

	ReadLEDSetting_8192EU(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);

	Hal_EfuseParseKFreeData_8192E(Adapter, pHalData->efuse_eeprom_data, pHalData->bautoload_fail_flag);

	/* set coex. ant info once efuse parsing is done */
	rtw_btcoex_set_ant_info(Adapter);
}

static void Hal_ReadPROMContent_8192EU(
	IN PADAPTER		Adapter
)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(Adapter);
	u8			eeValue;

	/* check system boot selection */
	eeValue = rtw_read8(Adapter, REG_SYS_EEPROM_CTRL);
	pHalData->EepromOrEfuse		= (eeValue & EEPROMSEL) ? _TRUE : _FALSE;
	pHalData->bautoload_fail_flag	= (eeValue & EEPROM_EN) ? _FALSE : _TRUE;

	RTW_INFO("Boot from %s, Autoload %s !\n", (pHalData->EepromOrEfuse ? "EEPROM" : "EFUSE"),
		 (pHalData->bautoload_fail_flag ? "Fail" : "OK"));

	/* pHalData->EEType = IS_BOOT_FROM_EEPROM(Adapter) ? EEPROM_93C46 : EEPROM_BOOT_EFUSE; */

	InitAdapterVariablesByPROM_8192EU(Adapter);
}

u8
ReadAdapterInfo8192EU(
	IN PADAPTER			Adapter
)
{
	/* Read all content in Efuse/EEPROM. */
	Hal_ReadPROMContent_8192EU(Adapter);

	/* We need to define the RF type after all PROM value is recognized. */
	hal_ReadRFType_8192E(Adapter);

	return _SUCCESS;
}

void UpdateInterruptMask8192EU(PADAPTER padapter, u8 bHIMR0 , u32 AddMSR, u32 RemoveMSR)
{
	HAL_DATA_TYPE *pHalData;

	u32 *himr;
	pHalData = GET_HAL_DATA(padapter);

	if (bHIMR0)
		himr = &(pHalData->IntrMask[0]);
	else
		himr = &(pHalData->IntrMask[1]);

	if (AddMSR)
		*himr |= AddMSR;

	if (RemoveMSR)
		*himr &= (~RemoveMSR);

	if (bHIMR0)
		rtw_write32(padapter, REG_HIMR0_8192E, *himr);
	else
		rtw_write32(padapter, REG_HIMR1_8192E, *himr);

}

u8 SetHwReg8192EU(PADAPTER Adapter, u8 variable, u8 *val)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct pwrctrl_priv *pwrctl = adapter_to_pwrctl(Adapter);
	u8 ret = _SUCCESS;


	switch (variable) {
	case HW_VAR_RXDMA_AGG_PG_TH:
#ifdef CONFIG_USB_RX_AGGREGATION
		{
			/* threshold == 1 , Disable Rx-agg when AP is B/G mode or wifi_spec=1 to prevent bad TP. */

			u8	threshold = *((u8 *)val);
			if (threshold == 0) {
				switch (pHalData->rxagg_mode) {
				case RX_AGG_DMA:
					threshold = (pHalData->rxagg_dma_size & 0x0F);
					break;
				case RX_AGG_USB:
				case RX_AGG_MIX:
					threshold = (pHalData->rxagg_usb_size & 0x0F);
					break;
				case RX_AGG_DISABLE:
				default:
					break;
				}
			}
			rtw_write8(Adapter, REG_RXDMA_AGG_PG_TH, threshold);

#ifdef CONFIG_80211N_HT
			{
				/* 2014-07-24 Fix WIFI Logo -5.2.4/5.2.9 - DT3 low TP issue */
				/* Adjust RxAggrTimeout to close to zero disable RxAggr for RxAgg-USB mode, suggested by designer */
				/* Timeout value is calculated by 34 / (2^n) */

				struct mlme_priv	*pmlmepriv = &Adapter->mlmepriv;
				struct ht_priv		*phtpriv = &pmlmepriv->htpriv;

				if (pHalData->rxagg_mode == RX_AGG_USB) {
#if 1
					/* BG mode || (wifi_spec=1 && BG mode Testbed)	 */
					if ((threshold == 1) && (_FALSE == phtpriv->ht_option))
#else
					/* (wifi_spec=1 && BG mode Testbed) */
					if ((Adapter->registrypriv.wifi_spec == 1) && (_FALSE == phtpriv->ht_option))
#endif
						rtw_write8(Adapter, REG_RXDMA_AGG_PG_TH + 1, 0);
					else
						rtw_write8(Adapter, REG_RXDMA_AGG_PG_TH + 1, pHalData->rxagg_usb_timeout);
				}
			}
#endif/* #ifdef CONFIG_80211N_HT */

		}
#endif
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
			rtw_write8(Adapter, REG_USB_HRPWM, ps_state);
		}
#endif
#ifdef CONFIG_AP_WOWLAN
		if (pwrctl->wowlan_ap_mode == _TRUE) {
			u8	ps_state = *((u8 *)val);
			RTW_INFO("%s, RPWM\n", __func__);
			/* rpwm value only use BIT0(clock bit) ,BIT6(Ack bit), and BIT7(Toggle bit) for 88e. */
			/* BIT0 value - 1: 32k, 0:40MHz. */
			/* BIT6 value - 1: report cpwm value after success set, 0:do not report. */
			/* BIT7 value - Toggle bit change. */
			/* modify by Thomas. 2012/4/2. */
			ps_state = ps_state & 0xC1;
			/* RTW_INFO("##### Change RPWM value to = %x for switch clk #####\n",ps_state); */
			rtw_write8(Adapter, REG_USB_HRPWM, ps_state);
		}
#endif
		break;
	default:
		ret = SetHwReg8192E(Adapter, variable, val);
		break;
	}

	return ret;
}


void GetHwReg8192EU(PADAPTER Adapter, u8 variable, u8 *val)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	switch (variable) {
	default:
		GetHwReg8192E(Adapter, variable, val);
		break;
	}

}
u8
GetHalDefVar8192EUsb(
	IN	PADAPTER				Adapter,
	IN	HAL_DEF_VARIABLE		eVariable,
	IN	PVOID					pValue
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8			bResult = _SUCCESS;

	switch (eVariable) {

	case HW_VAR_MAX_RX_AMPDU_FACTOR:
		*((u32 *)pValue) = MAX_AMPDU_FACTOR_64K; /* MAX_AMPDU_FACTOR_64K; */
		break;
	default:
		GetHalDefVar8192E(Adapter, eVariable, pValue);
		break;
	}
	return bResult;
}
/*
 *	Description:
 *		Change default setting of specified variable.
 *   */
u8
SetHalDefVar8192EUsb(
	IN	PADAPTER				Adapter,
	IN	HAL_DEF_VARIABLE		eVariable,
	IN	PVOID					pValue
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8			bResult = _SUCCESS;

	switch (eVariable) {
	default:
		SetHalDefVar8192E(Adapter, eVariable, pValue);
		break;
	}
	return bResult;
}


void _update_response_rate(_adapter *padapter, unsigned int mask)
{
	u8	RateIndex = 0;
	/* Set RRSR rate table. */
	rtw_write8(padapter, REG_RRSR, mask & 0xff);
	rtw_write8(padapter, REG_RRSR + 1, (mask >> 8) & 0xff);

	/* Set RTS initial rate */
	/* We just need to update to pHalData->RTSInitRate which will be set in Tx descriptor. */
}

static void rtl8192eu_init_default_value(_adapter *padapter)
{
	rtl8192e_init_default_value(padapter);
}

static u8 rtl8192eu_ps_func(PADAPTER Adapter, HAL_INTF_PS_FUNC efunc_id, u8 *val)
{
	u8 bResult = _TRUE;
	switch (efunc_id) {

#if defined(CONFIG_AUTOSUSPEND) && defined(SUPPORT_HW_RFOFF_DETECTED)
	case HAL_USB_SELECT_SUSPEND: {
		u8 bfwpoll = *((u8 *)val);
		/* rtl8188e_set_FwSelectSuspend_cmd(Adapter,bfwpoll ,500); */ /* note fw to support hw power down ping detect */
	}
	break;
#endif /* CONFIG_AUTOSUSPEND && SUPPORT_HW_RFOFF_DETECTED */

	default:
		break;
	}
	return bResult;
}

void rtl8192eu_set_hal_ops(_adapter *padapter)
{
	struct hal_ops	*pHalFunc = &padapter->hal_func;


	pHalFunc->hal_power_on = _InitPowerOn_8192EU;
	pHalFunc->hal_power_off = hal_poweroff_8192eu;

	pHalFunc->hal_init = &rtl8192eu_hal_init;
	pHalFunc->hal_deinit = &rtl8192eu_hal_deinit;

	pHalFunc->inirp_init = &rtl8192eu_inirp_init;
	pHalFunc->inirp_deinit = &rtl8192eu_inirp_deinit;

	pHalFunc->init_xmit_priv = &rtl8192eu_init_xmit_priv;
	pHalFunc->free_xmit_priv = &rtl8192eu_free_xmit_priv;

	pHalFunc->init_recv_priv = &rtl8192eu_init_recv_priv;
	pHalFunc->free_recv_priv = &rtl8192eu_free_recv_priv;
#ifdef CONFIG_RTW_SW_LED
	pHalFunc->InitSwLeds = &rtl8192eu_InitSwLeds;
	pHalFunc->DeInitSwLeds = &rtl8192eu_DeInitSwLeds;
#endif/* CONFIG_RTW_SW_LED */

	pHalFunc->init_default_value = &rtl8192eu_init_default_value;
	pHalFunc->intf_chip_configure = &rtl8192eu_interface_configure;
	pHalFunc->read_adapter_info = &ReadAdapterInfo8192EU;

	pHalFunc->set_hw_reg_handler = &SetHwReg8192EU;
	pHalFunc->GetHwRegHandler = &GetHwReg8192EU;
	pHalFunc->get_hal_def_var_handler = &GetHalDefVar8192EUsb;
	pHalFunc->SetHalDefVarHandler = &SetHalDefVar8192EUsb;

	pHalFunc->hal_xmit = &rtl8192eu_hal_xmit;
	pHalFunc->mgnt_xmit = &rtl8192eu_mgnt_xmit;
	pHalFunc->hal_xmitframe_enqueue = &rtl8192eu_hal_xmitframe_enqueue;


#ifdef CONFIG_HOSTAPD_MLME
	pHalFunc->hostap_mgnt_xmit_entry = &rtl8192eu_hostap_mgnt_xmit_entry;
#endif
	pHalFunc->interface_ps_func = &rtl8192eu_ps_func;
#ifdef CONFIG_XMIT_THREAD_MODE
	pHalFunc->xmit_thread_handler = &rtl8192eu_xmit_buf_handler;
#endif
#ifdef CONFIG_SUPPORT_USB_INT
	pHalFunc->interrupt_handler = interrupt_handler_8192eu;
#endif

	rtl8192e_set_hal_ops(pHalFunc);

}
