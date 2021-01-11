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
#define _RTL8192E_PHYCFG_C_

/* #include <drv_types.h> */

#include <rtl8192e_hal.h>

/*---------------------Define local function prototype-----------------------*/

/*----------------------------Function Body----------------------------------*/

/*
 * 1. BB register R/W API
 *   */

u32
PHY_QueryBBReg8192E(
	IN	PADAPTER	Adapter,
	IN	u32			RegAddr,
	IN	u32			BitMask
)
{
	u32	ReturnValue = 0, OriginalValue, BitShift;

#if (DISABLE_BB_RF == 1)
	return 0;
#endif

	/* RTW_INFO("--->PHY_QueryBBReg8812(): RegAddr(%#x), BitMask(%#x)\n", RegAddr, BitMask); */


	OriginalValue = rtw_read32(Adapter, RegAddr);
	BitShift = PHY_CalculateBitShift(BitMask);
	ReturnValue = (OriginalValue & BitMask) >> BitShift;

	/* RTW_INFO("BBR MASK=0x%x Addr[0x%x]=0x%x\n", BitMask, RegAddr, OriginalValue); */
	return ReturnValue;
}


VOID
PHY_SetBBReg8192E(
	IN	PADAPTER	Adapter,
	IN	u4Byte		RegAddr,
	IN	u4Byte		BitMask,
	IN	u4Byte		Data
)
{
	u4Byte			OriginalValue, BitShift;

#if (DISABLE_BB_RF == 1)
	return;
#endif

	if (BitMask != bMaskDWord) {
		/* if not "double word" write */
		OriginalValue = rtw_read32(Adapter, RegAddr);
		BitShift = PHY_CalculateBitShift(BitMask);
		Data = ((OriginalValue)&(~BitMask)) | (((Data << BitShift)) & BitMask);
	}

	rtw_write32(Adapter, RegAddr, Data);

	/* RTW_INFO("BBW MASK=0x%x Addr[0x%x]=0x%x\n", BitMask, RegAddr, Data); */
}

/*
 * 2. RF register R/W API
 *   */

static	u32
phy_RFSerialRead(
	IN	PADAPTER		Adapter,
	IN	enum rf_path		eRFPath,
	IN	u32				Offset
)
{

	u4Byte						retValue = 0;
	HAL_DATA_TYPE				*pHalData = GET_HAL_DATA(Adapter);
	BB_REGISTER_DEFINITION_T	*pPhyReg = &pHalData->PHYRegDef[eRFPath];
	u4Byte						NewOffset;
	u4Byte						tmplong2;
	u1Byte						RfPiEnable = 0;
	u1Byte						i;
	u4Byte						MaskforPhySet = 0;

	_enter_critical_mutex(&(adapter_to_dvobj(Adapter)->rf_read_reg_mutex) , NULL);
	Offset &= 0xff;

	/*  */
	/* Switch page for 8256 RF IC */
	/*  */
	NewOffset = Offset;

	/* For 92S LSSI Read RFLSSIRead */
	/* For RF A/B write 0x824/82c(does not work in the future) */
	/* We must use 0x824 for RF A and B to execute read trigger */

	if (eRFPath == RF_PATH_A) {
		tmplong2 = phy_query_bb_reg(Adapter, rFPGA0_XA_HSSIParameter2 | MaskforPhySet, bMaskDWord);
		tmplong2 = (tmplong2 & (~bLSSIReadAddress)) | (NewOffset << 23) | bLSSIReadEdge;	/* T65 RF */
		phy_set_bb_reg(Adapter, rFPGA0_XA_HSSIParameter2 | MaskforPhySet, bMaskDWord, tmplong2 & (~bLSSIReadEdge));
	} else {
		tmplong2 = phy_query_bb_reg(Adapter, rFPGA0_XB_HSSIParameter2 | MaskforPhySet, bMaskDWord);
		tmplong2 = (tmplong2 & (~bLSSIReadAddress)) | (NewOffset << 23) | bLSSIReadEdge;	/* T65 RF */
		phy_set_bb_reg(Adapter, rFPGA0_XB_HSSIParameter2 | MaskforPhySet, bMaskDWord, tmplong2 & (~bLSSIReadEdge));
	}

	tmplong2 = phy_query_bb_reg(Adapter, rFPGA0_XA_HSSIParameter2 | MaskforPhySet, bMaskDWord);
	phy_set_bb_reg(Adapter, rFPGA0_XA_HSSIParameter2 | MaskforPhySet, bMaskDWord, tmplong2 & (~bLSSIReadEdge));
	phy_set_bb_reg(Adapter, rFPGA0_XA_HSSIParameter2 | MaskforPhySet, bMaskDWord, tmplong2 | bLSSIReadEdge);

	rtw_udelay_os(10);/* PlatformStallExecution(10); */

	/* for(i=0;i<2;i++) */
	/*	PlatformStallExecution(MAX_STALL_TIME);	 */
	rtw_udelay_os(10);/* PlatformStallExecution(10); */

	if (eRFPath == RF_PATH_A)
		RfPiEnable = (u1Byte)phy_query_bb_reg(Adapter, rFPGA0_XA_HSSIParameter1 | MaskforPhySet, BIT8);
	else if (eRFPath == RF_PATH_B)
		RfPiEnable = (u1Byte)phy_query_bb_reg(Adapter, rFPGA0_XB_HSSIParameter1 | MaskforPhySet, BIT8);

	if (RfPiEnable) {
		/* Read from BBreg8b8, 12 bits for 8190, 20bits for T65 RF */
		retValue = phy_query_bb_reg(Adapter, pPhyReg->rfLSSIReadBackPi | MaskforPhySet, bLSSIReadBackData);

		/* RT_DISP(FINIT, INIT_RF, ("Readback from RF-PI : 0x%x\n", retValue)); */
	} else {
		/* Read from BBreg8a0, 12 bits for 8190, 20 bits for T65 RF */
		retValue = phy_query_bb_reg(Adapter, pPhyReg->rfLSSIReadBack | MaskforPhySet, bLSSIReadBackData);

		/* RT_DISP(FINIT, INIT_RF,("Readback from RF-SI : 0x%x\n", retValue)); */
	}
	/* RT_DISP(FPHY, PHY_RFR, ("RFR-%d Addr[0x%x]=0x%x\n", eRFPath, pPhyReg->rfLSSIReadBack, retValue)); */
	_exit_critical_mutex(&(adapter_to_dvobj(Adapter)->rf_read_reg_mutex) , NULL);
	return retValue;

}



