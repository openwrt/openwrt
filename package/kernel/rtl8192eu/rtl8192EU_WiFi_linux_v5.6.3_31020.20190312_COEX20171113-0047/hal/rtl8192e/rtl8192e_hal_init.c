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
#define _RTL8192E_HAL_INIT_C_

/* #include <drv_types.h> */
#include <rtl8192e_hal.h>
#include "hal8192e_fw.h"
/* -------------------------------------------------------------------------
 *
 * LLT R/W/Init function
 *
 * ------------------------------------------------------------------------- */
static s32 _LLTWrite(PADAPTER padapter, u32 address, u32 data)
{
	s32	status = _SUCCESS;
	s8	count = POLLING_LLT_THRESHOLD;
	u32	value = _LLT_INIT_ADDR(address) | _LLT_INIT_DATA(data) | _LLT_OP(_LLT_WRITE_ACCESS);

	rtw_write32(padapter, REG_LLT_INIT, value);
	/* polling */
	do {
		value = rtw_read32(padapter, REG_LLT_INIT);
		if (_LLT_NO_ACTIVE == _LLT_OP_VALUE(value))
			break;
	} while (--count);

	if (count <= 0) {
		RTW_INFO("Failed to polling write LLT done at address %d!\n", address);
		status = _FAIL;
	}
	return status;
}

u8 _LLTRead(PADAPTER padapter, u32 address)
{
	s32	count = POLLING_LLT_THRESHOLD;
	u32	value = _LLT_INIT_ADDR(address) | _LLT_OP(_LLT_READ_ACCESS);
	u16	LLTReg = REG_LLT_INIT;


	rtw_write32(padapter, LLTReg, value);

	/* polling and get value */
	do {
		value = rtw_read32(padapter, LLTReg);
		if (_LLT_NO_ACTIVE == _LLT_OP_VALUE(value))
			return (u8)value;
	} while (--count);




	return 0xFF;
}

s32 InitLLTTable8192E(PADAPTER padapter, u8 txpktbuf_bndy)
{
	s32	status = _FAIL;
	HAL_DATA_TYPE *pHalData	= GET_HAL_DATA(padapter);
	u32 value32;
	systime start = 0;
	u32 passing_time = 0;
#if 1
	value32 = rtw_read32(padapter, REG_AUTO_LLT);

	rtw_write32(padapter, REG_AUTO_LLT, value32 | BIT_AUTO_INIT_LLT);
	start = rtw_get_current_time();
	while (((value32 = rtw_read32(padapter, REG_AUTO_LLT)) & BIT_AUTO_INIT_LLT)
	       && ((passing_time = rtw_get_passing_time_ms(start)) < 1000)
	      )
		rtw_usleep_os(2);
	if (value32 & BIT_AUTO_INIT_LLT) {
		RTW_INFO("Auto %s(%08x) failed\n", __FUNCTION__, value32);
		status = _FAIL;
	} else {
		RTW_INFO("Auto %s success\n", __FUNCTION__);
		status = _SUCCESS;
	}

#else
	for (i = 0; i < (txpktbuf_bndy - 1); i++) {
		status = _LLTWrite(padapter, i, i + 1);
		if (_SUCCESS != status)
			return status;
	}

	/* end of list */
	status = _LLTWrite(padapter, (txpktbuf_bndy - 1), 0xFF);
	if (_SUCCESS != status)
		return status;

	/* Make the other pages as ring buffer */
	/* This ring buffer is used as beacon buffer if we config this MAC as two MAC transfer. */
	/* Otherwise used as local loopback buffer. */
	for (i = txpktbuf_bndy; i < Last_Entry_Of_TxPktBuf; i++) {
		status = _LLTWrite(padapter, i, (i + 1));
		if (_SUCCESS != status)
			return status;
	}

	/* Let last entry point to the start entry of ring buffer */
	status = _LLTWrite(padapter, Last_Entry_Of_TxPktBuf, txpktbuf_bndy);
	if (_SUCCESS != status)
		return status;
#endif
	return status;
}

BOOLEAN HalDetectPwrDownMode8192E(PADAPTER Adapter)
{
	u8 tmpvalue = 0;
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Adapter);
	struct pwrctrl_priv *pwrctrlpriv = adapter_to_pwrctl(Adapter);

	EFUSE_ShadowRead(Adapter, 1, EEPROM_RF_FEATURE_OPTION_8192E, (u32 *)&tmpvalue);

	/* 2010/08/25 MH INF priority > PDN Efuse value. */
	if (tmpvalue & BIT(4) && pwrctrlpriv->reg_pdnmode)
		pHalData->pwrdown = _TRUE;
	else
		pHalData->pwrdown = _FALSE;

	RTW_INFO("HalDetectPwrDownMode(): PDN=%d\n", pHalData->pwrdown);

	return pHalData->pwrdown;
}	/* HalDetectPwrDownMode */

#if defined(CONFIG_WOWLAN) || defined(CONFIG_AP_WOWLAN)
void Hal_DetectWoWMode(PADAPTER pAdapter)
{
	adapter_to_pwrctl(pAdapter)->bSupportRemoteWakeup = _TRUE;
}
#endif

static VOID
_FWDownloadEnable_8192E(
	IN	PADAPTER		padapter,
	IN	BOOLEAN			enable
)
{
	u8	tmp;

	if (enable) {
		/* MCU firmware download enable. */
		tmp = rtw_read8(padapter, REG_MCUFWDL);
		rtw_write8(padapter, REG_MCUFWDL, tmp | 0x01);

		/* 8051 reset */
		tmp = rtw_read8(padapter, REG_MCUFWDL + 2);
		rtw_write8(padapter, REG_MCUFWDL + 2, tmp & 0xf7);
	} else {

		/* MCU firmware download disable. */
		tmp = rtw_read8(padapter, REG_MCUFWDL);
		rtw_write8(padapter, REG_MCUFWDL, tmp & 0xfe);
	}
}

#ifdef CONFIG_PLATFORM_HISILICON
	#define MAX_REG_BOLCK_SIZE 4
#else
	#define MAX_REG_BOLCK_SIZE 254
#endif

static int
_BlockWrite_8192E(
	IN		PADAPTER		padapter,
	IN		PVOID		buffer,
	IN		u32			buffSize
)
{
	int ret = _SUCCESS;

	u32			blockSize_p1 = 4;	/* (Default) Phase #1 : PCI muse use 4-byte write to download FW */
	u32			blockSize_p2 = 8;	/* Phase #2 : Use 8-byte, if Phase#1 use big size to write FW. */
	u32			blockSize_p3 = 1;	/* Phase #3 : Use 1-byte, the remnant of FW image. */
	u32			blockCount_p1 = 0, blockCount_p2 = 0, blockCount_p3 = 0;
	u32			remainSize_p1 = 0, remainSize_p2 = 0;
	u8			*bufferPtr	= (u8 *)buffer;
	u32			i = 0, offset = 0;
#ifdef CONFIG_PCI_HCI
	u8			remainFW[4] = {0, 0, 0, 0};
	u8			*p = NULL;
#endif

#ifdef CONFIG_USB_HCI
	blockSize_p1 = MAX_REG_BOLCK_SIZE;
#endif

	/* 3 Phase #1 */
	blockCount_p1 = buffSize / blockSize_p1;
	remainSize_p1 = buffSize % blockSize_p1;



	for (i = 0; i < blockCount_p1; i++) {
#ifdef CONFIG_USB_HCI
		ret = rtw_writeN(padapter, (FW_START_ADDRESS + i * blockSize_p1), blockSize_p1, (bufferPtr + i * blockSize_p1));
#else
		ret = rtw_write32(padapter, (FW_START_ADDRESS + i * blockSize_p1), le32_to_cpu(*((u32 *)(bufferPtr + i * blockSize_p1))));
#endif

		if (ret == _FAIL)
			goto exit;
	}

#ifdef CONFIG_PCI_HCI
	p = (u8 *)((u32 *)(bufferPtr + blockCount_p1 * blockSize_p1));
	if (remainSize_p1) {
		switch (remainSize_p1) {
		case 0:
			break;
		case 3:
			remainFW[2] = *(p + 2);
		case 2:
			remainFW[1] = *(p + 1);
		case 1:
			remainFW[0] = *(p);
			ret = rtw_write32(padapter, (FW_START_ADDRESS + blockCount_p1 * blockSize_p1),
					  le32_to_cpu(*(u32 *)remainFW));
		}
		return ret;
	}
#endif

	/* 3 Phase #2 */
	if (remainSize_p1) {
		offset = blockCount_p1 * blockSize_p1;

		blockCount_p2 = remainSize_p1 / blockSize_p2;
		remainSize_p2 = remainSize_p1 % blockSize_p2;



#ifdef CONFIG_USB_HCI
		for (i = 0; i < blockCount_p2; i++) {
			ret = rtw_writeN(padapter, (FW_START_ADDRESS + offset + i * blockSize_p2), blockSize_p2, (bufferPtr + offset + i * blockSize_p2));

			if (ret == _FAIL)
				goto exit;
		}
#endif
	}

	/* 3 Phase #3 */
	if (remainSize_p2) {
		offset = (blockCount_p1 * blockSize_p1) + (blockCount_p2 * blockSize_p2);

		blockCount_p3 = remainSize_p2 / blockSize_p3;


		for (i = 0 ; i < blockCount_p3 ; i++) {
			ret = rtw_write8(padapter, (FW_START_ADDRESS + offset + i), *(bufferPtr + offset + i));

			if (ret == _FAIL)
				goto exit;
		}
	}

exit:
	return ret;
}

static int
_PageWrite_8192E(
	IN		PADAPTER	padapter,
	IN		u32			page,
	IN		PVOID		buffer,
	IN		u32			size
)
{
	u8 value8;
	u8 u8Page = (u8)(page & 0x07) ;

	value8 = (rtw_read8(padapter, REG_MCUFWDL + 2) & 0xF8) | u8Page ;
	rtw_write8(padapter, REG_MCUFWDL + 2, value8);

	return _BlockWrite_8192E(padapter, buffer, size);
}

static VOID
_FillDummy_8192E(
	u8		*pFwBuf,
	u32	*pFwLen
)
{
	u32	FwLen = *pFwLen;
	u8	remain = (u8)(FwLen % 4);
	remain = (remain == 0) ? 0 : (4 - remain);

	while (remain > 0) {
		pFwBuf[FwLen] = 0;
		FwLen++;
		remain--;
	}

	*pFwLen = FwLen;
}

static int
_WriteFW_8192E(
	IN		PADAPTER		padapter,
	IN		PVOID			buffer,
	IN		u32			size
)
{
	/* Since we need dynamic decide method of dwonload fw, so we call this function to get chip version. */
	int	ret = _SUCCESS;
	u32	pageNums, remainSize ;
	u32	page, offset;
	u8	*bufferPtr = (u8 *)buffer;

#ifdef CONFIG_PCI_HCI
	/* 20100120 Joseph: Add for 88CE normal chip. */
	/* Fill in zero to make firmware image to dword alignment.
	*		_FillDummy(bufferPtr, &size); */
#endif

	pageNums = size / MAX_DLFW_PAGE_SIZE ;
	/* RT_ASSERT((pageNums <= 4), ("Page numbers should not greater then 4\n")); */
	remainSize = size % MAX_DLFW_PAGE_SIZE;

	for (page = 0; page < pageNums; page++) {
		offset = page * MAX_DLFW_PAGE_SIZE;
		ret = _PageWrite_8192E(padapter, page, bufferPtr + offset, MAX_DLFW_PAGE_SIZE);

		if (ret == _FAIL)
			goto exit;
	}
	if (remainSize) {
		offset = pageNums * MAX_DLFW_PAGE_SIZE;
		page = pageNums;
		ret = _PageWrite_8192E(padapter, page, bufferPtr + offset, remainSize);

		if (ret == _FAIL)
			goto exit;

	}

exit:
	return ret;
}


void _8051Reset8192E(PADAPTER padapter)
{
	u8 u1bTmp, u1bTmp2;

	u1bTmp2 = rtw_read8(padapter, REG_RSV_CTRL);
	rtw_write8(padapter, REG_RSV_CTRL, (u1bTmp2 & (~BIT1)));

	/* Reset MCU IO Wrapper,suggested by SD1-Gimmy */
	u1bTmp2 = rtw_read8(padapter, REG_RSV_CTRL + 1);
	rtw_write8(padapter, REG_RSV_CTRL + 1, (u1bTmp2 & (~BIT0)));

	/* Reset 8051 */
	u1bTmp = rtw_read8(padapter, REG_SYS_FUNC_EN + 1);
	rtw_write8(padapter, REG_SYS_FUNC_EN + 1, u1bTmp & (~BIT2));

	u1bTmp2 = rtw_read8(padapter, REG_RSV_CTRL);
	rtw_write8(padapter, REG_RSV_CTRL, (u1bTmp2 & (~BIT1)));

	/* Enable MCU IO Wrapper */
	u1bTmp2 = rtw_read8(padapter, REG_RSV_CTRL + 1);
	rtw_write8(padapter, REG_RSV_CTRL + 1, u1bTmp2 | BIT0);

	/* Enable 8051 */
	rtw_write8(padapter, REG_SYS_FUNC_EN + 1, u1bTmp | (BIT2));

	RTW_INFO("=====> _8051Reset8192E(): 8051 reset success .\n");
}

static s32 polling_fwdl_chksum(_adapter *adapter, u32 min_cnt, u32 timeout_ms)
{
	s32 ret = _FAIL;
	u32 value32;
	systime start = rtw_get_current_time();
	u32 cnt = 0;

	/* polling CheckSum report */
	do {
		cnt++;
		value32 = rtw_read32(adapter, REG_MCUFWDL);
		if (value32 & FWDL_ChkSum_rpt || RTW_CANNOT_IO(adapter))
			break;
		rtw_yield_os();
	} while (rtw_get_passing_time_ms(start) < timeout_ms || cnt < min_cnt);

	if (!(value32 & FWDL_ChkSum_rpt))
		goto exit;

	if (rtw_fwdl_test_trigger_chksum_fail())
		goto exit;

	ret = _SUCCESS;

exit:
	RTW_INFO("%s: Checksum report %s! (%u, %dms), REG_MCUFWDL:0x%08x\n", __FUNCTION__
		, (ret == _SUCCESS) ? "OK" : "Fail", cnt, rtw_get_passing_time_ms(start), value32);

	return ret;
}

static s32 _FWFreeToGo8192E(_adapter *adapter, u32 min_cnt, u32 timeout_ms)
{
	s32 ret = _FAIL;
	u32	value32;
	systime start = rtw_get_current_time();
	u32 cnt = 0;

	value32 = rtw_read32(adapter, REG_MCUFWDL);
	value32 |= MCUFWDL_RDY;
	value32 &= ~WINTINI_RDY;
	rtw_write32(adapter, REG_MCUFWDL, value32);

	_8051Reset8192E(adapter);

	/*  polling for FW ready */
	do {
		cnt++;
		value32 = rtw_read32(adapter, REG_MCUFWDL);
		if (value32 & WINTINI_RDY || RTW_CANNOT_IO(adapter))
			break;
		rtw_yield_os();
	} while (rtw_get_passing_time_ms(start) < timeout_ms || cnt < min_cnt);

	if (!(value32 & WINTINI_RDY))
		goto exit;

	if (rtw_fwdl_test_trigger_wintint_rdy_fail())
		goto exit;

	ret = _SUCCESS;

exit:
	RTW_INFO("%s: Polling FW ready %s! (%u, %dms), REG_MCUFWDL:0x%08x\n", __FUNCTION__
		, (ret == _SUCCESS) ? "OK" : "Fail", cnt, rtw_get_passing_time_ms(start), value32);

	return ret;
}

#ifdef CONFIG_FILE_FWIMG
extern char *rtw_fw_file_path;
extern char *rtw_fw_wow_file_path;
u8 FwBuffer[FW_SIZE_8192E];
#endif

s32
FirmwareDownload8192E(
	IN	PADAPTER			Adapter,
	IN	BOOLEAN			bUsedWoWLANFw
)
{
	s32	rtStatus = _SUCCESS;
	u8	write_fw = 0;
	systime fwdl_start_time;
	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(Adapter);

	u8				*pFwImageFileName;
	u8				*pucMappedFile = NULL;
	PRT_FIRMWARE_8192E	pFirmware = NULL;
	u8				*pFwHdr = NULL;
	u8				*pFirmwareBuf;
	u32				FirmwareLen;
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(Adapter);
#ifdef CONFIG_FILE_FWIMG
	u8 *fwfilepath;
#endif

	pFirmware = (PRT_FIRMWARE_8192E)rtw_zmalloc(sizeof(RT_FIRMWARE_8192E));
	if (!pFirmware) {
		rtStatus = _FAIL;
		goto exit;
	}

#ifdef CONFIG_FILE_FWIMG
	#ifdef CONFIG_WOWLAN
	if (bUsedWoWLANFw)
		fwfilepath = rtw_fw_wow_file_path;
	else
	#endif
		fwfilepath = rtw_fw_file_path;

	if (rtw_is_file_readable(fwfilepath) == _TRUE) {
		RTW_INFO("%s acquire FW from file:%s\n", __func__, fwfilepath);
		pFirmware->eFWSource = FW_SOURCE_IMG_FILE;
	} else
#endif /* CONFIG_FILE_FWIMG */
	{
		#ifdef CONFIG_EMBEDDED_FWIMG
		pFirmware->eFWSource = FW_SOURCE_HEADER_FILE;
		#else
		pFirmware->eFWSource = FW_SOURCE_IMG_FILE; /* We should decided by Reg. */
		#endif
	}

	RTW_INFO(" ===> FirmwareDownload88E() fw source from %s.\n", (pFirmware->eFWSource ? "Header" : "File"));

	switch (pFirmware->eFWSource) {
	case FW_SOURCE_IMG_FILE:
		#ifdef CONFIG_FILE_FWIMG
		rtStatus = rtw_retrieve_from_file(fwfilepath, FwBuffer, FW_SIZE_8192E);
		pFirmware->ulFwLength = rtStatus >= 0 ? rtStatus : 0;
		pFirmware->szFwBuffer = FwBuffer;
		#endif
		break;
	case FW_SOURCE_HEADER_FILE:
		if (bUsedWoWLANFw) {
		#ifdef CONFIG_WOWLAN
			if (pwrpriv->wowlan_mode) {
				pFirmware->szFwBuffer = array_mp_8192e_fw_wowlan;
				pFirmware->ulFwLength = array_length_mp_8192e_fw_wowlan;
				RTW_INFO("%s fw:%s, size: %d\n", __FUNCTION__, "WoWLAN", pFirmware->ulFwLength);
			}
		#endif /* CONFIG_WOWLAN */

		#ifdef CONFIG_AP_WOWLAN
			if (pwrpriv->wowlan_ap_mode) {
				pFirmware->szFwBuffer = array_mp_8192e_fw_ap;
				pFirmware->ulFwLength = array_length_mp_8192e_fw_ap;
				RTW_INFO(" ===> %s fw: %s, size: %d\n", __FUNCTION__, "AP_WoWLAN", pFirmware->ulFwLength);
			}
		#endif /* CONFIG_AP_WOWLAN */
		} else {
			pFirmware->szFwBuffer = array_mp_8192e_fw_nic;
			pFirmware->ulFwLength = array_length_mp_8192e_fw_nic;
			RTW_INFO("%s fw:%s, size: %d\n", __FUNCTION__, "NIC", pFirmware->ulFwLength);
		}
		break;
	}

	if ((pFirmware->ulFwLength - 32) > FW_SIZE_8192E) {
		rtStatus = _FAIL;
		RTW_ERR("Firmware size:%u exceed %u\n", pFirmware->ulFwLength, FW_SIZE_8192E);
		goto exit;
	}

	pFirmwareBuf = pFirmware->szFwBuffer;
	FirmwareLen = pFirmware->ulFwLength;
	pFwHdr = (u8 *)pFirmware->szFwBuffer;

	if (IS_FW_HEADER_EXIST_8192E(pFwHdr)) {
		/* Shift 32 bytes for FW header */
		pFirmwareBuf = pFirmwareBuf + 32;
		FirmwareLen = FirmwareLen - 32;

		pHalData->firmware_version = (u16)GET_FIRMWARE_HDR_VERSION_8192E(pFwHdr);
		pHalData->firmware_sub_version = (u16)GET_FIRMWARE_HDR_SUB_VER_8192E(pFwHdr);
		pHalData->FirmwareSignature = (u16)GET_FIRMWARE_HDR_SIGNATURE_8192E(pFwHdr);

		RTW_INFO("%s: fw_ver=%d fw_subver=%d sig=0x%x\n",
			__FUNCTION__, pHalData->firmware_version, pHalData->firmware_sub_version, pHalData->FirmwareSignature);
	} else
		RTW_INFO("%s:FW header check failed .....\n", __FUNCTION__);

	/* Suggested by Filen. If 8051 is running in RAM code, driver should inform Fw to reset by itself, */
	/* or it will cause download Fw fail. 2010.02.01. by tynli. */
	if (rtw_read8(Adapter, REG_MCUFWDL) & BIT7) { /* 8051 RAM code */
		rtw_write8(Adapter, REG_MCUFWDL, 0x00);
		_8051Reset8192E(Adapter);
	}

	_FWDownloadEnable_8192E(Adapter, _TRUE);
	fwdl_start_time = rtw_get_current_time();
	while (!RTW_CANNOT_IO(Adapter)
	       && (write_fw++ < 3 || rtw_get_passing_time_ms(fwdl_start_time) < 500)) {
		/* reset FWDL chksum */
		rtw_write8(Adapter, REG_MCUFWDL, rtw_read8(Adapter, REG_MCUFWDL) | FWDL_ChkSum_rpt);

		rtStatus = _WriteFW_8192E(Adapter, pFirmwareBuf, FirmwareLen);
		if (rtStatus != _SUCCESS)
			continue;

		rtStatus = polling_fwdl_chksum(Adapter, 5, 50);
		if (rtStatus == _SUCCESS)
			break;
	}
	_FWDownloadEnable_8192E(Adapter, _FALSE);
	if (_SUCCESS != rtStatus)
		goto fwdl_stat;

	rtStatus = _FWFreeToGo8192E(Adapter, 10, 200);
	if (_SUCCESS != rtStatus)
		goto fwdl_stat;

fwdl_stat:
	RTW_INFO("FWDL %s. write_fw:%u, %dms\n"
		 , (rtStatus == _SUCCESS) ? "success" : "fail"
		 , write_fw
		 , rtw_get_passing_time_ms(fwdl_start_time)
		);

exit:
	if (pFirmware)
		rtw_mfree((u8 *)pFirmware, sizeof(RT_FIRMWARE_8192E));


	InitializeFirmwareVars8192E(Adapter);

	return rtStatus;
}


void InitializeFirmwareVars8192E(PADAPTER padapter)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(padapter);
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);

	/* Init Fw LPS related. */
	pwrpriv->bFwCurrentInPSMode = _FALSE;
	/* Init H2C counter. by tynli. 2009.12.09. */
	pHalData->LastHMEBoxNum = 0;
}

/* ***********************************************************
 *				Efuse related code
 * *********************************************************** */
void
hal_InitPGData_8192E(
	IN	PADAPTER		padapter,
	IN	OUT	u8			*PROMContent
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	u32			i;
	u16			value16;

	if (_FALSE == pHalData->bautoload_fail_flag) {
		/* autoload OK. */
		if (is_boot_from_eeprom(padapter)) {
			/* Read all Content from EEPROM or EFUSE. */
			for (i = 0; i < HWSET_MAX_SIZE_8192E; i += 2) {
				/* value16 = EF2Byte(ReadEEprom(pAdapter, (u2Byte) (i>>1))); */
				/* *((u16*)(&PROMContent[i])) = value16; */
			}
		} else {
			/* Read EFUSE real map to shadow. */
			EFUSE_ShadowMapUpdate(padapter, EFUSE_WIFI, _FALSE);
		}
	} else {
		/* autoload fail */
		/* pHalData->AutoloadFailFlag = _TRUE; */
		/* update to default value 0xFF */
		if (!is_boot_from_eeprom(padapter))
			EFUSE_ShadowMapUpdate(padapter, EFUSE_WIFI, _FALSE);
	}

#ifdef CONFIG_EFUSE_CONFIG_FILE
	if (check_phy_efuse_tx_power_info_valid(padapter) == _FALSE) {
		if (Hal_readPGDataFromConfigFile(padapter) != _SUCCESS)
			RTW_ERR("invalid phy efuse and read from file fail, will use driver default!!\n");
	}
#endif
}


VOID
Hal_EfuseParseBTCoexistInfo8192E(
	IN PADAPTER			Adapter,
	IN pu1Byte			hwinfo,
	IN BOOLEAN			AutoLoadFail
)
{

	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u1Byte			tempval;
	if (!AutoLoadFail) {
		tempval = hwinfo[EEPROM_RF_BOARD_OPTION_8192E];
		if (((tempval & 0xe0) >> 5) == 0x1) /* [7:5] */
			pHalData->EEPROMBluetoothCoexist = 1;
		else
			pHalData->EEPROMBluetoothCoexist = 0;
		pHalData->EEPROMBluetoothType = BT_RTL8192E;

		tempval = hwinfo[EEPROM_RF_BT_SETTING_8192E];
		/* pHalData->EEPROMBluetoothAntNum = (tempval&0x1);	 */ /* bit [0] */
		pHalData->EEPROMBluetoothAntNum = Ant_x2;
	} else {
		pHalData->EEPROMBluetoothCoexist = 1;
		pHalData->EEPROMBluetoothType = BT_RTL8192E;
		pHalData->EEPROMBluetoothAntNum = Ant_x2;
	}

#ifdef CONFIG_BT_COEXIST
	if (1 == pHalData->EEPROMBluetoothCoexist) {
#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
		if (!hal_btcoex_AntIsolationConfig_ParaFile(Adapter , PHY_FILE_WIFI_ANT_ISOLATION))
#endif
			hal_btcoex_SetAntIsolationType(Adapter, 0);
	}

	RTW_INFO("%s: %s BT-coex, wifi ant_num=%d\n",
		 __FUNCTION__,
		pHalData->EEPROMBluetoothCoexist == _TRUE ? "Enable" : "Disable",
		 pHalData->EEPROMBluetoothAntNum == Ant_x2 ? 2 : 1);
#endif /* CONFIG_BT_COEXIST */
}

