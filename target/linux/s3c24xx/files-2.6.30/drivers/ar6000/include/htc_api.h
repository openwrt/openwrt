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

#ifndef _HTC_API_H_
#define _HTC_API_H_

#include <htc.h>
#include <htc_services.h>
#include "htc_packet.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* TODO.. for BMI */
#define ENDPOINT1 0
// TODO -remove me, but we have to fix BMI first
#define HTC_MAILBOX_NUM_MAX    4


/* ------ Endpoint IDS ------ */
typedef enum
{
    ENDPOINT_UNUSED = -1,
    ENDPOINT_0 = 0,
    ENDPOINT_1 = 1,
    ENDPOINT_2 = 2,
    ENDPOINT_3,
    ENDPOINT_4,
    ENDPOINT_5,
    ENDPOINT_6,
    ENDPOINT_7,
    ENDPOINT_8,
    ENDPOINT_MAX,
} HTC_ENDPOINT_ID;

/* this is the amount of header room required by users of HTC */
#define HTC_HEADER_LEN         HTC_HDR_LENGTH

typedef void *HTC_HANDLE;

typedef A_UINT16 HTC_SERVICE_ID;

typedef struct _HTC_INIT_INFO {
    void   (*AddInstance)(HTC_HANDLE);
    void   (*DeleteInstance)(void *Instance);
    void   (*TargetFailure)(void *Instance, A_STATUS Status);
} HTC_INIT_INFO;

/* per service connection send completion */
typedef void   (*HTC_EP_SEND_PKT_COMPLETE)(void *,HTC_PACKET *);
/* per service connection pkt received */
typedef void   (*HTC_EP_RECV_PKT)(void *,HTC_PACKET *);

/* Optional per service connection receive buffer re-fill callback,
 * On some OSes (like Linux) packets are allocated from a global pool and indicated up
 * to the network stack.  The driver never gets the packets back from the OS.  For these OSes
 * a refill callback can be used to allocate and re-queue buffers into HTC.
 *
 * On other OSes, the network stack can call into the driver's OS-specifc "return_packet" handler and
 * the driver can re-queue these buffers into HTC. In this regard a refill callback is
 * unnecessary */
typedef void   (*HTC_EP_RECV_REFILL)(void *, HTC_ENDPOINT_ID Endpoint);

/* Optional per service connection callback when a send queue is full. This can occur if the
 * host continues queueing up TX packets faster than credits can arrive
 * To prevent the host (on some Oses like Linux) from continuously queueing packets
 * and consuming resources, this callback is provided so that that the host
 * can disable TX in the subsystem (i.e. network stack)
 * Other OSes require a "per-packet" indication_RAW_STREAM_NUM_MAX for each completed TX packet, this
 * closed loop mechanism will prevent the network stack from overunning the NIC */
typedef void (*HTC_EP_SEND_QUEUE_FULL)(void *, HTC_ENDPOINT_ID Endpoint);
/* Optional per service connection callback when a send queue is available for receive new packet. */
typedef void (*HTC_EP_SEND_QUEUE_AVAIL)(void *, HTC_ENDPOINT_ID Endpoint);

typedef struct _HTC_EP_CALLBACKS {
    void                     *pContext;     /* context for each callback */
    HTC_EP_SEND_PKT_COMPLETE EpTxComplete;  /* tx completion callback for connected endpoint */
    HTC_EP_RECV_PKT          EpRecv;        /* receive callback for connected endpoint */
    HTC_EP_RECV_REFILL       EpRecvRefill;  /* OPTIONAL receive re-fill callback for connected endpoint */
    HTC_EP_SEND_QUEUE_FULL   EpSendFull;    /* OPTIONAL send full callback */
    HTC_EP_SEND_QUEUE_AVAIL  EpSendAvail;    /* OPTIONAL send available callback */
} HTC_EP_CALLBACKS;

/* service connection information */
typedef struct _HTC_SERVICE_CONNECT_REQ {
    HTC_SERVICE_ID   ServiceID;                 /* service ID to connect to */
    A_UINT16         ConnectionFlags;           /* connection flags, see htc protocol definition */
    A_UINT8         *pMetaData;                 /* ptr to optional service-specific meta-data */
    A_UINT8          MetaDataLength;            /* optional meta data length */
    HTC_EP_CALLBACKS EpCallbacks;               /* endpoint callbacks */
    int              MaxSendQueueDepth;         /* maximum depth of any send queue */
} HTC_SERVICE_CONNECT_REQ;

