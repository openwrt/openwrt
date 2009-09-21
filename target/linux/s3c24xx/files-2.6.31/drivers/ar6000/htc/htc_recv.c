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

#include "htc_internal.h"

#define HTCIssueRecv(t, p) \
    DevRecvPacket(&(t)->Device,  \
                  (p),          \
                  (p)->ActualLength)

#define DO_RCV_COMPLETION(t,p,e)            \
{                                           \
    if ((p)->ActualLength > 0) {            \
        AR_DEBUG_PRINTF(ATH_DEBUG_RECV, (" completing packet 0x%X (%d bytes) on ep : %d \n", \
            (A_UINT32)(p), (p)->ActualLength, (p)->Endpoint));  \
        (e)->EpCallBacks.EpRecv((e)->EpCallBacks.pContext,      \
                                (p));                           \
    } else {                                                    \
        AR_DEBUG_PRINTF(ATH_DEBUG_RECV, (" recycling empty packet \n"));  \
        HTC_RECYCLE_RX_PKT((t), (p));                           \
    }                                                           \
}

#ifdef HTC_EP_STAT_PROFILING
#define HTC_RX_STAT_PROFILE(t,ep,lookAhead)            \
{                                                      \
    LOCK_HTC_RX((t));                                  \
    INC_HTC_EP_STAT((ep), RxReceived, 1);              \
    if ((lookAhead) != 0) {                            \
        INC_HTC_EP_STAT((ep), RxLookAheads, 1);        \
    }                                                  \
    UNLOCK_HTC_RX((t));                                \
}
#else
#define HTC_RX_STAT_PROFILE(t,ep,lookAhead)
#endif

static INLINE A_STATUS HTCProcessTrailer(HTC_TARGET *target,
                                         A_UINT8    *pBuffer,
                                         int         Length,
                                         A_UINT32   *pNextLookAhead,
                                         HTC_ENDPOINT_ID FromEndpoint)
{
    HTC_RECORD_HDR          *pRecord;
    A_UINT8                 *pRecordBuf;
    HTC_LOOKAHEAD_REPORT    *pLookAhead;
    A_UINT8                 *pOrigBuffer;
    int                     origLength;
    A_STATUS                status;

    AR_DEBUG_PRINTF(ATH_DEBUG_RECV, ("+HTCProcessTrailer (length:%d) \n", Length));

    if (AR_DEBUG_LVL_CHECK(ATH_DEBUG_RECV)) {
        AR_DEBUG_PRINTBUF(pBuffer,Length,"Recv Trailer");
    }

    pOrigBuffer = pBuffer;
    origLength = Length;
    status = A_OK;

    while (Length > 0) {

        if (Length < sizeof(HTC_RECORD_HDR)) {
            status = A_EPROTO;
            break;
        }
            /* these are byte aligned structs */
        pRecord = (HTC_RECORD_HDR *)pBuffer;
        Length -= sizeof(HTC_RECORD_HDR);
        pBuffer += sizeof(HTC_RECORD_HDR);

        if (pRecord->Length > Length) {
                /* no room left in buffer for record */
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR,
                (" invalid record length: %d (id:%d) buffer has: %d bytes left \n",
                        pRecord->Length, pRecord->RecordID, Length));
            status = A_EPROTO;
            break;
        }
            /* start of record follows the header */
        pRecordBuf = pBuffer;

        switch (pRecord->RecordID) {
            case HTC_RECORD_CREDITS:
                AR_DEBUG_ASSERT(pRecord->Length >= sizeof(HTC_CREDIT_REPORT));
                HTCProcessCreditRpt(target,
                                    (HTC_CREDIT_REPORT *)pRecordBuf,
                                    pRecord->Length / (sizeof(HTC_CREDIT_REPORT)),
                                    FromEndpoint);
                break;
            case HTC_RECORD_LOOKAHEAD:
                AR_DEBUG_ASSERT(pRecord->Length >= sizeof(HTC_LOOKAHEAD_REPORT));
                pLookAhead = (HTC_LOOKAHEAD_REPORT *)pRecordBuf;
                if ((pLookAhead->PreValid == ((~pLookAhead->PostValid) & 0xFF)) &&
                    (pNextLookAhead != NULL)) {

                    AR_DEBUG_PRINTF(ATH_DEBUG_RECV,
                                (" LookAhead Report Found (pre valid:0x%X, post valid:0x%X) \n",
                                pLookAhead->PreValid,
                                pLookAhead->PostValid));

                        /* look ahead bytes are valid, copy them over */
                    ((A_UINT8 *)pNextLookAhead)[0] = pLookAhead->LookAhead[0];
                    ((A_UINT8 *)pNextLookAhead)[1] = pLookAhead->LookAhead[1];
                    ((A_UINT8 *)pNextLookAhead)[2] = pLookAhead->LookAhead[2];
                    ((A_UINT8 *)pNextLookAhead)[3] = pLookAhead->LookAhead[3];

                    if (AR_DEBUG_LVL_CHECK(ATH_DEBUG_RECV)) {
                        DebugDumpBytes((A_UINT8 *)pNextLookAhead,4,"Next Look Ahead");
                    }
                }
                break;
            default:
                AR_DEBUG_PRINTF(ATH_DEBUG_ERR, (" unhandled record: id:%d length:%d \n",
                        pRecord->RecordID, pRecord->Length));
                break;
        }

        if (A_FAILED(status)) {
            break;
        }

            /* advance buffer past this record for next time around */
        pBuffer += pRecord->Length;
        Length -= pRecord->Length;
    }

    if (A_FAILED(status)) {
        DebugDumpBytes(pOrigBuffer,origLength,"BAD Recv Trailer");
    }

    AR_DEBUG_PRINTF(ATH_DEBUG_RECV, ("-HTCProcessTrailer \n"));
    return status;

}

