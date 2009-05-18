/*
 *
 * Copyright (c) 2007 Atheros Communications Inc.
 * All rights reserved.
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation;
 *
 *  Software distributed under the License is distributed on an "AS
 *  IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 *  implied. See the License for the specific language governing
 *  rights and limitations under the License.
 *
 *
 *
 */

#ifndef _HTC_INTERNAL_H_
#define _HTC_INTERNAL_H_

/* for debugging, uncomment this to capture the last frame header, on frame header
 * processing errors, the last frame header is dump for comparison */
//#define HTC_CAPTURE_LAST_FRAME

//#define HTC_EP_STAT_PROFILING

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Header files */
#include "a_config.h"
#include "athdefs.h"
#include "a_types.h"
#include "a_osapi.h"
#include "a_debug.h"
#include "htc.h"
#include "htc_api.h"
#include "bmi_msg.h"
#include "hif.h"
#include "ar6k.h"

/* HTC operational parameters */
#define HTC_TARGET_RESPONSE_TIMEOUT        2000 /* in ms */
#define HTC_TARGET_DEBUG_INTR_MASK         0x01
#define HTC_TARGET_CREDIT_INTR_MASK        0xF0

typedef struct _HTC_ENDPOINT {
    HTC_SERVICE_ID              ServiceID;      /* service ID this endpoint is bound to
                                                   non-zero value means this endpoint is in use */
    HTC_PACKET_QUEUE            TxQueue;        /* HTC frame buffer TX queue */
    HTC_PACKET_QUEUE            RxBuffers;      /* HTC frame buffer RX list */
    HTC_ENDPOINT_CREDIT_DIST    CreditDist;     /* credit distribution structure (exposed to driver layer) */
    HTC_EP_CALLBACKS            EpCallBacks;    /* callbacks associated with this endpoint */
    int                         MaxTxQueueDepth;   /* max depth of the TX queue before we need to
                                                      call driver's full handler */
    int                         CurrentTxQueueDepth; /* current TX queue depth */
    int                         MaxMsgLength;        /* max length of endpoint message */
#ifdef HTC_EP_STAT_PROFILING
    HTC_ENDPOINT_STATS          EndPointStats;  /* endpoint statistics */
#endif
} HTC_ENDPOINT;

#ifdef HTC_EP_STAT_PROFILING
#define INC_HTC_EP_STAT(p,stat,count) (p)->EndPointStats.stat += (count);
#else
#define INC_HTC_EP_STAT(p,stat,count)
#endif

#define HTC_SERVICE_TX_PACKET_TAG  HTC_TX_PACKET_TAG_INTERNAL

#define NUM_CONTROL_BUFFERS     8
#define NUM_CONTROL_TX_BUFFERS  2
#define NUM_CONTROL_RX_BUFFERS  (NUM_CONTROL_BUFFERS - NUM_CONTROL_TX_BUFFERS)

#define HTC_CONTROL_BUFFER_SIZE (HTC_MAX_CONTROL_MESSAGE_LENGTH + HTC_HDR_LENGTH)

typedef struct HTC_CONTROL_BUFFER {
    HTC_PACKET    HtcPacket;
    A_UINT8       Buffer[HTC_CONTROL_BUFFER_SIZE];
} HTC_CONTROL_BUFFER;

/* our HTC target state */
typedef struct _HTC_TARGET {
    HTC_ENDPOINT                EndPoint[ENDPOINT_MAX];
    HTC_CONTROL_BUFFER          HTCControlBuffers[NUM_CONTROL_BUFFERS];
    HTC_ENDPOINT_CREDIT_DIST   *EpCreditDistributionListHead;
    HTC_PACKET_QUEUE            ControlBufferTXFreeList;
    HTC_PACKET_QUEUE            ControlBufferRXFreeList;
    HTC_CREDIT_DIST_CALLBACK    DistributeCredits;
    HTC_CREDIT_INIT_CALLBACK    InitCredits;
    void                       *pCredDistContext;
    int                         TargetCredits;
    int                         TargetCreditSize;
    A_MUTEX_T                   HTCLock;
    A_MUTEX_T                   HTCRxLock;
    A_MUTEX_T                   HTCTxLock;
    AR6K_DEVICE                 Device;         /* AR6K - specific state */
    A_UINT32                    HTCStateFlags;
    HTC_ENDPOINT_ID             EpWaitingForBuffers;
    A_BOOL                      TargetFailure;
    void                       *pInstanceContext;
#define HTC_STATE_WAIT_BUFFERS  (1 << 0)
#define HTC_STATE_STOPPING      (1 << 1)
#ifdef HTC_CAPTURE_LAST_FRAME
    HTC_FRAME_HDR               LastFrameHdr;  /* useful for debugging */
    A_UINT8                     LastTrailer[256];
    A_UINT8                     LastTrailerLength;
#endif
} HTC_TARGET;