static	VOID
phy_RFSerialWrite(
	IN	PADAPTER		Adapter,
	IN	enum rf_path		eRFPath,
	IN	u32				Offset,
	IN	u32				Data
)
{
	u32							DataAndAddr = 0;
	HAL_DATA_TYPE				*pHalData = GET_HAL_DATA(Adapter);
	BB_REGISTER_DEFINITION_T		*pPhyReg = &pHalData->PHYRegDef[eRFPath];
	u32							NewOffset, MaskforPhySet = 0;

	/* 2009/06/17 MH We can not execute IO for power save or other accident mode. */
	/* if(RT_CANNOT_IO(Adapter)) */
	/* { */
	/*	RTPRINT(FPHY, PHY_RFW, ("phy_RFSerialWrite stop\n")); */
	/*	return; */
	/* } */

	/* <20121026, Kordan> If 0x818 == 1, the second value written on the previous address. */
	if (IS_HARDWARE_TYPE_8192EU(Adapter))
		phy_set_bb_reg(Adapter, ODM_AFE_SETTING, 0x20000, 0x0);

	Offset &= 0xff;

	/* Shadow Update */
	/* PHY_RFShadowWrite(Adapter, eRFPath, Offset, Data); */

	/*  */
	/* Switch page for 8256 RF IC */
	/*  */
	NewOffset = Offset;

	/*  */
	/* Put write addr in [5:0]  and write data in [31:16] */
	/*  */
	/* DataAndAddr = (Data<<16) | (NewOffset&0x3f); */
	DataAndAddr = ((NewOffset << 20) | (Data & 0x000fffff)) & 0x0fffffff;	/* T65 RF */

	/*  */
	/* Write Operation */
	/*  */
	phy_set_bb_reg(Adapter, pPhyReg->rf3wireOffset | MaskforPhySet, bMaskDWord, DataAndAddr);

	/* <20121026, Kordan> Restore the value on exit. */
	if (IS_HARDWARE_TYPE_8192EU(Adapter))
		phy_set_bb_reg(Adapter, ODM_AFE_SETTING, 0x20000, 0x1);
}

u32
PHY_QueryRFReg8192E(
	IN	PADAPTER		Adapter,
	IN	enum rf_path		eRFPath,
	IN	u32				RegAddr,
	IN	u32				BitMask
)
{
	u32				Original_Value, Readback_Value, BitShift;

#if (DISABLE_BB_RF == 1)
	return 0;
#endif

	Original_Value = phy_RFSerialRead(Adapter, eRFPath, RegAddr);

	BitShift =  PHY_CalculateBitShift(BitMask);
	Readback_Value = (Original_Value & BitMask) >> BitShift;

	return Readback_Value;
}

VOID
PHY_SetRFReg8192E(
	IN	PADAPTER		Adapter,
	IN	enum rf_path		eRFPath,
	IN	u32				RegAddr,
	IN	u32				BitMask,
	IN	u32				Data
)
{
	u32			Original_Value, BitShift;
#if (DISABLE_BB_RF == 1)
	return;
#endif

	if (BitMask == 0)
		return;

	/* RF data is 20 bits only */
	if (BitMask != bRFRegOffsetMask) {
		Original_Value = phy_RFSerialRead(Adapter, eRFPath, RegAddr);
		BitShift =  PHY_CalculateBitShift(BitMask);
		Data = ((Original_Value)&(~BitMask)) | (Data << BitShift);
	}


	phy_RFSerialWrite(Adapter, eRFPath, RegAddr, Data);

}

/*
 * 3. Initial MAC/BB/RF config by reading MAC/BB/RF txt.
 *   */

s32 PHY_MACConfig8192E(PADAPTER Adapter)
{
	int				rtStatus = _SUCCESS;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	/*  */
	/* Config MAC */
	/*  */
#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	rtStatus = phy_ConfigMACWithParaFile(Adapter, PHY_FILE_MAC_REG);
	if (rtStatus == _FAIL)
#endif
	{
#ifdef CONFIG_EMBEDDED_FWIMG
		odm_config_mac_with_header_file(&pHalData->odmpriv);
		rtStatus = _SUCCESS;
#endif/* CONFIG_EMBEDDED_FWIMG */
	}

	return rtStatus;
}


static	VOID
phy_InitBBRFRegisterDefinition(
	IN	PADAPTER		Adapter
)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);

	/* RF Interface Sowrtware Control */
	pHalData->PHYRegDef[RF_PATH_A].rfintfs = rFPGA0_XAB_RFInterfaceSW; /* 16 LSBs if read 32-bit from 0x870 */
	pHalData->PHYRegDef[RF_PATH_B].rfintfs = rFPGA0_XAB_RFInterfaceSW; /* 16 MSBs if read 32-bit from 0x870 (16-bit for 0x872) */

	/* RF Interface Output (and Enable) */
	pHalData->PHYRegDef[RF_PATH_A].rfintfo = rFPGA0_XA_RFInterfaceOE; /* 16 LSBs if read 32-bit from 0x860 */
	pHalData->PHYRegDef[RF_PATH_B].rfintfo = rFPGA0_XB_RFInterfaceOE; /* 16 LSBs if read 32-bit from 0x864 */

	/* RF Interface (Output and)  Enable */
	pHalData->PHYRegDef[RF_PATH_A].rfintfe = rFPGA0_XA_RFInterfaceOE; /* 16 MSBs if read 32-bit from 0x860 (16-bit for 0x862) */
	pHalData->PHYRegDef[RF_PATH_B].rfintfe = rFPGA0_XB_RFInterfaceOE; /* 16 MSBs if read 32-bit from 0x864 (16-bit for 0x866) */

	pHalData->PHYRegDef[RF_PATH_A].rf3wireOffset = rFPGA0_XA_LSSIParameter; /* LSSI Parameter */
	pHalData->PHYRegDef[RF_PATH_B].rf3wireOffset = rFPGA0_XB_LSSIParameter;

	pHalData->PHYRegDef[RF_PATH_A].rfHSSIPara2 = rFPGA0_XA_HSSIParameter2;  /* wire control parameter2 */
	pHalData->PHYRegDef[RF_PATH_B].rfHSSIPara2 = rFPGA0_XB_HSSIParameter2;  /* wire control parameter2 */

	/* Tranceiver Readback LSSI/HSPI mode */
	pHalData->PHYRegDef[RF_PATH_A].rfLSSIReadBack = rFPGA0_XA_LSSIReadBack;
	pHalData->PHYRegDef[RF_PATH_B].rfLSSIReadBack = rFPGA0_XB_LSSIReadBack;
	pHalData->PHYRegDef[RF_PATH_A].rfLSSIReadBackPi = TransceiverA_HSPI_Readback;
	pHalData->PHYRegDef[RF_PATH_B].rfLSSIReadBackPi = TransceiverB_HSPI_Readback;

	/* pHalData->bPhyValueInitReady=TRUE; */
}

static	int
phy_BB8192E_Config_ParaFile(
	IN	PADAPTER	Adapter
)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	int			rtStatus = _SUCCESS;

	/* Read PHY_REG.TXT BB INIT!! */
#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	if (phy_ConfigBBWithParaFile(Adapter, PHY_FILE_PHY_REG, CONFIG_BB_PHY_REG) == _FAIL)
#endif
	{
#ifdef CONFIG_EMBEDDED_FWIMG
		if (HAL_STATUS_SUCCESS != odm_config_bb_with_header_file(&pHalData->odmpriv, CONFIG_BB_PHY_REG))
			rtStatus = _FAIL;
#endif
	}

	if (rtStatus != _SUCCESS) {
		RTW_INFO("phy_BB8192E_Config_ParaFile():Write BB Reg Fail!!\n");
		goto phy_BB_Config_ParaFile_Fail;
	}

	/* Read PHY_REG_MP.TXT BB INIT!! */
#if (MP_DRIVER == 1)
	if (Adapter->registrypriv.mp_mode == 1) {
#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
		if (phy_ConfigBBWithMpParaFile(Adapter, PHY_FILE_PHY_REG_MP) == _FAIL)
#endif
		{
#ifdef CONFIG_EMBEDDED_FWIMG
			if (HAL_STATUS_SUCCESS != odm_config_bb_with_header_file(&pHalData->odmpriv, CONFIG_BB_PHY_REG_MP))
				rtStatus = _FAIL;
#endif
		}

		if (rtStatus != _SUCCESS) {
			RTW_INFO("%s():Write BB Reg MP Fail!!\n", __FUNCTION__);
			goto phy_BB_Config_ParaFile_Fail;
		}
	}