void
Hal_EfuseParseIDCode8192E(
	IN	PADAPTER	padapter,
	IN	u8			*hwinfo
)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(padapter);
	u16			EEPROMId;


	/* Checl 0x8129 again for making sure autoload status!! */
	EEPROMId = le16_to_cpu(*((u16 *)hwinfo));
	if (EEPROMId != RTL_EEPROM_ID) {
		RTW_INFO("EEPROM ID(%#x) is invalid!!\n", EEPROMId);
		pHalData->bautoload_fail_flag = _TRUE;
	} else
		pHalData->bautoload_fail_flag = _FALSE;

	RTW_INFO("EEPROM ID=0x%04x\n", EEPROMId);
}

VOID
Hal_ReadPROMVersion8192E(
	IN	PADAPTER	Adapter,
	IN	u8			*PROMContent,
	IN	BOOLEAN	AutoloadFail
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	if (AutoloadFail)
		pHalData->EEPROMVersion = EEPROM_Default_Version;
	else {
		pHalData->EEPROMVersion = *(u8 *)&PROMContent[EEPROM_VERSION_8192E];
		if (pHalData->EEPROMVersion == 0xFF)
			pHalData->EEPROMVersion = EEPROM_Default_Version;
	}
	/* RTW_INFO("pHalData->EEPROMVersion is 0x%x\n", pHalData->EEPROMVersion); */
}

void Hal_ReadPowerSavingMode8192E(
	PADAPTER		padapter,
	IN	u8			*hwinfo,
	IN	BOOLEAN			AutoLoadFail
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct pwrctrl_priv *pwrctl = adapter_to_pwrctl(padapter);
	u8 tmpvalue;

	if (AutoLoadFail) {
		pwrctl->bHWPowerdown = _FALSE;
		pwrctl->bSupportRemoteWakeup = _FALSE;
	} else	{

		/* hw power down mode selection , 0:rf-off / 1:power down */

		if (padapter->registrypriv.hwpdn_mode == 2)
			pwrctl->bHWPowerdown = (hwinfo[EEPROM_RF_FEATURE_OPTION_8192E] & BIT4);
		else
			pwrctl->bHWPowerdown = padapter->registrypriv.hwpdn_mode;

		/* decide hw if support remote wakeup function */
		/* if hw supported, 8051 (SIE) will generate WeakUP signal( D+/D- toggle) when autoresume */
#ifdef CONFIG_USB_HCI
		pwrctl->bSupportRemoteWakeup = (hwinfo[EEPROM_USB_OPTIONAL_FUNCTION0] & BIT1) ? _TRUE : _FALSE;
#endif /* CONFIG_USB_HCI */


		RTW_INFO("%s...bHWPwrPindetect(%x)-bHWPowerdown(%x) ,bSupportRemoteWakeup(%x)\n", __FUNCTION__,
			pwrctl->bHWPwrPindetect, pwrctl->bHWPowerdown, pwrctl->bSupportRemoteWakeup);

		RTW_INFO("### PS params=>  power_mgnt(%x),usbss_enable(%x) ###\n", padapter->registrypriv.power_mgnt, padapter->registrypriv.usbss_enable);

	}

}

void
Hal_ReadTxPowerInfo8192E(
	IN	PADAPTER		Adapter,
	IN	u8				*PROMContent,
	IN	BOOLEAN			AutoLoadFail
)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Adapter);
	TxPowerInfo24G pwrInfo24G;

	hal_load_txpwr_info(Adapter, &pwrInfo24G, NULL, PROMContent);

	/* 2010/10/19 MH Add Regulator recognize for CU. */
	if (!AutoLoadFail) {
		struct registry_priv  *registry_par = &Adapter->registrypriv;

		if (PROMContent[EEPROM_RF_BOARD_OPTION_8192E] == 0xFF)
			pHalData->EEPROMRegulatory = 2; /* (EEPROM_DEFAULT_BOARD_OPTION&0x7);	 */ /* bit0~2 */
		else
			pHalData->EEPROMRegulatory = (PROMContent[EEPROM_RF_BOARD_OPTION_8192E] & 0x7);	/* bit0~2 */

	} else
		pHalData->EEPROMRegulatory = 0;
	RTW_INFO("EEPROMRegulatory = 0x%x\n", pHalData->EEPROMRegulatory);

}

VOID
Hal_ReadBoardType8192E(
	IN	PADAPTER	Adapter,
	IN	u8			*PROMContent,
	IN	BOOLEAN		AutoloadFail
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8 board_type;
	/*
	Bit[7:5]: Board Type (PCIe)
		0h: WiFi solo-mCard
		1h: WiFi+BT combo-mCard
	*/
	if (!AutoloadFail) {
		board_type = (PROMContent[EEPROM_RF_BOARD_OPTION_8192E] & 0xE0) >> 5;

		if (PROMContent[EEPROM_RF_BOARD_OPTION_8192E] == 0xFF)
			pHalData->InterfaceSel = INTF_SEL0_USB ;
		else
			pHalData->InterfaceSel = (board_type == 1) ? INTF_SEL4_USB_Combo : INTF_SEL0_USB;

	} else
		pHalData->InterfaceSel = 0;
	/* RTW_INFO("Board Type: 0x%2x\n", pHalData->InterfaceSel); */
	if (pHalData->InterfaceSel == INTF_SEL4_USB_Combo)
		RTW_INFO("Board Type: Combo Card\n");
	else
		RTW_INFO("Board Type: Dongle or WIFI only Module\n");

}

VOID
Hal_ReadThermalMeter_8192E(
	IN	PADAPTER	Adapter,
	IN	u8			*PROMContent,
	IN	BOOLEAN	AutoloadFail
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	/* u8	tempval; */

	/*  */
	/* ThermalMeter from EEPROM */
	/*  */
	if (!AutoloadFail)
		pHalData->eeprom_thermal_meter = PROMContent[EEPROM_THERMAL_METER_8192E];
	else
		pHalData->eeprom_thermal_meter = EEPROM_Default_ThermalMeter_8192E;
	/* pHalData->eeprom_thermal_meter = (tempval&0x1f);	 */ /* [4:0] */

	if (pHalData->eeprom_thermal_meter == 0xff || AutoloadFail) {
		pHalData->odmpriv.rf_calibrate_info.is_apk_thermal_meter_ignore = _TRUE;
		pHalData->eeprom_thermal_meter = EEPROM_Default_ThermalMeter_8192E;
	}

	/* pHalData->ThermalMeter[0] = pHalData->eeprom_thermal_meter;	 */
	RTW_INFO("ThermalMeter = 0x%x\n", pHalData->eeprom_thermal_meter);
}

VOID
Hal_ReadChannelPlan8192E(
	IN	PADAPTER		padapter,
	IN	u8				*hwinfo,
	IN	BOOLEAN			AutoLoadFail
)
{
	hal_com_config_channel_plan(
		padapter
		, hwinfo ? &hwinfo[EEPROM_COUNTRY_CODE_8192E] : NULL
		, hwinfo ? hwinfo[EEPROM_ChannelPlan_8192E] : 0xFF
		, padapter->registrypriv.alpha2
		, padapter->registrypriv.channel_plan
		, RTW_CHPLAN_WORLD_NULL
		, AutoLoadFail
	);
}

VOID
Hal_EfuseParseXtal_8192E(
	IN	PADAPTER	pAdapter,
	IN	u8			*hwinfo,
	IN	BOOLEAN		AutoLoadFail
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);

	if (!AutoLoadFail) {
		pHalData->crystal_cap = hwinfo[EEPROM_XTAL_8192E];
		if (pHalData->crystal_cap == 0xFF)
			pHalData->crystal_cap = EEPROM_Default_CrystalCap_8192E;	 /* what value should 8812 set? */
	} else
		pHalData->crystal_cap = EEPROM_Default_CrystalCap_8192E;
	RTW_INFO("crystal_cap: 0x%2x\n", pHalData->crystal_cap);
}

VOID
Hal_ReadAntennaDiversity8192E(
	IN	PADAPTER		pAdapter,
	IN	u8				*PROMContent,
	IN	BOOLEAN			AutoLoadFail
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);

	pHalData->AntDivCfg = 0;
	RTW_INFO("SWAS: bHwAntDiv = %x, TRxAntDivType = %x\n", pHalData->AntDivCfg, pHalData->TRxAntDivType);
}

VOID
Hal_ReadPAType_8192E(
	IN	PADAPTER	Adapter,
	IN	u8			*PROMContent,
	IN	BOOLEAN		AutoloadFail
)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	u8			PA_LNAType_2G = 0;

	if (!AutoloadFail) {
		if (GetRegAmplifierType2G(Adapter) == 0) { /* AUTO*/

			/* PA & LNA Type */
			PA_LNAType_2G = LE_BITS_TO_1BYTE(&PROMContent[EEPROM_RFE_OPTION_8192E], 2, 2); /* 0xCA[3:2] */
			/*
				ePA/eLNA sel.(ePA+eLNA=0x0, ePA+iLNA enable = 0x1, iPA+eLNA enable =0x2, iPA+iLNA=0x3)
			*/
			switch (PA_LNAType_2G) {
			case 0:
				pHalData->ExternalPA_2G  = 1;
				pHalData->ExternalLNA_2G = 1;
				break;
			case 1:
				pHalData->ExternalPA_2G  = 1;
				pHalData->ExternalLNA_2G = 0;
				break;
			case 2:
				pHalData->ExternalPA_2G  = 0;
				pHalData->ExternalLNA_2G = 1;
				break;
			case 3:
			default:
				pHalData->ExternalPA_2G  = 0;
				pHalData->ExternalLNA_2G = 0;
				break;
			}
		} else {
			pHalData->ExternalPA_2G  = (GetRegAmplifierType2G(Adapter) & ODM_BOARD_EXT_PA)  ? 1 : 0;
			pHalData->ExternalLNA_2G = (GetRegAmplifierType2G(Adapter) & ODM_BOARD_EXT_LNA) ? 1 : 0;
		}
#if 0
		if (GetRegAmplifierType5G(Adapter) == 0) { /*  AUTO */
			pHalData->external_pa_5g = ((pHalData->PAType_5G & BIT1) && (pHalData->PAType_5G & BIT0)) ? 1 : 0;
			pHalData->external_lna_5g = ((pHalData->LNAType_5G & BIT7) && (pHalData->LNAType_5G & BIT3)) ? 1 : 0;	/*  5G only now.			 */
		} else {
			pHalData->external_pa_5g  = (GetRegAmplifierType5G(Adapter) & ODM_BOARD_EXT_PA_5G)  ? 1 : 0;
			pHalData->external_lna_5g = (GetRegAmplifierType5G(Adapter) & ODM_BOARD_EXT_LNA_5G) ? 1 : 0;
		}
#endif

	} else {
		pHalData->ExternalPA_2G  = EEPROM_Default_PAType;
		pHalData->external_pa_5g  = EEPROM_Default_PAType;
		pHalData->ExternalLNA_2G = EEPROM_Default_LNAType;
		pHalData->external_lna_5g = EEPROM_Default_LNAType;

		if (GetRegAmplifierType2G(Adapter) == 0) { /* AUTO*/
			pHalData->ExternalPA_2G  = EEPROM_Default_PAType;
			pHalData->ExternalLNA_2G = EEPROM_Default_LNAType;
		} else {
			pHalData->ExternalPA_2G  = (GetRegAmplifierType2G(Adapter) & ODM_BOARD_EXT_PA)  ? 1 : 0;
			pHalData->ExternalLNA_2G = (GetRegAmplifierType2G(Adapter) & ODM_BOARD_EXT_LNA) ? 1 : 0;
		}
#if 0
		if (GetRegAmplifierType5G(Adapter) == 0) { /*  AUTO */
			pHalData->external_pa_5g  = 0;
			pHalData->external_lna_5g = 0;
		} else {
			pHalData->external_pa_5g  = (GetRegAmplifierType5G(Adapter) & ODM_BOARD_EXT_PA_5G)  ? 1 : 0;
			pHalData->external_lna_5g = (GetRegAmplifierType5G(Adapter) & ODM_BOARD_EXT_LNA_5G) ? 1 : 0;
		}
#endif

	}
	RTW_INFO("pHalData->ExternalPA_2G = %d , pHalData->ExternalLNA_2G = %d\n",  pHalData->ExternalPA_2G, pHalData->ExternalLNA_2G);

}

VOID
Hal_ReadAmplifierType_8192E(
	IN	PADAPTER	Adapter,
	IN	pu1Byte		PROMContent,
	IN	BOOLEAN		AutoloadFail
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8	GLNA_type = 0;

	if (!AutoloadFail) {
		if (GetRegGLNAType(Adapter) == 0) /* AUTO */
			GLNA_type = LE_BITS_TO_1BYTE(&PROMContent[EEPROM_RFE_OPTION_8192E], 4, 3); /* 0xCA[6:4] */
		else
			GLNA_type  = GetRegGLNAType(Adapter) & 0x7;
	} else {
		if (GetRegGLNAType(Adapter) == 0) /* AUTO */
			GLNA_type = 0;
		else
			GLNA_type  = GetRegGLNAType(Adapter) & 0x7;
	}
	/*
		Ext-LNA Gain sel.(form 10dB to 24dB, 1table/2dB,ext: 000=10dB, 001=12dB...)
	*/
	switch (GLNA_type) {
	case 2:
		pHalData->TypeGLNA = 0xf;  /* (14dB) */
		break;
	case 3:
		pHalData->TypeGLNA = 0xa; /* (16dB) */
		break;
	case 4:
		pHalData->TypeGLNA = 0x5;/* (18dB) */
		break;
	case 6:
	default:
		pHalData->TypeGLNA = 0x0;/* (22dB) */
		break;
	}
	RTW_INFO("pHalData->TypeGLNA is 0x%x\n", pHalData->TypeGLNA);
}
VOID
Hal_ReadRFEType_8192E(
	IN	PADAPTER	Adapter,
	IN	pu1Byte		PROMContent,
	IN	BOOLEAN		AutoloadFail
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	if (!AutoloadFail) {
		if (GetRegRFEType(Adapter) != 64) {
			pHalData->rfe_type = GetRegRFEType(Adapter);
			/*
				Above 3, rfe_type is filled the default value.
			*/
			if (pHalData->rfe_type > 3)
				pHalData->rfe_type = EEPROM_DEFAULT_RFE_OPTION_8192E;

		} else if ((0xFF == PROMContent[EEPROM_RFE_OPTION_8192E]) ||
			((pHalData->ExternalPA_2G == 0) && (pHalData->ExternalLNA_2G == 0)))
			pHalData->rfe_type = EEPROM_DEFAULT_RFE_OPTION_8192E;
		else {
			/*
				type 0:0x00 for 92EE/ER_HP RFE control
				type 1:0x01 for 92EU/ES_HP RFE control
				type 2:0x10 for 92EU/ES_HP_COMBO RFE control
				type 3:0x11 for 92EE_HP_COMBO RFE control
			*/
			pHalData->rfe_type = PROMContent[EEPROM_RFE_OPTION_8192E] & 0x3; /* 0xCA[1:0] */
		}
	} else {
		if (GetRegRFEType(Adapter) != 64) {
			pHalData->rfe_type = GetRegRFEType(Adapter);
			/*
				 Above 3, rfe_type is filled the default value.
			*/
			if (pHalData->rfe_type > 3)
				pHalData->rfe_type = EEPROM_DEFAULT_RFE_OPTION_8192E;

		} else
			pHalData->rfe_type = EEPROM_DEFAULT_RFE_OPTION_8192E;

	}

	RTW_INFO("pHalData->rfe_type is 0x%x\n", pHalData->rfe_type);
}


void Hal_EfuseParseKFreeData_8192E(
	IN		PADAPTER		Adapter,
	IN		u8				*PROMContent,
	IN		BOOLEAN			AutoloadFail)
{
#ifdef CONFIG_RF_POWER_TRIM

	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct kfree_data_t *kfree_data = &pHalData->kfree_data;
	u8 Efuse_BB_GAIN;
	u8 pg_pwrtrim = 0xFF, pg_therm = 0xFF;

	if ((Adapter->registrypriv.RegPwrTrimEnable == 1) || !AutoloadFail) {
		efuse_OneByteRead(Adapter, PPG_BB_GAIN_2G_TXA_OFFSET_8192E, &pg_pwrtrim, _FALSE);
		efuse_OneByteRead(Adapter, PPG_THERMAL_OFFSET_8192E, &pg_therm, _FALSE);

		kfree_data->bb_gain[BB_GAIN_2G][RF_PATH_A]
			= KFREE_BB_GAIN_2G_TX_OFFSET(pg_pwrtrim & PPG_BB_GAIN_2G_TX_OFFSET_MASK);

		kfree_data->bb_gain[BB_GAIN_2G][RF_PATH_B]
			= KFREE_BB_GAIN_2G_TX_OFFSET(pg_pwrtrim & PPG_BB_GAIN_2G_TXB_OFFSET_MASK >> 4);

		kfree_data->thermal = KFREE_THERMAL_OFFSET(pg_therm & PPG_THERMAL_OFFSET_MASK);

		if (GET_PG_KFREE_ON_8192E(PROMContent) && PROMContent[0xc1] != 0xff)
			kfree_data->flag |= KFREE_FLAG_ON;
		if (GET_PG_KFREE_THERMAL_K_ON_8192E(PROMContent) && PROMContent[0xc8] != 0xff)
			kfree_data->flag |= KFREE_FLAG_THERMAL_K_ON;
	}

	if (Adapter->registrypriv.RegPwrTrimEnable == 1) {
		kfree_data->flag |= KFREE_FLAG_ON;
		kfree_data->flag |= KFREE_FLAG_THERMAL_K_ON;

	}

	if (kfree_data->flag & KFREE_FLAG_THERMAL_K_ON)
		pHalData->eeprom_thermal_meter += kfree_data->thermal;

	RTW_INFO("kfree flag:%u\n", kfree_data->flag);
	if (Adapter->registrypriv.RegPwrTrimEnable == 1 || kfree_data->flag & KFREE_FLAG_ON)
		RTW_INFO("bb_gain A:%d\n", kfree_data->bb_gain[BB_GAIN_2G][RF_PATH_A]);
	if (Adapter->registrypriv.RegPwrTrimEnable == 1 || kfree_data->flag & KFREE_FLAG_ON)
		RTW_INFO("bb_gain B:%d\n", kfree_data->bb_gain[BB_GAIN_2G][RF_PATH_B]);
	if (Adapter->registrypriv.RegPwrTrimEnable == 1 || kfree_data->flag & KFREE_FLAG_THERMAL_K_ON)
		RTW_INFO("thermal:%d\n", kfree_data->thermal);

#endif /*CONFIG_RF_POWER_TRIM*/
}


enum {
	VOLTAGE_V25						= 0x03,
	LDOE25_SHIFT						= 28 ,
};

static VOID
Hal_EfusePowerSwitch8192E(
	IN	PADAPTER	pAdapter,
	IN	u8		bWrite,
	IN	u8		PwrState)
{
	u8	tempval;
	u16	tmpV16;
	u8	EFUSE_ACCESS_ON_8192E = 0x69;
	u8	EFUSE_ACCESS_OFF_8192E = 0x00;

	if (PwrState == _TRUE) {
		rtw_write8(pAdapter, REG_EFUSE_ACCESS, EFUSE_ACCESS_ON_8192E);

		/* Reset: 0x0000h[28], default valid */
		tmpV16 =  rtw_read16(pAdapter, REG_SYS_FUNC_EN);
		if (!(tmpV16 & FEN_ELDR)) {
			tmpV16 |= FEN_ELDR ;
			rtw_write16(pAdapter, REG_SYS_FUNC_EN, tmpV16);
		}

		/* Clock: Gated(0x0008h[5]) 8M(0x0008h[1]) clock from ANA, default valid */
		tmpV16 = rtw_read16(pAdapter, REG_SYS_CLKR);
		if ((!(tmpV16 & LOADER_CLK_EN))  || (!(tmpV16 & ANA8M))) {
			tmpV16 |= (LOADER_CLK_EN | ANA8M) ;
			rtw_write16(pAdapter, REG_SYS_CLKR, tmpV16);
		}


		if (bWrite == _TRUE) {
			/* Enable LDO 2.5V before read/write action */
			tempval = rtw_read8(pAdapter, EFUSE_TEST + 3);
			tempval &= 0x07; /* 0x34[30:27] = 4¡¦1110 => LDOE25 voltage select to 2.25V Suggested by SD1 Jackie & DD -Tm_lin */
			/* tempval |= (VOLTAGE_V25 << 4); */
			tempval |= 0x70;
			rtw_write8(pAdapter, EFUSE_TEST + 3, (tempval | 0x80));
		}

	} else {
		rtw_write8(pAdapter, REG_EFUSE_ACCESS, EFUSE_ACCESS_OFF_8192E);

		if (bWrite == _TRUE) {
			/* Disable LDO 2.5V after read/write action */
			tempval = rtw_read8(pAdapter, EFUSE_TEST + 3);
			rtw_write8(pAdapter, EFUSE_TEST + 3, (tempval & 0x7F));
		}
	}
}

static VOID
rtl8192E_EfusePowerSwitch(
	IN	PADAPTER	pAdapter,
	IN	u8		bWrite,
	IN	u8		PwrState)
{
	Hal_EfusePowerSwitch8192E(pAdapter, bWrite, PwrState);
}


static bool efuse_read_phymap(
	PADAPTER	Adapter,
	u8			*pbuf,	/* buffer to store efuse physical map */
	u16			*size	/* the max byte to read. will update to byte read */
)
{
	u8 *pos = pbuf;
	u16 limit = *size;
	u16 addr = 0;
	bool reach_end = _FALSE;

	/*  */
	/* Refresh efuse init map as all 0xFF. */
	/*  */
	_rtw_memset(pbuf, 0xFF, limit);


	/*  */
	/* Read physical efuse content. */
	/*  */
	while (addr < limit) {
		ReadEFuseByte(Adapter, addr, pos, _FALSE);
		if (*pos != 0xFF) {
			pos++;
			addr++;
		} else {
			reach_end = _TRUE;
			break;
		}
	}

	*size = addr;

	return reach_end;

}



static VOID
Hal_EfuseReadEFuse8192E(
	PADAPTER		Adapter,
	u16			_offset,
	u16			_size_byte,
	u8		*pbuf,
	IN	BOOLEAN	bPseudoTest
)
{
	u8	*efuseTbl = NULL;
	u8	rtemp8[1];
	u16	eFuse_Addr = 0;
	u8	offset, wren;
	u16	i, j;
	u16	**eFuseWord = NULL;
	u16	efuse_utilized = 0;
	u8	efuse_usage = 0;
	u8	u1temp = 0;

	/*  */
	/* Do NOT excess total size of EFuse table. Added by Roger, 2008.11.10. */
	/*  */
	if ((_offset + _size_byte) > EFUSE_MAP_LEN_8192E) {
		/* total E-Fuse table is 512bytes */
		RTW_INFO("Hal_EfuseReadEFuse8812A(): Invalid offset(%#x) with read bytes(%#x)!!\n", _offset, _size_byte);
		goto exit;
	}

	efuseTbl = (u8 *)rtw_zmalloc(EFUSE_MAP_LEN_8192E);
	if (efuseTbl == NULL) {
		RTW_INFO("%s: alloc efuseTbl fail!\n", __FUNCTION__);
		goto exit;
	}

	eFuseWord = (u16 **)rtw_malloc2d(EFUSE_MAX_SECTION_8192E, EFUSE_MAX_WORD_UNIT, 2);
	if (eFuseWord == NULL) {
		RTW_INFO("%s: alloc eFuseWord fail!\n", __FUNCTION__);
		goto exit;
	}

	/* 0. Refresh efuse init map as all oxFF. */
	for (i = 0; i < EFUSE_MAX_SECTION_8192E; i++)
		for (j = 0; j < EFUSE_MAX_WORD_UNIT; j++)
			eFuseWord[i][j] = 0xFFFF;

	/*  */
	/* 1. Read the first byte to check if efuse is empty!!! */
	/*  */
	/*  */
	ReadEFuseByte(Adapter, eFuse_Addr, rtemp8, bPseudoTest);
	if (*rtemp8 != 0xFF) {
		efuse_utilized++;
		/* RTW_INFO("efuse_Addr-%d efuse_data=%x\n", eFuse_Addr, *rtemp8); */
		eFuse_Addr++;
	} else {
		RTW_INFO("EFUSE is empty efuse_Addr-%d efuse_data=%x\n", eFuse_Addr, *rtemp8);
		goto exit;
	}


	/*  */
	/* 2. Read real efuse content. Filter PG header and every section data. */
	/*  */
	while ((*rtemp8 != 0xFF) && (eFuse_Addr < EFUSE_REAL_CONTENT_LEN_8192E)) {
		/* RTPRINT(FEEPROM, EFUSE_READ_ALL, ("efuse_Addr-%d efuse_data=%x\n", eFuse_Addr-1, *rtemp8)); */

		/* Check PG header for section num. */
		if ((*rtemp8 & 0x1F) == 0x0F) {	/* extended header */
			u1temp = ((*rtemp8 & 0xE0) >> 5);
			/* RTPRINT(FEEPROM, EFUSE_READ_ALL, ("extended header u1temp=%x *rtemp&0xE0 0x%x\n", u1temp, *rtemp8 & 0xE0)); */

			/* RTPRINT(FEEPROM, EFUSE_READ_ALL, ("extended header u1temp=%x\n", u1temp)); */

			ReadEFuseByte(Adapter, eFuse_Addr, rtemp8, bPseudoTest);

			/* RTPRINT(FEEPROM, EFUSE_READ_ALL, ("extended header efuse_Addr-%d efuse_data=%x\n", eFuse_Addr, *rtemp8));	 */

			if ((*rtemp8 & 0x0F) == 0x0F && (*rtemp8 != 0xFF)) {
				eFuse_Addr++;
				ReadEFuseByte(Adapter, eFuse_Addr, rtemp8, bPseudoTest);

				if (*rtemp8 != 0xFF && (eFuse_Addr < EFUSE_REAL_CONTENT_LEN_8192E))
					eFuse_Addr++;
				continue;
			} else {
				if (*rtemp8 == 0xFF) {
					*rtemp8 = 0x0F;
					efuse_OneByteWrite(Adapter, eFuse_Addr, *rtemp8, bPseudoTest);
					continue;
				}
				offset = ((*rtemp8 & 0xF0) >> 1) | u1temp;
				wren = (*rtemp8 & 0x0F);
				eFuse_Addr++;
			}
		} else {
			offset = ((*rtemp8 >> 4) & 0x0f);
			wren = (*rtemp8 & 0x0f);
		}

		if (offset < EFUSE_MAX_SECTION_8192E) {
			/* Get word enable value from PG header */
			/* RTPRINT(FEEPROM, EFUSE_READ_ALL, ("Offset-%d Worden=%x\n", offset, wren)); */

			for (i = 0; i < EFUSE_MAX_WORD_UNIT; i++) {
				/* Check word enable condition in the section				 */
				if (!(wren & 0x01)) {
					/* RTPRINT(FEEPROM, EFUSE_READ_ALL, ("Addr=%d\n", eFuse_Addr)); */
					ReadEFuseByte(Adapter, eFuse_Addr, rtemp8, bPseudoTest);
					eFuse_Addr++;
					/* RTPRINT(FEEPROM, EFUSE_READ_ALL, ("Data=0x%x\n", *rtemp8)); 				 */
					efuse_utilized++;
					eFuseWord[offset][i] = (*rtemp8 & 0xff);


					if (eFuse_Addr >= EFUSE_REAL_CONTENT_LEN_8192E)
						break;

					/* RTPRINT(FEEPROM, EFUSE_READ_ALL, ("Addr=%d", eFuse_Addr)); */
					ReadEFuseByte(Adapter, eFuse_Addr, rtemp8, bPseudoTest);
					eFuse_Addr++;
					/* RTPRINT(FEEPROM, EFUSE_READ_ALL, ("Data=0x%x\n", *rtemp8)); 				 */

					efuse_utilized++;
					eFuseWord[offset][i] |= (((u2Byte)*rtemp8 << 8) & 0xff00);

					if (eFuse_Addr >= EFUSE_REAL_CONTENT_LEN_8192E)
						break;
				}

				wren >>= 1;

			}
		} else { /* deal with error offset,skip error data		 */
			RTW_PRINT("invalid offset:0x%02x\n", offset);
			for (i = 0; i < EFUSE_MAX_WORD_UNIT; i++) {
				/* Check word enable condition in the section				 */
				if (!(wren & 0x01)) {
					eFuse_Addr++;
					efuse_utilized++;
					if (eFuse_Addr >= EFUSE_REAL_CONTENT_LEN_8192E)
						break;
					eFuse_Addr++;
					efuse_utilized++;
					if (eFuse_Addr >= EFUSE_REAL_CONTENT_LEN_8192E)
						break;
				}
			}
		}
		/* Read next PG header */
		ReadEFuseByte(Adapter, eFuse_Addr, rtemp8, bPseudoTest);
		/* RTPRINT(FEEPROM, EFUSE_READ_ALL, ("Addr=%d rtemp 0x%x\n", eFuse_Addr, *rtemp8)); */

		if (*rtemp8 != 0xFF && (eFuse_Addr < EFUSE_REAL_CONTENT_LEN_8192E)) {
			efuse_utilized++;
			eFuse_Addr++;
		}
	}

	/*  */
	/* 3. Collect 16 sections and 4 word unit into Efuse map. */
	/*  */
	for (i = 0; i < EFUSE_MAX_SECTION_8192E; i++) {
		for (j = 0; j < EFUSE_MAX_WORD_UNIT; j++) {
			efuseTbl[(i * 8) + (j * 2)] = (eFuseWord[i][j] & 0xff);
			efuseTbl[(i * 8) + ((j * 2) + 1)] = ((eFuseWord[i][j] >> 8) & 0xff);
		}
	}


	/*  */
	/* 4. Copy from Efuse map to output pointer memory!!! */
	/*  */
	for (i = 0; i < _size_byte; i++)
		pbuf[i] = efuseTbl[_offset + i];

	/*  */
	/* 5. Calculate Efuse utilization. */
	/*  */
	efuse_usage = (u1Byte)((eFuse_Addr * 100) / EFUSE_REAL_CONTENT_LEN_8192E);
	rtw_hal_set_hwreg(Adapter, HW_VAR_EFUSE_BYTES, (u8 *)&eFuse_Addr);

exit:
	if (efuseTbl)
		rtw_mfree(efuseTbl, EFUSE_MAP_LEN_8192E);

	if (eFuseWord)
		rtw_mfree2d((void *)eFuseWord, EFUSE_MAX_SECTION_8192E, EFUSE_MAX_WORD_UNIT, sizeof(u16));
}