#define HTC_STOPPING(t) ((t)->HTCStateFlags & HTC_STATE_STOPPING)
#define LOCK_HTC(t)      A_MUTEX_LOCK(&(t)->HTCLock);
#define UNLOCK_HTC(t)    A_MUTEX_UNLOCK(&(t)->HTCLock);
#define LOCK_HTC_RX(t)   A_MUTEX_LOCK(&(t)->HTCRxLock);
#define UNLOCK_HTC_RX(t) A_MUTEX_UNLOCK(&(t)->HTCRxLock);
#define LOCK_HTC_TX(t)   A_MUTEX_LOCK(&(t)->HTCTxLock);
#define UNLOCK_HTC_TX(t) A_MUTEX_UNLOCK(&(t)->HTCTxLock);

#define GET_HTC_TARGET_FROM_HANDLE(hnd) ((HTC_TARGET *)(hnd))
#define HTC_RECYCLE_RX_PKT(target,p)                \
{                                                   \
    HTC_PACKET_RESET_RX(pPacket);                   \
    HTCAddReceivePkt((HTC_HANDLE)(target),(p));     \
}

/* internal HTC functions */
void        HTCControlTxComplete(void *Context, HTC_PACKET *pPacket);
void        HTCControlRecv(void *Context, HTC_PACKET *pPacket);
A_STATUS    HTCWaitforControlMessage(HTC_TARGET *target, HTC_PACKET **ppControlPacket);
HTC_PACKET *HTCAllocControlBuffer(HTC_TARGET *target, HTC_PACKET_QUEUE *pList);
void        HTCFreeControlBuffer(HTC_TARGET *target, HTC_PACKET *pPacket, HTC_PACKET_QUEUE *pList);
A_STATUS    HTCIssueSend(HTC_TARGET *target, HTC_PACKET *pPacket, A_UINT8 Flags);
A_STATUS    HTCIssueRecv(HTC_TARGET *target, HTC_PACKET *pPacket);
void        HTCRecvCompleteHandler(void *Context, HTC_PACKET *pPacket);
A_STATUS    HTCRecvMessagePendingHandler(void *Context, A_UINT32 LookAhead, A_BOOL *pAsyncProc);
void        HTCProcessCreditRpt(HTC_TARGET *target, HTC_CREDIT_REPORT *pRpt, int NumEntries, HTC_ENDPOINT_ID FromEndpoint);
A_STATUS    HTCSendSetupComplete(HTC_TARGET *target);
void        HTCFlushRecvBuffers(HTC_TARGET *target);
void        HTCFlushSendPkts(HTC_TARGET *target);
void        DumpCreditDist(HTC_ENDPOINT_CREDIT_DIST *pEPDist);
void        DumpCreditDistStates(HTC_TARGET *target);
void 		DebugDumpBytes(A_UCHAR *buffer, A_UINT16 length, char *pDescription);

static INLINE HTC_PACKET *HTC_ALLOC_CONTROL_TX(HTC_TARGET *target) {
    HTC_PACKET *pPacket = HTCAllocControlBuffer(target,&target->ControlBufferTXFreeList);
    if (pPacket != NULL) {
            /* set payload pointer area with some headroom */
        pPacket->pBuffer = pPacket->pBufferStart + HTC_HDR_LENGTH;
    }
    return pPacket;
}

#define HTC_FREE_CONTROL_TX(t,p) HTCFreeControlBuffer((t),(p),&(t)->ControlBufferTXFreeList)
#define HTC_ALLOC_CONTROL_RX(t)  HTCAllocControlBuffer((t),&(t)->ControlBufferRXFreeList)
#define HTC_FREE_CONTROL_RX(t,p) \
{                                                                \
    HTC_PACKET_RESET_RX(p);                                      \
    HTCFreeControlBuffer((t),(p),&(t)->ControlBufferRXFreeList); \
}

#ifdef __cplusplus
}
#endif

#endif /* _HTC_INTERNAL_H_ */