/* process a received message (i.e. strip off header, process any trailer data)
 * note : locks must be released when this function is called */
static A_STATUS HTCProcessRecvHeader(HTC_TARGET *target, HTC_PACKET *pPacket, A_UINT32 *pNextLookAhead)
{
    A_UINT8   temp;
    A_UINT8   *pBuf;
    A_STATUS  status = A_OK;
    A_UINT16  payloadLen;
    A_UINT32  lookAhead;

    pBuf = pPacket->pBuffer;

    AR_DEBUG_PRINTF(ATH_DEBUG_RECV, ("+HTCProcessRecvHeader \n"));

    if (AR_DEBUG_LVL_CHECK(ATH_DEBUG_RECV)) {
        AR_DEBUG_PRINTBUF(pBuf,pPacket->ActualLength,"HTC Recv PKT");
    }

    do {
        /* note, we cannot assume the alignment of pBuffer, so we use the safe macros to
         * retrieve 16 bit fields */
        payloadLen = A_GET_UINT16_FIELD(pBuf, HTC_FRAME_HDR, PayloadLen);

        ((A_UINT8 *)&lookAhead)[0] = pBuf[0];
        ((A_UINT8 *)&lookAhead)[1] = pBuf[1];
        ((A_UINT8 *)&lookAhead)[2] = pBuf[2];
        ((A_UINT8 *)&lookAhead)[3] = pBuf[3];

        if (lookAhead != pPacket->HTCReserved) {
            /* somehow the lookahead that gave us the full read length did not
             * reflect the actual header in the pending message */
             AR_DEBUG_PRINTF(ATH_DEBUG_ERR,
                    ("HTCProcessRecvHeader, lookahead mismatch! \n"));
             DebugDumpBytes((A_UINT8 *)&pPacket->HTCReserved,4,"Expected Message LookAhead");
             DebugDumpBytes(pBuf,sizeof(HTC_FRAME_HDR),"Current Frame Header");
#ifdef HTC_CAPTURE_LAST_FRAME
            DebugDumpBytes((A_UINT8 *)&target->LastFrameHdr,sizeof(HTC_FRAME_HDR),"Last Frame Header");
            if (target->LastTrailerLength != 0) {
                DebugDumpBytes(target->LastTrailer,
                               target->LastTrailerLength,
                               "Last trailer");
            }
#endif
            status = A_EPROTO;
            break;
        }

            /* get flags */
        temp = A_GET_UINT8_FIELD(pBuf, HTC_FRAME_HDR, Flags);

        if (temp & HTC_FLAGS_RECV_TRAILER) {
            /* this packet has a trailer */

                /* extract the trailer length in control byte 0 */
            temp = A_GET_UINT8_FIELD(pBuf, HTC_FRAME_HDR, ControlBytes[0]);

            if ((temp < sizeof(HTC_RECORD_HDR)) || (temp > payloadLen)) {
                AR_DEBUG_PRINTF(ATH_DEBUG_ERR,
                    ("HTCProcessRecvHeader, invalid header (payloadlength should be :%d, CB[0] is:%d) \n",
                        payloadLen, temp));
                status = A_EPROTO;
                break;
            }

                /* process trailer data that follows HDR + application payload */
            status = HTCProcessTrailer(target,
                                       (pBuf + HTC_HDR_LENGTH + payloadLen - temp),
                                       temp,
                                       pNextLookAhead,
                                       pPacket->Endpoint);

            if (A_FAILED(status)) {
                break;
            }

#ifdef HTC_CAPTURE_LAST_FRAME
            A_MEMCPY(target->LastTrailer, (pBuf + HTC_HDR_LENGTH + payloadLen - temp), temp);
            target->LastTrailerLength = temp;
#endif
                /* trim length by trailer bytes */
            pPacket->ActualLength -= temp;
        }
#ifdef HTC_CAPTURE_LAST_FRAME
         else {
            target->LastTrailerLength = 0;
        }
#endif

            /* if we get to this point, the packet is good */
            /* remove header and adjust length */
        pPacket->pBuffer += HTC_HDR_LENGTH;
        pPacket->ActualLength -= HTC_HDR_LENGTH;

    } while (FALSE);

    if (A_FAILED(status)) {
            /* dump the whole packet */
        DebugDumpBytes(pBuf,pPacket->ActualLength,"BAD HTC Recv PKT");
    } else {
#ifdef HTC_CAPTURE_LAST_FRAME
        A_MEMCPY(&target->LastFrameHdr,pBuf,sizeof(HTC_FRAME_HDR));
#endif
        if (AR_DEBUG_LVL_CHECK(ATH_DEBUG_RECV)) {
            if (pPacket->ActualLength > 0) {
                AR_DEBUG_PRINTBUF(pPacket->pBuffer,pPacket->ActualLength,"HTC - Application Msg");
            }
        }
    }

    AR_DEBUG_PRINTF(ATH_DEBUG_RECV, ("-HTCProcessRecvHeader \n"));
    return status;
}