static VOID
rtl8192E_ReadEFuse(
	PADAPTER	Adapter,
	u8		efuseType,
	u16		_offset,
	u16		_size_byte,
	u8	*pbuf,
	IN	BOOLEAN	bPseudoTest
)
{
	Hal_EfuseReadEFuse8192E(Adapter, _offset, _size_byte, pbuf, bPseudoTest);
}

/* Do not support BT */
VOID
Hal_EFUSEGetEfuseDefinition8192E(
	IN		PADAPTER	pAdapter,
	IN		u1Byte		efuseType,
	IN		u1Byte		type,
	OUT		PVOID		pOut
)
{
	switch (type) {
	case TYPE_EFUSE_MAX_SECTION: {
		pu1Byte	pMax_section;
		pMax_section = (pu1Byte)pOut;

		if (efuseType == EFUSE_WIFI)
			*pMax_section = EFUSE_MAX_SECTION_8192E;
		else
			*pMax_section = EFUSE_BT_MAX_SECTION_8192E;
	}
	break;
	case TYPE_EFUSE_REAL_CONTENT_LEN: {
		pu2Byte pu2Tmp;
		pu2Tmp = (pu2Byte)pOut;
		if (efuseType == EFUSE_WIFI)
			*pu2Tmp = EFUSE_REAL_CONTENT_LEN_8192E;
		else
			*pu2Tmp = EFUSE_BT_REAL_CONTENT_LEN_8192E;
	}
	break;
	case TYPE_EFUSE_CONTENT_LEN_BANK: {
		pu2Byte pu2Tmp;
		pu2Tmp = (pu2Byte)pOut;
		if (efuseType == EFUSE_WIFI)
			*pu2Tmp = EFUSE_REAL_CONTENT_LEN_8192E;
		else
			*pu2Tmp = EFUSE_BT_REAL_BANK_CONTENT_LEN_8192E;
	}
	break;
	case TYPE_AVAILABLE_EFUSE_BYTES_BANK: {
		pu2Byte pu2Tmp;
		pu2Tmp = (pu2Byte)pOut;
		if (efuseType == EFUSE_WIFI)
			*pu2Tmp = (u2Byte)(EFUSE_REAL_CONTENT_LEN_8192E-EFUSE_OOB_PROTECT_BYTES_8192E);
		else
			*pu2Tmp = (u2Byte)(EFUSE_BT_REAL_BANK_CONTENT_LEN_8192E-EFUSE_PROTECT_BYTES_BANK_8192E);
	}
	break;
	case TYPE_AVAILABLE_EFUSE_BYTES_TOTAL: {
		pu2Byte pu2Tmp;
		pu2Tmp = (pu2Byte)pOut;
		if (efuseType == EFUSE_WIFI)
			*pu2Tmp = (u2Byte)(EFUSE_REAL_CONTENT_LEN_8192E-EFUSE_OOB_PROTECT_BYTES_8192E);
		else
			*pu2Tmp = (u2Byte)(EFUSE_BT_REAL_CONTENT_LEN_8192E-(EFUSE_PROTECT_BYTES_BANK_8192E * 3));
	}
	break;
	case TYPE_EFUSE_MAP_LEN: {
		pu2Byte pu2Tmp;
		pu2Tmp = (pu2Byte)pOut;

		if (efuseType == EFUSE_WIFI)
			*pu2Tmp = (u2Byte)EFUSE_MAP_LEN_8192E;
		else
			*pu2Tmp = (u2Byte)EFUSE_BT_MAP_LEN_8192E;
	}
	break;
	case TYPE_EFUSE_PROTECT_BYTES_BANK: {
		pu1Byte pu1Tmp;
		pu1Tmp = (pu1Byte)pOut;
		if (efuseType == EFUSE_WIFI)
			*pu1Tmp = (u1Byte)(EFUSE_OOB_PROTECT_BYTES_8192E);
		else
			*pu1Tmp = (u1Byte)(EFUSE_PROTECT_BYTES_BANK_8192E);
	}
	break;
	default: {
		pu1Byte pu1Tmp;
		pu1Tmp = (pu1Byte)pOut;
		*pu1Tmp = 0;
	}
	break;
	}
}
VOID
Hal_EFUSEGetEfuseDefinition_Pseudo8192E(
	IN		PADAPTER	pAdapter,
	IN		u8			efuseType,
	IN		u8			type,
	OUT		PVOID		pOut
)
{
	switch (type) {
	case TYPE_EFUSE_MAX_SECTION: {
		pu1Byte	pMax_section;
		pMax_section = (pu1Byte)pOut;
		if (efuseType == EFUSE_WIFI)
			*pMax_section = EFUSE_MAX_SECTION_8192E;
		else
			*pMax_section = EFUSE_BT_MAX_SECTION_8192E;
	}
	break;
	case TYPE_EFUSE_REAL_CONTENT_LEN: {
		pu2Byte pu2Tmp;
		pu2Tmp = (pu2Byte)pOut;
		if (efuseType == EFUSE_WIFI)
			*pu2Tmp = EFUSE_REAL_CONTENT_LEN_8192E;
		else
			*pu2Tmp = EFUSE_BT_REAL_CONTENT_LEN_8192E;
	}
	break;
	case TYPE_EFUSE_CONTENT_LEN_BANK: {
		pu2Byte pu2Tmp;
		pu2Tmp = (pu2Byte)pOut;
		if (efuseType == EFUSE_WIFI)
			*pu2Tmp = EFUSE_REAL_CONTENT_LEN_8192E;
		else
			*pu2Tmp = EFUSE_BT_REAL_BANK_CONTENT_LEN_8192E;
	}
	break;
	case TYPE_AVAILABLE_EFUSE_BYTES_BANK: {
		pu2Byte pu2Tmp;
		pu2Tmp = (pu2Byte)pOut;
		if (efuseType == EFUSE_WIFI)
			*pu2Tmp = (u2Byte)(EFUSE_REAL_CONTENT_LEN_8192E-EFUSE_OOB_PROTECT_BYTES_8192E);
		else
			*pu2Tmp = (u2Byte)(EFUSE_BT_REAL_BANK_CONTENT_LEN_8192E-EFUSE_PROTECT_BYTES_BANK_8192E);
	}
	break;
	case TYPE_AVAILABLE_EFUSE_BYTES_TOTAL: {
		pu2Byte pu2Tmp;
		pu2Tmp = (pu2Byte)pOut;
		if (efuseType == EFUSE_WIFI)
			*pu2Tmp = (u2Byte)(EFUSE_REAL_CONTENT_LEN_8192E-EFUSE_OOB_PROTECT_BYTES_8192E);

		else
			*pu2Tmp = (u2Byte)(EFUSE_BT_REAL_CONTENT_LEN_8192E-(EFUSE_PROTECT_BYTES_BANK_8192E * 3));
	}
	break;
	case TYPE_EFUSE_MAP_LEN: {
		pu2Byte pu2Tmp;
		pu2Tmp = (pu2Byte)pOut;
		if (efuseType == EFUSE_WIFI)
			*pu2Tmp = (u2Byte)EFUSE_MAP_LEN_8192E;
		else
			*pu2Tmp = (u2Byte)EFUSE_BT_MAP_LEN_8192E;
	}
	break;
	case TYPE_EFUSE_PROTECT_BYTES_BANK: {
		pu1Byte pu1Tmp;
		pu1Tmp = (pu1Byte)pOut;
		if (efuseType == EFUSE_WIFI)
			*pu1Tmp = (u1Byte)(EFUSE_OOB_PROTECT_BYTES_8192E);
		else
			*pu1Tmp = (u1Byte)(EFUSE_PROTECT_BYTES_BANK_8192E);
	}
	break;
	default: {
		pu1Byte pu1Tmp;
		pu1Tmp = (pu1Byte)pOut;
		*pu1Tmp = 0;
	}
	break;
	}
}


static VOID
rtl8192E_EFUSE_GetEfuseDefinition(
	IN		PADAPTER	pAdapter,
	IN		u8		efuseType,
	IN		u8		type,
	OUT		void		*pOut,
	IN		BOOLEAN		bPseudoTest
)
{
	if (bPseudoTest)
		Hal_EFUSEGetEfuseDefinition_Pseudo8192E(pAdapter, efuseType, type, pOut);
	else
		Hal_EFUSEGetEfuseDefinition8192E(pAdapter, efuseType, type, pOut);
}

static u8
Hal_EfuseWordEnableDataWrite8192E(IN	PADAPTER	pAdapter,
				  IN	u16		efuse_addr,
				  IN	u8		word_en,
				  IN	u8		*data,
				  IN	BOOLEAN		bPseudoTest)
{
	u16	tmpaddr = 0;
	u16	start_addr = efuse_addr;
	u8	badworden = 0x0F;
	u8	tmpdata[8];

	_rtw_memset((PVOID)tmpdata, 0xff, PGPKT_DATA_SIZE);

	if (!(word_en & BIT0)) {
		tmpaddr = start_addr;
		efuse_OneByteWrite(pAdapter, start_addr++, data[0], bPseudoTest);
		efuse_OneByteWrite(pAdapter, start_addr++, data[1], bPseudoTest);
		phy_set_mac_reg(pAdapter, EFUSE_TEST, BIT26, 0);

		efuse_OneByteRead(pAdapter, tmpaddr, &tmpdata[0], bPseudoTest);
		efuse_OneByteRead(pAdapter, tmpaddr + 1, &tmpdata[1], bPseudoTest);
		phy_set_mac_reg(pAdapter, EFUSE_TEST, BIT26, 1);

		if ((data[0] != tmpdata[0]) || (data[1] != tmpdata[1]))
			badworden &= (~BIT0);
	}
	if (!(word_en & BIT1)) {
		tmpaddr = start_addr;
		efuse_OneByteWrite(pAdapter, start_addr++, data[2], bPseudoTest);
		efuse_OneByteWrite(pAdapter, start_addr++, data[3], bPseudoTest);
		phy_set_mac_reg(pAdapter, EFUSE_TEST, BIT26, 0);

		efuse_OneByteRead(pAdapter, tmpaddr    , &tmpdata[2], bPseudoTest);
		efuse_OneByteRead(pAdapter, tmpaddr + 1, &tmpdata[3], bPseudoTest);
		phy_set_mac_reg(pAdapter, EFUSE_TEST, BIT26, 1);

		if ((data[2] != tmpdata[2]) || (data[3] != tmpdata[3]))
			badworden &= (~BIT1);
	}
	if (!(word_en & BIT2)) {
		tmpaddr = start_addr;
		efuse_OneByteWrite(pAdapter, start_addr++, data[4], bPseudoTest);
		efuse_OneByteWrite(pAdapter, start_addr++, data[5], bPseudoTest);
		phy_set_mac_reg(pAdapter, EFUSE_TEST, BIT26, 0);

		efuse_OneByteRead(pAdapter, tmpaddr, &tmpdata[4], bPseudoTest);
		efuse_OneByteRead(pAdapter, tmpaddr + 1, &tmpdata[5], bPseudoTest);
		phy_set_mac_reg(pAdapter, EFUSE_TEST, BIT26, 1);

		if ((data[4] != tmpdata[4]) || (data[5] != tmpdata[5]))
			badworden &= (~BIT2);
	}
	if (!(word_en & BIT3)) {
		tmpaddr = start_addr;
		efuse_OneByteWrite(pAdapter, start_addr++, data[6], bPseudoTest);
		efuse_OneByteWrite(pAdapter, start_addr++, data[7], bPseudoTest);
		phy_set_mac_reg(pAdapter, EFUSE_TEST, BIT26, 0);

		efuse_OneByteRead(pAdapter, tmpaddr, &tmpdata[6], bPseudoTest);
		efuse_OneByteRead(pAdapter, tmpaddr + 1, &tmpdata[7], bPseudoTest);
		phy_set_mac_reg(pAdapter, EFUSE_TEST, BIT26, 1);

		if ((data[6] != tmpdata[6]) || (data[7] != tmpdata[7]))
			badworden &= (~BIT3);
	}
	return badworden;
}

static u8
rtl8192E_Efuse_WordEnableDataWrite(IN	PADAPTER	pAdapter,
				   IN	u16		efuse_addr,
				   IN	u8		word_en,
				   IN	u8		*data,
				   IN	BOOLEAN		bPseudoTest)
{
	u8	ret = 0;

	ret = Hal_EfuseWordEnableDataWrite8192E(pAdapter, efuse_addr, word_en, data, bPseudoTest);

	return ret;
}

static u16
hal_EfuseGetCurrentSize_8192E(IN	PADAPTER	pAdapter,
			      IN		BOOLEAN			bPseudoTest)
{
	int	bContinual = _TRUE;

	u16	efuse_addr = 0;
	u8	hoffset = 0, hworden = 0;
	u8	efuse_data, word_cnts = 0;

	if (bPseudoTest)
		efuse_addr = (u16)(fakeEfuseUsedBytes);
	else
		rtw_hal_get_hwreg(pAdapter, HW_VAR_EFUSE_BYTES, (u8 *)&efuse_addr);
	/* RTPRINT(FEEPROM, EFUSE_PG, ("hal_EfuseGetCurrentSize_8723A(), start_efuse_addr = %d\n", efuse_addr)); */

	while (bContinual &&
	       efuse_OneByteRead(pAdapter, efuse_addr , &efuse_data, bPseudoTest) &&
	       (efuse_addr  < EFUSE_REAL_CONTENT_LEN_8192E)) {
		if (efuse_data != 0xFF) {
			if ((efuse_data & 0x1F) == 0x0F) {	/* extended header */
				hoffset = efuse_data;
				efuse_addr++;
				efuse_OneByteRead(pAdapter, efuse_addr , &efuse_data, bPseudoTest);
				if ((efuse_data & 0x0F) == 0x0F && (efuse_data != 0xFF)) {
					efuse_addr++;
					continue;
				} else {
					hoffset = ((hoffset & 0xE0) >> 5) | ((efuse_data & 0xF0) >> 1);
					hworden = efuse_data & 0x0F;
				}
			} else {
				hoffset = (efuse_data >> 4) & 0x0F;
				hworden =  efuse_data & 0x0F;
			}
			word_cnts = Efuse_CalculateWordCnts(hworden);
			/* read next header */
			efuse_addr = efuse_addr + (word_cnts * 2) + 1;
		} else
			bContinual = _FALSE ;
	}

	if (bPseudoTest) {
		fakeEfuseUsedBytes = efuse_addr;
		/* RTPRINT(FEEPROM, EFUSE_PG, ("hal_EfuseGetCurrentSize_8723A(), return %d\n", fakeEfuseUsedBytes)); */
	} else {
		rtw_hal_set_hwreg(pAdapter, HW_VAR_EFUSE_BYTES, (u8 *)&efuse_addr);
		RTW_INFO("%s(), return %d\n", __func__, efuse_addr);
	}

	return efuse_addr;
}

static u16
rtl8192E_EfuseGetCurrentSize(
	IN	PADAPTER	pAdapter,
	IN	u8			efuseType,
	IN	BOOLEAN		bPseudoTest)
{
	u16	ret = 0;

	ret = hal_EfuseGetCurrentSize_8192E(pAdapter, bPseudoTest);

	return ret;
}


static int
hal_EfusePgPacketRead_8192E(
	IN	PADAPTER	pAdapter,
	IN	u8			offset,
	IN	u8			*data,
	IN	BOOLEAN		bPseudoTest)
{
	u8	ReadState = PG_STATE_HEADER;

	int	bContinual = _TRUE;
	int	bDataEmpty = _TRUE ;

	u8	efuse_data, word_cnts = 0;
	u16	efuse_addr = 0;
	u8	hoffset = 0, hworden = 0;
	u8	tmpidx = 0;
	u8	tmpdata[8];
	u8	max_section = 0;
	u8	tmp_header = 0;

	if (data == NULL)
		return _FALSE;
	if (offset > EFUSE_MAX_SECTION_8192E)
		return _FALSE;

	_rtw_memset((PVOID)data, 0xff, sizeof(u8) * PGPKT_DATA_SIZE);
	_rtw_memset((PVOID)tmpdata, 0xff, sizeof(u8) * PGPKT_DATA_SIZE);


	/*  */
	/* <Roger_TODO> Efuse has been pre-programmed dummy 5Bytes at the end of Efuse by CP. */
	/* Skip dummy parts to prevent unexpected data read from Efuse. */
	/* By pass right now. 2009.02.19. */
	/*  */
	while (bContinual && (efuse_addr  < EFUSE_REAL_CONTENT_LEN_8192E)) {
		/* -------  Header Read ------------- */
		if (ReadState & PG_STATE_HEADER) {
			if (efuse_OneByteRead(pAdapter, efuse_addr , &efuse_data, bPseudoTest) && (efuse_data != 0xFF)) {
				if (EXT_HEADER(efuse_data)) {
					tmp_header = efuse_data;
					efuse_addr++;
					efuse_OneByteRead(pAdapter, efuse_addr , &efuse_data, bPseudoTest);
					if (!ALL_WORDS_DISABLED(efuse_data)) {
						hoffset = ((tmp_header & 0xE0) >> 5) | ((efuse_data & 0xF0) >> 1);
						hworden = efuse_data & 0x0F;
					} else {
						RTW_INFO("Error, All words disabled\n");
						efuse_addr++;
						continue;
					}
				} else {
					hoffset = (efuse_data >> 4) & 0x0F;
					hworden =  efuse_data & 0x0F;
				}
				word_cnts = Efuse_CalculateWordCnts(hworden);
				bDataEmpty = _TRUE ;

				if (hoffset == offset) {
					for (tmpidx = 0; tmpidx < word_cnts * 2 ; tmpidx++) {
						if (efuse_OneByteRead(pAdapter, efuse_addr + 1 + tmpidx , &efuse_data, bPseudoTest)) {
							tmpdata[tmpidx] = efuse_data;
							if (efuse_data != 0xff)
								bDataEmpty = _FALSE;
						}
					}
					if (bDataEmpty == _FALSE)
						ReadState = PG_STATE_DATA;
					else { /* read next header */
						efuse_addr = efuse_addr + (word_cnts * 2) + 1;
						ReadState = PG_STATE_HEADER;
					}
				} else { /* read next header */
					efuse_addr = efuse_addr + (word_cnts * 2) + 1;
					ReadState = PG_STATE_HEADER;
				}

			} else
				bContinual = _FALSE ;
		}
		/* -------  Data section Read ------------- */
		else if (ReadState & PG_STATE_DATA) {
			efuse_WordEnableDataRead(hworden, tmpdata, data);
			efuse_addr = efuse_addr + (word_cnts * 2) + 1;
			ReadState = PG_STATE_HEADER;
		}

	}

	if ((data[0] == 0xff) && (data[1] == 0xff) && (data[2] == 0xff)  && (data[3] == 0xff) &&
	    (data[4] == 0xff) && (data[5] == 0xff) && (data[6] == 0xff)  && (data[7] == 0xff))
		return _FALSE;
	else
		return _TRUE;

}