#endif	/*  #if (MP_DRIVER == 1) */

	/* BB AGC table Initialization */
#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	if (phy_ConfigBBWithParaFile(Adapter, PHY_FILE_AGC_TAB, CONFIG_BB_AGC_TAB) == _FAIL)
#endif
	{
#ifdef CONFIG_EMBEDDED_FWIMG
		if (HAL_STATUS_SUCCESS != odm_config_bb_with_header_file(&pHalData->odmpriv, CONFIG_BB_AGC_TAB))
			rtStatus = _FAIL;
#endif
	}

	if (rtStatus != _SUCCESS)
		RTW_INFO("phy_BB8192E_Config_ParaFile():AGC Table Fail\n");

phy_BB_Config_ParaFile_Fail:

	return rtStatus;
}

int
PHY_BBConfig8192E(
	IN	PADAPTER	Adapter
)
{
	int	rtStatus = _SUCCESS;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u16	TmpU2B = 0;

	phy_InitBBRFRegisterDefinition(Adapter);

	/* Enable BB and RF */
	TmpU2B = rtw_read16(Adapter, REG_SYS_FUNC_EN);

#ifdef CONFIG_PCI_HCI
	if (IS_HARDWARE_TYPE_8192EE(Adapter))
		TmpU2B |= (FEN_PPLL | FEN_PCIEA | FEN_DIO_PCIE);
#endif
#ifdef CONFIG_USB_HCI
	if (IS_HARDWARE_TYPE_8192EU(Adapter))
		TmpU2B |= (FEN_USBA | FEN_USBD);
#endif

	TmpU2B |= (FEN_EN_25_1 | FEN_BB_GLB_RSTn | FEN_BBRSTB);

	rtw_write16(Adapter, REG_SYS_FUNC_EN, TmpU2B);

	/* 6. 0x1f[7:0] = 0x07 PathA RF Power On */
	rtw_write8(Adapter, REG_RF_CTRL, RF_EN | RF_RSTB | RF_SDMRSTB);

	/* rtw_write8(Adapter, REG_AFE_XTAL_CTRL+1, 0x80); */
	/*  */
	/* Config BB and AGC */
	/*  */
	rtStatus = phy_BB8192E_Config_ParaFile(Adapter);

	hal_set_crystal_cap(Adapter, pHalData->crystal_cap);

#if 1
	/* write 0x24= 000f81fb ,suggest by Ed		 */
	rtw_write32(Adapter, REG_AFE_CTRL1_8192E, 0x000f81fb);
#endif

	return rtStatus;

}

int
PHY_RFConfig8192E(
	IN	PADAPTER	Adapter
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	int		rtStatus = _SUCCESS;

	if (RTW_CANNOT_RUN(Adapter))
		return _FAIL;

	switch (pHalData->rf_chip) {
	case RF_6052:
		rtStatus = PHY_RF6052_Config_8192E(Adapter);
		break;

	case RF_PSEUDO_11N:
		break;
	default: /* for MacOs Warning: "RF_TYPE_MIN" not handled in switch */
		break;
	}
	/* <20121002, Kordan> Do LCK, because the PHY reg files make no effect. (Asked by Edlu)
	 * Only Test chip need set 0xb1= 0x55418, (Edlu) */
	/* phy_set_rf_reg(Adapter, RF_PATH_A, RF_LDO, bRFRegOffsetMask, 0x55418); */
	/* phy_set_rf_reg(Adapter, RF_PATH_B, RF_LDO, bRFRegOffsetMask, 0x55418);	 */

	return rtStatus;
}

VOID
PHY_GetTxPowerLevel8192E(
	IN	PADAPTER		Adapter,
	OUT s32		*powerlevel
)
{
#if 0
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PMGNT_INFO		pMgntInfo = &(Adapter->MgntInfo);
	s4Byte			TxPwrDbm = 13;

	if (pMgntInfo->ClientConfigPwrInDbm != UNSPECIFIED_PWR_DBM)
		*powerlevel = pMgntInfo->ClientConfigPwrInDbm;
	else
		*powerlevel = TxPwrDbm;
#endif
}

