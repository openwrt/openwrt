// ****************************************************************************
//
// Copyright (c) 2008 Broadcom Corporation
//
// This program is the proprietary software of Broadcom Corporation and/or
// its licensors, and may only be used, duplicated, modified or distributed
// pursuant to the terms and conditions of a separate, written license
// agreement executed between you and Broadcom (an "Authorized License").
// Except as set forth in an Authorized License, Broadcom grants no license
// (express or implied), right to use, or waiver of any kind with respect to
// the Software, and Broadcom expressly reserves all rights in and to the
// Software and all intellectual property rights therein.  IF YOU HAVE NO
// AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY,
// AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE
// SOFTWARE.  
//
// Except as expressly set forth in the Authorized License,
//
// 1.     This program, including its structure, sequence and organization,
// constitutes the valuable trade secrets of Broadcom, and you shall use all
// reasonable efforts to protect the confidentiality thereof, and to use this
// information only in connection with your use of Broadcom integrated circuit
// products.
//
// 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
// "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
// OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
// RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL
// IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR
// A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET
// ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME
// THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
//
// 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
// OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
// INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY
// RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM
// HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN
// EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1,
// WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY
// FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
//
// ****************************************************************************
//
//  Filename:         ioproc_blockdef.h
//  Generated by:     RDB Utility
//  Creation Date:    8/7/2008
//  Command Line:     
// ****************************************************************************
//
// IMPORTANT: DO NOT MODIFY, THIS IS AN AUTOGENERATED FILE. 
// Please modify the source .rdb file instead if you need to change this file. 
// Contact Jeff Bauch if you need more information.
//
// ****************************************************************************
#ifndef IOPROC_BLOCKDEF_H__
#define IOPROC_BLOCKDEF_H__

#include "ioproc.h"

#include "ioproc.h"