static int
rtl8192E_Efuse_PgPacketRead(IN	PADAPTER	pAdapter,
			    IN	u8			offset,
			    IN	u8			*data,
			    IN	BOOLEAN		bPseudoTest)
{
	int	ret = 0;

	ret = hal_EfusePgPacketRead_8192E(pAdapter, offset, data, bPseudoTest);

	return ret;
}
#if 0
int
hal_EfusePgPacketWrite_8192E(IN	PADAPTER	pAdapter,
			     IN	u8			offset,
			     IN	u8			word_en,
			     IN	u8			*data,
			     IN	BOOLEAN		bPseudoTest)
{
	u8 WriteState = PG_STATE_HEADER;

	int bContinual = _TRUE, bDataEmpty = _TRUE;
	/* int bResult = _TRUE; */
	u16	efuse_addr = 0;
	u8	efuse_data;

	u8	pg_header = 0, pg_header_temp = 0;

	u8	tmp_word_cnts = 0, target_word_cnts = 0;
	u8	tmp_header, match_word_en, tmp_word_en;

	PGPKT_STRUCT target_pkt;
	PGPKT_STRUCT tmp_pkt;

	u8	originaldata[sizeof(u8) * 8];
	u8	tmpindex = 0, badworden = 0x0F;

	static int	repeat_times = 0;

	BOOLEAN		bExtendedHeader = _FALSE;
	u8	efuseType = EFUSE_WIFI;

	/*  */
	/* <Roger_Notes> Efuse has been pre-programmed dummy 5Bytes at the end of Efuse by CP. */
	/* So we have to prevent unexpected data string connection, which will cause */
	/* incorrect data auto-load from HW. The total size is equal or smaller than 498bytes */
	/* (i.e., offset 0~497, and dummy 1bytes) expected after CP test. */
	/* 2009.02.19. */
	/*  */
	if (Efuse_GetCurrentSize(pAdapter, efuseType, bPseudoTest) >= (EFUSE_REAL_CONTENT_LEN_8192E-EFUSE_OOB_PROTECT_BYTES_8192E)) {
		RTW_INFO("hal_EfusePgPacketWrite_8812A() error: %x >= %x\n", Efuse_GetCurrentSize(pAdapter, efuseType, bPseudoTest), (EFUSE_REAL_CONTENT_LEN_8192E-EFUSE_OOB_PROTECT_BYTES_8192E));
		return _FALSE;
	}

	/* Init the 8 bytes content as 0xff */
	target_pkt.offset = offset;
	target_pkt.word_en = word_en;
	/* Initial the value to avoid compile warning */
	tmp_pkt.offset = 0;
	tmp_pkt.word_en = 0;

	/* RTW_INFO("hal_EfusePgPacketWrite_8812A target offset 0x%x word_en 0x%x\n", target_pkt.offset, target_pkt.word_en); */

	_rtw_memset((PVOID)target_pkt.data, 0xFF, sizeof(u8) * 8);

	efuse_WordEnableDataRead(word_en, data, target_pkt.data);
	target_word_cnts = Efuse_CalculateWordCnts(target_pkt.word_en);

	/* efuse_reg_ctrl(pAdapter,_TRUE); */ /* power on */
	/* RTW_INFO("EFUSE Power ON\n"); */

	/*  */
	/* <Roger_Notes> Efuse has been pre-programmed dummy 5Bytes at the end of Efuse by CP. */
	/* So we have to prevent unexpected data string connection, which will cause */
	/* incorrect data auto-load from HW. Dummy 1bytes is additional. */
	/* 2009.02.19. */
	/*  */
	while (bContinual && (efuse_addr  < (EFUSE_REAL_CONTENT_LEN_8192E-EFUSE_OOB_PROTECT_BYTES_8192E))) {
		if (WriteState == PG_STATE_HEADER) {
			bDataEmpty = _TRUE;
			badworden = 0x0F;
			/* ************	so ******************* */
			/* RTW_INFO("EFUSE PG_STATE_HEADER\n"); */
			if (efuse_OneByteRead(pAdapter, efuse_addr , &efuse_data, bPseudoTest) &&
			    (efuse_data != 0xFF)) {
				if ((efuse_data & 0x1F) == 0x0F) {	/* extended header */
					tmp_header = efuse_data;
					efuse_addr++;
					efuse_OneByteRead(pAdapter, efuse_addr , &efuse_data, bPseudoTest);
					if ((efuse_data & 0x0F) == 0x0F) { /* wren fail */
						efuse_addr++;
						continue;
					} else {
						tmp_pkt.offset = ((tmp_header & 0xE0) >> 5) | ((efuse_data & 0xF0) >> 1);
						tmp_pkt.word_en = efuse_data & 0x0F;
					}
				} else {
					tmp_header	=  efuse_data;
					tmp_pkt.offset	= (tmp_header >> 4) & 0x0F;
					tmp_pkt.word_en = tmp_header & 0x0F;
				}
				tmp_word_cnts =  Efuse_CalculateWordCnts(tmp_pkt.word_en);

				/* RTW_INFO("section offset 0x%x worden 0x%x\n", tmp_pkt.offset, tmp_pkt.word_en); */

				/* ************	so-1 ******************* */
				if (tmp_pkt.offset  != target_pkt.offset) {
					efuse_addr = efuse_addr + (tmp_word_cnts * 2) + 1; /* Next pg_packet */
#if (EFUSE_ERROE_HANDLE == 1)
					WriteState = PG_STATE_HEADER;
#endif
				} else {	/* write the same offset */
					/* RTW_INFO("hal_EfusePgPacketWrite_8812A section offset the same\n"); */
					/* ************	so-2 ******************* */
					for (tmpindex = 0 ; tmpindex < (tmp_word_cnts * 2) ; tmpindex++) {
						if (efuse_OneByteRead(pAdapter, (efuse_addr + 1 + tmpindex) , &efuse_data, bPseudoTest) && (efuse_data != 0xFF))
							bDataEmpty = _FALSE;
					}
					/* ************	so-2-1 ******************* */
					if (bDataEmpty == _FALSE) {
						/* RTW_INFO("hal_EfusePgPacketWrite_8812A section offset the same and data is NOT empty\n"); */

						efuse_addr = efuse_addr + (tmp_word_cnts * 2) + 1; /* Next pg_packet										 */
#if (EFUSE_ERROE_HANDLE == 1)
						WriteState = PG_STATE_HEADER;
#endif
					} else {
						/* ************  so-2-2 ******************* */
						/* RTW_INFO("hal_EfusePgPacketWrite_8812A section data empty\n"); */
						match_word_en = 0x0F;			/* same bit as original wren */
						if (!((target_pkt.word_en & BIT0) | (tmp_pkt.word_en & BIT0)))
							match_word_en &= (~BIT0);
						if (!((target_pkt.word_en & BIT1) | (tmp_pkt.word_en & BIT1)))
							match_word_en &= (~BIT1);
						if (!((target_pkt.word_en & BIT2) | (tmp_pkt.word_en & BIT2)))
							match_word_en &= (~BIT2);
						if (!((target_pkt.word_en & BIT3) | (tmp_pkt.word_en & BIT3)))
							match_word_en &= (~BIT3);

						/* ************	so-2-2-A ******************* */
						if ((match_word_en & 0x0F) != 0x0F) {
							badworden = Efuse_WordEnableDataWrite(pAdapter, efuse_addr + 1, tmp_pkt.word_en , target_pkt.data, bPseudoTest);

							/* ************	so-2-2-A-1 ******************* */
							/* ############################ */
							if (0x0F != (badworden & 0x0F)) {
								u8	reorg_offset = offset;
								u8	reorg_worden = badworden;
								Efuse_PgPacketWrite(pAdapter, reorg_offset, reorg_worden, target_pkt.data, bPseudoTest);
							}
							/* ############################ */

							tmp_word_en = 0x0F;	/* not the same bit as original wren */
							if ((target_pkt.word_en & BIT0) ^ (match_word_en & BIT0))
								tmp_word_en &= (~BIT0);
							if ((target_pkt.word_en & BIT1) ^ (match_word_en & BIT1))
								tmp_word_en &=	(~BIT1);
							if ((target_pkt.word_en & BIT2) ^ (match_word_en & BIT2))
								tmp_word_en &= (~BIT2);
							if ((target_pkt.word_en & BIT3) ^ (match_word_en & BIT3))
								tmp_word_en &= (~BIT3);

							/* ************	so-2-2-A-2 *******************	 */
							if ((tmp_word_en & 0x0F) != 0x0F) {
								/* reorganize other pg packet
								* efuse_addr = efuse_addr + (2*tmp_word_cnts) +1; */ /* next pg packet addr							 */
								efuse_addr = Efuse_GetCurrentSize(pAdapter, efuseType, bPseudoTest);
								/* =========================== */
								target_pkt.offset = offset;
								target_pkt.word_en = tmp_word_en;
								/* =========================== */
							} else
								bContinual = _FALSE;
#if (EFUSE_ERROE_HANDLE == 1)
							WriteState = PG_STATE_HEADER;
							repeat_times++;
							if (repeat_times > EFUSE_REPEAT_THRESHOLD_) {
								bContinual = _FALSE;
								/* bResult = _FALSE; */
							}
#endif
						} else { /* ************  so-2-2-B ******************* */
							/* reorganize other pg packet						 */
							efuse_addr = efuse_addr + (2 * tmp_word_cnts) + 1; /* next pg packet addr							 */
							/* =========================== */
							target_pkt.offset = offset;
							target_pkt.word_en = target_pkt.word_en;
							/* ===========================			 */
#if (EFUSE_ERROE_HANDLE == 1)
							WriteState = PG_STATE_HEADER;
#endif
						}
					}
				}
				RTW_INFO("EFUSE PG_STATE_HEADER-1\n");
			} else {	/* ************	s1: header == oxff	******************* */
				bExtendedHeader = _FALSE;

				if (target_pkt.offset >= EFUSE_MAX_SECTION_BASE) {
					pg_header = ((target_pkt.offset & 0x07) << 5) | 0x0F;

					/* RTW_INFO("hal_EfusePgPacketWrite_8812A extended pg_header[2:0] |0x0F 0x%x\n", pg_header); */

					efuse_OneByteWrite(pAdapter, efuse_addr, pg_header, bPseudoTest);
					phy_set_mac_reg(pAdapter, EFUSE_TEST, BIT26, 0);

					efuse_OneByteRead(pAdapter, efuse_addr, &tmp_header, bPseudoTest);
					phy_set_mac_reg(pAdapter, EFUSE_TEST, BIT26, 1);

					while (tmp_header == 0xFF) {
						/* RTW_INFO("hal_EfusePgPacketWrite_8812A extended pg_header[2:0] wirte fail\n"); */

						repeat_times++;

						if (repeat_times > EFUSE_REPEAT_THRESHOLD_) {
							bContinual = _FALSE;
							/* bResult = _FALSE; */
							efuse_addr++;
							break;
						}
						efuse_OneByteWrite(pAdapter, efuse_addr, pg_header, bPseudoTest);
						efuse_OneByteRead(pAdapter, efuse_addr, &tmp_header, bPseudoTest);
					}

					if (!bContinual)
						break;

					if (tmp_header == pg_header) {
						efuse_addr++;
						pg_header_temp = pg_header;
						pg_header = ((target_pkt.offset & 0x78) << 1) | target_pkt.word_en;

						/* RTW_INFO("hal_EfusePgPacketWrite_8812A extended pg_header[6:3] | worden 0x%x word_en 0x%x\n", pg_header, target_pkt.word_en); */

						efuse_OneByteWrite(pAdapter, efuse_addr, pg_header, bPseudoTest);
						phy_set_mac_reg(pAdapter, EFUSE_TEST, BIT26, 0);

						efuse_OneByteRead(pAdapter, efuse_addr, &tmp_header, bPseudoTest);
						phy_set_mac_reg(pAdapter, EFUSE_TEST, BIT26, 1);

						while (tmp_header == 0xFF) {
							repeat_times++;

							if (repeat_times > EFUSE_REPEAT_THRESHOLD_) {
								bContinual = _FALSE;
								/* bResult = _FALSE; */
								break;
							}
							efuse_OneByteWrite(pAdapter, efuse_addr, pg_header, bPseudoTest);
							efuse_OneByteRead(pAdapter, efuse_addr, &tmp_header, bPseudoTest);
						}

						if (!bContinual)
							break;

						if ((tmp_header & 0x0F) == 0x0F) { /* wren PG fail */
							repeat_times++;

							if (repeat_times > EFUSE_REPEAT_THRESHOLD_) {
								bContinual = _FALSE;
								/* bResult = _FALSE; */
								break;
							} else {
								efuse_addr++;
								continue;
							}
						} else if (pg_header != tmp_header) {	/* offset PG fail						 */
							bExtendedHeader = _TRUE;
							tmp_pkt.offset = ((pg_header_temp & 0xE0) >> 5) | ((tmp_header & 0xF0) >> 1);
							tmp_pkt.word_en =  tmp_header & 0x0F;
							tmp_word_cnts =  Efuse_CalculateWordCnts(tmp_pkt.word_en);
						}
					} else if ((tmp_header & 0x1F) == 0x0F) {	/* wrong extended header */
						efuse_addr += 2;
						continue;
					}
				} else {
					pg_header = ((target_pkt.offset << 4) & 0xf0) | target_pkt.word_en;
					efuse_OneByteWrite(pAdapter, efuse_addr, pg_header, bPseudoTest);
					efuse_OneByteRead(pAdapter, efuse_addr, &tmp_header, bPseudoTest);
				}

				if (tmp_header == pg_header) {
					/* ************  s1-1*******************								 */
					WriteState = PG_STATE_DATA;
				}
#if (EFUSE_ERROE_HANDLE == 1)
				else if (tmp_header == 0xFF) { /* ************	s1-3: if Write or read func doesn't work *******************		 */
					/* efuse_addr doesn't change */
					WriteState = PG_STATE_HEADER;
					repeat_times++;
					if (repeat_times > EFUSE_REPEAT_THRESHOLD_) {
						bContinual = _FALSE;
						/* bResult = _FALSE; */
					}
				}
#endif
				else {
					/* ************  s1-2 : fixed the header procedure *******************							 */
					if (!bExtendedHeader) {
						tmp_pkt.offset = (tmp_header >> 4) & 0x0F;
						tmp_pkt.word_en =  tmp_header & 0x0F;
						tmp_word_cnts =  Efuse_CalculateWordCnts(tmp_pkt.word_en);
					}

					/* ************	s1-2-A :cover the exist data ******************* */
					_rtw_memset(originaldata, 0xff, sizeof(u8) * 8);

					if (Efuse_PgPacketRead(pAdapter, tmp_pkt.offset, originaldata, bPseudoTest)) {
						/* check if data exist					 */
						/* efuse_reg_ctrl(pAdapter,_TRUE); */ /* power on */
						badworden = Efuse_WordEnableDataWrite(pAdapter, efuse_addr + 1, tmp_pkt.word_en, originaldata, bPseudoTest);
						/* ############################ */
						if (0x0F != (badworden & 0x0F)) {
							u8	reorg_offset = tmp_pkt.offset;
							u8	reorg_worden = badworden;
							Efuse_PgPacketWrite(pAdapter, reorg_offset, reorg_worden, originaldata, bPseudoTest);
							efuse_addr = Efuse_GetCurrentSize(pAdapter, efuseType, bPseudoTest);
						}
						/* ############################ */
						else {
							efuse_addr = efuse_addr + (tmp_word_cnts * 2) + 1; /* Next pg_packet							 */
						}
					}
					/* ************  s1-2-B: wrong address******************* */
					else {
						efuse_addr = efuse_addr + (tmp_word_cnts * 2) + 1; /* Next pg_packet */
					}

#if (EFUSE_ERROE_HANDLE == 1)
					WriteState = PG_STATE_HEADER;
					repeat_times++;
					if (repeat_times > EFUSE_REPEAT_THRESHOLD_) {
						bContinual = _FALSE;
						/* bResult = _FALSE; */
					}
#endif

					/* RTW_INFO("EFUSE PG_STATE_HEADER-2\n"); */
				}

			}

		}
		/* write data state */
		else if (WriteState == PG_STATE_DATA) {
			/* ************	s1-1  ******************* */
			/* RTW_INFO("EFUSE PG_STATE_DATA\n"); */
			badworden = 0x0f;
			badworden = Efuse_WordEnableDataWrite(pAdapter, efuse_addr + 1, target_pkt.word_en, target_pkt.data, bPseudoTest);
			if ((badworden & 0x0F) == 0x0F) {
				/* ************  s1-1-A ******************* */
				bContinual = _FALSE;
			} else {
				/* reorganize other pg packet */ /* ************  s1-1-B ******************* */
				efuse_addr = efuse_addr + (2 * target_word_cnts) + 1; /* next pg packet addr */

				/* =========================== */
				target_pkt.offset = offset;
				target_pkt.word_en = badworden;
				target_word_cnts = Efuse_CalculateWordCnts(target_pkt.word_en);
				/* ===========================			 */
#if (EFUSE_ERROE_HANDLE == 1)
				WriteState = PG_STATE_HEADER;
				repeat_times++;
				if (repeat_times > EFUSE_REPEAT_THRESHOLD_) {
					bContinual = _FALSE;
					/* bResult = _FALSE; */
				}
#endif
				/* RTW_INFO("EFUSE PG_STATE_HEADER-3\n"); */
			}
		}
	}

	if (efuse_addr  >= (EFUSE_REAL_CONTENT_LEN_8192E-EFUSE_OOB_PROTECT_BYTES_8192E))
		RTW_INFO("hal_EfusePgPacketWrite_8812A(): efuse_addr(%#x) Out of size!!\n", efuse_addr);
	/* efuse_reg_ctrl(pAdapter,_FALSE); */ /* power off */

	return _TRUE;
}
#else

BOOLEAN efuse_PgPacketCheck(
	PADAPTER	pAdapter,
	u8		efuseType,
	BOOLEAN		bPseudoTest
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);

	if (Efuse_GetCurrentSize(pAdapter, efuseType, bPseudoTest) >= (EFUSE_REAL_CONTENT_LEN_8192E - EFUSE_OOB_PROTECT_BYTES_8192E)) {
		RTW_INFO("%s()error: %x >= %x\n", __func__, Efuse_GetCurrentSize(pAdapter, efuseType, bPseudoTest), (EFUSE_REAL_CONTENT_LEN_8192E - EFUSE_OOB_PROTECT_BYTES_8192E));
		return _FALSE;
	}

	return _TRUE;
}


VOID
efuse_PgPacketConstruct(
	IN	    u8			offset,
	IN	    u8			word_en,
	IN	    u8			*pData,
	IN OUT	PPGPKT_STRUCT	pTargetPkt
)
{
	_rtw_memset((PVOID)pTargetPkt->data, 0xFF, sizeof(u8) * 8);
	pTargetPkt->offset = offset;
	pTargetPkt->word_en = word_en;
	efuse_WordEnableDataRead(word_en, pData, pTargetPkt->data);
	pTargetPkt->word_cnts = Efuse_CalculateWordCnts(pTargetPkt->word_en);

	RTW_INFO("efuse_PgPacketConstruct(), targetPkt, offset=%d, word_en=0x%x, word_cnts=%d\n", pTargetPkt->offset, pTargetPkt->word_en, pTargetPkt->word_cnts);
}

u2Byte Hal_EfusePgPacketExceptionHandle_8192E(
		PADAPTER	pAdapter,
		u2Byte		ErrOffset
		)
{
	BOOLEAN bPseudoTest = FALSE;
	u8 next = 0, next_next = 0, data = 0, i = 0, header = 0;
	u8 s = 0;
	u2Byte offset = ErrOffset;

	efuse_OneByteRead(pAdapter, offset, &header, bPseudoTest);

	if (EXT_HEADER(header)) {
		s = ((header & 0xF0) >> 4);

		/* Skip bad word enable to look two bytes ahead and determine if recoverable.*/
		offset += 1;
		efuse_OneByteRead(pAdapter, offset+1, &next, bPseudoTest);
		efuse_OneByteRead(pAdapter, offset+2, &next_next, bPseudoTest);
		if (next == 0xFF && next_next == 0xFF) {/* Have enough space to make fake data to recover bad header.*/
			switch (s) {
			case 0x0: case 0x2: case 0x4: case 0x6:
			case 0x8: case 0xA: case 0xC:
					for (i = 0; i < 3; ++i) {
						efuse_OneByteWrite(pAdapter, offset, 0x27, bPseudoTest);
						efuse_OneByteRead(pAdapter, offset, &data, bPseudoTest);
						if (data == 0x27)
							break;
					}
					break;
			case 0xE:
					for (i = 0; i < 3; ++i) {
						efuse_OneByteWrite(pAdapter, offset, 0x17, bPseudoTest);
						efuse_OneByteRead(pAdapter, offset, &data, bPseudoTest);
						if (data == 0x17)
							break;

					break;
			default:
					break;
			}
			efuse_OneByteWrite(pAdapter, offset+1, 0xFF, bPseudoTest);
			efuse_OneByteWrite(pAdapter, offset+2, 0xFF, bPseudoTest);
			offset += 3;
			ErrOffset = offset;
		}
	} else {/* 1-Byte header*/
		if (ALL_WORDS_DISABLED(header)) {
			u8 next = 0;

			efuse_OneByteRead(pAdapter, offset+1, &next, bPseudoTest);
			if (next == 0xFF) {/* Have enough space to make fake data to recover bad header.*/
				header = (header & 0xF0) | 0x7;
				for (i = 0; i < 3; ++i) {
					efuse_OneByteWrite(pAdapter, offset, header, bPseudoTest);
					efuse_OneByteRead(pAdapter, offset, &data, bPseudoTest);
					if (data == header)
						break;
				}
				efuse_OneByteWrite(pAdapter, offset+1, 0xFF, bPseudoTest);
				efuse_OneByteWrite(pAdapter, offset+2, 0xFF, bPseudoTest);
				offset += 2;
				ErrOffset = offset;
				}
		}
	}
	}

	return ErrOffset;
}

BOOLEAN hal_EfuseCheckIfDatafollowed(
	IN		PADAPTER		pAdapter,
	IN		u8			word_cnts,
	IN		u16			startAddr,
	IN		BOOLEAN			bPseudoTest
)
{
	BOOLEAN		bRet = FALSE;
	u8		i, efuse_data;

	for (i = 0; i < (word_cnts * 2) ; i++) {
		if (efuse_OneByteRead(pAdapter, (startAddr + i), &efuse_data, bPseudoTest) && (efuse_data != 0xFF))
			bRet = TRUE;
	}

	return bRet;
}


BOOLEAN
hal_EfuseWordEnMatched(
	IN	PPGPKT_STRUCT	pTargetPkt,
	IN	PPGPKT_STRUCT	pCurPkt,
	IN	u8			*pWden
)
{
	u8	match_word_en = 0x0F;	/* default all words are disabled */

	/* check if the same words are enabled both target and current PG packet */
	if (((pTargetPkt->word_en & BIT0) == 0) &&
		((pCurPkt->word_en & BIT0) == 0)) {
		match_word_en &= ~BIT0;				/* enable word 0 */
	}
	if (((pTargetPkt->word_en & BIT1) == 0) &&
		((pCurPkt->word_en & BIT1) == 0)) {
		match_word_en &= ~BIT1;				/* enable word 1 */
	}
	if (((pTargetPkt->word_en & BIT2) == 0) &&
		((pCurPkt->word_en & BIT2) == 0)) {
		match_word_en &= ~BIT2;				/* enable word 2 */
	}
	if (((pTargetPkt->word_en & BIT3) == 0) &&
		((pCurPkt->word_en & BIT3) == 0)) {
		match_word_en &= ~BIT3;				/* enable word 3 */
	}

	*pWden = match_word_en;

	if (match_word_en != 0xf)
		return TRUE;
	else
		return FALSE;
}


BOOLEAN
efuse_PgPacketPartialWrite(
	IN	    PADAPTER		pAdapter,
	IN	    u8			efuseType,
	IN OUT	u16			*pAddr,
	IN	    PPGPKT_STRUCT	pTargetPkt,
	IN	    BOOLEAN			bPseudoTest
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	PEFUSE_HAL		pEfuseHal = &(pHalData->EfuseHal);
	BOOLEAN			bRet = _FALSE;
	u8			i, efuse_data = 0, cur_header = 0;
	u8			matched_wden = 0, badworden = 0;
	u16			startAddr = 0;
	PGPKT_STRUCT	curPkt;
	u16			max_sec_num = (efuseType == EFUSE_WIFI) ? pEfuseHal->MaxSecNum_WiFi : pEfuseHal->MaxSecNum_BT;
	u16			efuse_max = pEfuseHal->BankSize;
	u16			efuse_max_available_len =
		(efuseType == EFUSE_WIFI) ? pEfuseHal->TotalAvailBytes_WiFi : pEfuseHal->TotalAvailBytes_BT;

	RTW_INFO("efuse_PgPacketPartialWrite()\n");

	if (bPseudoTest) {
		pEfuseHal->fakeEfuseBank = (efuseType == EFUSE_WIFI) ? 0 : pEfuseHal->fakeEfuseBank;
		Efuse_GetCurrentSize(pAdapter, efuseType, _TRUE);
	}

	 EFUSE_GetEfuseDefinition(pAdapter, efuseType, TYPE_AVAILABLE_EFUSE_BYTES_TOTAL, &efuse_max_available_len, bPseudoTest);
	 EFUSE_GetEfuseDefinition(pAdapter, efuseType, TYPE_EFUSE_CONTENT_LEN_BANK, &efuse_max, bPseudoTest);

	if (efuseType == EFUSE_WIFI) {
		if (bPseudoTest) {
#ifdef HAL_EFUSE_MEMORY
			startAddr = (u16)pEfuseHal->fakeEfuseUsedBytes;
#else
			startAddr = (u16)fakeEfuseUsedBytes;
#endif

		} else
			rtw_hal_get_hwreg(pAdapter, HW_VAR_EFUSE_BYTES, (u8 *)&startAddr);
	} else {
		if (bPseudoTest) {
#ifdef HAL_EFUSE_MEMORY
			startAddr = (u16)pEfuseHal->fakeBTEfuseUsedBytes;
#else
			startAddr = (u16)fakeBTEfuseUsedBytes;
#endif

		} else
			rtw_hal_get_hwreg(pAdapter, HW_VAR_EFUSE_BT_BYTES, (u8 *)&startAddr);
	}

	startAddr %= efuse_max;
	RTW_INFO("%s: startAddr=%#X\n", __FUNCTION__, startAddr);
	RTW_INFO("efuse_PgPacketPartialWrite(), startAddr = 0x%X\n", startAddr);

	while (1) {
		if (startAddr >= efuse_max_available_len) {
			bRet = _FALSE;
			RTW_INFO("startAddr(%d) >= efuse_max_available_len(%d)\n",
				 startAddr, efuse_max_available_len);
			break;
		}

		if (efuse_OneByteRead(pAdapter, startAddr, &efuse_data, bPseudoTest) && (efuse_data != 0xFF)) {
			if (EXT_HEADER(efuse_data)) {
				cur_header = efuse_data;
				startAddr++;
				efuse_OneByteRead(pAdapter, startAddr, &efuse_data, bPseudoTest);
				if (ALL_WORDS_DISABLED(efuse_data)) {
					u16 recoveredAddr = startAddr;

					recoveredAddr = Hal_EfusePgPacketExceptionHandle_8192E(pAdapter, startAddr - 1);

					if (recoveredAddr == (startAddr - 1)) {
						RTW_INFO("Error! All words disabled and the recovery failed, (Addr, Data) = (0x%X, 0x%X)\n",
							 startAddr, efuse_data);
						pAdapter->LastError = ERR_INVALID_DATA;
						break;

					} else {
						startAddr = recoveredAddr;
						RTW_INFO("Bad extension header but recovered => Keep going.\n");
						continue;
					}
				} else {
					curPkt.offset = ((cur_header & 0xE0) >> 5) | ((efuse_data & 0xF0) >> 1);
					curPkt.word_en = efuse_data & 0x0F;
				}
			} else {
				if (ALL_WORDS_DISABLED(efuse_data)) {
					u16 recoveredAddr = startAddr;

					recoveredAddr = Hal_EfusePgPacketExceptionHandle_8192E(pAdapter, startAddr);
					if (recoveredAddr != startAddr) {
						efuse_OneByteRead(pAdapter, startAddr, &efuse_data, bPseudoTest);
						RTW_INFO("Bad header but recovered => Read header again.\n");
					}
				}

				cur_header  =  efuse_data;
				curPkt.offset = (cur_header >> 4) & 0x0F;
				curPkt.word_en = cur_header & 0x0F;
			}

			if (curPkt.offset > max_sec_num) {
				pAdapter->LastError = ERR_OUT_OF_RANGE;
				pEfuseHal->Status = ERR_OUT_OF_RANGE;
				bRet = _FALSE;
				break;
			}

			curPkt.word_cnts = Efuse_CalculateWordCnts(curPkt.word_en);
			/* if same header is found but no data followed */
			/* write some part of data followed by the header. */
			if ((curPkt.offset == pTargetPkt->offset) &&
				(!hal_EfuseCheckIfDatafollowed(pAdapter, curPkt.word_cnts, startAddr + 1, bPseudoTest)) &&
				hal_EfuseWordEnMatched(pTargetPkt, &curPkt, &matched_wden)) {
				RTW_INFO("Need to partial write data by the previous wrote header\n");
				/* RT_ASSERT(_FALSE, ("Error, Need to partial write data by the previous wrote header!!\n")); */
				/* Here to write partial data */
				badworden = Efuse_WordEnableDataWrite(pAdapter, startAddr + 1, matched_wden, pTargetPkt->data, bPseudoTest);
				if (badworden != 0x0F) {
					u32 PgWriteSuccess = 0;
					/* if write fail on some words, write these bad words again */

					PgWriteSuccess = Efuse_PgPacketWrite(pAdapter, pTargetPkt->offset, badworden, pTargetPkt->data, bPseudoTest);

					if (!PgWriteSuccess) {
						bRet = _FALSE;	/* write fail, return */
						break;
					}
				}
				/* partial write ok, update the target packet for later use		 */
				for (i = 0; i < 4; i++) {
					if ((matched_wden & (0x1 << i)) == 0) {	/* this word has been written */
						pTargetPkt->word_en |= (0x1 << i);	/* disable the word */
					}
				}
				pTargetPkt->word_cnts = Efuse_CalculateWordCnts(pTargetPkt->word_en);
			}
			/* read from next header */
			startAddr = startAddr + (curPkt.word_cnts * 2) + 1;
		} else {
			/* not used header, 0xff */
			*pAddr = startAddr;
			RTW_INFO("Started from unused header offset=%d\n", startAddr);
			bRet = _TRUE;
			break;
		}
	}
	return bRet;
}