VOID
PHY_SetTxPowerIndex_8192E(
	IN	PADAPTER			Adapter,
	IN	u32					PowerIndex,
	IN	enum rf_path			RFPath,
	IN	u8					Rate
)
{
	if (RFPath == RF_PATH_A) {
		switch (Rate) {
		case MGN_1M:
			phy_set_bb_reg(Adapter, rTxAGC_A_CCK1_Mcs32,      bMaskByte1, PowerIndex);
			break;
		case MGN_2M:
			phy_set_bb_reg(Adapter, rTxAGC_B_CCK11_A_CCK2_11, bMaskByte1, PowerIndex);
			break;
		case MGN_5_5M:
			phy_set_bb_reg(Adapter, rTxAGC_B_CCK11_A_CCK2_11, bMaskByte2, PowerIndex);
			break;
		case MGN_11M:
			phy_set_bb_reg(Adapter, rTxAGC_B_CCK11_A_CCK2_11, bMaskByte3, PowerIndex);
			break;

		case MGN_6M:
			phy_set_bb_reg(Adapter, rTxAGC_A_Rate18_06, bMaskByte0, PowerIndex);
			break;
		case MGN_9M:
			phy_set_bb_reg(Adapter, rTxAGC_A_Rate18_06, bMaskByte1, PowerIndex);
			break;
		case MGN_12M:
			phy_set_bb_reg(Adapter, rTxAGC_A_Rate18_06, bMaskByte2, PowerIndex);
			break;
		case MGN_18M:
			phy_set_bb_reg(Adapter, rTxAGC_A_Rate18_06, bMaskByte3, PowerIndex);
			break;

		case MGN_24M:
			phy_set_bb_reg(Adapter, rTxAGC_A_Rate54_24, bMaskByte0, PowerIndex);
			break;
		case MGN_36M:
			phy_set_bb_reg(Adapter, rTxAGC_A_Rate54_24, bMaskByte1, PowerIndex);
			break;
		case MGN_48M:
			phy_set_bb_reg(Adapter, rTxAGC_A_Rate54_24, bMaskByte2, PowerIndex);
			break;
		case MGN_54M:
			phy_set_bb_reg(Adapter, rTxAGC_A_Rate54_24, bMaskByte3, PowerIndex);
			break;

		case MGN_MCS0:
			phy_set_bb_reg(Adapter, rTxAGC_A_Mcs03_Mcs00, bMaskByte0, PowerIndex);
			break;
		case MGN_MCS1:
			phy_set_bb_reg(Adapter, rTxAGC_A_Mcs03_Mcs00, bMaskByte1, PowerIndex);
			break;
		case MGN_MCS2:
			phy_set_bb_reg(Adapter, rTxAGC_A_Mcs03_Mcs00, bMaskByte2, PowerIndex);
			break;
		case MGN_MCS3:
			phy_set_bb_reg(Adapter, rTxAGC_A_Mcs03_Mcs00, bMaskByte3, PowerIndex);
			break;

		case MGN_MCS4:
			phy_set_bb_reg(Adapter, rTxAGC_A_Mcs07_Mcs04, bMaskByte0, PowerIndex);
			break;
		case MGN_MCS5:
			phy_set_bb_reg(Adapter, rTxAGC_A_Mcs07_Mcs04, bMaskByte1, PowerIndex);
			break;
		case MGN_MCS6:
			phy_set_bb_reg(Adapter, rTxAGC_A_Mcs07_Mcs04, bMaskByte2, PowerIndex);
			break;
		case MGN_MCS7:
			phy_set_bb_reg(Adapter, rTxAGC_A_Mcs07_Mcs04, bMaskByte3, PowerIndex);
			break;

		case MGN_MCS8:
			phy_set_bb_reg(Adapter, rTxAGC_A_Mcs11_Mcs08, bMaskByte0, PowerIndex);
			break;
		case MGN_MCS9:
			phy_set_bb_reg(Adapter, rTxAGC_A_Mcs11_Mcs08, bMaskByte1, PowerIndex);
			break;
		case MGN_MCS10:
			phy_set_bb_reg(Adapter, rTxAGC_A_Mcs11_Mcs08, bMaskByte2, PowerIndex);
			break;
		case MGN_MCS11:
			phy_set_bb_reg(Adapter, rTxAGC_A_Mcs11_Mcs08, bMaskByte3, PowerIndex);
			break;

		case MGN_MCS12:
			phy_set_bb_reg(Adapter, rTxAGC_A_Mcs15_Mcs12, bMaskByte0, PowerIndex);
			break;
		case MGN_MCS13:
			phy_set_bb_reg(Adapter, rTxAGC_A_Mcs15_Mcs12, bMaskByte1, PowerIndex);
			break;
		case MGN_MCS14:
			phy_set_bb_reg(Adapter, rTxAGC_A_Mcs15_Mcs12, bMaskByte2, PowerIndex);
			break;
		case MGN_MCS15:
			phy_set_bb_reg(Adapter, rTxAGC_A_Mcs15_Mcs12, bMaskByte3, PowerIndex);
			break;

		default:
			RTW_INFO("Invalid Rate!!\n");
			break;
		}
	} else if (RFPath == RF_PATH_B) {
		switch (Rate) {
		case MGN_1M:
			phy_set_bb_reg(Adapter, rTxAGC_B_CCK1_55_Mcs32, bMaskByte1, PowerIndex);
			break;
		case MGN_2M:
			phy_set_bb_reg(Adapter, rTxAGC_B_CCK1_55_Mcs32, bMaskByte2, PowerIndex);
			break;
		case MGN_5_5M:
			phy_set_bb_reg(Adapter, rTxAGC_B_CCK1_55_Mcs32, bMaskByte3, PowerIndex);
			break;
		case MGN_11M:
			phy_set_bb_reg(Adapter, rTxAGC_B_CCK11_A_CCK2_11, bMaskByte0, PowerIndex);
			break;

		case MGN_6M:
			phy_set_bb_reg(Adapter, rTxAGC_B_Rate18_06, bMaskByte0, PowerIndex);
			break;
		case MGN_9M:
			phy_set_bb_reg(Adapter, rTxAGC_B_Rate18_06, bMaskByte1, PowerIndex);
			break;
		case MGN_12M:
			phy_set_bb_reg(Adapter, rTxAGC_B_Rate18_06, bMaskByte2, PowerIndex);
			break;
		case MGN_18M:
			phy_set_bb_reg(Adapter, rTxAGC_B_Rate18_06, bMaskByte3, PowerIndex);
			break;

		case MGN_24M:
			phy_set_bb_reg(Adapter, rTxAGC_B_Rate54_24, bMaskByte0, PowerIndex);
			break;
		case MGN_36M:
			phy_set_bb_reg(Adapter, rTxAGC_B_Rate54_24, bMaskByte1, PowerIndex);
			break;
		case MGN_48M:
			phy_set_bb_reg(Adapter, rTxAGC_B_Rate54_24, bMaskByte2, PowerIndex);
			break;
		case MGN_54M:
			phy_set_bb_reg(Adapter, rTxAGC_B_Rate54_24, bMaskByte3, PowerIndex);
			break;

		case MGN_MCS0:
			phy_set_bb_reg(Adapter, rTxAGC_B_Mcs03_Mcs00, bMaskByte0, PowerIndex);
			break;
		case MGN_MCS1:
			phy_set_bb_reg(Adapter, rTxAGC_B_Mcs03_Mcs00, bMaskByte1, PowerIndex);
			break;
		case MGN_MCS2:
			phy_set_bb_reg(Adapter, rTxAGC_B_Mcs03_Mcs00, bMaskByte2, PowerIndex);
			break;
		case MGN_MCS3:
			phy_set_bb_reg(Adapter, rTxAGC_B_Mcs03_Mcs00, bMaskByte3, PowerIndex);
			break;

		case MGN_MCS4:
			phy_set_bb_reg(Adapter, rTxAGC_B_Mcs07_Mcs04, bMaskByte0, PowerIndex);
			break;
		case MGN_MCS5:
			phy_set_bb_reg(Adapter, rTxAGC_B_Mcs07_Mcs04, bMaskByte1, PowerIndex);
			break;
		case MGN_MCS6:
			phy_set_bb_reg(Adapter, rTxAGC_B_Mcs07_Mcs04, bMaskByte2, PowerIndex);
			break;
		case MGN_MCS7:
			phy_set_bb_reg(Adapter, rTxAGC_B_Mcs07_Mcs04, bMaskByte3, PowerIndex);
			break;

		case MGN_MCS8:
			phy_set_bb_reg(Adapter, rTxAGC_B_Mcs11_Mcs08, bMaskByte0, PowerIndex);
			break;
		case MGN_MCS9:
			phy_set_bb_reg(Adapter, rTxAGC_B_Mcs11_Mcs08, bMaskByte1, PowerIndex);
			break;
		case MGN_MCS10:
			phy_set_bb_reg(Adapter, rTxAGC_B_Mcs11_Mcs08, bMaskByte2, PowerIndex);
			break;
		case MGN_MCS11:
			phy_set_bb_reg(Adapter, rTxAGC_B_Mcs11_Mcs08, bMaskByte3, PowerIndex);
			break;

		case MGN_MCS12:
			phy_set_bb_reg(Adapter, rTxAGC_B_Mcs15_Mcs12, bMaskByte0, PowerIndex);
			break;
		case MGN_MCS13:
			phy_set_bb_reg(Adapter, rTxAGC_B_Mcs15_Mcs12, bMaskByte1, PowerIndex);
			break;
		case MGN_MCS14:
			phy_set_bb_reg(Adapter, rTxAGC_B_Mcs15_Mcs12, bMaskByte2, PowerIndex);
			break;
		case MGN_MCS15:
			phy_set_bb_reg(Adapter, rTxAGC_B_Mcs15_Mcs12, bMaskByte3, PowerIndex);
			break;

		default:
			RTW_INFO("Invalid Rate!!\n");
			break;
		}
	} else
		RTW_INFO("Invalid RFPath!!\n");
}


