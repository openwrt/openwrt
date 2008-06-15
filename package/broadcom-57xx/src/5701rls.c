/******************************************************************************/
/*                                                                            */
/* Broadcom BCM5700 Linux Network Driver, Copyright (c) 2000 - 2004 Broadcom  */
/* Corporation.                                                               */
/* All rights reserved.                                                       */
/*                                                                            */
/* This program is free software; you can redistribute it and/or modify       */
/* it under the terms of the GNU General Public License as published by       */
/* the Free Software Foundation, located in the file LICENSE.                 */
/*                                                                            */
/* History:                                                                   */
/*                                                                            */
/******************************************************************************/

#ifdef INCLUDE_5701_AX_FIX

#include "mm.h"
#include "5701rls.h"

LM_STATUS LM_LoadRlsFirmware(PLM_DEVICE_BLOCK pDevice)
{
  T3_FWIMG_INFO FwImgInfo;

  FwImgInfo.StartAddress = t3FwStartAddr;
  FwImgInfo.Text.Buffer = (PLM_UINT8)t3FwText;
  FwImgInfo.Text.Offset  = t3FwTextAddr;
  FwImgInfo.Text.Length  = t3FwTextLen;
  FwImgInfo.ROnlyData.Buffer = (PLM_UINT8)t3FwRodata;
  FwImgInfo.ROnlyData.Offset  = t3FwRodataAddr;
  FwImgInfo.ROnlyData.Length  = t3FwRodataLen;
  FwImgInfo.Data.Buffer = (PLM_UINT8)t3FwData;
  FwImgInfo.Data.Offset  = t3FwDataAddr;
  FwImgInfo.Data.Length  = t3FwDataLen;

  if (LM_LoadFirmware(pDevice,
                      &FwImgInfo,
                      T3_RX_CPU_ID | T3_TX_CPU_ID,
                      T3_RX_CPU_ID) != LM_STATUS_SUCCESS)
    {
      return LM_STATUS_FAILURE;
    }
  
  return LM_STATUS_SUCCESS;
}

#endif /* INCLUDE_5701_AX_FIX */