BOOLEAN
hal_EfuseFixHeaderProcess(
	IN		PADAPTER			pAdapter,
	IN		u8				efuseType,
	IN		PPGPKT_STRUCT		pFixPkt,
	IN		u16				*pAddr,
	IN		BOOLEAN				bPseudoTest
)
{
	u8	originaldata[8], badworden = 0;
	u16	efuse_addr = *pAddr;
	u32	PgWriteSuccess = 0;

	_rtw_memset((PVOID)originaldata, 0xff, 8);

	if (Efuse_PgPacketRead(pAdapter, pFixPkt->offset, originaldata, bPseudoTest)) {
		/* check if data exist */
		badworden = Efuse_WordEnableDataWrite(pAdapter, efuse_addr + 1, pFixPkt->word_en, originaldata, bPseudoTest);

		if (badworden != 0xf) {	/* write fail */
			PgWriteSuccess = Efuse_PgPacketWrite(pAdapter, pFixPkt->offset, badworden, originaldata, bPseudoTest);
			if (!PgWriteSuccess)
				return _FALSE;

			else
				efuse_addr = Efuse_GetCurrentSize(pAdapter, efuseType, bPseudoTest);
		} else
			efuse_addr = efuse_addr + (pFixPkt->word_cnts * 2) + 1;
	} else
		efuse_addr = efuse_addr + (pFixPkt->word_cnts * 2) + 1;
	*pAddr = efuse_addr;
	return _TRUE;
}



static u8
hal_EfusePgPacketWrite2ByteHeader(
	PADAPTER		padapter,
	u8				efuseType,
	u16				*pAddr,
	PPGPKT_STRUCT	pTargetPkt,
	u8				bPseudoTest)
{
	u16	efuse_addr, efuse_max_available_len = 0;
	u8	pg_header = 0, tmp_header = 0, pg_header_temp = 0;
	u8	repeatcnt = 0;


	/*	RTW_INFO("%s\n", __FUNCTION__); */
	EFUSE_GetEfuseDefinition(padapter, efuseType, TYPE_AVAILABLE_EFUSE_BYTES_BANK, &efuse_max_available_len, bPseudoTest);

	efuse_addr = *pAddr;

	if (efuse_addr >= efuse_max_available_len) {
		RTW_INFO("%s: addr(%d) over available(%d)!!\n", __FUNCTION__, efuse_addr, efuse_max_available_len);
		return _FALSE;
	}

	while (efuse_addr < efuse_max_available_len) {
		pg_header = ((pTargetPkt->offset & 0x07) << 5) | 0x0F;
		efuse_OneByteWrite(padapter, efuse_addr, pg_header, bPseudoTest);
		phy_set_mac_reg(padapter, EFUSE_TEST, BIT26, 0);
		efuse_OneByteRead(padapter, efuse_addr, &tmp_header, bPseudoTest);
		phy_set_mac_reg(padapter, EFUSE_TEST, BIT26, 1);


		while (tmp_header == 0xFF || pg_header != tmp_header) {
			if (repeatcnt++ > EFUSE_REPEAT_THRESHOLD_) {
				RTW_INFO("%s, Repeat over limit for pg_header!!\n", __FUNCTION__);
				return _FALSE;
			}

			efuse_OneByteWrite(padapter, efuse_addr, pg_header, bPseudoTest);
			efuse_OneByteRead(padapter, efuse_addr, &tmp_header, bPseudoTest);
		}

		/*to write ext_header*/
		if (tmp_header == pg_header) {
			efuse_addr++;
			pg_header_temp = pg_header;
			pg_header = ((pTargetPkt->offset & 0x78) << 1) | pTargetPkt->word_en;

			efuse_OneByteWrite(padapter, efuse_addr, pg_header, bPseudoTest);
			phy_set_mac_reg(padapter, EFUSE_TEST, BIT26, 0);
			efuse_OneByteRead(padapter, efuse_addr, &tmp_header, bPseudoTest);
			phy_set_mac_reg(padapter, EFUSE_TEST, BIT26, 1);

			while (tmp_header == 0xFF || pg_header != tmp_header) {
				if (repeatcnt++ > EFUSE_REPEAT_THRESHOLD_) {
					RTW_INFO("%s, Repeat over limit for ext_header!!\n", __FUNCTION__);
					return _FALSE;
				}

				efuse_OneByteWrite(padapter, efuse_addr, pg_header, bPseudoTest);
				efuse_OneByteRead(padapter, efuse_addr, &tmp_header, bPseudoTest);
			}

			if ((tmp_header & 0x0F) == 0x0F) {
				if (repeatcnt++ > EFUSE_REPEAT_THRESHOLD_) {
					RTW_INFO("Repeat over limit for word_en!!\n");
					return _FALSE;

				} else {
					if (tmp_header == 0xFF) {
						RTW_INFO("wrong word_en empty!!\n");
						return _FALSE;
					}
					efuse_addr++;
					continue;
				}
			} else if (pg_header != tmp_header) {
				PGPKT_STRUCT	fixPkt;

				RTW_INFO("Error, efuse_PgPacketWrite2ByteHeader(), offset PG fail, need to cover the existed data!!\n");
				RTW_INFO("Error condition for offset PG fail, need to cover the existed data\n");
				fixPkt.offset = ((pg_header_temp & 0xE0) >> 5) | ((tmp_header & 0xF0) >> 1);
				fixPkt.word_en = tmp_header & 0x0F;
				fixPkt.word_cnts = Efuse_CalculateWordCnts(fixPkt.word_en);
				if (!hal_EfuseFixHeaderProcess(padapter, efuseType, &fixPkt, &efuse_addr, bPseudoTest))
					return _FALSE;
			} else
				break;
		} else if ((tmp_header & 0x1F) == 0x0F) {/*wrong extended header*/
			if (tmp_header == 0xFF) {
				RTW_INFO("wrong word_en empty!!\n");
				return _FALSE;
			}
			efuse_addr += 2;
			continue;
		}
	}

	*pAddr = efuse_addr;

	return _TRUE;
}


static u8
hal_EfusePgPacketWrite1ByteHeader(
	PADAPTER		pAdapter,
	u8				efuseType,
	u16				*pAddr,
	PPGPKT_STRUCT	pTargetPkt,
	u8				bPseudoTest)
{
	u8	bRet = _FALSE;
	u8	pg_header = 0, tmp_header = 0;
	u16	efuse_addr = *pAddr;
	u8	repeatcnt = 0;


	pg_header = ((pTargetPkt->offset << 4) & 0xf0) | pTargetPkt->word_en;

	efuse_OneByteWrite(pAdapter, efuse_addr, pg_header, bPseudoTest);
	phy_set_mac_reg(pAdapter, EFUSE_TEST, BIT26, 0);
	efuse_OneByteRead(pAdapter, efuse_addr, &tmp_header, bPseudoTest);
	phy_set_mac_reg(pAdapter, EFUSE_TEST, BIT26, 1);

	while (tmp_header == 0xFF || pg_header != tmp_header) {
		if (repeatcnt++ > EFUSE_REPEAT_THRESHOLD_) {
			RTW_INFO("retry %d times fail!!\n", repeatcnt);
			return FALSE;
		}
		efuse_OneByteWrite(pAdapter, efuse_addr, pg_header, bPseudoTest);
		efuse_OneByteRead(pAdapter, efuse_addr, &tmp_header, bPseudoTest);
		RTW_INFO("===> %s: Keep %d-th retrying, tmp_header = 0x%X\n", __func__, repeatcnt, tmp_header);
	}

	if (tmp_header != pg_header) {
		PGPKT_STRUCT	fixPkt;

		RTW_INFO("Error, %s(), offset PG fail, need to cover the existed data!!\n", __func__);
		RTW_INFO("pg_header(0x%X) != tmp_header(0x%X)\n", pg_header, tmp_header);
		RTW_INFO("Error condition for fixed PG packet, need to cover the existed data: (Addr, Data) = (0x%X, 0x%X)\n",
							efuse_addr, tmp_header);
		fixPkt.offset = (tmp_header>>4) & 0x0F;
		fixPkt.word_en = tmp_header & 0x0F;
		fixPkt.word_cnts = Efuse_CalculateWordCnts(fixPkt.word_en);
		if (!hal_EfuseFixHeaderProcess(pAdapter, efuseType, &fixPkt, &efuse_addr, bPseudoTest))
			return FALSE;
	}

	*pAddr = efuse_addr;

	return _TRUE;
}

BOOLEAN	efuse_PgPacketWriteHeader(
	PADAPTER		pAdapter,
	u8			efuseType,
	u16			*pAddr,
	PPGPKT_STRUCT	pTargetPkt,
	BOOLEAN			bPseudoTest)
{
	BOOLEAN		bRet = _FALSE;

	if (pTargetPkt->offset >= EFUSE_MAX_SECTION_BASE)
		bRet = hal_EfusePgPacketWrite2ByteHeader(pAdapter, efuseType, pAddr, pTargetPkt, bPseudoTest);
	else
		bRet = hal_EfusePgPacketWrite1ByteHeader(pAdapter, efuseType, pAddr, pTargetPkt, bPseudoTest);

	return bRet;
}


static u8
hal_EfusePgPacketWriteData(
	PADAPTER		pAdapter,
	u8				efuseType,
	u16				*pAddr,
	PPGPKT_STRUCT	pTargetPkt,
	u8				bPseudoTest)
{
	u16	efuse_addr;
	u8	badworden;


	efuse_addr = *pAddr;
	badworden = rtl8192E_Efuse_WordEnableDataWrite(pAdapter, efuse_addr + 1, pTargetPkt->word_en, pTargetPkt->data, bPseudoTest);
	if (badworden != 0x0F) {
		RTW_INFO("%s: Fail!!\n", __FUNCTION__);
		return _FALSE;
	}

	/*	RTW_INFO("%s: ok\n", __FUNCTION__); */
	return _TRUE;
}


int
hal_EfusePgPacketWrite_8192E(IN	PADAPTER	pAdapter,
			u8			offset,
			u8			word_en,
			u8			*pData,
			BOOLEAN		bPseudoTest)
{
	u8 efuseType = EFUSE_WIFI;
	PGPKT_STRUCT	targetPkt;
	u16			startAddr = 0;

	RTW_INFO("===> efuse_PgPacketWrite[%s], offset: 0x%X\n", (efuseType == EFUSE_WIFI) ? "WIFI" : "BT", offset);

	/* 4 [1] Check if the remaining space is available to write. */
	if (!efuse_PgPacketCheck(pAdapter, efuseType, bPseudoTest)) {
		pAdapter->LastError = ERR_WRITE_PROTECT;
		RTW_INFO("efuse_PgPacketCheck(), fail!!\n");
		return _FALSE;
	}

	/* 4 [2] Construct a packet to write: (Data, Offset, and WordEnable) */
	efuse_PgPacketConstruct(offset, word_en, pData, &targetPkt);


	/* 4 [3] Fix headers without data or fix bad headers, and then return the address where to get started. */
	if (!efuse_PgPacketPartialWrite(pAdapter, efuseType, &startAddr, &targetPkt, bPseudoTest)) {
		pAdapter->LastError = ERR_INVALID_DATA;
		RTW_INFO("efuse_PgPacketPartialWrite(), fail!!\n");
		return _FALSE;
	}

	/* 4 [4] Write the (extension) header. */
	if (!efuse_PgPacketWriteHeader(pAdapter, efuseType, &startAddr, &targetPkt, bPseudoTest)) {
		pAdapter->LastError = ERR_IO_FAILURE;
		RTW_INFO("efuse_PgPacketWriteHeader(), fail!!\n");
		return _FALSE;
	}

	/* 4 [5] Write the data. */
	if (!hal_EfusePgPacketWriteData(pAdapter, efuseType, &startAddr, &targetPkt, bPseudoTest)) {
		pAdapter->LastError = ERR_IO_FAILURE;
		RTW_INFO("efuse_PgPacketWriteData(), fail!!\n");
		return _FALSE;
	}

	RTW_INFO("<=== efuse_PgPacketWrite\n");
	return _TRUE;
}

#endif


static int
rtl8192E_Efuse_PgPacketWrite(IN	PADAPTER	pAdapter,
			     IN	u8			offset,
			     IN	u8			word_en,
			     IN	u8			*data,
			     IN	BOOLEAN		bPseudoTest)
{
	int	ret;

	ret = hal_EfusePgPacketWrite_8192E(pAdapter, offset, word_en, data, bPseudoTest);

	return ret;
}


u8
GetEEPROMSize8192E(
	IN	PADAPTER	Adapter
)
{
	u8	size = 0;
	u32	curRCR;

	curRCR = rtw_read16(Adapter, REG_SYS_EEPROM_CTRL);
	size = (curRCR & EEPROMSEL) ? 6 : 4; /* 6: EEPROM used is 93C46, 4: boot from E-Fuse. */

	RTW_INFO("EEPROM type is %s\n", size == 4 ? "E-FUSE" : "93C46");
	/* return size; */
	return 4; /* <20120713, Kordan> The default value of HW is 6 ?!! */
}
/* ***********************************************************
 *				Efuse related code
 * *********************************************************** */

static void hw_var_set_monitor(PADAPTER Adapter, u8 variable, u8 *val)
{
	u32	rcr_bits;
	u16	value_rxfltmap2;
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Adapter);
	struct mlme_priv *pmlmepriv = &(Adapter->mlmepriv);

	if (*((u8 *)val) == _HW_STATE_MONITOR_) {

#ifdef CONFIG_CUSTOMER_ALIBABA_GENERAL
		/*	Remove control frame, Dont Append FCS  */
		rcr_bits = RCR_AAP | RCR_APM | RCR_AM | RCR_AB | RCR_APWRMGT | RCR_ADF | RCR_AMF | RCR_APP_PHYST_RXFF;
#else
		/* Receive all type */
		rcr_bits = RCR_AAP | RCR_APM | RCR_AM | RCR_AB | RCR_APWRMGT | RCR_ADF | RCR_ACF | RCR_AMF | RCR_APP_PHYST_RXFF;

		/* Append FCS */
		rcr_bits |= RCR_APPFCS;
#endif
#if 0
		/*
		   CRC and ICV packet will drop in recvbuf2recvframe()
		   We no turn on it.
		 */
		rcr_bits |= (RCR_ACRC32 | RCR_AICV);
#endif

		rtw_hal_get_hwreg(Adapter, HW_VAR_RCR, (u8 *)&pHalData->rcr_backup);
		rtw_hal_set_hwreg(Adapter, HW_VAR_RCR, (u8 *)&rcr_bits);

		/* Receive all data frames */
		value_rxfltmap2 = 0xFFFF;
		rtw_write16(Adapter, REG_RXFLTMAP2, value_rxfltmap2);

#if 0
		/* tx pause */
		rtw_write8(padapter, REG_TXPAUSE, 0xFF);
#endif
	} else {
		/* do nothing */
	}

}

static void hw_var_set_opmode(PADAPTER Adapter, u8 variable, u8 *val)
{
	u8	val8;
	u32 val32;
	u8	mode = *((u8 *)val);
	static u8 isMonitor = _FALSE;

	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Adapter);

	if (isMonitor == _TRUE) {
		/* reset RCR from backup */
		rtw_hal_set_hwreg(Adapter, HW_VAR_RCR, (u8 *)&pHalData->rcr_backup);
		rtw_hal_rcr_set_chk_bssid(Adapter, MLME_ACTION_NONE);
		isMonitor = _FALSE;
	}

	RTW_INFO(ADPT_FMT "- Port-%d  set opmode = %d\n", ADPT_ARG(Adapter),
		 get_hw_port(Adapter), mode);

	if (mode == _HW_STATE_MONITOR_) {
		isMonitor = _TRUE;
		/* set net_type */
		Set_MSR(Adapter, _HW_STATE_NOLINK_);

		hw_var_set_monitor(Adapter, variable, val);
		return;
	}

	rtw_hal_set_hwreg(Adapter, HW_VAR_MAC_ADDR, adapter_mac_addr(Adapter)); /* set mac addr to mac register */

#ifdef CONFIG_CONCURRENT_MODE
	if (Adapter->hw_port == HW_PORT1) {
		/* disable Port1 TSF update */
		rtw_iface_disable_tsf_update(Adapter);

		Set_MSR(Adapter, mode);

		/*RTW_INFO("#### %s() -%d hw_port(%d) mode = %d ####\n", __FUNCTION__, __LINE__, Adapter->hw_port,mode);*/

		if ((mode == _HW_STATE_STATION_) || (mode == _HW_STATE_NOLINK_)) {
			if (!rtw_mi_get_ap_num(Adapter) && !rtw_mi_get_mesh_num(Adapter)) {
#ifdef CONFIG_INTERRUPT_BASED_TXBCN

#ifdef CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
				rtw_write8(Adapter, REG_DRVERLYINT, 0x05);/* restore early int time to 5ms */
#if defined(CONFIG_USB_HCI)
				UpdateInterruptMask8192EU(Adapter, _TRUE, 0, IMR_BCNDMAINT1_8192E);
#elif defined(CONFIG_SDIO_HCI)
				UpdateInterruptMask8192ESdio(Adapter, 0, SDIO_HIMR_BCNERLY_INT_MSK);
#endif
#endif /* CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT */

#ifdef CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
#if defined(CONFIG_USB_HCI)
				UpdateInterruptMask8192EU(Adapter, _TRUE , 0, (IMR_TXBCN0ERR_8192E | IMR_TXBCN0OK_8192E));
#elif defined(CONFIG_SDIO_HCI)
				UpdateInterruptMask8192ESdio(Adapter, 0, (SDIO_HIMR_TXBCNOK_MSK | SDIO_HIMR_TXBCNERR_MSK));
#endif
#endif/* CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR */

#endif /* CONFIG_INTERRUPT_BASED_TXBCN */
				StopTxBeacon(Adapter);
			}

			rtw_write8(Adapter, REG_BCN_CTRL_1, DIS_TSF_UDT | DIS_ATIM); /* disable atim wnd and disable beacon function */
			/* rtw_write8(Adapter,REG_BCN_CTRL_1, DIS_TSF_UDT | EN_BCN_FUNCTION); */
		} else if (mode == _HW_STATE_ADHOC_) {
			ResumeTxBeacon(Adapter);
			rtw_write8(Adapter, REG_BCN_CTRL_1, DIS_TSF_UDT | EN_BCN_FUNCTION | DIS_BCNQ_SUB);
		} else if (mode == _HW_STATE_AP_) {
#ifdef CONFIG_INTERRUPT_BASED_TXBCN
#ifdef CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
#if defined(CONFIG_USB_HCI)
			UpdateInterruptMask8192EU(Adapter, _TRUE , IMR_BCNDMAINT1_8192E, 0);
#elif defined(CONFIG_SDIO_HCI)
			UpdateInterruptMask8192ESdio(Adapter, SDIO_HIMR_BCNERLY_INT_MSK, 0);
#endif
#endif/* CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT */

#ifdef CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
#if defined(CONFIG_USB_HCI)
			UpdateInterruptMask8192EU(Adapter, _TRUE , (IMR_TXBCN0ERR_8192E | IMR_TXBCN0OK_8192E), 0);
#elif defined(CONFIG_SDIO_HCI)
			UpdateInterruptMask8192ESdio(Adapter, (SDIO_HIMR_TXBCNOK_MSK | SDIO_HIMR_TXBCNERR_MSK), 0);
#endif
#endif/* CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR */

#endif /* CONFIG_INTERRUPT_BASED_TXBCN */
			rtw_write8(Adapter, REG_BCN_CTRL_1, DIS_TSF_UDT | DIS_BCNQ_SUB);

			/* enable to rx data frame				 */
			rtw_write16(Adapter, REG_RXFLTMAP2, 0xFFFF);
			/* enable to rx ps-poll */
			rtw_write16(Adapter, REG_RXFLTMAP1, 0x0400);

			/* Beacon Control related register for first time */
			rtw_write8(Adapter, REG_BCNDMATIM, 0x02); /* 2ms		 */

			/* rtw_write8(Adapter, REG_BCN_MAX_ERR, 0xFF); */
			rtw_write8(Adapter, REG_ATIMWND_1, 0x0a); /* 10ms for port1 */

			rtw_write16(Adapter, REG_TSFTR_SYN_OFFSET, 0x7fff);/* +32767 (~32ms) */

			/* reset TSF2	 */
			rtw_write8(Adapter, REG_DUAL_TSF_RST, BIT(1));

			/* enable BCN1 Function for if2 */
			/* don't enable update TSF1 for if2 (due to TSF update when beacon/probe rsp are received) */
			rtw_write8(Adapter, REG_BCN_CTRL_1, (DIS_TSF_UDT | EN_BCN_FUNCTION | EN_TXBCN_RPT | DIS_RX_BSSID_FIT));

			/* SW_BCN_SEL - Port1 */
			/* rtw_write8(Adapter, REG_DWBCN1_CTRL_8192E+2, rtw_read8(Adapter, REG_DWBCN1_CTRL_8192E+2)|BIT4); */
			rtw_hal_set_hwreg(Adapter, HW_VAR_DL_BCN_SEL, NULL);

			/* select BCN on port 1 */
			rtw_write8(Adapter, REG_CCK_CHECK_8192E,
				(rtw_read8(Adapter, REG_CCK_CHECK_8192E) | BIT_BCN_PORT_SEL));

			if (!rtw_mi_buddy_check_mlmeinfo_state(Adapter, WIFI_FW_ASSOC_SUCCESS))
				rtw_write8(Adapter, REG_BCN_CTRL,
					rtw_read8(Adapter, REG_BCN_CTRL) & ~EN_BCN_FUNCTION);

			/* dis BCN0 ATIM  WND if if1 is station */
			/* rtw_write8(Adapter, REG_BCN_CTRL, rtw_read8(Adapter, REG_BCN_CTRL)|DIS_ATIM); */

#ifdef CONFIG_TSF_RESET_OFFLOAD
			/* Reset TSF for STA+AP concurrent mode */
			if (DEV_STA_LD_NUM(adapter_to_dvobj(Adapter))) {
				if (rtw_hal_reset_tsf(Adapter, HW_PORT1) == _FAIL)
					RTW_INFO("ERROR! %s()-%d: Reset port1 TSF fail\n",
						 __FUNCTION__, __LINE__);
			}
#endif /* CONFIG_TSF_RESET_OFFLOAD	 */

		}
	} else