u8
PHY_GetTxPowerIndex_8192E(
	IN	PADAPTER			pAdapter,
	IN	enum rf_path			RFPath,
	IN	u8					Rate,
	IN	u8					BandWidth,
	IN	u8					Channel,
	struct txpwr_idx_comp *tic
)
{
	PHAL_DATA_TYPE pHalData = GET_HAL_DATA(pAdapter);
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(pAdapter);
	s16 power_idx;
	u8 base_idx = 0;
	s8 by_rate_diff = 0, limit = 0, tpt_offset = 0, extra_bias = 0;
	u8 ntx_idx = phy_get_current_tx_num(pAdapter, Rate);
	BOOLEAN bIn24G = _FALSE;

	base_idx = PHY_GetTxPowerIndexBase(pAdapter, RFPath, Rate, ntx_idx, BandWidth, Channel, &bIn24G);

	by_rate_diff = PHY_GetTxPowerByRate(pAdapter, BAND_ON_2_4G, RFPath, Rate);
	limit = PHY_GetTxPowerLimit(pAdapter, NULL, (u8)(!bIn24G), pHalData->current_channel_bw, RFPath, Rate, ntx_idx, pHalData->current_channel);

	tpt_offset =  PHY_GetTxPowerTrackingOffset(pAdapter, RFPath, Rate);

	if (tic) {
		tic->ntx_idx = ntx_idx;
		tic->base = base_idx;
		tic->by_rate = by_rate_diff;
		tic->limit = limit;
		tic->tpt = tpt_offset;
		tic->ebias = extra_bias;
	}

	by_rate_diff = by_rate_diff > limit ? limit : by_rate_diff;
	power_idx = base_idx + by_rate_diff + tpt_offset + extra_bias;

	if (power_idx < 0)
		power_idx = 0;
	else if (power_idx > hal_spec->txgi_max)
		power_idx = hal_spec->txgi_max;

	return power_idx;
}

VOID
PHY_SetTxPowerLevel8192E(
	IN	PADAPTER		Adapter,
	IN	u8				Channel
)
{

	PHAL_DATA_TYPE	pHalData = GET_HAL_DATA(Adapter);
	u8			path = 0;

	/* RTW_INFO("==>PHY_SetTxPowerLevel8192E()\n"); */

	for (path = RF_PATH_A; path < pHalData->NumTotalRFPath; ++path)
		phy_set_tx_power_level_by_path(Adapter, Channel, path);

	/* RTW_INFO("<==PHY_SetTxPowerLevel8192E()\n"); */
}

u8
phy_GetSecondaryChnl_8192E(
	IN	PADAPTER	Adapter
)
{
	u8					SCSettingOf40 = 0, SCSettingOf20 = 0;
	PHAL_DATA_TYPE		pHalData = GET_HAL_DATA(Adapter);

	/* RTW_INFO("SCMapping: VHT Case: pHalData->current_channel_bw %d, pHalData->nCur80MhzPrimeSC %d, pHalData->nCur40MhzPrimeSC %d\n",pHalData->current_channel_bw,pHalData->nCur80MhzPrimeSC,pHalData->nCur40MhzPrimeSC); */
	if (pHalData->current_channel_bw == CHANNEL_WIDTH_80) {
		if (pHalData->nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER)
			SCSettingOf40 = VHT_DATA_SC_40_LOWER_OF_80MHZ;
		else if (pHalData->nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER)
			SCSettingOf40 = VHT_DATA_SC_40_UPPER_OF_80MHZ;
		else
			RTW_INFO("%s- current_channel_bw:%d, SCMapping: DONOT CARE Mode Setting\n", __func__, pHalData->current_channel_bw);

		if ((pHalData->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER) && (pHalData->nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER))
			SCSettingOf20 = VHT_DATA_SC_20_LOWEST_OF_80MHZ;
		else if ((pHalData->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER) && (pHalData->nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER))
			SCSettingOf20 = VHT_DATA_SC_20_LOWER_OF_80MHZ;
		else if ((pHalData->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER) && (pHalData->nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER))
			SCSettingOf20 = VHT_DATA_SC_20_UPPER_OF_80MHZ;
		else if ((pHalData->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER) && (pHalData->nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER))
			SCSettingOf20 = VHT_DATA_SC_20_UPPERST_OF_80MHZ;
		else
			RTW_INFO("%s- current_channel_bw:%d, SCMapping: DONOT CARE Mode Setting\n", __func__, pHalData->current_channel_bw);
	} else if (pHalData->current_channel_bw == CHANNEL_WIDTH_40) {
		/* RTW_INFO("SCMapping: VHT Case: pHalData->current_channel_bw %d, pHalData->nCur40MhzPrimeSC %d\n",pHalData->current_channel_bw,pHalData->nCur40MhzPrimeSC); */

		if (pHalData->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER)
			SCSettingOf20 = VHT_DATA_SC_20_UPPER_OF_80MHZ;
		else if (pHalData->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER)
			SCSettingOf20 = VHT_DATA_SC_20_LOWER_OF_80MHZ;
		else if (pHalData->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_DONT_CARE)
			RTW_INFO("%s- current_channel_bw:%d, PRIME_CHNL_OFFSET_DONT_CARE\n", __func__, pHalData->current_channel_bw);
		else
			RTW_INFO("%s- current_channel_bw:%d, SCMapping: DONOT CARE Mode Setting\n", __func__, pHalData->current_channel_bw);
	}

	/*RTW_INFO("SCMapping: SC Value %x\n", ((SCSettingOf40 << 4) | SCSettingOf20));*/
	return (SCSettingOf40 << 4) | SCSettingOf20;
}

VOID
phy_SetRegBW_8192E(
	IN	PADAPTER		Adapter,
	enum channel_width	CurrentBW
)
{
	u16	RegRfMod_BW, u2tmp = 0;
	RegRfMod_BW = rtw_read16(Adapter, REG_TRXPTCL_CTL_8192E);

	switch (CurrentBW) {
	case CHANNEL_WIDTH_20:
		rtw_write16(Adapter, REG_TRXPTCL_CTL_8192E, (RegRfMod_BW & 0xFE7F)); /* BIT 7 = 0, BIT 8 = 0 */
		break;

	case CHANNEL_WIDTH_40:
		u2tmp = RegRfMod_BW | BIT7;
		rtw_write16(Adapter, REG_TRXPTCL_CTL_8192E, (u2tmp & 0xFEFF)); /* BIT 7 = 1, BIT 8 = 0 */
		break;

	case CHANNEL_WIDTH_80:
		u2tmp = RegRfMod_BW | BIT8;
		rtw_write16(Adapter, REG_TRXPTCL_CTL_8192E, (u2tmp & 0xFF7F)); /* BIT 7 = 0, BIT 8 = 1 */
		break;

	default:
		RTW_INFO("phy_PostSetBWMode8812():	unknown Bandwidth: %#X\n", CurrentBW);
		break;
	}

}


VOID
phy_PostSetBwMode8192E(
	IN	PADAPTER	Adapter
)
{
	u1Byte			SubChnlNum = 0;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);


	/* 3 Set Reg668 Reg440 BW */
	phy_SetRegBW_8192E(Adapter, pHalData->current_channel_bw);

	/* 3 Set Reg483 */
	SubChnlNum = phy_GetSecondaryChnl_8192E(Adapter);
	rtw_write8(Adapter, REG_DATA_SC_8192E, SubChnlNum);

	switch (pHalData->current_channel_bw) {
	case CHANNEL_WIDTH_20:
		phy_set_bb_reg(Adapter, rFPGA0_RFMOD, BIT0, 0x0);
		phy_set_bb_reg(Adapter, rFPGA1_RFMOD, BIT0, 0x0);
		phy_set_rf_reg(Adapter, RF_PATH_A, RF_CHNLBW, BIT11 | BIT10, 0x3);
		phy_set_rf_reg(Adapter, RF_PATH_B, RF_CHNLBW, BIT11 | BIT10, 0x3);

		/* phy_set_bb_reg(Adapter, rFPGA0_AnalogParameter2, BIT10, 1);			 */
		phy_set_bb_reg(Adapter, rOFDM0_TxPseudoNoiseWgt, (BIT31 | BIT30), 0x0);

		break;

	case CHANNEL_WIDTH_40:
		phy_set_bb_reg(Adapter, rFPGA0_RFMOD, BIT0, 0x1);
		phy_set_bb_reg(Adapter, rFPGA1_RFMOD, BIT0, 0x1);
		phy_set_rf_reg(Adapter, RF_PATH_A, RF_CHNLBW, BIT11 | BIT10, 0x1);
		phy_set_rf_reg(Adapter, RF_PATH_B, RF_CHNLBW, BIT11 | BIT10, 0x1);

		/* Set Control channel to upper or lower. These settings are required only for 40MHz */
		phy_set_bb_reg(Adapter, rCCK0_System, bCCKSideBand, (pHalData->nCur40MhzPrimeSC >> 1));

		phy_set_bb_reg(Adapter, rOFDM1_LSTF, 0xC00, pHalData->nCur40MhzPrimeSC);

		/*			phy_set_bb_reg(Adapter, rFPGA0_AnalogParameter2, BIT10, 0); */

		phy_set_bb_reg(Adapter, 0x818, (BIT26 | BIT27), (pHalData->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER) ? 2 : 1);
		break;

	default:
		/* RT_DISP(FPHY, PHY_BBW, ("phy_PostSetBWMode8192E():	unknown Bandwidth: %#X\n",pHalData->current_channel_bw)); */
		break;
	}
}