/* service connection response information */
typedef struct _HTC_SERVICE_CONNECT_RESP {
    A_UINT8     *pMetaData;             /* caller supplied buffer to optional meta-data */
    A_UINT8     BufferLength;           /* length of caller supplied buffer */
    A_UINT8     ActualLength;           /* actual length of meta data */
    HTC_ENDPOINT_ID Endpoint;           /* endpoint to communicate over */
    int         MaxMsgLength;           /* max length of all messages over this endpoint */
    A_UINT8     ConnectRespCode;        /* connect response code from target */
} HTC_SERVICE_CONNECT_RESP;

/* endpoint distribution structure */
typedef struct _HTC_ENDPOINT_CREDIT_DIST {
    struct _HTC_ENDPOINT_CREDIT_DIST *pNext;
    struct _HTC_ENDPOINT_CREDIT_DIST *pPrev;
    HTC_SERVICE_ID      ServiceID;          /* Service ID (set by HTC) */
    HTC_ENDPOINT_ID     Endpoint;           /* endpoint for this distribution struct (set by HTC) */
    A_UINT32            DistFlags;          /* distribution flags, distribution function can
                                               set default activity using SET_EP_ACTIVE() macro */
    int                 TxCreditsNorm;      /* credits for normal operation, anything above this
                                               indicates the endpoint is over-subscribed, this field
                                               is only relevant to the credit distribution function */
    int                 TxCreditsMin;       /* floor for credit distribution, this field is
                                               only relevant to the credit distribution function */
    int                 TxCreditsAssigned;  /* number of credits assigned to this EP, this field
                                               is only relevant to the credit dist function */
    int                 TxCredits;          /* current credits available, this field is used by
                                               HTC to determine whether a message can be sent or
                                               must be queued */
    int                 TxCreditsToDist;    /* pending credits to distribute on this endpoint, this
                                               is set by HTC when credit reports arrive.
                                               The credit distribution functions sets this to zero
                                               when it distributes the credits */
    int                 TxCreditsSeek;      /* this is the number of credits that the current pending TX
                                               packet needs to transmit.  This is set by HTC when
                                               and endpoint needs credits in order to transmit */
    int                 TxCreditSize;       /* size in bytes of each credit (set by HTC) */
    int                 TxCreditsPerMaxMsg; /* credits required for a maximum sized messages (set by HTC) */
    void                *pHTCReserved;      /* reserved for HTC use */
} HTC_ENDPOINT_CREDIT_DIST;

#define HTC_EP_ACTIVE                            (1 << 31)

/* macro to check if an endpoint has gone active, useful for credit
 * distributions */
#define IS_EP_ACTIVE(epDist)  ((epDist)->DistFlags & HTC_EP_ACTIVE)
#define SET_EP_ACTIVE(epDist) (epDist)->DistFlags |= HTC_EP_ACTIVE

    /* credit distibution code that is passed into the distrbution function,
     * there are mandatory and optional codes that must be handled */
typedef enum _HTC_CREDIT_DIST_REASON {
    HTC_CREDIT_DIST_SEND_COMPLETE = 0,     /* credits available as a result of completed
                                              send operations (MANDATORY) resulting in credit reports */
    HTC_CREDIT_DIST_ACTIVITY_CHANGE = 1,   /* a change in endpoint activity occured (OPTIONAL) */
    HTC_CREDIT_DIST_SEEK_CREDITS,          /* an endpoint needs to "seek" credits (OPTIONAL) */
    HTC_DUMP_CREDIT_STATE                  /* for debugging, dump any state information that is kept by
                                              the distribution function */
} HTC_CREDIT_DIST_REASON;

typedef void (*HTC_CREDIT_DIST_CALLBACK)(void                     *Context,
                                         HTC_ENDPOINT_CREDIT_DIST *pEPList,
                                         HTC_CREDIT_DIST_REASON   Reason);

typedef void (*HTC_CREDIT_INIT_CALLBACK)(void *Context,
                                         HTC_ENDPOINT_CREDIT_DIST *pEPList,
                                         int                      TotalCredits);

    /* endpoint statistics action */
typedef enum _HTC_ENDPOINT_STAT_ACTION {
    HTC_EP_STAT_SAMPLE = 0,                /* only read statistics */
    HTC_EP_STAT_SAMPLE_AND_CLEAR = 1,      /* sample and immediately clear statistics */
    HTC_EP_STAT_CLEAR                      /* clear only */
} HTC_ENDPOINT_STAT_ACTION;

    /* endpoint statistics */