/* asynchronous completion handler for recv packet fetching, when the device layer
 * completes a read request, it will call this completion handler */
void HTCRecvCompleteHandler(void *Context, HTC_PACKET *pPacket)
{
    HTC_TARGET      *target = (HTC_TARGET *)Context;
    HTC_ENDPOINT    *pEndpoint;
    A_UINT32        nextLookAhead = 0;
    A_STATUS        status;

    AR_DEBUG_PRINTF(ATH_DEBUG_RECV, ("+HTCRecvCompleteHandler (status:%d, ep:%d) \n",
                pPacket->Status, pPacket->Endpoint));

    AR_DEBUG_ASSERT(pPacket->Endpoint < ENDPOINT_MAX);
    pEndpoint = &target->EndPoint[pPacket->Endpoint];
    pPacket->Completion = NULL;

        /* get completion status */
    status = pPacket->Status;

    do {
        if (A_FAILED(status)) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("HTCRecvCompleteHandler: request failed (status:%d, ep:%d) \n",
                pPacket->Status, pPacket->Endpoint));
            break;
        }
            /* process the header for any trailer data */
        status = HTCProcessRecvHeader(target,pPacket,&nextLookAhead);

        if (A_FAILED(status)) {
            break;
        }
            /* was there a lookahead for the next packet? */
        if (nextLookAhead != 0) {
            A_STATUS nextStatus;
            AR_DEBUG_PRINTF(ATH_DEBUG_RECV,
                            ("HTCRecvCompleteHandler - next look ahead was non-zero : 0x%X \n",
                             nextLookAhead));
                /* we have another packet, get the next packet fetch started (pipelined) before
                 * we call into the endpoint's callback, this will start another async request */
            nextStatus = HTCRecvMessagePendingHandler(target,nextLookAhead,NULL);
            if (A_EPROTO == nextStatus) {
                AR_DEBUG_PRINTF(ATH_DEBUG_ERR,
                            ("Next look ahead from recv header was INVALID\n"));
                DebugDumpBytes((A_UINT8 *)&nextLookAhead,
                                4,
                                "BAD lookahead from lookahead report");
            }
        } else {
             AR_DEBUG_PRINTF(ATH_DEBUG_RECV,
            ("HTCRecvCompleteHandler - rechecking for more messages...\n"));
            /* if we did not get anything on the look-ahead,
             * call device layer to asynchronously re-check for messages. If we can keep the async
             * processing going we get better performance.  If there is a pending message we will keep processing
             * messages asynchronously which should pipeline things nicely */
            DevCheckPendingRecvMsgsAsync(&target->Device);
        }

        HTC_RX_STAT_PROFILE(target,pEndpoint,nextLookAhead);
        DO_RCV_COMPLETION(target,pPacket,pEndpoint);

    } while (FALSE);

    if (A_FAILED(status)) {
         AR_DEBUG_PRINTF(ATH_DEBUG_ERR,
                         ("HTCRecvCompleteHandler , message fetch failed (status = %d) \n",
                         status));
            /* recyle this packet */
         HTC_RECYCLE_RX_PKT(target, pPacket);
    }

    AR_DEBUG_PRINTF(ATH_DEBUG_RECV, ("-HTCRecvCompleteHandler\n"));
}