/* <20130320, VincentLan> A workaround to eliminate the 2480MHz spur for 92E */
void
phy_SpurCalibration_8192E(
	IN	PADAPTER			Adapter,
	IN	enum spur_cal_method	Method
)
{
	u32			reg0x18 = 0;
	u8			retryNum = 0;
	u8			MaxRetryCount = 8;
	u8			Pass_A = _FALSE, Pass_B = _FALSE;
	u8			SpurOccur = _FALSE;
	u32			PSDReport = 0;
	u32			Best_PSD_PathA = 999;
	u32			Best_Phase_PathA = 0;


	if (Method == PLL_RESET) {
		MaxRetryCount = 3;
		RTW_INFO("%s =>PLL_RESET\n", __FUNCTION__);
	} else if (Method == AFE_PHASE_SEL) {
		rtw_write8(Adapter, RF_TX_G1, rtw_read8(Adapter, RF_TX_G1) | BIT4); /* enable 0x20[4] */
		RTW_INFO("%s =>AFE_PHASE_SEL\n", __FUNCTION__);
	}


	/* Backup current channel */
	reg0x18 = phy_query_rf_reg(Adapter, RF_PATH_A, RF_CHNLBW, bRFRegOffsetMask);


	while (retryNum++ < MaxRetryCount) {
		phy_set_rf_reg(Adapter, RF_PATH_A, RF_CHNLBW, bRFRegOffsetMask, 0x7C0D); /* CH13 */
		phy_set_bb_reg(Adapter, rOFDM0_XAAGCCore1, bMaskByte0, 0x30); /* Path A initial gain */
		phy_set_bb_reg(Adapter, rOFDM0_XBAGCCore1, bMaskByte0, 0x30); /* Path B initial gain */
		phy_set_bb_reg(Adapter, rFPGA0_AnalogParameter4, bMaskDWord, 0xccf000c0);	/* disable 3-wire */

		/* Path A */
		phy_set_bb_reg(Adapter, rFPGA0_TxInfo, bMaskByte0, 0x3);
		phy_set_bb_reg(Adapter, rFPGA0_PSDFunction, bMaskDWord, 0xfccd);
		phy_set_bb_reg(Adapter, rFPGA0_PSDFunction, bMaskDWord, 0x40fccd);
		/* rtw_msleep_os(30); */
		rtw_mdelay_os(30);
		PSDReport = phy_query_bb_reg(Adapter, rFPGA0_PSDReport, bMaskDWord);
		/* RTW_INFO(" Path A== PSDReport = 0x%x (%d)\n",PSDReport,PSDReport); */
		if (PSDReport < 0x16)
			Pass_A = _TRUE;
		if (PSDReport < Best_PSD_PathA) {
			Best_PSD_PathA = PSDReport;
			Best_Phase_PathA = rtw_read8(Adapter, RF_TX_G1) >> 5;
		}

		/* Path B */
		phy_set_bb_reg(Adapter, rFPGA0_TxInfo, bMaskByte0, 0x13);
		phy_set_bb_reg(Adapter, rFPGA0_PSDFunction, bMaskDWord, 0xfccd);
		phy_set_bb_reg(Adapter, rFPGA0_PSDFunction, bMaskDWord, 0x40fccd);
		/* rtw_msleep_os(30); */
		rtw_mdelay_os(30);
		PSDReport = phy_query_bb_reg(Adapter, rFPGA0_PSDReport, bMaskDWord);
		/* RTW_INFO(" Path B== PSDReport = 0x%x (%d)\n",PSDReport,PSDReport); */
		if (PSDReport < 0x16)
			Pass_B = _TRUE;

		if (Pass_A && Pass_B) {
			RTW_INFO("=== PathA=%d, PathB=%d\n", Pass_A, Pass_B);
			RTW_INFO("===FixSpur Pass!\n");
			phy_set_bb_reg(Adapter, rFPGA0_AnalogParameter4, bMaskDWord, 0xcc0000c0);	/* enable 3-wire */
			phy_set_bb_reg(Adapter, rFPGA0_PSDFunction, bMaskDWord, 0xfc00);
			phy_set_bb_reg(Adapter, rOFDM0_XAAGCCore1, bMaskByte0, 0x20);
			phy_set_bb_reg(Adapter, rOFDM0_XBAGCCore1, bMaskByte0, 0x20);
			break;
		} else {
			Pass_A = _FALSE;
			Pass_B = _FALSE;
			if (Method == PLL_RESET) {
				/* phy_set_mac_reg(Adapter, 0x28, bMaskByte1, 0x7);	 */ /* PLL gated 320M CLK disable */
				/* phy_set_mac_reg(Adapter, 0x28, bMaskByte1, 0x47);	 */ /* PLL gated 320M CLK enable */
				rtw_write8(Adapter, 0x29, 0x7);	/* PLL gated 320M CLK disable */
				rtw_write8(Adapter, 0x29, 0x47);	/* PLL gated 320M CLK enable */
			} else if (Method == AFE_PHASE_SEL) {
				if (!SpurOccur) {
					SpurOccur = _TRUE;
					RTW_INFO("===FixSpur NOT Pass!\n");
					/* phy_set_mac_reg(Adapter, RF_TX_G1, BIT4, 0x1); */
					/* phy_set_mac_reg(Adapter, 0x28, bMaskByte0, 0x80); */
					/* phy_set_mac_reg(Adapter, 0x28, bMaskByte0, 0x83); */
					rtw_write8(Adapter, RF_TX_G1, rtw_read8(Adapter, RF_TX_G1) | BIT4); /* enable 0x20[4] */
					rtw_write8(Adapter, 0x28, 0x80);
					rtw_write8(Adapter, 0x28, 0x83);

				}
				/* RTW_INFO("===Round %d\n", retryNum+1); */
				if (retryNum < 7)
					/* phy_set_mac_reg(Adapter, RF_TX_G1, BIT5|BIT6|BIT7, 1+retryNum); */
					rtw_write8(Adapter, RF_TX_G1, (rtw_read8(Adapter, RF_TX_G1) & 0x1F) | ((1 + retryNum) << 5));
				else
					break;
			}
		}
	}

	if (Pass_A && Pass_B)
		;
	/* 0x20 Selection Focus on Path A PSD Result */
	else if (Method == AFE_PHASE_SEL) {
		if (Best_Phase_PathA < 8)
			/* phy_set_mac_reg(Adapter, RF_TX_G1, BIT5|BIT6|BIT7, Best_Phase_PathA); */
			rtw_write8(Adapter, RF_TX_G1, (rtw_read8(Adapter, RF_TX_G1) & 0x1F) | (Best_Phase_PathA << 5));
		else
			/* phy_set_mac_reg(Adapter, RF_TX_G1, BIT5|BIT6|BIT7, 0); */
			rtw_write8(Adapter, RF_TX_G1, (rtw_read8(Adapter, RF_TX_G1) & 0x1F));
	}
	/* Restore the settings */
	phy_set_bb_reg(Adapter, rFPGA0_AnalogParameter4, bMaskDWord, 0xcc0000c0);	/* enable 3-wire */
	phy_set_bb_reg(Adapter, rFPGA0_PSDFunction, bMaskDWord, 0xfccd); /* reset PSD */
	phy_set_bb_reg(Adapter, rOFDM0_XAAGCCore1, bMaskByte0, 0x20);
	phy_set_bb_reg(Adapter, rOFDM0_XBAGCCore1, bMaskByte0, 0x20);

	phy_set_rf_reg(Adapter, RF_PATH_A, RF_CHNLBW, bRFRegOffsetMask, reg0x18); /* restore chnl */

}