#endif /* CONFIG_CONCURRENT_MODE */
	{
#ifdef CONFIG_MI_WITH_MBSSID_CAM /*For Port0 - MBSS CAM*/
		hw_var_set_opmode_mbid(Adapter, mode);
#else
		/* disable Port0 TSF update */
		rtw_iface_disable_tsf_update(Adapter);

		Set_MSR(Adapter, mode);

		/*RTW_INFO("#### %s() -%d hw_port(0) mode = %d ####\n", __FUNCTION__, __LINE__, mode);*/

		if ((mode == _HW_STATE_STATION_) || (mode == _HW_STATE_NOLINK_)) {
#ifdef CONFIG_CONCURRENT_MODE
			if (!rtw_mi_get_ap_num(Adapter) && !rtw_mi_get_mesh_num(Adapter))
#endif /*CONFIG_CONCURRENT_MODE*/
			{
#ifdef CONFIG_INTERRUPT_BASED_TXBCN
#ifdef CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
				rtw_write8(Adapter, REG_DRVERLYINT, 0x05);/* restore early int time to 5ms					 */
#if defined(CONFIG_USB_HCI)
				UpdateInterruptMask8192EU(Adapter, _TRUE, 0, IMR_BCNDMAINT0_8192E);
#elif defined(CONFIG_SDIO_HCI)
				UpdateInterruptMask8192ESdio(Adapter, 0, SDIO_HIMR_BCNERLY_INT_MSK);
#endif
#endif/* CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT */

#ifdef CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
#if defined(CONFIG_USB_HCI)
				UpdateInterruptMask8192EU(Adapter, _TRUE , 0, (IMR_TXBCN0ERR_8192E | IMR_TXBCN0OK_8192E));
#elif defined(CONFIG_SDIO_HCI)
				UpdateInterruptMask8192ESdio(Adapter, 0, (SDIO_HIMR_TXBCNOK_MSK | SDIO_HIMR_TXBCNERR_MSK));
#endif
#endif /* CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR */

#endif /* CONFIG_INTERRUPT_BASED_TXBCN		 */
				StopTxBeacon(Adapter);
			}
			rtw_write8(Adapter, REG_BCN_CTRL, DIS_TSF_UDT | EN_BCN_FUNCTION | DIS_ATIM); /* disable atim wnd */
			/* rtw_write8(Adapter,REG_BCN_CTRL, DIS_TSF_UDT | EN_BCN_FUNCTION); */
		} else if (mode == _HW_STATE_ADHOC_) {
			/*Beacon is polled to TXBUF*/
			rtw_write16(Adapter, REG_CR, rtw_read16(Adapter, REG_CR) | BIT(8));

			ResumeTxBeacon(Adapter);
			rtw_write8(Adapter, REG_BCN_CTRL, DIS_TSF_UDT | EN_BCN_FUNCTION | DIS_BCNQ_SUB);
		} else if (mode == _HW_STATE_AP_) {

#ifdef CONFIG_INTERRUPT_BASED_TXBCN
#ifdef CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT
#if defined(CONFIG_USB_HCI)
			UpdateInterruptMask8192EU(Adapter, _TRUE , IMR_BCNDMAINT0_8192E, 0);
#elif defined(CONFIG_SDIO_HCI)
			UpdateInterruptMask8192ESdio(Adapter, SDIO_HIMR_BCNERLY_INT_MSK, 0);
#endif
#endif/* CONFIG_INTERRUPT_BASED_TXBCN_EARLY_INT */

#ifdef CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR
#if defined(CONFIG_USB_HCI)
			UpdateInterruptMask8192EU(Adapter, _TRUE , (IMR_TXBCN0ERR_8192E | IMR_TXBCN0OK_8192E), 0);
#elif defined(CONFIG_SDIO_HCI)
			UpdateInterruptMask8192ESdio(Adapter, (SDIO_HIMR_TXBCNOK_MSK | SDIO_HIMR_TXBCNERR_MSK), 0);
#endif
#endif/* CONFIG_INTERRUPT_BASED_TXBCN_BCN_OK_ERR */

#endif /* CONFIG_INTERRUPT_BASED_TXBCN */

			rtw_write8(Adapter, REG_BCN_CTRL, DIS_TSF_UDT | DIS_BCNQ_SUB);

			/*Beacon is polled to TXBUF*/
			rtw_write16(Adapter, REG_CR, rtw_read16(Adapter, REG_CR) | BIT(8));

			/* enable to rx data frame */
			rtw_write16(Adapter, REG_RXFLTMAP2, 0xFFFF);
			/* enable to rx ps-poll */
			rtw_write16(Adapter, REG_RXFLTMAP1, 0x0400);

			/* Beacon Control related register for first time */
			rtw_write8(Adapter, REG_BCNDMATIM, 0x02); /* 2ms			 */

			/* rtw_write8(Adapter, REG_BCN_MAX_ERR, 0xFF); */
			rtw_write8(Adapter, REG_ATIMWND, 0x0c); /* 12ms */

			rtw_write16(Adapter, REG_TSFTR_SYN_OFFSET, 0x7fff);/* +32767 (~32ms) */

			/* reset TSF */
			rtw_write8(Adapter, REG_DUAL_TSF_RST, BIT(0));

			/* enable BCN0 Function for if1 */
			/* don't enable update TSF0 for if1 (due to TSF update when beacon/probe rsp are received) */
			rtw_write8(Adapter, REG_BCN_CTRL, (DIS_TSF_UDT | EN_BCN_FUNCTION | EN_TXBCN_RPT | DIS_RX_BSSID_FIT));

			/* SW_BCN_SEL - Port0 */
			/* rtw_write8(Adapter, REG_DWBCN1_CTRL_8192E+2, rtw_read8(Adapter, REG_DWBCN1_CTRL_8192E+2) & ~BIT4); */
			rtw_hal_set_hwreg(Adapter, HW_VAR_DL_BCN_SEL, NULL);

			/* select BCN on port 0 */
			rtw_write8(Adapter, REG_CCK_CHECK_8192E,
				(rtw_read8(Adapter, REG_CCK_CHECK_8192E) & ~BIT_BCN_PORT_SEL));


#ifdef CONFIG_CONCURRENT_MODE
			if (!rtw_mi_buddy_check_mlmeinfo_state(Adapter, WIFI_FW_ASSOC_SUCCESS))
				rtw_write8(Adapter, REG_BCN_CTRL_1,
					rtw_read8(Adapter, REG_BCN_CTRL_1) & ~EN_BCN_FUNCTION);
#endif

			/* dis BCN1 ATIM  WND if if2 is station */
			/* rtw_write8(Adapter, REG_BCN_CTRL_1, rtw_read8(Adapter, REG_BCN_CTRL_1)|DIS_ATIM);	 */
#ifdef CONFIG_TSF_RESET_OFFLOAD
			/* Reset TSF for STA+AP concurrent mode */
			if (DEV_STA_LD_NUM(adapter_to_dvobj(Adapter))) {
				if (rtw_hal_reset_tsf(Adapter, HW_PORT0) == _FAIL)
					RTW_INFO("ERROR! %s()-%d: Reset port0 TSF fail\n", __FUNCTION__, __LINE__);
			}
#endif /* CONFIG_TSF_RESET_OFFLOAD */

		}
#endif
	}
}

/***********************************************************/
/* RTL8192E-MAC Setting */
VOID
_InitTxBufferBoundary_8192E(
	IN PADAPTER Adapter,
	IN u8 txpktbuf_bndy
)
{
	struct registry_priv *pregistrypriv = &Adapter->registrypriv;

	rtw_write8(Adapter, REG_BCNQ_BDNY, txpktbuf_bndy);
	rtw_write8(Adapter, REG_MGQ_BDNY, txpktbuf_bndy);
	rtw_write8(Adapter, REG_WMAC_LBK_BF_HD, txpktbuf_bndy);
	rtw_write8(Adapter, REG_TRXFF_BNDY, txpktbuf_bndy);
	rtw_write8(Adapter, REG_DWBCN0_CTRL_8192E+1, txpktbuf_bndy);/* BCN_HEAD */

#ifdef CONFIG_CONCURRENT_MODE
	rtw_write8(Adapter, REG_BCNQ1_BDNY, txpktbuf_bndy + 8);
	rtw_write8(Adapter, REG_DWBCN1_CTRL_8192E+1, txpktbuf_bndy + 8); /* BCN1_HEAD */
	/* BIT1- BIT_SW_BCN_SEL_EN */
	rtw_write8(Adapter, REG_DWBCN1_CTRL_8192E+2, rtw_read8(Adapter, REG_DWBCN1_CTRL_8192E+2) | BIT1);
#endif
}

VOID
_InitQueueReservedPage_8192E(
	IN  PADAPTER Adapter
)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	struct registry_priv	*pregistrypriv = &Adapter->registrypriv;
	u32			numHQ		= 0;
	u32			numLQ		= 0;
	u32			numNQ		= 0;
	u32			numEQ		= 0;
	u32			numPubQ	= 0;
	u32			value32;


	if (!pregistrypriv->wifi_spec) { /* Typical setting */

		if (pHalData->OutEpQueueSel & TX_SELE_HQ)
			numHQ = NORMAL_PAGE_NUM_HPQ_8192E;

		if (pHalData->OutEpQueueSel & TX_SELE_LQ)
			numLQ = NORMAL_PAGE_NUM_LPQ_8192E;

		/* NOTE: This step shall be proceed before writting REG_RQPN.		 */
		if (pHalData->OutEpQueueSel & TX_SELE_NQ)
			numNQ = NORMAL_PAGE_NUM_NPQ_8192E;

		if (pHalData->OutEpQueueSel & TX_SELE_EQ)
			numEQ = NORMAL_PAGE_NUM_EPQ_8192E;
	} else {
		/* WMM */

		if (pHalData->OutEpQueueSel & TX_SELE_HQ)
			numHQ = WMM_NORMAL_PAGE_NUM_HPQ_8192E;

		if (pHalData->OutEpQueueSel & TX_SELE_LQ)
			numLQ = WMM_NORMAL_PAGE_NUM_LPQ_8192E;

		/* NOTE: This step shall be proceed before writting REG_RQPN.		 */
		if (pHalData->OutEpQueueSel & TX_SELE_NQ)
			numNQ = WMM_NORMAL_PAGE_NUM_NPQ_8192E;

		if (pHalData->OutEpQueueSel & TX_SELE_EQ)
			numEQ = NORMAL_PAGE_NUM_EPQ_8192E;
	}

	/*
		Reserve 1 page at the boundary for safety
	*/
	numPubQ = TX_TOTAL_PAGE_NUMBER_8192E - numHQ - numLQ - numNQ - numEQ - 1;

	value32 = _NPQ(numNQ) | _EPQ(numEQ);
	rtw_write32(Adapter, REG_RQPN_NPQ, value32);

	/* TX DMA */
	value32 = _HPQ(numHQ) | _LPQ(numLQ) | _PUBQ(numPubQ) | LD_RQPN;
	rtw_write32(Adapter, REG_RQPN, value32);

#ifdef CONFIG_SDIO_HCI
	rtw_hal_set_sdio_tx_max_length(Adapter, numHQ, numNQ, numLQ, numPubQ, SDIO_TX_DIV_NUM);

#ifdef CONFIG_SDIO_TX_ENABLE_AVAL_INT
	_init_available_page_threshold(Adapter, numHQ, numNQ, numLQ, numPubQ);
#endif

#endif

}


static VOID
_InitNormalChipRegPriority_8192E(
	IN	PADAPTER	Adapter,
	IN	u16		beQ,
	IN	u16		bkQ,
	IN	u16		viQ,
	IN	u16		voQ,
	IN	u16		mgtQ,
	IN	u16		hiQ
)
{
	u16 value16	= (rtw_read16(Adapter, REG_TRXDMA_CTRL) & 0x7);

	value16 |=	_TXDMA_BEQ_MAP(beQ)	| _TXDMA_BKQ_MAP(bkQ) |
			_TXDMA_VIQ_MAP(viQ)	| _TXDMA_VOQ_MAP(voQ) |
			_TXDMA_MGQ_MAP(mgtQ) | _TXDMA_HIQ_MAP(hiQ);

	rtw_write16(Adapter, REG_TRXDMA_CTRL, value16);
}

static VOID
_InitNormalChipTwoOutEpPriority_8192E(
	IN	PADAPTER Adapter
)
{
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(Adapter);
	struct registry_priv *pregistrypriv = &Adapter->registrypriv;
	u16			beQ, bkQ, viQ, voQ, mgtQ, hiQ;


	u16	valueHi = 0;
	u16	valueLow = 0;

	switch (pHalData->OutEpQueueSel) {
	case (TX_SELE_HQ | TX_SELE_LQ):
		valueHi = QUEUE_HIGH;
		valueLow = QUEUE_LOW;
		break;
	case (TX_SELE_NQ | TX_SELE_LQ):
		valueHi = QUEUE_NORMAL;
		valueLow = QUEUE_LOW;
		break;
	case (TX_SELE_HQ | TX_SELE_NQ):
		valueHi = QUEUE_HIGH;
		valueLow = QUEUE_NORMAL;
		break;
	default:
		valueHi = QUEUE_HIGH;
		valueLow = QUEUE_NORMAL;
		break;
	}

	if (!pregistrypriv->wifi_spec) {
		beQ		= valueLow;
		bkQ		= valueLow;
		viQ		= valueHi;
		voQ		= valueHi;
		mgtQ	= valueHi;
		hiQ		= valueHi;
	} else { /* for WMM ,CONFIG_OUT_EP_WIFI_MODE */
		beQ		= valueLow;
		bkQ		= valueHi;
		viQ		= valueHi;
		voQ		= valueLow;
		mgtQ	= valueHi;
		hiQ		= valueHi;
	}

	_InitNormalChipRegPriority_8192E(Adapter, beQ, bkQ, viQ, voQ, mgtQ, hiQ);

}

static VOID
_InitNormalChipThreeOutEpPriority_8192E(
	IN	PADAPTER Adapter
)
{
	struct registry_priv *pregistrypriv = &Adapter->registrypriv;
	u16			beQ, bkQ, viQ, voQ, mgtQ, hiQ;

	if (!pregistrypriv->wifi_spec) { /* typical setting */
		beQ		= QUEUE_LOW;
		bkQ		= QUEUE_LOW;
		viQ		= QUEUE_NORMAL;
		voQ		= QUEUE_HIGH;
		mgtQ	= QUEUE_HIGH;
		hiQ		= QUEUE_HIGH;
	} else { /* for WMM */
		beQ		= QUEUE_LOW;
		bkQ		= QUEUE_NORMAL;
		viQ		= QUEUE_NORMAL;
		voQ		= QUEUE_HIGH;
		mgtQ	= QUEUE_HIGH;
		hiQ		= QUEUE_HIGH;
	}
	_InitNormalChipRegPriority_8192E(Adapter, beQ, bkQ, viQ, voQ, mgtQ, hiQ);
}

static VOID
_InitNormalChipFourOutEpPriority_8192E(
	IN	PADAPTER Adapter
)
{
	struct registry_priv *pregistrypriv = &Adapter->registrypriv;
	u16			beQ, bkQ, viQ, voQ, mgtQ, hiQ;

	if (!pregistrypriv->wifi_spec) { /* typical setting */
		beQ		= QUEUE_LOW;
		bkQ		= QUEUE_LOW;
		viQ		= QUEUE_NORMAL;
		voQ		= QUEUE_HIGH;
		mgtQ	= QUEUE_HIGH;
		hiQ		= QUEUE_EXTRA;
	} else { /* for WMM */
		beQ		= QUEUE_LOW;
		bkQ		= QUEUE_NORMAL;
		viQ		= QUEUE_NORMAL;
		voQ		= QUEUE_HIGH;
		mgtQ	= QUEUE_HIGH;
		hiQ		= QUEUE_EXTRA;
	}
	_InitNormalChipRegPriority_8192E(Adapter, beQ, bkQ, viQ, voQ, mgtQ, hiQ);
}

VOID
_InitQueuePriority_8192E(
	IN	PADAPTER Adapter
)
{
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(Adapter);

	switch (pHalData->OutEpNumber) {
	case 2:
		_InitNormalChipTwoOutEpPriority_8192E(Adapter);
		break;
	case 3:
		_InitNormalChipThreeOutEpPriority_8192E(Adapter);
		break;
	case 4:
		/* TBD - for AP mode ,extra-Q */
		_InitNormalChipFourOutEpPriority_8192E(Adapter);
		break;
	default:
		RTW_INFO("_InitQueuePriority_8192E(): Shall not reach here!\n");
		break;
	}
}



VOID
_InitPageBoundary_8192E(
	IN  PADAPTER Adapter
)
{
	u16 rxff_bndy = 0;

	rxff_bndy = MAX_RX_DMA_BUFFER_SIZE_8192E - 1;

	rtw_write16(Adapter, (REG_TRXFF_BNDY + 2), rxff_bndy);
}


VOID
_InitDriverInfoSize_8192E(
	IN  PADAPTER	Adapter,
	IN	u8		drvInfoSize
)
{
	rtw_write8(Adapter, REG_RX_DRVINFO_SZ, drvInfoSize);
}

VOID _InitRDGSetting_8192E(PADAPTER Adapter)
{
	rtw_write8(Adapter, REG_RD_CTRL, 0xFF);
	rtw_write16(Adapter, REG_RD_NAV_NXT, 0x200);
	rtw_write8(Adapter, REG_RD_RESP_PKT_TH, 0x05);
}

VOID
_InitNetworkType_8192E(
	IN  PADAPTER Adapter
)
{
	u32	value32;

	value32 = rtw_read32(Adapter, REG_CR);
	/* TODO: use the other function to set network type */
	value32 = (value32 & ~MASK_NETTYPE) | _NETTYPE(NT_LINK_AP);

	rtw_write32(Adapter, REG_CR, value32);
}
VOID
_InitWMACSetting_8192E(
	IN  PADAPTER Adapter
)
{
	/* u4Byte			value32; */
	/* u16			value16; */
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u32 rcr;

	/* rcr = AAP | APM | AM | AB | APP_ICV | ADF | AMF | APP_FCS | HTC_LOC_CTRL | APP_MIC | APP_PHYSTS; */
	/* rcr = */
	/* RCR_AAP | RCR_APM | RCR_AM | RCR_AB |RCR_CBSSID_DATA| RCR_CBSSID_BCN| RCR_APP_ICV | RCR_AMF | RCR_HTC_LOC_CTRL | RCR_APP_MIC | RCR_APP_PHYST_RXFF;	  */
	/* don't turn on AAP, it will allow all packets to driver */
	rcr = RCR_APM | RCR_AM | RCR_AB | RCR_CBSSID_DATA | RCR_CBSSID_BCN | RCR_APP_ICV | RCR_AMF | RCR_HTC_LOC_CTRL | RCR_APP_MIC | RCR_APP_PHYST_RXFF;
#if (1 == RTL8192E_RX_PACKET_INCLUDE_CRC)
	rcr |= ACRC32;
#endif
	rtw_hal_set_hwreg(Adapter, HW_VAR_RCR, (u8 *)&rcr);

	/* Accept all multicast address */
	rtw_write32(Adapter, REG_MAR, 0xFFFFFFFF);
	rtw_write32(Adapter, REG_MAR + 4, 0xFFFFFFFF);


	/* Accept all data frames */
	/* value16 = 0xFFFF; */
	/* rtw_write16(Adapter, REG_RXFLTMAP2, value16); */

	/* 2010.09.08 hpfan */
	/* Since ADF is removed from RCR, ps-poll will not be indicate to driver, */
	/* RxFilterMap should mask ps-poll to gurantee AP mode can rx ps-poll. */
	/* value16 = 0x400; */
	/* rtw_write16(Adapter, REG_RXFLTMAP1, value16); */

	/* Accept all management frames */
	/* value16 = 0xFFFF; */
	/* rtw_write16(Adapter, REG_RXFLTMAP0, value16); */

	/* enable RX_SHIFT bits */
	/* rtw_write8(Adapter, REG_TRXDMA_CTRL, rtw_read8(Adapter, REG_TRXDMA_CTRL)|BIT(1));	 */

}

VOID _InitAdaptiveCtrl_8192E(IN  PADAPTER Adapter)
{
	u16	value16;
	u32	value32;

	/* Response Rate Set */
	value32 = rtw_read32(Adapter, REG_RRSR);
	value32 &= ~RATE_BITMAP_ALL;
	value32 |= RATE_RRSR_CCK_ONLY_1M;
	rtw_write32(Adapter, REG_RRSR, value32);

	/* CF-END Threshold */
	/* m_spIoBase->rtw_write8(REG_CFEND_TH, 0x1); */

	/* SIFS (used in NAV) */
	value16 = _SPEC_SIFS_CCK(0x10) | _SPEC_SIFS_OFDM(0x10);
	rtw_write16(Adapter, REG_SPEC_SIFS, value16);

	/* Retry Limit */
	value16 = BIT_LRL(RL_VAL_STA) | BIT_SRL(RL_VAL_STA);
	rtw_write16(Adapter, REG_RETRY_LIMIT, value16);
}

VOID _InitEDCA_8192E(IN  PADAPTER Adapter)
{
	/* Set Spec SIFS (used in NAV) */
	rtw_write16(Adapter, REG_SPEC_SIFS, 0x100a);
	rtw_write16(Adapter, REG_MAC_SPEC_SIFS, 0x100a);

	/* Set SIFS for CCK */
	rtw_write16(Adapter, REG_SIFS_CTX, 0x100a);

	/* Set SIFS for OFDM */
	rtw_write16(Adapter, REG_SIFS_TRX, 0x100a);

	/* TXOP */
	rtw_write32(Adapter, REG_EDCA_BE_PARAM, 0x005EA42B);
	rtw_write32(Adapter, REG_EDCA_BK_PARAM, 0x0000A44F);
	rtw_write32(Adapter, REG_EDCA_VI_PARAM, 0x005EA324);
	rtw_write32(Adapter, REG_EDCA_VO_PARAM, 0x002FA226);
}
VOID _InitRetryFunction_8192E(IN  PADAPTER Adapter)
{
	u8	value8;

	value8 = rtw_read8(Adapter, REG_FWHW_TXQ_CTRL);
	value8 |= EN_AMPDU_RTY_NEW;
	rtw_write8(Adapter, REG_FWHW_TXQ_CTRL, value8);

	/* Set ACK timeout */
	rtw_write8(Adapter, REG_ACKTO, 0x40);  /* masked by page for BCM IOT issue temporally */
	/* rtw_write8(Adapter, REG_ACKTO, 0x80); */
}

VOID
_BeaconFunctionEnable(
	IN	PADAPTER		Adapter,
	IN	BOOLEAN			Enable,
	IN	BOOLEAN			Linked
)
{
	rtw_write8(Adapter, REG_BCN_CTRL, (DIS_TSF_UDT | EN_BCN_FUNCTION));

	rtw_write8(Adapter, REG_RD_CTRL + 1, 0x6F);
}

VOID _InitBeaconParameters_8192E(IN  PADAPTER Adapter)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u16 val16;
	u8  val8;

	val8 = DIS_TSF_UDT;
	val16 = val8 | (val8 << 8); /* port0 and port1 */
#ifdef CONFIG_BT_COEXIST
	if (pHalData->EEPROMBluetoothCoexist == 1) {
		/* Enable prot0 beacon function for PSTDMA */
		val16 |= EN_BCN_FUNCTION;
	}
#endif
	rtw_write16(Adapter, REG_BCN_CTRL, val16);

	/* TBTT setup time */
	rtw_write8(Adapter, REG_TBTT_PROHIBIT, TBTT_PROHIBIT_SETUP_TIME);

	/* TBTT hold time: 0x540[19:8] */
	rtw_write8(Adapter, REG_TBTT_PROHIBIT + 1, TBTT_PROHIBIT_HOLD_TIME_STOP_BCN & 0xFF);
	rtw_write8(Adapter, REG_TBTT_PROHIBIT + 2,
		(rtw_read8(Adapter, REG_TBTT_PROHIBIT + 2) & 0xF0) | (TBTT_PROHIBIT_HOLD_TIME_STOP_BCN >> 8));

	rtw_write8(Adapter, REG_DRVERLYINT, DRIVER_EARLY_INT_TIME_8192E);/* 5ms */
	rtw_write8(Adapter, REG_BCNDMATIM, BCN_DMA_ATIME_INT_TIME_8192E); /* 2ms */

	/* Suggested by designer timchen. Change beacon AIFS to the largest number */
	/* beacause test chip does not contension before sending beacon. by tynli. 2009.11.03 */
	rtw_write16(Adapter, REG_BCNTCFG, 0x4413);

}

void SetBeaconRelatedRegisters8192E(PADAPTER padapter)
{
	u32	value32;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct mlme_ext_priv	*pmlmeext = &(padapter->mlmeextpriv);
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	u32 bcn_ctrl_reg			= REG_BCN_CTRL;
	/* reset TSF, enable update TSF, correcting TSF On Beacon */

	/* REG_MBSSID_BCN_SPACE */
	/* REG_BCNDMATIM */
	/* REG_ATIMWND */
	/* REG_TBTT_PROHIBIT */
	/* REG_DRVERLYINT */
	/* REG_BCN_MAX_ERR	 */
	/* REG_BCNTCFG */ /* (0x510) */
	/* REG_DUAL_TSF_RST */
	/* REG_BCN_CTRL  */ /* (0x550) */

#ifdef CONFIG_CONCURRENT_MODE
	if (padapter->hw_port == HW_PORT1)
		bcn_ctrl_reg = REG_BCN_CTRL_1;
#endif

	/* BCN interval */
	rtw_hal_set_hwreg(padapter, HW_VAR_BEACON_INTERVAL, (u8 *)&pmlmeinfo->bcn_interval);

	rtw_write8(padapter, REG_ATIMWND, 0x02);/* 2ms */

	_InitBeaconParameters_8192E(padapter);

	rtw_write8(padapter, REG_SLOT, 0x09);

	value32 = rtw_read32(padapter, REG_TCR);
	value32 &= ~TSFRST;
	rtw_write32(padapter,  REG_TCR, value32);

	value32 |= TSFRST;
	rtw_write32(padapter, REG_TCR, value32);

	/* NOTE: Fix test chip's bug (about contention windows's randomness) */
	rtw_write8(padapter,  REG_RXTSF_OFFSET_CCK, 0x50);
	rtw_write8(padapter, REG_RXTSF_OFFSET_OFDM, 0x50);

	_BeaconFunctionEnable(padapter, _TRUE, _TRUE);

	ResumeTxBeacon(padapter);

	/* rtw_write8(padapter, 0x422, rtw_read8(padapter, 0x422)|BIT(6)); */

	/* rtw_write8(padapter, 0x541, 0xff); */

	/* rtw_write8(padapter, 0x542, rtw_read8(padapter, 0x541)|BIT(0)); */

	rtw_write8(padapter, bcn_ctrl_reg, rtw_read8(padapter, bcn_ctrl_reg) | DIS_BCNQ_SUB);

}

VOID _InitBeaconMaxError_8192E(
	IN  PADAPTER	Adapter,
	IN	BOOLEAN		InfraMode
)
{
#ifdef CONFIG_ADHOC_WORKAROUND_SETTING
	rtw_write8(Adapter, REG_BCN_MAX_ERR, 0xFF);
#else
	/* rtw_write8(Adapter, REG_BCN_MAX_ERR, (InfraMode ? 0xFF : 0x10));	 */
#endif
}
/* Set CCK and OFDM Block "ON" */
void _BBTurnOnBlock_8192E(PADAPTER padapter)
{
#if (DISABLE_BB_RF)
	return;
#endif

	phy_set_bb_reg(padapter, rFPGA0_RFMOD, bCCKEn, 0x1);
	phy_set_bb_reg(padapter, rFPGA0_RFMOD, bOFDMEn, 0x1);
}
VOID
hal_ReadRFType_8192E(
	IN	PADAPTER	Adapter
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

#if DISABLE_BB_RF
	pHalData->rf_chip = RF_PSEUDO_11N;
#else
	pHalData->rf_chip = RF_6052;
#endif

	pHalData->BandSet = BAND_ON_2_4G;

}

u8 Hal_CrystalAFEAdjust(_adapter *Adapter)
{
	u8 val8;
	u32 val32;
	/* 40Mhz crystal source,MAC 0x28[2]=0 */
	val8 = rtw_read8(Adapter, REG_AFE_CTRL2_8192E);
	val8 &= 0xfb;
	rtw_write8(Adapter, REG_AFE_CTRL2_8192E, val8);

	val32 = rtw_read32(Adapter, REG_AFE_CTRL4_8192E);
	val32 &= 0xfffffc7f;
	rtw_write32(Adapter, REG_AFE_CTRL4_8192E, val32);

	/* 92E AFE parameter */
	/* AFE PLL KVCO selection, MAC 0x28[6]=1 */
	val8 = rtw_read8(Adapter, REG_AFE_CTRL2_8192E);
	val8 &= 0xBF;
	rtw_write8(Adapter, REG_AFE_CTRL2_8192E, val8);

	/* AFE PLL KVCO selection, MAC 0x78[21]=0 */
	val32 = rtw_read32(Adapter, REG_AFE_CTRL4_8192E);
	val32 &= 0xffdfffff;
	rtw_write32(Adapter, REG_AFE_CTRL4_8192E, val32);

	return _SUCCESS;
}


/* RTL8192E-MAC Setting
 ***********************************************************/