typedef struct _HTC_ENDPOINT_STATS {
    A_UINT32  TxCreditLowIndications;  /* number of times the host set the credit-low flag in a send message on
                                        this endpoint */
    A_UINT32  TxIssued;               /* running count of TX packets issued */
    A_UINT32  TxCreditRpts;           /* running count of total credit reports received for this endpoint */
    A_UINT32  TxCreditRptsFromRx;
    A_UINT32  TxCreditRptsFromOther;
    A_UINT32  TxCreditRptsFromEp0;
    A_UINT32  TxCreditsFromRx;        /* count of credits received via Rx packets on this endpoint */
    A_UINT32  TxCreditsFromOther;     /* count of credits received via another endpoint */
    A_UINT32  TxCreditsFromEp0;       /* count of credits received via another endpoint */
    A_UINT32  TxCreditsConsummed;     /* count of consummed credits */
    A_UINT32  TxCreditsReturned;      /* count of credits returned */
    A_UINT32  RxReceived;             /* count of RX packets received */
    A_UINT32  RxLookAheads;           /* count of lookahead records
                                         found in messages received on this endpoint */
} HTC_ENDPOINT_STATS;

/* ------ Function Prototypes ------ */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Initialize HTC
  @function name: HTCInit
  @input:  pInfo - initialization information
  @output:
  @return: A_OK on success
  @notes: The caller initializes global HTC state and registers various instance
          notification callbacks (see HTC_INIT_INFO).

  @example:
  @see also: HTCShutdown
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
A_STATUS    HTCInit(HTC_INIT_INFO *pInfo);
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Get the underlying HIF device handle
  @function name: HTCGetHifDevice
  @input:  HTCHandle - handle passed into the AddInstance callback
  @output:
  @return: opaque HIF device handle usable in HIF API calls.
  @notes:
  @example:
  @see also:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void       *HTCGetHifDevice(HTC_HANDLE HTCHandle);
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Set the associated instance for the HTC handle
  @function name: HTCSetInstance
  @input:  HTCHandle - handle passed into the AddInstance callback
           Instance - caller supplied instance object
  @output:
  @return:
  @notes:  Caller must set the instance information for the HTC handle in order to receive
           notifications for instance deletion (DeleteInstance callback is called) and for target
           failure notification.
  @example:
  @see also:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void        HTCSetInstance(HTC_HANDLE HTCHandle, void *Instance);
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Set credit distribution parameters
  @function name: HTCSetCreditDistribution
  @input:  HTCHandle - HTC handle
           pCreditDistCont - caller supplied context to pass into distribution functions
           CreditDistFunc - Distribution function callback
           CreditDistInit - Credit Distribution initialization callback
           ServicePriorityOrder - Array containing list of service IDs, lowest index is highest
                                  priority
           ListLength - number of elements in ServicePriorityOrder
  @output:
  @return:
  @notes:  The user can set a custom credit distribution function to handle special requirements
           for each endpoint.  A default credit distribution routine can be used by setting
           CreditInitFunc to NULL.  The default credit distribution is only provided for simple
           "fair" credit distribution without regard to any prioritization.

  @example:
  @see also:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void        HTCSetCreditDistribution(HTC_HANDLE               HTCHandle,
                                     void                     *pCreditDistContext,
                                     HTC_CREDIT_DIST_CALLBACK CreditDistFunc,
                                     HTC_CREDIT_INIT_CALLBACK CreditInitFunc,
                                     HTC_SERVICE_ID           ServicePriorityOrder[],
                                     int                      ListLength);
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Wait for the target to indicate the HTC layer is ready
  @function name: HTCWaitTarget
  @input:  HTCHandle - HTC handle
  @output:
  @return:
  @notes:  This API blocks until the target responds with an HTC ready message.
           The caller should not connect services until the target has indicated it is
           ready.
  @example:
  @see also: HTCConnectService
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
A_STATUS    HTCWaitTarget(HTC_HANDLE HTCHandle);
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Start target service communications
  @function name: HTCStart
  @input:  HTCHandle - HTC handle
  @output:
  @return:
  @notes: This API indicates to the target that the service connection phase is complete
          and the target can freely start all connected services.  This API should only be
          called AFTER all service connections have been made.  TCStart will issue a
          SETUP_COMPLETE message to the target to indicate that all service connections
          have been made and the target can start communicating over the endpoints.
  @example:
  @see also: HTCConnectService
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
A_STATUS    HTCStart(HTC_HANDLE HTCHandle);
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Add receive packet to HTC
  @function name: HTCAddReceivePkt
  @input:  HTCHandle - HTC handle
           pPacket - HTC receive packet to add
  @output:
  @return: A_OK on success
  @notes:  user must supply HTC packets for capturing incomming HTC frames.  The caller
           must initialize each HTC packet using the SET_HTC_PACKET_INFO_RX_REFILL()
           macro.
  @example:
  @see also:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