void PHY_SpurCalibration_8192E(IN PADAPTER Adapter)
{
	if (rtw_read32(Adapter, REG_SYS_CFG1_8192E) & BIT_SPSLDO_SEL) {
		/* LDO */
		phy_SpurCalibration_8192E(Adapter, PLL_RESET);
	} else {
		/* SPS - 4OM */
		phy_SpurCalibration_8192E(Adapter, AFE_PHASE_SEL);
		/* todo SPS-25M -check */
	}
}


#ifdef CONFIG_SPUR_CAL_NBI
/* to eliminate the 2480MHz spur for 92E suggest by James */
void
phy_SpurCalibration_8192E_NBI(PADAPTER Adapter)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	/* DbgPrint("===> %s  current_channel_bw = %d, current_channel = %d\n", __FUNCTION__,pHalData->current_channel_bw, pHalData->current_channel); */
	if (pHalData->current_channel_bw == CHANNEL_WIDTH_20 && (pHalData->current_channel == 13 || pHalData->current_channel == 14)) {
		phy_set_bb_reg(Adapter, rOFDM0_RxDSP, BIT(9), 0x1);                     /* enable notch filter */
		phy_set_bb_reg(Adapter, rOFDM1_IntfDet, BIT(8) | BIT(7) | BIT(6), 0x5);	/* intf_TH */
	} else if (pHalData->current_channel_bw == CHANNEL_WIDTH_40 && pHalData->current_channel == 11) {
		phy_set_bb_reg(Adapter, rOFDM0_RxDSP, BIT(9), 0x1);                     /* enable notch filter */
		phy_set_bb_reg(Adapter, rOFDM1_IntfDet, BIT(8) | BIT(7) | BIT(6), 0x5);	/* intf_TH */
	} else {
		if (Adapter->registrypriv.notch_filter == 0)
			phy_set_bb_reg(Adapter, rOFDM0_RxDSP, BIT(9), 0x0);	/* disable notch filter */
	}
}
#endif
VOID
phy_SwChnl8192E(
	IN	PADAPTER	pAdapter
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	u8				channelToSW = pHalData->current_channel;

	if (pHalData->rf_chip == RF_PSEUDO_11N) {
		return;
	}
	/* pHalData->RfRegChnlVal[0] = ((pHalData->RfRegChnlVal[0] & 0xfffff00) | channelToSW  ); */
	phy_set_rf_reg(pAdapter, RF_PATH_A, RF_CHNLBW, 0x3FF, channelToSW);
	phy_set_rf_reg(pAdapter, RF_PATH_B, RF_CHNLBW, 0x3FF, channelToSW);

}

VOID
phy_SwChnlAndSetBwMode8192E(
	IN  PADAPTER		Adapter
)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);

	/* RTW_INFO("phy_SwChnlAndSetBwMode8192E(): bSwChnl %d, bSetChnlBW %d\n", pHalData->bSwChnl, pHalData->bSetChnlBW); */
	if (Adapter->bNotifyChannelChange) {
		RTW_INFO("[%s] bSwChnl=%d, ch=%d, bSetChnlBW=%d, bw=%d\n",
			 __FUNCTION__,
			 pHalData->bSwChnl,
			 pHalData->current_channel,
			 pHalData->bSetChnlBW,
			 pHalData->current_channel_bw);

#ifdef CONFIG_TDLS
#ifdef CONFIG_TDLS_CH_SW
		if (Adapter->tdlsinfo.chsw_info.dump_stack == _TRUE)
			dump_stack();
#endif
#endif /* CONFIG_TDLS */
	}

	if (RTW_CANNOT_RUN(Adapter))
		return;

	if (pHalData->bSwChnl) {
		phy_SwChnl8192E(Adapter);
		pHalData->bSwChnl = _FALSE;
	}

	if (pHalData->bSetChnlBW) {
		phy_PostSetBwMode8192E(Adapter);
		pHalData->bSetChnlBW = _FALSE;
	}

	if (pHalData->bNeedIQK == _TRUE) {
		if (pHalData->neediqk_24g == _TRUE) {

			halrf_iqk_trigger(&pHalData->odmpriv, _FALSE);
			pHalData->bIQKInitialized = _TRUE;
			pHalData->neediqk_24g = _FALSE;
		}
		pHalData->bNeedIQK = _FALSE;
	}
#ifdef CONFIG_SPUR_CAL_NBI
	phy_SpurCalibration_8192E_NBI(Adapter);
#endif

#ifdef CONFIG_TDLS
#ifdef CONFIG_TDLS_CH_SW
	/* It takes too much time of setting tx power, influence channel switch */
	if ((ATOMIC_READ(&Adapter->tdlsinfo.chsw_info.chsw_on) == _FALSE))
#endif
#endif /* CONFIG_TDLS */
		PHY_SetTxPowerLevel8192E(Adapter, pHalData->current_channel);
}