u8 SetHwReg8192E(PADAPTER Adapter, u8 variable, u8 *val)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct dm_struct		*podmpriv = &pHalData->odmpriv;
	u8 ret = _SUCCESS;

	switch (variable) {
	case HW_VAR_SET_OPMODE:
		hw_var_set_opmode(Adapter, variable, val);
		break;
	case HW_VAR_BASIC_RATE: {
		struct mlme_ext_info *mlmext_info = &Adapter->mlmeextpriv.mlmext_info;
		u16 input_b = 0, masked = 0, ioted = 0, BrateCfg = 0;
		u16 rrsr_2g_force_mask = RRSR_CCK_RATES;
		u16 rrsr_2g_allow_mask = (RRSR_24M | RRSR_12M | RRSR_6M | RRSR_CCK_RATES);

		HalSetBrateCfg(Adapter, val, &BrateCfg);
		input_b = BrateCfg;

		/* apply force and allow mask */
		BrateCfg |= rrsr_2g_force_mask;
		BrateCfg &= rrsr_2g_allow_mask;
		masked = BrateCfg;

		/* IOT consideration */
		if (mlmext_info->assoc_AP_vendor == HT_IOT_PEER_CISCO) {
			/* if peer is cisco and didn't use ofdm rate, we enable 6M ack */
			if ((BrateCfg & (RRSR_24M | RRSR_12M | RRSR_6M)) == 0)
				BrateCfg |= RRSR_6M;
		}
		ioted = BrateCfg;

		pHalData->BasicRateSet = BrateCfg;

		RTW_INFO("HW_VAR_BASIC_RATE: %#x->%#x->%#x\n", input_b, masked, ioted);

		/* Set RRSR rate table. */
		rtw_write16(Adapter, REG_RRSR, BrateCfg);
		rtw_write8(Adapter, REG_RRSR + 2, rtw_read8(Adapter, REG_RRSR + 2) & 0xf0);
	}
	break;
	case HW_VAR_TXPAUSE:
		rtw_write8(Adapter, REG_TXPAUSE, *((u8 *)val));
		break;

	case HW_VAR_SLOT_TIME: {
		rtw_write8(Adapter, REG_SLOT, val[0]);
	}
	break;
	case HW_VAR_RESP_SIFS: {
		struct mlme_ext_priv	*pmlmeext = &Adapter->mlmeextpriv;

		if ((pmlmeext->cur_wireless_mode == WIRELESS_11G) ||
		    (pmlmeext->cur_wireless_mode == WIRELESS_11BG)) { /* WIRELESS_MODE_G){ */
			val[0] = 0x0a;
			val[1] = 0x0a;
		} else {
			val[0] = 0x0e;
			val[1] = 0x0e;
		}
		/* SIFS for OFDM Data ACK */
		PlatformEFIOWrite1Byte(Adapter, REG_SIFS_CTX_8192E+1, val[0]);
		/* SIFS for OFDM consecutive tx like CTS data! */
		PlatformEFIOWrite1Byte(Adapter, REG_SIFS_TRX_8192E+1, val[1]);

		PlatformEFIOWrite1Byte(Adapter, REG_SPEC_SIFS_8192E+1, val[0]);
		PlatformEFIOWrite1Byte(Adapter, REG_MAC_SPEC_SIFS_8192E+1, val[0]);

		/* Revise SIFS setting due to Hardware register definition change. */
		PlatformEFIOWrite1Byte(Adapter, REG_RESP_SIFS_OFDM_8192E+1, val[0]);
		PlatformEFIOWrite1Byte(Adapter, REG_RESP_SIFS_OFDM_8192E, val[0]);

	}
#if 0
	{
		/* SIFS_Timer = 0x0a0a0808; */
		/* RESP_SIFS for CCK */

		rtw_write8(Adapter, REG_RESP_SIFS_CCK, val[0]);	/*  SIFS_T2T_CCK (0x08) */
		rtw_write8(Adapter, REG_RESP_SIFS_CCK + 1, val[1]);	/*  SIFS_R2T_CCK(0x08) */
		/* RESP_SIFS for OFDM */
		rtw_write8(Adapter, REG_RESP_SIFS_OFDM, val[2]); /* SIFS_T2T_OFDM (0x0a) */
		rtw_write8(Adapter, REG_RESP_SIFS_OFDM + 1, val[3]); /* SIFS_R2T_OFDM(0x0a) */

	}
#endif
	break;
	case HW_VAR_ACK_PREAMBLE: {
		u8	regTmp;
		u8	bShortPreamble = *((PBOOLEAN)val);
		/* Joseph marked out for Netgear 3500 TKIP channel 7 issue.(Temporarily) */
		regTmp = (pHalData->nCur40MhzPrimeSC) << 5;
		rtw_write8(Adapter, REG_RRSR + 2, regTmp);

		regTmp = BIT(4) | BIT(5);
		if (bShortPreamble)
			regTmp |= BIT1;
		else
			regTmp &= (~BIT1);
		rtw_write8(Adapter, REG_TRXPTCL_CTL_8192E+2, regTmp);
	}
	break;
	case HW_VAR_CAM_EMPTY_ENTRY: {
		u8	ucIndex = *((u8 *)val);
		u8	i;
		u32	ulCommand = 0;
		u32	ulContent = 0;
		u32	ulEncAlgo = CAM_AES;

		for (i = 0; i < CAM_CONTENT_COUNT; i++) {
			/* filled id in CAM config 2 byte */
			if (i == 0) {
				ulContent |= (ucIndex & 0x03) | ((u16)(ulEncAlgo) << 2);
				/* ulContent |= CAM_VALID; */
			} else
				ulContent = 0;
			/* polling bit, and No Write enable, and address */
			ulCommand = CAM_CONTENT_COUNT * ucIndex + i;
			ulCommand = ulCommand | CAM_POLLINIG | CAM_WRITE;
			/* write content 0 is equall to mark invalid */
			rtw_write32(Adapter, WCAMI, ulContent);  /* delay_ms(40); */
			rtw_write32(Adapter, RWCAM, ulCommand);  /* delay_ms(40); */
		}
	}
	break;
	case HW_VAR_CAM_INVALID_ALL:
		rtw_write32(Adapter, RWCAM, BIT(31) | BIT(30));
		break;
	case HW_VAR_AC_PARAM_VO:
		rtw_write32(Adapter, REG_EDCA_VO_PARAM, ((u32 *)(val))[0]);
		break;
	case HW_VAR_AC_PARAM_VI:
		rtw_write32(Adapter, REG_EDCA_VI_PARAM, ((u32 *)(val))[0]);
		break;
	case HW_VAR_AC_PARAM_BE:
		pHalData->ac_param_be = ((u32 *)(val))[0];
		rtw_write32(Adapter, REG_EDCA_BE_PARAM, ((u32 *)(val))[0]);
		break;
	case HW_VAR_AC_PARAM_BK:
		rtw_write32(Adapter, REG_EDCA_BK_PARAM, ((u32 *)(val))[0]);
		break;
	case HW_VAR_ACM_CTRL: {
		u8	acm_ctrl = *((u8 *)val);
		u8	AcmCtrl = rtw_read8(Adapter, REG_ACMHWCTRL);

		if (acm_ctrl > 1)
			AcmCtrl = AcmCtrl | 0x1;

		if (acm_ctrl & BIT(1))
			AcmCtrl |= AcmHw_VoqEn;
		else
			AcmCtrl &= (~AcmHw_VoqEn);

		if (acm_ctrl & BIT(2))
			AcmCtrl |= AcmHw_ViqEn;
		else
			AcmCtrl &= (~AcmHw_ViqEn);

		if (acm_ctrl & BIT(3))
			AcmCtrl |= AcmHw_BeqEn;
		else
			AcmCtrl &= (~AcmHw_BeqEn);

		RTW_INFO("[HW_VAR_ACM_CTRL] Write 0x%X\n", AcmCtrl);
		rtw_write8(Adapter, REG_ACMHWCTRL, AcmCtrl);
	}
	break;
#ifdef CONFIG_80211N_HT
	case HW_VAR_AMPDU_FACTOR: {
		u32	AMPDULen = (*(u8 *)val);
		if (AMPDULen < HT_AGG_SIZE_64K)
			AMPDULen = (0x2000 << (*(u8 *)val)) - 1;
		else
			AMPDULen = 0xffff;

		rtw_write16(Adapter, REG_AMPDU_MAX_LENGTH_8192E, AMPDULen);
	}
	break;
#endif /* CONFIG_80211N_HT */
	case HW_VAR_H2C_FW_PWRMODE: {
		u8	psmode = (*(u8 *)val);

		rtl8192e_set_FwPwrMode_cmd(Adapter, psmode);
	}
	break;
	case HW_VAR_H2C_FW_JOINBSSRPT: {
		u8	mstatus = (*(u8 *)val);

		rtl8192e_set_FwJoinBssReport_cmd(Adapter, mstatus);
	}
	break;
	case HW_VAR_DL_RSVD_PAGE:
#ifdef CONFIG_BT_COEXIST
		if (pHalData->EEPROMBluetoothCoexist == 1) {
			if (check_fwstate(&Adapter->mlmepriv, WIFI_AP_STATE) == _TRUE)
				rtl8192e_download_BTCoex_AP_mode_rsvd_page(Adapter);
		}
#endif /* CONFIG_BT_COEXIST */
	break;

#ifdef CONFIG_P2P_PS
	case HW_VAR_H2C_FW_P2P_PS_OFFLOAD: {
		u8	p2p_ps_state = (*(u8 *)val);
		rtl8192e_set_p2p_ps_offload_cmd(Adapter, p2p_ps_state);
	}
	break;
#endif /* CONFIG_P2P */
	case HW_VAR_TRIGGER_GPIO_0:

		break;

	case HW_VAR_EFUSE_BYTES: /* To set EFUE total used bytes, added by Roger, 2008.12.22. */
		pHalData->EfuseUsedBytes = *((u16 *)val);
		break;
	case HW_VAR_FIFO_CLEARN_UP: {
		struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(Adapter);
		u8 trycnt = 100;
		u32 reg_hw_ssn;

		/* pause tx */
		rtw_write8(Adapter, REG_TXPAUSE, 0xff);

		if (Adapter->xmitpriv.hw_ssn_seq_no == 1)
			reg_hw_ssn = REG_HW_SEQ1;
		else if (Adapter->xmitpriv.hw_ssn_seq_no == 2)
			reg_hw_ssn = REG_HW_SEQ2;
		else if (Adapter->xmitpriv.hw_ssn_seq_no == 3)
			reg_hw_ssn = REG_HW_SEQ3;
		else
			reg_hw_ssn = REG_HW_SEQ0;

		/* keep sn */
		Adapter->xmitpriv.nqos_ssn = rtw_read16(Adapter, reg_hw_ssn);

		if (pwrpriv->bkeepfwalive != _TRUE) {
			/* RX DMA stop */
			rtw_write32(Adapter, REG_RXPKT_NUM, (rtw_read32(Adapter, REG_RXPKT_NUM) | RW_RELEASE_EN));
			do {
				if (!(rtw_read32(Adapter, REG_RXPKT_NUM) & RXDMA_IDLE))
					break;
			} while (trycnt--);
			if (trycnt == 0)
				RTW_INFO("Stop RX DMA failed......\n");

			/* RQPN Load 0 */
			rtw_write16(Adapter, REG_RQPN_NPQ, 0x0);
			rtw_write32(Adapter, REG_RQPN, 0x80000000);
			rtw_mdelay_os(10);
		}
	}
	break;
	case HW_VAR_RESTORE_HW_SEQ:
		{
			/* restore Sequence No. */
			u32 reg_hw_ssn;

			if (Adapter->xmitpriv.hw_ssn_seq_no == 1)
				reg_hw_ssn = REG_HW_SEQ1;
			else if (Adapter->xmitpriv.hw_ssn_seq_no == 2)
				reg_hw_ssn = REG_HW_SEQ2;
			else if (Adapter->xmitpriv.hw_ssn_seq_no == 3)
				reg_hw_ssn = REG_HW_SEQ3;
			else
				reg_hw_ssn = REG_HW_SEQ0;

			rtw_write8(Adapter, reg_hw_ssn, Adapter->xmitpriv.nqos_ssn);
		}
		break;
	case HW_VAR_CHECK_TXBUF:
#ifdef CONFIG_CONCURRENT_MODE
		{
			u8	RetryLimit = 0x01;
			u16 val16;
			u32 reg_200 = 0, reg_204 = 0, reg_214 = 0;
			u32 init_reg_200 = 0, init_reg_204 = 0, init_reg_214 = 0;
			systime start = rtw_get_current_time();
			u32 pass_ms;
			int i = 0;

			/* Shorten TX packet retry limit to save flush time */
			rtw_write16(Adapter, REG_RETRY_LIMIT, BIT_SRL(RetryLimit) | BIT_LRL(RetryLimit));

			init_reg_200 = rtw_read32(Adapter, REG_RQPN); 		/* PUBQ/LQ/HQ */
			init_reg_204 = rtw_read32(Adapter, REG_FIFOPAGE);
			init_reg_214 = rtw_read32(Adapter, REG_RQPN_NPQ);

			while (rtw_get_passing_time_ms(start) < 2000
			       && !RTW_CANNOT_RUN(Adapter)
			      ) {
				reg_200 = rtw_read32(Adapter, REG_RQPN);
				reg_204 = rtw_read32(Adapter, REG_FIFOPAGE);
				reg_214 = rtw_read32(Adapter, REG_RQPN_NPQ);

				/* Make sure all reserve pages are returned */
				if (((reg_200 & 0x00ffffff) == (reg_204 & 0x00ffffff)) &&	/* PUBQ/LQ/HQ */
				    ((reg_214 & 0x00ff00ff) == ((reg_214 >> 8) & 0x00ff00ff)))		/* EXQ/NQ */
					break;

				i++;
				/* wait MAC to flush out reserve pages */
				rtw_msleep_os(10);
			}

			pass_ms = rtw_get_passing_time_ms(start);

			if (RTW_CANNOT_RUN(Adapter)) {
				; /* do nothing */
			} else if (pass_ms >= 2000 || (reg_200 & 0x00ffffff) != (reg_204 & 0x00ffffff)) {
				RTW_PRINT("%s:(HW_VAR_CHECK_TXBUF)NOT empty(%d) in %d ms\n", __FUNCTION__, i, pass_ms);
				RTW_PRINT("%s:(HW_VAR_CHECK_TXBUF)0x200=0x%08x, 0x204=0x%08x 0x214=0x%08x (0x%08x, 0x%08x, 0x%08x)\n",
					__FUNCTION__, reg_200, reg_204, reg_214, init_reg_200, init_reg_204, init_reg_214);
				/* rtw_warn_on(1); */
			} else
				RTW_INFO("%s:(HW_VAR_CHECK_TXBUF)TXBUF Empty(%d) in %d ms\n", __FUNCTION__, i, pass_ms);

			/* Restore TX packet retry limit */
			RetryLimit = RL_VAL_STA;
			rtw_write16(Adapter, REG_RETRY_LIMIT, BIT_SRL(RetryLimit) | BIT_LRL(RetryLimit));
		}
#endif
		break;

	case HW_VAR_BCN_VALID:
#ifdef CONFIG_CONCURRENT_MODE
		if (Adapter->hw_port == HW_PORT1)
			rtw_write8(Adapter, REG_DWBCN1_CTRL_8192E+2, rtw_read8(Adapter, REG_DWBCN1_CTRL_8192E+2) | BIT0);
		else
#endif

			/* BCN_VALID, BIT16 of REG_DWBCN0_CTRL_8192E = BIT0 of REG_DWBCN0_CTRL_8192E+2, write 1 to clear, Clear by sw */
			rtw_write8(Adapter, REG_DWBCN0_CTRL_8192E+2, rtw_read8(Adapter, REG_DWBCN0_CTRL_8192E+2) | BIT0);

		break;
	case HW_VAR_DL_BCN_SEL:
#ifdef CONFIG_CONCURRENT_MODE
		if (Adapter->hw_port == HW_PORT1)
			/*SW_BCN_SEL - Port1*/
			rtw_write8(Adapter, REG_DWBCN1_CTRL_8192E+2, rtw_read8(Adapter, REG_DWBCN1_CTRL_8192E+2) | BIT4);

		else
#endif
			/*SW_BCN_SEL - Port0*/
			rtw_write8(Adapter, REG_DWBCN1_CTRL_8192E+2, rtw_read8(Adapter, REG_DWBCN1_CTRL_8192E+2) & ~BIT4);


		break;
#ifdef CONFIG_GPIO_WAKEUP
	case HW_SET_GPIO_WL_CTRL: {
		u8 enable = *val;
		u8 value = rtw_read8(Adapter, 0x4e);
		if (enable && (value & BIT(6))) {
			value &= ~BIT(6);
			rtw_write8(Adapter, 0x4e, value);
		} else if (enable == _FALSE) {
			value |= BIT(6);
			rtw_write8(Adapter, 0x4e, value);
		}
		RTW_INFO("%s: set WL control, 0x4E=0x%02X\n",
			 __func__, rtw_read8(Adapter, 0x4e));
	}
	break;
#endif

#ifdef CONFIG_BEAMFORMING
#if (BEAMFORMING_SUPPORT == 1)
	case HW_VAR_HW_REG_TIMER_INIT: {
		HAL_HW_TIMER_TYPE TimerType = (*(PHAL_HW_TIMER_TYPE)val) >> 16;

		rtw_write8(Adapter, 0x164, 1);

		if (TimerType == HAL_TIMER_TXBF)
			rtw_write16(Adapter, 0x15C, (*(pu2Byte)val));
		else if (TimerType == HAL_TIMER_EARLYMODE)
			rtw_write32(Adapter, 0x160, 0x05000190);
	}
	break;
	case HW_VAR_HW_REG_TIMER_START: {
		HAL_HW_TIMER_TYPE TimerType = *(PHAL_HW_TIMER_TYPE)val;

		if (TimerType == HAL_TIMER_TXBF)
			rtw_write8(Adapter, 0x15F, 0x5);
		else if (TimerType == HAL_TIMER_EARLYMODE)
			rtw_write8(Adapter, 0x163, 0x5);
	}
	break;
	case HW_VAR_HW_REG_TIMER_RESTART: {
		HAL_HW_TIMER_TYPE TimerType = *(PHAL_HW_TIMER_TYPE)val & 0xffff;

		if (TimerType == HAL_TIMER_TXBF) {
			u4Byte Reg15C = (*(pu4Byte)val) >> 16 | BIT24 | BIT26;

			rtw_write8(Adapter, 0x15F, 0x0);
			rtw_write32(Adapter, 0x15F, Reg15C);
		} else if (TimerType == HAL_TIMER_EARLYMODE) {
			rtw_write8(Adapter, 0x163, 0x0);
			rtw_write8(Adapter, 0x163, 0x5);
		}
	}
	break;
	case HW_VAR_HW_REG_TIMER_STOP: {
		HAL_HW_TIMER_TYPE TimerType = *(PHAL_HW_TIMER_TYPE)val;

		if (TimerType == HAL_TIMER_TXBF)
			rtw_write8(Adapter, 0x15F, 0);
		else if (TimerType == HAL_TIMER_EARLYMODE)
			rtw_write8(Adapter, 0x163, 0x0);
	}
	break;
#endif/*(BEAMFORMING_SUPPORT ==1)*/
#endif/*CONFIG_BEAMFORMING*/
#if defined(CONFIG_TDLS) && defined(CONFIG_TDLS_CH_SW)
	case HW_VAR_TDLS_BCN_EARLY_C2H_RPT:
		rtl8192e_set_BcnEarly_C2H_Rpt_cmd(Adapter, *val);
		break;
#endif
	default:
		ret = SetHwReg(Adapter, variable, val);
		break;
	}

	return ret;
}

struct qinfo_92e {
	u32 head:8;
	u32 pkt_num:7;
	u32 tail:8;
	u32 ac:2;
	u32 macid:7;
};

struct bcn_qinfo_92e {
	u16 head:8;
	u16 pkt_num:8;
};

void dump_qinfo_92e(void *sel, struct qinfo_92e *info, const char *tag)
{
	/* if (info->pkt_num) */
	RTW_PRINT_SEL(sel, "%shead:0x%02x, tail:0x%02x, pkt_num:%u, macid:%u, ac:%u\n"
		, tag ? tag : "", info->head, info->tail, info->pkt_num, info->macid, info->ac
		     );
}

void dump_bcn_qinfo_92e(void *sel, struct bcn_qinfo_92e *info, const char *tag)
{
	/* if (info->pkt_num) */
	RTW_PRINT_SEL(sel, "%shead:0x%02x, pkt_num:%u\n"
		      , tag ? tag : "", info->head, info->pkt_num
		     );
}

void dump_mac_qinfo_92e(void *sel, _adapter *adapter)
{
	u32 q0_info;
	u32 q1_info;
	u32 q2_info;
	u32 q3_info;
	u32 q4_info;
	u32 q5_info;
	u32 q6_info;
	u32 q7_info;
	u32 mg_q_info;
	u32 hi_q_info;
	u16 bcn_q_info;

	q0_info = rtw_read32(adapter, REG_Q0_INFO);
	q1_info = rtw_read32(adapter, REG_Q1_INFO);
	q2_info = rtw_read32(adapter, REG_Q2_INFO);
	q3_info = rtw_read32(adapter, REG_Q3_INFO);
	q4_info = rtw_read32(adapter, REG_Q4_INFO);
	q5_info = rtw_read32(adapter, REG_Q5_INFO);
	q6_info = rtw_read32(adapter, REG_Q6_INFO);
	q7_info = rtw_read32(adapter, REG_Q7_INFO);
	mg_q_info = rtw_read32(adapter, REG_MGQ_INFO);
	hi_q_info = rtw_read32(adapter, REG_HGQ_INFO);
	bcn_q_info = rtw_read16(adapter, REG_BCNQ_INFO);

	dump_qinfo_92e(sel, (struct qinfo_92e *)&q0_info, "Q0 ");
	dump_qinfo_92e(sel, (struct qinfo_92e *)&q1_info, "Q1 ");
	dump_qinfo_92e(sel, (struct qinfo_92e *)&q2_info, "Q2 ");
	dump_qinfo_92e(sel, (struct qinfo_92e *)&q3_info, "Q3 ");
	dump_qinfo_92e(sel, (struct qinfo_92e *)&q4_info, "Q4 ");
	dump_qinfo_92e(sel, (struct qinfo_92e *)&q5_info, "Q5 ");
	dump_qinfo_92e(sel, (struct qinfo_92e *)&q6_info, "Q6 ");
	dump_qinfo_92e(sel, (struct qinfo_92e *)&q7_info, "Q7 ");
	dump_qinfo_92e(sel, (struct qinfo_92e *)&mg_q_info, "MG ");
	dump_qinfo_92e(sel, (struct qinfo_92e *)&hi_q_info, "HI ");
	dump_bcn_qinfo_92e(sel, (struct bcn_qinfo_92e *)&bcn_q_info, "BCN ");
}

static void dump_mac_txfifo_92e(void *sel, _adapter *adapter)
{
	u32 rqpn, rqpn_npq;
	u32 hpq, lpq, npq, epq, pubq;

	rqpn = rtw_read32(adapter, REG_FIFOPAGE);
	rqpn_npq = rtw_read32(adapter, REG_RQPN_NPQ);

	hpq = (rqpn & 0xFF);
	lpq = ((rqpn & 0xFF00)>>8);
	pubq = ((rqpn & 0xFF0000)>>16);
	npq = ((rqpn_npq & 0xFF00)>>8);
	epq = ((rqpn_npq & 0xFF000000)>>24);

	RTW_PRINT_SEL(sel, "Tx: available page num: ");
	if ((hpq == 0xEA) && (hpq == lpq) && (hpq == pubq))
		RTW_PRINT_SEL(sel, "N/A (reg val = 0xea)\n");
	else
		RTW_PRINT_SEL(sel, "HPQ: %d, LPQ: %d, NPQ: %d, EPQ: %d, PUBQ: %d\n"
			, hpq, lpq, npq, epq, pubq);
}

void rtl8192e_read_wmmedca_reg(PADAPTER adapter, u16 *vo_params, u16 *vi_params, u16 *be_params, u16 *bk_params)
{
	u8 vo_reg_params[4];
	u8 vi_reg_params[4];
	u8 be_reg_params[4];
	u8 bk_reg_params[4];

	GetHwReg8192E(adapter, HW_VAR_AC_PARAM_VO, vo_reg_params);
	GetHwReg8192E(adapter, HW_VAR_AC_PARAM_VI, vi_reg_params);
	GetHwReg8192E(adapter, HW_VAR_AC_PARAM_BE, be_reg_params);
	GetHwReg8192E(adapter, HW_VAR_AC_PARAM_BK, bk_reg_params);

	vo_params[0] = vo_reg_params[0];
	vo_params[1] = vo_reg_params[1] & 0x0F;
	vo_params[2] = (vo_reg_params[1] & 0xF0) >> 4;
	vo_params[3] = ((vo_reg_params[3] << 8) | (vo_reg_params[2])) * 32;

	vi_params[0] = vi_reg_params[0];
	vi_params[1] = vi_reg_params[1] & 0x0F;
	vi_params[2] = (vi_reg_params[1] & 0xF0) >> 4;
	vi_params[3] = ((vi_reg_params[3] << 8) | (vi_reg_params[2])) * 32;

	be_params[0] = be_reg_params[0];
	be_params[1] = be_reg_params[1] & 0x0F;
	be_params[2] = (be_reg_params[1] & 0xF0) >> 4;
	be_params[3] = ((be_reg_params[3] << 8) | (be_reg_params[2])) * 32;

	bk_params[0] = bk_reg_params[0];
	bk_params[1] = bk_reg_params[1] & 0x0F;
	bk_params[2] = (bk_reg_params[1] & 0xF0) >> 4;
	bk_params[3] = ((bk_reg_params[3] << 8) | (bk_reg_params[2])) * 32;

	vo_params[1] = (1 << vo_params[1]) - 1;
	vo_params[2] = (1 << vo_params[2]) - 1;
	vi_params[1] = (1 << vi_params[1]) - 1;
	vi_params[2] = (1 << vi_params[2]) - 1;
	be_params[1] = (1 << be_params[1]) - 1;
	be_params[2] = (1 << be_params[2]) - 1;
	bk_params[1] = (1 << bk_params[1]) - 1;
	bk_params[2] = (1 << bk_params[2]) - 1;
}