/* synchronously wait for a control message from the target,
 * This function is used at initialization time ONLY.  At init messages
 * on ENDPOINT 0 are expected. */
A_STATUS HTCWaitforControlMessage(HTC_TARGET *target, HTC_PACKET **ppControlPacket)
{
    A_STATUS        status;
    A_UINT32        lookAhead;
    HTC_PACKET      *pPacket = NULL;
    HTC_FRAME_HDR   *pHdr;

    AR_DEBUG_PRINTF(ATH_DEBUG_RECV,("+HTCWaitforControlMessage \n"));

    do  {

        *ppControlPacket = NULL;

            /* call the polling function to see if we have a message */
        status = DevPollMboxMsgRecv(&target->Device,
                                    &lookAhead,
                                    HTC_TARGET_RESPONSE_TIMEOUT);

        if (A_FAILED(status)) {
            break;
        }

        AR_DEBUG_PRINTF(ATH_DEBUG_RECV,
                ("HTCWaitforControlMessage : lookAhead : 0x%X \n", lookAhead));

            /* check the lookahead */
        pHdr = (HTC_FRAME_HDR *)&lookAhead;

        if (pHdr->EndpointID != ENDPOINT_0) {
                /* unexpected endpoint number, should be zero */
            AR_DEBUG_ASSERT(FALSE);
            status = A_EPROTO;
            break;
        }

        if (A_FAILED(status)) {
                /* bad message */
            AR_DEBUG_ASSERT(FALSE);
            status = A_EPROTO;
            break;
        }

        pPacket = HTC_ALLOC_CONTROL_RX(target);

        if (pPacket == NULL) {
            AR_DEBUG_ASSERT(FALSE);
            status = A_NO_MEMORY;
            break;
        }

        pPacket->HTCReserved = lookAhead;
        pPacket->ActualLength = pHdr->PayloadLen + HTC_HDR_LENGTH;

        if (pPacket->ActualLength > pPacket->BufferLength) {
            AR_DEBUG_ASSERT(FALSE);
            status = A_EPROTO;
            break;
        }

            /* we want synchronous operation */
        pPacket->Completion = NULL;

            /* get the message from the device, this will block */
        status = HTCIssueRecv(target, pPacket);

        if (A_FAILED(status)) {
            break;
        }

            /* process receive header */
        status = HTCProcessRecvHeader(target,pPacket,NULL);

        pPacket->Status = status;

        if (A_FAILED(status)) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR,
                    ("HTCWaitforControlMessage, HTCProcessRecvHeader failed (status = %d) \n",
                     status));
            break;
        }

            /* give the caller this control message packet, they are responsible to free */
        *ppControlPacket = pPacket;

    } while (FALSE);

    if (A_FAILED(status)) {
        if (pPacket != NULL) {
                /* cleanup buffer on error */
            HTC_FREE_CONTROL_RX(target,pPacket);
        }
    }

    AR_DEBUG_PRINTF(ATH_DEBUG_RECV,("-HTCWaitforControlMessage \n"));

    return status;
}