#include "ioproc.h"

  
typedef struct {
  IoprocIoprocControlRegs        Control;
  uint8                          Pad0[0x50];
} IoprocBlockControl;

  
typedef struct {
  IoprocIoprocOutgoingmsgfifoRegs OutgoingMessageFifo;
} IoprocBlockOutgoing;

  
typedef struct {
  IoprocIoprocMsgidRegs          MessageId;
  uint8                          Pad0[0x600];
} IoprocBlockMsgid;

  
typedef struct {
  IoprocIoprocIncomingmsgfifoRegs IncomingMessageFifo;
  uint8                          Pad0[0x80];
} IoprocBlockIncoming;

  
typedef struct {
  IoprocIoprocDmaRegs            Dma0;
  IoprocIoprocDmaRegs            Dma1;
  uint8                          Pad0[0xc0];
} IoprocBlockDmaBlock;

  
typedef struct {
  IoprocIoprocTokenRegs          Token;
  uint8                          Pad0[0xf0];
} IoprocBlockTokenBlock;

  
typedef struct {
  IoprocIoprocDqmRegs            Dqm;
  uint8                          Pad0[0xd4];
} IoprocBlockDqm;

  
typedef struct {
  IoprocIoprocQueueControl       Queue0Cntrl;
  IoprocIoprocQueueControl       Queue1Cntrl;
  IoprocIoprocQueueControl       Queue2Cntrl;
  IoprocIoprocQueueControl       Queue3Cntrl;
  IoprocIoprocQueueControl       Queue4Cntrl;
  IoprocIoprocQueueControl       Queue5Cntrl;
  IoprocIoprocQueueControl       Queue6Cntrl;
  IoprocIoprocQueueControl       Queue7Cntrl;
  IoprocIoprocQueueControl       Queue8Cntrl;
  IoprocIoprocQueueControl       Queue9Cntrl;
  IoprocIoprocQueueControl       Queue10Cntrl;
  IoprocIoprocQueueControl       Queue11Cntrl;
  IoprocIoprocQueueControl       Queue12Cntrl;
  IoprocIoprocQueueControl       Queue13Cntrl;
  IoprocIoprocQueueControl       Queue14Cntrl;
  IoprocIoprocQueueControl       Queue15Cntrl;
  IoprocIoprocQueueControl       Queue16Cntrl;
  IoprocIoprocQueueControl       Queue17Cntrl;
  IoprocIoprocQueueControl       Queue18Cntrl;
  IoprocIoprocQueueControl       Queue19Cntrl;
  IoprocIoprocQueueControl       Queue20Cntrl;
  IoprocIoprocQueueControl       Queue21Cntrl;
  IoprocIoprocQueueControl       Queue22Cntrl;
  IoprocIoprocQueueControl       Queue23Cntrl;
  IoprocIoprocQueueControl       Queue24Cntrl;
  IoprocIoprocQueueControl       Queue25Cntrl;
  IoprocIoprocQueueControl       Queue26Cntrl;
  IoprocIoprocQueueControl       Queue27Cntrl;
  IoprocIoprocQueueControl       Queue28Cntrl;
  IoprocIoprocQueueControl       Queue29Cntrl;
  IoprocIoprocQueueControl       Queue30Cntrl;
  IoprocIoprocQueueControl       Queue31Cntrl;
} IoprocBlockDqmQueueCntrl;

  
typedef struct {
  IoprocIoprocQueueData          Queue0Data;
  IoprocIoprocQueueData          Queue1Data;
  IoprocIoprocQueueData          Queue2Data;
  IoprocIoprocQueueData          Queue3Data;
  IoprocIoprocQueueData          Queue4Data;
  IoprocIoprocQueueData          Queue5Data;
  IoprocIoprocQueueData          Queue6Data;
  IoprocIoprocQueueData          Queue7Data;
  IoprocIoprocQueueData          Queue8Data;
  IoprocIoprocQueueData          Queue9Data;
  IoprocIoprocQueueData          Queue10Data;
  IoprocIoprocQueueData          Queue11Data;
  IoprocIoprocQueueData          Queue12Data;
  IoprocIoprocQueueData          Queue13Data;
  IoprocIoprocQueueData          Queue14Data;
  IoprocIoprocQueueData          Queue15Data;
  IoprocIoprocQueueData          Queue16Data;
  IoprocIoprocQueueData          Queue17Data;
  IoprocIoprocQueueData          Queue18Data;
  IoprocIoprocQueueData          Queue19Data;
  IoprocIoprocQueueData          Queue20Data;
  IoprocIoprocQueueData          Queue21Data;
  IoprocIoprocQueueData          Queue22Data;
  IoprocIoprocQueueData          Queue23Data;
  IoprocIoprocQueueData          Queue24Data;
  IoprocIoprocQueueData          Queue25Data;
  IoprocIoprocQueueData          Queue26Data;
  IoprocIoprocQueueData          Queue27Data;
  IoprocIoprocQueueData          Queue28Data;
  IoprocIoprocQueueData          Queue29Data;
  IoprocIoprocQueueData          Queue30Data;
  IoprocIoprocQueueData          Queue31Data;
} IoprocBlockDqmQueueData;

  
typedef struct {
  IoprocIoprocQueueMib           QueueMib;
  uint8                          Pad0[0x80];
} IoprocBlockDqmQueueMib;

  
typedef struct {
  IoprocIoprocSharedMem          SharedMem;
} IoprocBlockSharedmeMxx;

  
typedef struct {
  union {
    struct {
      uint8                      Pad0[0x1000];
      IoprocBlockControl         Cntrl;
    };
    struct {
      uint8                      Pad1[0x1100];
      IoprocBlockOutgoing        Og;
    };
    struct {
      uint8                      Pad2[0x1200];
      IoprocBlockIncoming        In;
    };
    struct {
      uint8                      Pad3[0x1300];
      IoprocBlockDmaBlock        Dma;
    };
    struct {
      uint8                      Pad4[0x1400];
      IoprocBlockTokenBlock      Token;
    };
    struct {
      uint8                      Pad5[0x1700];
      IoprocBlockMsgid           Msgid;
    };
    struct {
      uint8                      Pad6[0x1800];
      IoprocBlockDqm             DqMa;
    };
    struct {
      uint8                      Pad7[0x1a00];
      IoprocBlockDqmQueueCntrl   Dqmqcntrl;
    };
    struct {
      uint8                      Pad8[0x1c00];
      IoprocBlockDqmQueueData    Dqmqdata;
    };
    struct {
      uint8                      Pad9[0x2000];
      IoprocBlockDqmQueueMib     Dqmqmib;
    };
    struct {
      uint8                      Pad10[0x4000];
      IoprocBlockSharedmeMxx     Sharedmem;
    };
  };
  uint8                          Pad11[0x4000];
}  IoprocBlockIoProc;

#endif 