void GetHwReg8192E(PADAPTER Adapter, u8 variable, u8 *val)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct dm_struct		*podmpriv = &pHalData->odmpriv;
	u32 val32;

	switch (variable) {
	case HW_VAR_TXPAUSE:
		val[0] = rtw_read8(Adapter, REG_TXPAUSE);
		break;
	case HW_VAR_BCN_VALID:
#ifdef CONFIG_CONCURRENT_MODE
		if (Adapter->hw_port == HW_PORT1)
			val[0] = (BIT0 & rtw_read8(Adapter, REG_DWBCN1_CTRL_8192E+2)) ? _TRUE : _FALSE;

		else
#endif
			/*BCN_VALID, BIT16 of REG_DWBCN0_CTRL_8192E = BIT0 of REG_DWBCN0_CTRL_8192E+2*/
			val[0] = (BIT0 & rtw_read8(Adapter, REG_DWBCN0_CTRL_8192E+2)) ? _TRUE : _FALSE;

		break;

	case HW_VAR_AC_PARAM_VO:
		val32 = rtw_read32(Adapter, REG_EDCA_VO_PARAM);
		val[0] = val32 & 0xFF;
		val[1] = (val32 >> 8) & 0xFF;
		val[2] = (val32 >> 16) & 0xFF;
		val[3] = (val32 >> 24) & 0x07;
		break;

	case HW_VAR_AC_PARAM_VI:
		val32 = rtw_read32(Adapter, REG_EDCA_VI_PARAM);
		val[0] = val32 & 0xFF;
		val[1] = (val32 >> 8) & 0xFF;
		val[2] = (val32 >> 16) & 0xFF;
		val[3] = (val32 >> 24) & 0x07;
		break;

	case HW_VAR_AC_PARAM_BE:
		val32 = rtw_read32(Adapter, REG_EDCA_BE_PARAM);
		val[0] = val32 & 0xFF;
		val[1] = (val32 >> 8) & 0xFF;
		val[2] = (val32 >> 16) & 0xFF;
		val[3] = (val32 >> 24) & 0x07;
		break;

	case HW_VAR_AC_PARAM_BK:
		val32 = rtw_read32(Adapter, REG_EDCA_BK_PARAM);
		val[0] = val32 & 0xFF;
		val[1] = (val32 >> 8) & 0xFF;
		val[2] = (val32 >> 16) & 0xFF;
		val[3] = (val32 >> 24) & 0x07;
		break;

	case HW_VAR_EFUSE_BYTES: /* To get EFUE total used bytes, added by Roger, 2008.12.22. */
		*((u16 *)(val)) = pHalData->EfuseUsedBytes;
		break;
	case HW_VAR_CHK_HI_QUEUE_EMPTY:
		/* *val = ((rtw_read32(Adapter, REG_HGQ_INFO)&0x00007f00)==0) ? _TRUE:_FALSE; */
		*val = (rtw_read16(Adapter, REG_TXPKT_EMPTY) & BIT(10)) ? _TRUE : _FALSE;
		break;
	case HW_VAR_CHK_MGQ_CPU_EMPTY:
		*val = (rtw_read16(Adapter, REG_TXPKT_EMPTY) & BIT(8)) ? _TRUE : _FALSE;
		break;
	case HW_VAR_SYS_CLKR:
		*val = rtw_read8(Adapter, REG_SYS_CLKR);
		break;
	case HW_VAR_DUMP_MAC_QUEUE_INFO:
		dump_mac_qinfo_92e(val, Adapter);
		break;
	case HW_VAR_DUMP_MAC_TXFIFO:
		dump_mac_txfifo_92e(val, Adapter);
		break;
	default:
		GetHwReg(Adapter, variable, val);
		break;
	}

}
/*
 *	Description:
 *		Change default setting of specified variable.
 *   */
u8
SetHalDefVar8192E(
	IN	PADAPTER				Adapter,
	IN	HAL_DEF_VARIABLE		eVariable,
	IN	PVOID					pValue
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8			bResult = _SUCCESS;

	switch (eVariable) {
	default:
		bResult = SetHalDefVar(Adapter, eVariable, pValue);
		break;
	}

	return bResult;
}
void hal_ra_info_dump(_adapter *padapter , void *sel)
{
	int i;
	u8 mac_id;
	u32 cmd;
	u32 ra_info1, ra_info2, bw_set;
	u32 rate_mask1, rate_mask2;
	u8 curr_tx_rate, curr_tx_sgi, hight_rate, lowest_rate;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	HAL_DATA_TYPE *HalData = GET_HAL_DATA(padapter);
	struct macid_ctl_t *macid_ctl = dvobj_to_macidctl(dvobj);

	for (i = 0; i < macid_ctl->num; i++) {
		if (rtw_macid_is_used(macid_ctl, i) && !rtw_macid_is_bmc(macid_ctl, i)) {


			mac_id = (u8) i;
			_RTW_PRINT_SEL(sel , "============ RA status check  Mac_id:%d ===================\n", mac_id);
			cmd = 0x40000100 | mac_id;
			rtw_write32(padapter, REG_HMEBOX_E2_E3_8192E, cmd);
			rtw_msleep_os(10);
			ra_info1 = rtw_read32(padapter, REG_RSVD5_8192E);
			curr_tx_sgi = rtw_get_current_tx_sgi(padapter, macid_ctl->sta[mac_id]);
			curr_tx_rate = rtw_get_current_tx_rate(padapter, macid_ctl->sta[mac_id]);

			_RTW_PRINT_SEL(sel , "[ ra_info1:0x%08x ] =>PWRSTS = 0x%02x\n", ra_info1, (ra_info1 >> 8)  & 0x07);
			_RTW_PRINT_SEL(sel , "[ ra_info1:0x%08x ] =>cur_tx_rate= %s, cur_sgi:%d\n", ra_info1, HDATA_RATE(curr_tx_rate), curr_tx_sgi);

			cmd = 0x40000400 | mac_id;
			rtw_write32(padapter, REG_HMEBOX_E2_E3_8192E, cmd);
			rtw_msleep_os(10);
			ra_info1 = rtw_read32(padapter, REG_RSVD5_8192E);
			ra_info2 = rtw_read32(padapter, REG_RSVD6_8192E);
			rate_mask1 = rtw_read32(padapter, REG_RSVD7_8192E);
			rate_mask2 = rtw_read32(padapter, REG_RSVD8_8192E);
			hight_rate = ra_info2 & 0xFF;
			lowest_rate = (ra_info2 >> 8) & 0xFF;
			bw_set = (ra_info1 >> 8)  & 0xFF;

			_RTW_PRINT_SEL(sel , "[ ra_info1:0x%08x ] => VHT_EN=0x%02x, ", ra_info1, (ra_info1 >> 24) & 0xFF);

			switch (bw_set) {

			case CHANNEL_WIDTH_20:
				_RTW_PRINT_SEL(sel , "BW_setting=20M\n");
				break;

			case CHANNEL_WIDTH_40:
				_RTW_PRINT_SEL(sel , "BW_setting=40M\n");
				break;

			case CHANNEL_WIDTH_80:
				_RTW_PRINT_SEL(sel , "BW_setting=80M\n");
				break;

			case CHANNEL_WIDTH_160:
				_RTW_PRINT_SEL(sel , "BW_setting=160M\n");
				break;

			default:
				_RTW_PRINT_SEL(sel , "BW_setting=0x%02x\n", bw_set);
				break;

			}


			_RTW_PRINT_SEL(sel , "[ ra_info1:0x%08x ] =>RSSI = %d,DISRA = 0x%02x\n",
				       ra_info1,
				       ra_info1 & 0xFF,
				       (ra_info1 >> 16) & 0xFF);

			_RTW_PRINT_SEL(sel, "[ ra_info2:0x%08x ] =>hight_rate= %s, lowest_rate= %s, SGI = 0x%02x, RateID = %d\n",
				       ra_info2,
				       HDATA_RATE(hight_rate),
				       HDATA_RATE(lowest_rate),
				       (ra_info2 >> 16) & 0xFF,
				       (ra_info2 >> 24) & 0xFF);

			_RTW_PRINT_SEL(sel , "rate_mask2:0x%08x , rate_mask1:0x%08x\n", rate_mask2, rate_mask1);

		}
	}

}
/*
 *	Description:
 *		Query setting of specified variable.
 *   */
u8
GetHalDefVar8192E(
	IN	PADAPTER				Adapter,
	IN	HAL_DEF_VARIABLE		eVariable,
	IN	PVOID					pValue
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8			bResult = _SUCCESS;

	switch (eVariable) {

	case HAL_DEF_IS_SUPPORT_ANT_DIV:
#ifdef CONFIG_ANTENNA_DIVERSITY
		*((u8 *)pValue) = (pHalData->AntDivCfg == 0) ? _FALSE : _TRUE;
#endif
		break;
	case HAL_DEF_DRVINFO_SZ:
		*((u32 *)pValue) = DRVINFO_SZ;
		break;
	case HAL_DEF_MAX_RECVBUF_SZ:
		*((u32 *)pValue) = MAX_RECVBUF_SZ;
		break;
	case HAL_DEF_RX_PACKET_OFFSET:
		*((u32 *)pValue) = RXDESC_SIZE + (DRVINFO_SZ * 8);
		break;

	case HAL_DEF_TX_LDPC:
		if (IS_NORMAL_CHIP(pHalData->version_id))
			*((PBOOLEAN)pValue) = _FALSE;
		else
			*((PBOOLEAN)pValue) = _FALSE;
		break;

	case HAL_DEF_RX_LDPC:
		if (IS_NORMAL_CHIP(pHalData->version_id))
			*((PBOOLEAN)pValue) = _FALSE;
		else
			*((PBOOLEAN)pValue) = _FALSE;
		break;

	case HAL_DEF_TX_STBC:
		if (pHalData->rf_type == RF_1T2R || pHalData->rf_type == RF_1T1R)
			*(u8 *)pValue = 0;
		else
			*(u8 *)pValue = 1;
		break;

	case HAL_DEF_RX_STBC:
		*(u8 *)pValue = 1;
		break;

	case HAL_DEF_EXPLICIT_BEAMFORMER:
	case HAL_DEF_EXPLICIT_BEAMFORMEE:
		*((PBOOLEAN)pValue) = _FALSE;
		break;

	case HW_DEF_RA_INFO_DUMP:
		hal_ra_info_dump(Adapter, pValue);
		break;
	case HAL_DEF_TX_PAGE_SIZE:
		*((u32 *)pValue) = PAGE_SIZE_TX_92E;
		break;
	case HAL_DEF_TX_PAGE_BOUNDARY:
		*(u8 *)pValue = TX_PAGE_BOUNDARY_8192E;
		break;
	case HAL_DEF_RX_DMA_SZ_WOW:
		*(u32 *)pValue =
			MAX_RX_DMA_BUFFER_SIZE_8192E - RESV_FMWF;
		break;
	case HAL_DEF_RX_DMA_SZ:
		*(u32 *)pValue = MAX_RX_DMA_BUFFER_SIZE_8192E;
		break;
	case HAL_DEF_RX_PAGE_SIZE:
		*((u32 *)pValue) = 8;
		break;
	case HW_VAR_BEST_AMPDU_DENSITY:
		*((u32 *)pValue) = AMPDU_DENSITY_VALUE_7;
		break;
	default:
		bResult = GetHalDefVar(Adapter, eVariable, pValue);
		break;
	}

	return bResult;
}


void rtl8192e_start_thread(_adapter *padapter)
{
#ifdef CONFIG_SDIO_HCI
#ifndef CONFIG_SDIO_TX_TASKLET
	struct xmit_priv *xmitpriv = &padapter->xmitpriv;

	if (xmitpriv->SdioXmitThread == NULL) {
		RTW_INFO(FUNC_ADPT_FMT " start RTWHALXT\n", FUNC_ADPT_ARG(padapter));
		xmitpriv->SdioXmitThread = kthread_run(rtl8192es_xmit_thread, padapter, "RTWHALXT");
		if (IS_ERR(xmitpriv->SdioXmitThread)) {
			RTW_ERR("%s: start rtl8192es_xmit_thread FAIL!!\n", __func__);
			xmitpriv->SdioXmitThread = NULL;
		}
	}
#endif

#endif
}

void rtl8192e_stop_thread(_adapter *padapter)
{
#ifdef CONFIG_SDIO_HCI
#ifndef CONFIG_SDIO_TX_TASKLET
	struct xmit_priv *xmitpriv = &padapter->xmitpriv;

	/* stop xmit_buf_thread */
	if (xmitpriv->SdioXmitThread) {
		_rtw_up_sema(&xmitpriv->SdioXmitSema);
		rtw_thread_stop(xmitpriv->SdioXmitThread);
		xmitpriv->SdioXmitThread = NULL;
	}
#endif
#endif
}
void hal_notch_filter_8192E(_adapter *adapter, bool enable)
{
	if (enable) {
		RTW_INFO("Enable notch filter\n");
		/* rtw_write8(adapter, rOFDM0_RxDSP+1, rtw_read8(adapter, rOFDM0_RxDSP+1) | BIT1); */
	} else {
		RTW_INFO("Disable notch filter\n");
		/* rtw_write8(adapter, rOFDM0_RxDSP+1, rtw_read8(adapter, rOFDM0_RxDSP+1) & ~BIT1); */
	}
}


static void read_chip_version_8192e(PADAPTER Adapter)
{
	u32	value32;
	HAL_DATA_TYPE	*pHalData;
	u8	tmpvdr;
	pHalData = GET_HAL_DATA(Adapter);

	value32 = rtw_read32(Adapter, REG_SYS_CFG1_8192E);
	RTW_INFO("%s 0xF0 = 0x%x\n", __FUNCTION__, value32);

	pHalData->version_id.ICType = CHIP_8192E;
	pHalData->version_id.RFType = (value32 & RF_TYPE_ID) ? RF_2T2R : RF_1T1R;
	pHalData->version_id.ChipType = ((value32 & RTL_ID) ? TEST_CHIP : NORMAL_CHIP);

	tmpvdr = (value32 & EXT_VENDOR_ID) >> EXT_VENDOR_ID_SHIFT;

	if (tmpvdr == 0x00)
		pHalData->version_id.VendorType = CHIP_VENDOR_TSMC;
	else if (tmpvdr == 0x01)
		pHalData->version_id.VendorType = CHIP_VENDOR_SMIC;
	else if (tmpvdr == 0x02)
		pHalData->version_id.VendorType = CHIP_VENDOR_UMC;

	pHalData->version_id.CUTVersion = (value32 & CHIP_VER_RTL_MASK) >> CHIP_VER_RTL_SHIFT; /* IC version (CUT) */
	pHalData->MultiFunc = RT_MULTI_FUNC_NONE;

	rtw_hal_config_rftype(Adapter);
#if 1
	dump_chip_info(pHalData->version_id);
#endif

}

void init_hal_spec_8192e(_adapter *adapter)
{
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(adapter);

	hal_spec->ic_name = "rtl8192e";
	hal_spec->macid_num = 128;
	hal_spec->sec_cam_ent_num = 64;
	hal_spec->sec_cap = 0;
	hal_spec->rfpath_num_2g = 2;
	hal_spec->rfpath_num_5g = 0;
	hal_spec->txgi_max = 63;
	hal_spec->txgi_pdbm = 2;
	hal_spec->max_tx_cnt = 2;
	hal_spec->tx_nss_num = 2;
	hal_spec->rx_nss_num = 2;
	hal_spec->band_cap = BAND_CAP_2G;
	hal_spec->bw_cap = BW_CAP_20M | BW_CAP_40M;
	hal_spec->port_num = 2;
	hal_spec->proto_cap = PROTO_CAP_11B | PROTO_CAP_11G | PROTO_CAP_11N;

	hal_spec->wl_func = 0
			    | WL_FUNC_P2P
			    | WL_FUNC_MIRACAST
			    | WL_FUNC_TDLS
			    ;

	hal_spec->pg_txpwr_saddr = 0x10;
	hal_spec->pg_txgi_diff_factor = 1;

	rtw_macid_ctl_init_sleep_reg(adapter_to_macidctl(adapter)
		, REG_MACID_SLEEP
		, REG_MACID_SLEEP_1
		, REG_MACID_SLEEP_2
		, REG_MACID_SLEEP_3);
}

void rtl8192e_init_default_value(_adapter *padapter)
{
	PHAL_DATA_TYPE pHalData;
	struct pwrctrl_priv *pwrctrlpriv;
	u8 i;

	pHalData = GET_HAL_DATA(padapter);
	pwrctrlpriv = adapter_to_pwrctl(padapter);

	pHalData->fw_ractrl = _FALSE;
	if (!pwrctrlpriv->bkeepfwalive)
		pHalData->LastHMEBoxNum = 0;

	/* init phydm default value */
	pHalData->bIQKInitialized = _FALSE;

#if defined(CONFIG_USB_HCI) || defined(CONFIG_PCI_HCI)
	pHalData->IntrMask[0]	= (u32)(\
					/* IMR_ROK 		| */
					/* IMR_RDU			| */
					/* IMR_VODOK		| */
					/* IMR_VIDOK		| */
					/* IMR_BEDOK		| */
					/* IMR_BKDOK		| */
					/* IMR_MGNTDOK		| */
					/* IMR_HIGHDOK		| */
					/* IMR_CPWM		| */
					/* IMR_CPWM2		| */
					/* IMR_C2HCMD		| */
					/* IMR_HISR1_IND_INT	| */
					/* IMR_ATIMEND		| */
					/* IMR_BCNDMAINT_E	| */
					/* IMR_HSISR_IND_ON_INT	| */
					/* IMR_BCNDOK0		| */
					/* IMR_BCNDMAINT0	| */
					/* IMR_TSF_BIT32_TOGGLE	| */
					/* IMR_TXBCN0OK	| */
					/* IMR_TXBCN0ERR	| */
					/* IMR_GTINT3		| */
					/* IMR_GTINT4		| */
					/* IMR_TXCCK		| */
					0);

	pHalData->IntrMask[1]	= (u32)(\
					/* IMR_RXFOVW		| */
					/* IMR_TXFOVW		| */
					/* IMR_RXERR		| */
					/* IMR_TXERR		| */
					/* IMR_ATIMEND_E	| */
					/* IMR_BCNDOK1		| */
					/* IMR_BCNDOK2		| */
					/* IMR_BCNDOK3		| */
					/* IMR_BCNDOK4		| */
					/* IMR_BCNDOK5		| */
					/* IMR_BCNDOK6		| */
					/* IMR_BCNDOK7		| */
					/* IMR_BCNDMAINT1	| */
					/* IMR_BCNDMAINT2	| */
					/* IMR_BCNDMAINT3	| */
					/* IMR_BCNDMAINT4	| */
					/* IMR_BCNDMAINT5	| */
					/* IMR_BCNDMAINT6	| */
					/* IMR_BCNDMAINT7	| */
					0);
#endif
	pHalData->EfuseHal.fakeEfuseBank = 0;
	pHalData->EfuseHal.fakeEfuseUsedBytes = 0;
	_rtw_memset(pHalData->EfuseHal.fakeEfuseContent, 0xFF, EFUSE_MAX_HW_SIZE);
	_rtw_memset(pHalData->EfuseHal.fakeEfuseInitMap, 0xFF, EFUSE_MAX_MAP_LEN);
	_rtw_memset(pHalData->EfuseHal.fakeEfuseModifiedMap, 0xFF, EFUSE_MAX_MAP_LEN);
}

#ifdef CONFIG_BT_COEXIST
void rtl8192e_combo_card_WifiOnlyHwInit(PADAPTER Adapter)
{
	u8  u1Tmp;
	u16 u2Tmp;
	u32 u4Tmp;

	RTW_INFO("%s !\n", __FUNCTION__);

	/* antenna fixed to wifi */
	rtw_write8(Adapter, 0x944, 0x24);
	rtw_write32(Adapter, 0x930, 0x700700);
	rtw_write8(Adapter, 0x92c, 0x04);
#ifdef CONFIG_USB_HCI
	rtw_write32(Adapter, REG_PAD_CTRL1_8192E, 0x30430004);
#else
	rtw_write32(Adapter, REG_PAD_CTRL1_8192E, 0x30030004);
#endif
	/* 0x4c[27][24]='00', Set Antenna to BB */
	u4Tmp = rtw_read32(Adapter, BIT_REG_LED_CFG_8192E);
	u4Tmp &= ~BIT24;
	u4Tmp &= ~BIT27;
	rtw_write32(Adapter, BIT_REG_LED_CFG_8192E, u4Tmp);

	/* coex. table */
	rtw_write32(Adapter , REG_BT_COEX_TABLE0_8192E, 0x55555555);
	rtw_write32(Adapter , REG_BT_COEX_TABLE1_8192E, 0x55555555);
	rtw_write32(Adapter , REG_BT_COEX_TABLE2_8192E, 0xffffff);
	rtw_write32(Adapter , REG_BT_COEX_TABLE3_8192E, 0x3);
	/* coex parameters */
	rtw_write8(Adapter, REG_BT_STATISTICS_OTH_CTRL_8192E, 0x3);
	/* 0x790[5:0]=0x5 */
	u1Tmp =  rtw_read8(Adapter, REG_TDMA_TIME_AND_RPT_SAM_SET_8192E);
	u1Tmp &= 0xc0;
	u1Tmp |= 0x5;
	rtw_write8(Adapter, REG_TDMA_TIME_AND_RPT_SAM_SET_8192E, u1Tmp);

	/* enable counter statistics */
	rtw_write8(Adapter, REG_BT_STATISTICS_CTRL_8192E, 0x4);

	/* enable PTA */
	rtw_write8(Adapter, REG_GPIO_MUXCFG_8192E, 0x20);
	/* enable mailbox interface */
	u2Tmp =  rtw_read16(Adapter, REG_GPIO_MUXCFG_8192E);
	u2Tmp |= BIT9;
	rtw_write16(Adapter, REG_GPIO_MUXCFG_8192E, u2Tmp);

	/* enable PTA I2C mailbox */
	u1Tmp =  rtw_read8(Adapter, REG_CR_8192E+1);
	u1Tmp |= BIT4;
	rtw_write8(Adapter, REG_CR_8192E+1, u1Tmp);

	/* enable bt clock when wifi is disabled. */
	u1Tmp =  rtw_read8(Adapter, REG_WLLPS_CTRL_8192E+3);
	u1Tmp |= BIT0;
	rtw_write8(Adapter, REG_WLLPS_CTRL_8192E+3, u1Tmp);
	/* enable bt clock when suspend. */
	u1Tmp =  rtw_read8(Adapter, REG_SYS_PW_CTRL_8192E+3);
	u1Tmp |= BIT0;
	rtw_write8(Adapter, REG_SYS_PW_CTRL_8192E+3, u1Tmp);

}
#endif /* CONFIG_BT_COEXIST */

void rtl8192e_set_hal_ops(struct hal_ops *pHalFunc)
{
	pHalFunc->dm_init = &rtl8192e_init_dm_priv;
	pHalFunc->dm_deinit = &rtl8192e_deinit_dm_priv;

	pHalFunc->read_chip_version = read_chip_version_8192e;

	pHalFunc->set_chnl_bw_handler = &PHY_SetSwChnlBWMode8192E;

	pHalFunc->set_tx_power_level_handler = &PHY_SetTxPowerLevel8192E;
	pHalFunc->get_tx_power_level_handler = &PHY_GetTxPowerLevel8192E;

	pHalFunc->set_tx_power_index_handler = PHY_SetTxPowerIndex_8192E;
	pHalFunc->get_tx_power_index_handler = PHY_GetTxPowerIndex_8192E;

	pHalFunc->hal_dm_watchdog = &rtl8192e_HalDmWatchDog;

	pHalFunc->SetBeaconRelatedRegistersHandler = &SetBeaconRelatedRegisters8192E;

	pHalFunc->run_thread = &rtl8192e_start_thread;
	pHalFunc->cancel_thread = &rtl8192e_stop_thread;

	pHalFunc->read_bbreg = &PHY_QueryBBReg8192E;
	pHalFunc->write_bbreg = &PHY_SetBBReg8192E;
	pHalFunc->read_rfreg = &PHY_QueryRFReg8192E;
	pHalFunc->write_rfreg = &PHY_SetRFReg8192E;

	pHalFunc->read_wmmedca_reg = &rtl8192e_read_wmmedca_reg;

	/* Efuse related function */
	pHalFunc->EfusePowerSwitch = &rtl8192E_EfusePowerSwitch;
	pHalFunc->ReadEFuse = &rtl8192E_ReadEFuse;
	pHalFunc->EFUSEGetEfuseDefinition = &rtl8192E_EFUSE_GetEfuseDefinition;
	pHalFunc->EfuseGetCurrentSize = &rtl8192E_EfuseGetCurrentSize;
	pHalFunc->Efuse_PgPacketRead = &rtl8192E_Efuse_PgPacketRead;
	pHalFunc->Efuse_PgPacketWrite = &rtl8192E_Efuse_PgPacketWrite;
	pHalFunc->Efuse_WordEnableDataWrite = &rtl8192E_Efuse_WordEnableDataWrite;

#ifdef DBG_CONFIG_ERROR_DETECT
	pHalFunc->sreset_init_value = &sreset_init_value;
	pHalFunc->sreset_reset_value = &sreset_reset_value;
	pHalFunc->silentreset = &sreset_reset;
	pHalFunc->sreset_xmit_status_check = &rtl8192e_sreset_xmit_status_check;
	pHalFunc->sreset_linked_status_check  = &rtl8192e_sreset_linked_status_check;
	pHalFunc->sreset_get_wifi_status  = &sreset_get_wifi_status;
	pHalFunc->sreset_inprogress = &sreset_inprogress;
#endif /* DBG_CONFIG_ERROR_DETECT */

	pHalFunc->GetHalODMVarHandler = GetHalODMVar;
	pHalFunc->SetHalODMVarHandler = SetHalODMVar;
	/* pHalFunc->hal_notch_filter = &hal_notch_filter_rtl8192E; */

	pHalFunc->c2h_handler = c2h_handler_8192e;
	pHalFunc->fill_h2c_cmd = &FillH2CCmd_8192E;
	pHalFunc->fill_fake_txdesc = &rtl8192e_fill_fake_txdesc;
	pHalFunc->fw_dl = &FirmwareDownload8192E;
	pHalFunc->hal_get_tx_buff_rsvd_page_num = &GetTxBufferRsvdPageNum8192E;
}