/* callback when device layer or lookahead report parsing detects a pending message */
A_STATUS HTCRecvMessagePendingHandler(void *Context, A_UINT32 LookAhead, A_BOOL *pAsyncProc)
{
    HTC_TARGET      *target = (HTC_TARGET *)Context;
    A_STATUS         status = A_OK;
    HTC_PACKET      *pPacket = NULL;
    HTC_FRAME_HDR   *pHdr;
    HTC_ENDPOINT    *pEndpoint;
    A_BOOL          asyncProc = FALSE;

    AR_DEBUG_PRINTF(ATH_DEBUG_RECV,("+HTCRecvMessagePendingHandler LookAhead:0x%X \n",LookAhead));

    if (IS_DEV_IRQ_PROCESSING_ASYNC_ALLOWED(&target->Device)) {
            /* We use async mode to get the packets if the device layer supports it.
             * The device layer interfaces with HIF in which HIF may have restrictions on
             * how interrupts are processed */
        asyncProc = TRUE;
    }

    if (pAsyncProc != NULL) {
            /* indicate to caller how we decided to process this */
        *pAsyncProc = asyncProc;
    }

    while (TRUE) {

        pHdr = (HTC_FRAME_HDR *)&LookAhead;

        if (pHdr->EndpointID >= ENDPOINT_MAX) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("Invalid Endpoint in look-ahead: %d \n",pHdr->EndpointID));
                /* invalid endpoint */
            status = A_EPROTO;
            break;
        }

        if (pHdr->PayloadLen > HTC_MAX_PAYLOAD_LENGTH) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("Payload length %d exceeds max HTC : %d !\n",
                    pHdr->PayloadLen, HTC_MAX_PAYLOAD_LENGTH));
            status = A_EPROTO;
            break;
        }

        pEndpoint = &target->EndPoint[pHdr->EndpointID];

        if (0 == pEndpoint->ServiceID) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR,("Endpoint %d is not connected !\n",pHdr->EndpointID));
                /* endpoint isn't even connected */
            status = A_EPROTO;
            break;
        }

            /* lock RX to get a buffer */
        LOCK_HTC_RX(target);

            /* get a packet from the endpoint recv queue */
        pPacket = HTC_PACKET_DEQUEUE(&pEndpoint->RxBuffers);

        if (NULL == pPacket) {
                /* check for refill handler */
            if (pEndpoint->EpCallBacks.EpRecvRefill != NULL) {
                UNLOCK_HTC_RX(target);
                    /* call the re-fill handler */
                pEndpoint->EpCallBacks.EpRecvRefill(pEndpoint->EpCallBacks.pContext,
                                                    pHdr->EndpointID);
                LOCK_HTC_RX(target);
                    /* check if we have more buffers */
                pPacket = HTC_PACKET_DEQUEUE(&pEndpoint->RxBuffers);
                    /* fall through */
            }
        }

        if (NULL == pPacket) {
                /* this is not an error, we simply need to mark that we are waiting for buffers.*/
            target->HTCStateFlags |= HTC_STATE_WAIT_BUFFERS;
            target->EpWaitingForBuffers = pHdr->EndpointID;
            status = A_NO_MEMORY;
        }

        UNLOCK_HTC_RX(target);

        if (A_FAILED(status)) {
                /* no buffers */
            break;
        }

        AR_DEBUG_ASSERT(pPacket->Endpoint == pHdr->EndpointID);

            /* make sure this message can fit in the endpoint buffer */
        if ((pHdr->PayloadLen + HTC_HDR_LENGTH) > pPacket->BufferLength) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR,
                    ("Payload Length Error : header reports payload of: %d, endpoint buffer size: %d \n",
                    pHdr->PayloadLen, pPacket->BufferLength));
            status = A_EPROTO;
            break;
        }

        pPacket->HTCReserved = LookAhead; /* set expected look ahead */
            /* set the amount of data to fetch */
        pPacket->ActualLength = pHdr->PayloadLen + HTC_HDR_LENGTH;

        if (asyncProc) {
                /* we use async mode to get the packet if the device layer supports it
                 * set our callback and context */
            pPacket->Completion = HTCRecvCompleteHandler;
            pPacket->pContext = target;
        } else {
                /* fully synchronous */
            pPacket->Completion = NULL;
        }

            /* go fetch the packet */
        status = HTCIssueRecv(target, pPacket);

        if (A_FAILED(status)) {
            break;
        }

        if (asyncProc) {
                /* we did this asynchronously so we can get out of the loop, the asynch processing
                 * creates a chain of requests to continue processing pending messages in the
                 * context of callbacks  */
            break;
        }

            /* in the sync case, we process the packet, check lookaheads and then repeat */

        LookAhead = 0;
        status = HTCProcessRecvHeader(target,pPacket,&LookAhead);

        if (A_FAILED(status)) {
            break;
        }

        HTC_RX_STAT_PROFILE(target,pEndpoint,LookAhead);
        DO_RCV_COMPLETION(target,pPacket,pEndpoint);

        pPacket = NULL;

        if (0 == LookAhead) {
            break;
        }

    }

    if (A_NO_MEMORY == status) {
        AR_DEBUG_PRINTF(ATH_DEBUG_ERR,
                (" Endpoint :%d has no buffers, blocking receiver to prevent overrun.. \n",
                pHdr->EndpointID));
            /* try to stop receive at the device layer */
        DevStopRecv(&target->Device, asyncProc ? DEV_STOP_RECV_ASYNC : DEV_STOP_RECV_SYNC);
        status = A_OK;
    } else if (A_FAILED(status)) {
        AR_DEBUG_PRINTF(ATH_DEBUG_ERR,
                        ("Failed to get pending message : LookAhead Value: 0x%X (status = %d) \n",
                        LookAhead, status));
        if (pPacket != NULL) {
                /* clean up packet on error */
            HTC_RECYCLE_RX_PKT(target, pPacket);
        }
    }

    AR_DEBUG_PRINTF(ATH_DEBUG_RECV,("-HTCRecvMessagePendingHandler \n"));

    return status;
}