VOID
PHY_HandleSwChnlAndSetBW8192E(
	IN	PADAPTER			Adapter,
	IN	BOOLEAN				bSwitchChannel,
	IN	BOOLEAN				bSetBandWidth,
	IN	u8					ChannelNum,
	IN	enum channel_width	ChnlWidth,
	IN	EXTCHNL_OFFSET	ExtChnlOffsetOf40MHz,
	IN	EXTCHNL_OFFSET	ExtChnlOffsetOf80MHz,
	IN	u8					CenterFrequencyIndex1
)
{
	/* static BOOLEAN		bInitialzed = _FALSE; */
	PADAPTER			pDefAdapter =  GetDefaultAdapter(Adapter);
	PHAL_DATA_TYPE		pHalData = GET_HAL_DATA(pDefAdapter);
	u8					tmpChannel = pHalData->current_channel;
	enum channel_width	tmpBW = pHalData->current_channel_bw;
	u8					tmpnCur40MhzPrimeSC = pHalData->nCur40MhzPrimeSC;
	u8					tmpnCur80MhzPrimeSC = pHalData->nCur80MhzPrimeSC;
	u8					tmpCenterFrequencyIndex1 = pHalData->CurrentCenterFrequencyIndex1;
	struct mlme_ext_priv	*pmlmeext = &Adapter->mlmeextpriv;

	/* RTW_INFO("=> PHY_HandleSwChnlAndSetBW8812: bSwitchChannel %d, bSetBandWidth %d\n",bSwitchChannel,bSetBandWidth); */

	/* check is swchnl or setbw */
	if (!bSwitchChannel && !bSetBandWidth) {
		RTW_INFO("PHY_HandleSwChnlAndSetBW8192e:  not switch channel and not set bandwidth\n");
		return;
	}

	/* skip change for channel or bandwidth is the same */
	if (bSwitchChannel) {
		if (pHalData->current_channel != ChannelNum) {
			if (HAL_IsLegalChannel(Adapter, ChannelNum))
				pHalData->bSwChnl = _TRUE;
		}
	}

	if (bSetBandWidth) {
#if 0
		if (bInitialzed == _FALSE) {
			bInitialzed = _TRUE;
			pHalData->bSetChnlBW = _TRUE;
		} else if ((pHalData->current_channel_bw != ChnlWidth) || (pHalData->nCur40MhzPrimeSC != ExtChnlOffsetOf40MHz) || (pHalData->CurrentCenterFrequencyIndex1 != CenterFrequencyIndex1))
			pHalData->bSetChnlBW = _TRUE;
#else
		pHalData->bSetChnlBW = _TRUE;
#endif
	}

	if (!pHalData->bSetChnlBW && !pHalData->bSwChnl) {
		/* RTW_INFO("<= PHY_HandleSwChnlAndSetBW8812: bSwChnl %d, bSetChnlBW %d\n",pHalData->bSwChnl,pHalData->bSetChnlBW); */
		return;
	}


	if (pHalData->bSwChnl) {
		pHalData->current_channel = ChannelNum;
		pHalData->CurrentCenterFrequencyIndex1 = ChannelNum;
	}


	if (pHalData->bSetChnlBW) {
		pHalData->current_channel_bw = ChnlWidth;
#if 0
		if (ExtChnlOffsetOf40MHz == EXTCHNL_OFFSET_LOWER)
			pHalData->nCur40MhzPrimeSC = HAL_PRIME_CHNL_OFFSET_UPPER;
		else if (ExtChnlOffsetOf40MHz == EXTCHNL_OFFSET_UPPER)
			pHalData->nCur40MhzPrimeSC = HAL_PRIME_CHNL_OFFSET_LOWER;
		else
			pHalData->nCur40MhzPrimeSC = HAL_PRIME_CHNL_OFFSET_DONT_CARE;

		if (ExtChnlOffsetOf80MHz == EXTCHNL_OFFSET_LOWER)
			pHalData->nCur80MhzPrimeSC = HAL_PRIME_CHNL_OFFSET_UPPER;
		else if (ExtChnlOffsetOf80MHz == EXTCHNL_OFFSET_UPPER)
			pHalData->nCur80MhzPrimeSC = HAL_PRIME_CHNL_OFFSET_LOWER;
		else
			pHalData->nCur80MhzPrimeSC = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
#else
		pHalData->nCur40MhzPrimeSC = ExtChnlOffsetOf40MHz;
		pHalData->nCur80MhzPrimeSC = ExtChnlOffsetOf80MHz;
#endif

		pHalData->CurrentCenterFrequencyIndex1 = CenterFrequencyIndex1;
	}

	/* Switch workitem or set timer to do switch channel or setbandwidth operation */
	if (!RTW_CANNOT_RUN(Adapter))
		phy_SwChnlAndSetBwMode8192E(Adapter);
	else {
		if (pHalData->bSwChnl) {
			pHalData->current_channel = tmpChannel;
			pHalData->CurrentCenterFrequencyIndex1 = tmpChannel;
		}
		if (pHalData->bSetChnlBW) {
			pHalData->current_channel_bw = tmpBW;
			pHalData->nCur40MhzPrimeSC = tmpnCur40MhzPrimeSC;
			pHalData->nCur80MhzPrimeSC = tmpnCur80MhzPrimeSC;
			pHalData->CurrentCenterFrequencyIndex1 = tmpCenterFrequencyIndex1;
		}
	}

	/* RTW_INFO("Channel %d ChannelBW %d ",pHalData->current_channel, pHalData->current_channel_bw); */
	/* RTW_INFO("40MhzPrimeSC %d 80MhzPrimeSC %d ",pHalData->nCur40MhzPrimeSC, pHalData->nCur80MhzPrimeSC); */
	/* RTW_INFO("CenterFrequencyIndex1 %d\n",pHalData->CurrentCenterFrequencyIndex1); */

	/* RTW_INFO("<= PHY_HandleSwChnlAndSetBW8812: bSwChnl %d, bSetChnlBW %d\n",pHalData->bSwChnl,pHalData->bSetChnlBW); */

}

VOID
PHY_SetSwChnlBWMode8192E(
	IN	PADAPTER			Adapter,
	IN	u8					channel,
	IN	enum channel_width	Bandwidth,
	IN	u8					Offset40,
	IN	u8					Offset80
)
{
	/* RTW_INFO("%s()===>\n",__FUNCTION__); */

	PHY_HandleSwChnlAndSetBW8192E(Adapter, _TRUE, _TRUE, channel, Bandwidth, Offset40, Offset80, channel);

	/* RTW_INFO("<==%s()\n",__FUNCTION__); */
}
VOID
PHY_SetRFEReg_8192E(
	IN PADAPTER		Adapter
)
{
	u8			u1tmp = 0;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	if ((pHalData->ExternalPA_2G == 0) && (pHalData->ExternalLNA_2G == 0))
		return;

	switch (pHalData->rfe_type) {
	case 0:
		phy_set_bb_reg(Adapter, BIT_REG_LED_CFG_8192E, bMaskByte2, 0x62);/* Reg[4E] = 0x62 */
		phy_set_bb_reg(Adapter, BIT_REG_LED_CFG_8192E, bMaskByte3, 0x0);/* Reg[4F] = 0x0 */
		phy_set_bb_reg(Adapter, 0x930, bMaskDWord, 0x00540000);
		phy_set_bb_reg(Adapter, 0x934, bMaskDWord, 0x0);
		phy_set_bb_reg(Adapter, 0x938, bMaskDWord, 0x00000540);
		phy_set_bb_reg(Adapter, 0x93C, bMaskDWord, 0x0);
		phy_set_bb_reg(Adapter, 0x940, bMaskDWord, 0x00000015);
		phy_set_bb_reg(Adapter, 0x944, bMaskDWord, 0x0000ffff);
		break;
	case 1:
		phy_set_bb_reg(Adapter, BIT_REG_LED_CFG_8192E, bMaskByte2, 0x62);/* Reg[4E] = 0x62 */
		phy_set_bb_reg(Adapter, BIT_REG_LED_CFG_8192E, bMaskByte3, 0x70);/* Reg[4F] = 0x70 */
		phy_set_bb_reg(Adapter, 0x930, bMaskDWord, 0x00005000);
		phy_set_bb_reg(Adapter, 0x934, bMaskDWord, 0x00004000);
		phy_set_bb_reg(Adapter, 0x938, bMaskDWord, 0x00000540);
		phy_set_bb_reg(Adapter, 0x93C, bMaskDWord, 0x0);
		phy_set_bb_reg(Adapter, 0x940, bMaskDWord, 0x00000015);
		phy_set_bb_reg(Adapter, 0x944, bMaskDWord, 0x0000083F);
		break;

	default:
		break;
	}

}