A_STATUS    HTCAddReceivePkt(HTC_HANDLE HTCHandle, HTC_PACKET *pPacket);
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Connect to an HTC service
  @function name: HTCConnectService
  @input:  HTCHandle - HTC handle
           pReq - connection details
  @output: pResp - connection response
  @return:
  @notes:  Service connections must be performed before HTCStart.  User provides callback handlers
           for various endpoint events.
  @example:
  @see also: HTCStart
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
A_STATUS    HTCConnectService(HTC_HANDLE HTCHandle,
                              HTC_SERVICE_CONNECT_REQ  *pReq,
                              HTC_SERVICE_CONNECT_RESP *pResp);
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Send an HTC packet
  @function name: HTCSendPkt
  @input:  HTCHandle - HTC handle
           pPacket - packet to send
  @output:
  @return: A_OK
  @notes:  Caller must initialize packet using SET_HTC_PACKET_INFO_TX() macro.
           This interface is fully asynchronous.  On error, HTC SendPkt will
           call the registered Endpoint callback to cleanup the packet.
  @example:
  @see also: HTCFlushEndpoint
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
A_STATUS    HTCSendPkt(HTC_HANDLE HTCHandle, HTC_PACKET *pPacket);
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Stop HTC service communications
  @function name: HTCStop
  @input:  HTCHandle - HTC handle
  @output:
  @return:
  @notes: HTC communications is halted.  All receive and pending TX packets will
          be flushed.
  @example:
  @see also:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void        HTCStop(HTC_HANDLE HTCHandle);
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Shutdown HTC
  @function name: HTCShutdown
  @input:
  @output:
  @return:
  @notes:  This cleans up all resources allocated by HTCInit().
  @example:
  @see also: HTCInit
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void        HTCShutDown(void);
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Flush pending TX packets
  @function name: HTCFlushEndpoint
  @input:  HTCHandle - HTC handle
           Endpoint - Endpoint to flush
           Tag - flush tag
  @output:
  @return:
  @notes:  The Tag parameter is used to selectively flush packets with matching tags.
           The value of 0 forces all packets to be flush regardless of tag.
  @example:
  @see also: HTCSendPkt
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void        HTCFlushEndpoint(HTC_HANDLE HTCHandle, HTC_ENDPOINT_ID Endpoint, HTC_TX_TAG Tag);
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Dump credit distribution state
  @function name: HTCDumpCreditStates
  @input:  HTCHandle - HTC handle
  @output:
  @return:
  @notes:  This dumps all credit distribution information to the debugger
  @example:
  @see also:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void        HTCDumpCreditStates(HTC_HANDLE HTCHandle);
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Indicate a traffic activity change on an endpoint
  @function name: HTCIndicateActivityChange
  @input:  HTCHandle - HTC handle
           Endpoint - endpoint in which activity has changed
           Active - TRUE if active, FALSE if it has become inactive
  @output:
  @return:
  @notes:  This triggers the registered credit distribution function to
           re-adjust credits for active/inactive endpoints.
  @example:
  @see also:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void        HTCIndicateActivityChange(HTC_HANDLE      HTCHandle,
                                      HTC_ENDPOINT_ID Endpoint,
                                      A_BOOL          Active);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  @desc: Get endpoint statistics
  @function name: HTCGetEndpointStatistics
  @input:  HTCHandle - HTC handle
           Endpoint - Endpoint identifier
           Action - action to take with statistics
  @output:
           pStats - statistics that were sampled (can be NULL if Action is HTC_EP_STAT_CLEAR)

  @return: TRUE if statistics profiling is enabled, otherwise FALSE.

  @notes:  Statistics is a compile-time option and this function may return FALSE
           if HTC is not compiled with profiling.

           The caller can specify the statistic "action" to take when sampling
           the statistics.  This includes:

           HTC_EP_STAT_SAMPLE: The pStats structure is filled with the current values.
           HTC_EP_STAT_SAMPLE_AND_CLEAR: The structure is filled and the current statistics
                             are cleared.
           HTC_EP_STAT_CLEA : the statistics are cleared, the called can pass a NULL value for
                   pStats

  @example:
  @see also:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
A_BOOL       HTCGetEndpointStatistics(HTC_HANDLE               HTCHandle,
                                      HTC_ENDPOINT_ID          Endpoint,
                                      HTC_ENDPOINT_STAT_ACTION Action,
                                      HTC_ENDPOINT_STATS       *pStats);

#ifdef __cplusplus
}
#endif

#endif /* _HTC_API_H_ */