/* Makes a buffer available to the HTC module */
A_STATUS HTCAddReceivePkt(HTC_HANDLE HTCHandle, HTC_PACKET *pPacket)
{
    HTC_TARGET *target = GET_HTC_TARGET_FROM_HANDLE(HTCHandle);
    HTC_ENDPOINT *pEndpoint;
    A_BOOL       unblockRecv = FALSE;
    A_STATUS     status = A_OK;

    AR_DEBUG_PRINTF(ATH_DEBUG_SEND,
                    ("+- HTCAddReceivePkt: endPointId: %d, buffer: 0x%X, length: %d\n",
                    pPacket->Endpoint, (A_UINT32)pPacket->pBuffer, pPacket->BufferLength));

    do {

        if (HTC_STOPPING(target)) {
            status = A_ECANCELED;
            break;
        }

        AR_DEBUG_ASSERT(pPacket->Endpoint < ENDPOINT_MAX);

        pEndpoint = &target->EndPoint[pPacket->Endpoint];

        LOCK_HTC_RX(target);

            /* store receive packet */
        HTC_PACKET_ENQUEUE(&pEndpoint->RxBuffers, pPacket);

            /* check if we are blocked waiting for a new buffer */
        if (target->HTCStateFlags & HTC_STATE_WAIT_BUFFERS) {
            if (target->EpWaitingForBuffers == pPacket->Endpoint) {
                AR_DEBUG_PRINTF(ATH_DEBUG_RECV,(" receiver was blocked on ep:%d, unblocking.. \n",
                    target->EpWaitingForBuffers));
                target->HTCStateFlags &= ~HTC_STATE_WAIT_BUFFERS;
                target->EpWaitingForBuffers = ENDPOINT_MAX;
                unblockRecv = TRUE;
            }
        }

        UNLOCK_HTC_RX(target);

        if (unblockRecv && !HTC_STOPPING(target)) {
                /* TODO : implement a buffer threshold count? */
            DevEnableRecv(&target->Device,DEV_ENABLE_RECV_SYNC);
        }

    } while (FALSE);

    return status;
}

static void HTCFlushEndpointRX(HTC_TARGET *target, HTC_ENDPOINT *pEndpoint)
{
    HTC_PACKET  *pPacket;

    LOCK_HTC_RX(target);

    while (1) {
        pPacket = HTC_PACKET_DEQUEUE(&pEndpoint->RxBuffers);
        if (NULL == pPacket) {
            break;
        }
        UNLOCK_HTC_RX(target);
        pPacket->Status = A_ECANCELED;
        pPacket->ActualLength = 0;
        AR_DEBUG_PRINTF(ATH_DEBUG_RECV, ("  Flushing RX packet:0x%X, length:%d, ep:%d \n",
                (A_UINT32)pPacket, pPacket->BufferLength, pPacket->Endpoint));
            /* give the packet back */
        pEndpoint->EpCallBacks.EpRecv(pEndpoint->EpCallBacks.pContext,
                                      pPacket);
        LOCK_HTC_RX(target);
    }

    UNLOCK_HTC_RX(target);


}

void HTCFlushRecvBuffers(HTC_TARGET *target)
{
    HTC_ENDPOINT    *pEndpoint;
    int             i;

        /* NOTE: no need to flush endpoint 0, these buffers were
         * allocated as part of the HTC struct */
    for (i = ENDPOINT_1; i < ENDPOINT_MAX; i++) {
        pEndpoint = &target->EndPoint[i];
        if (pEndpoint->ServiceID == 0) {
                /* not in use.. */
            continue;
        }
        HTCFlushEndpointRX(target,pEndpoint);
    }


}


