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

#define DO_EP_TX_COMPLETION(ep,p)                                    \
{                                                                    \
    (p)->Completion = NULL;                                          \
    (ep)->EpCallBacks.EpTxComplete((ep)->EpCallBacks.pContext,(p));  \
}


/* call the distribute credits callback with the distribution */
#define DO_DISTRIBUTION(t,reason,description,pList) \
{                                             \
    AR_DEBUG_PRINTF(ATH_DEBUG_SEND,           \
        ("  calling distribute function (%s) (dfn:0x%X, ctxt:0x%X, dist:0x%X) \n", \
                (description),                                           \
                (A_UINT32)(t)->DistributeCredits,                        \
                (A_UINT32)(t)->pCredDistContext,                         \
                (A_UINT32)pList));                                       \
    (t)->DistributeCredits((t)->pCredDistContext,                        \
                           (pList),                                      \
                           (reason));                                    \
}

/* our internal send packet completion handler when packets are submited to the AR6K device
 * layer */
static void HTCSendPktCompletionHandler(void *Context, HTC_PACKET *pPacket)
{
    HTC_TARGET      *target = (HTC_TARGET *)Context;
    HTC_ENDPOINT    *pEndpoint = &target->EndPoint[pPacket->Endpoint];


    if (A_FAILED(pPacket->Status)) {
        AR_DEBUG_PRINTF(ATH_DEBUG_ERR,
            ("HTCSendPktCompletionHandler: request failed (status:%d, ep:%d) \n",
                pPacket->Status, pPacket->Endpoint));
    }
        /* first, fixup the head room we allocated */
    pPacket->pBuffer += HTC_HDR_LENGTH;
        /* do completion */
    DO_EP_TX_COMPLETION(pEndpoint,pPacket);
}

A_STATUS HTCIssueSend(HTC_TARGET *target, HTC_PACKET *pPacket, A_UINT8 SendFlags)
{
    A_STATUS status;
    A_UINT8 *pHdrBuf;
    A_BOOL   sync = FALSE;

        /* caller always provides headrooom */
    pPacket->pBuffer -= HTC_HDR_LENGTH;
    pHdrBuf = pPacket->pBuffer;
        /* setup frame header */
    A_SET_UINT16_FIELD(pHdrBuf,HTC_FRAME_HDR,PayloadLen,(A_UINT16)pPacket->ActualLength);
    A_SET_UINT8_FIELD(pHdrBuf,HTC_FRAME_HDR,Flags,SendFlags);
    A_SET_UINT8_FIELD(pHdrBuf,HTC_FRAME_HDR,EndpointID, (A_UINT8)pPacket->Endpoint);

    if (pPacket->Completion == NULL) {
            /* mark that this request was synchronously issued */
        sync = TRUE;
    }

    AR_DEBUG_PRINTF(ATH_DEBUG_SEND,
                    ("+-HTCIssueSend: transmit length : %d (%s) \n",
                    pPacket->ActualLength + HTC_HDR_LENGTH,
                    sync ? "SYNC" : "ASYNC" ));

        /* send message to device */
    status = DevSendPacket(&target->Device,
                           pPacket,
                           pPacket->ActualLength + HTC_HDR_LENGTH);

    if (sync) {
            /* use local sync variable.  If this was issued asynchronously, pPacket is no longer
             * safe to access. */
        pPacket->pBuffer += HTC_HDR_LENGTH;
    }

    /* if this request was asynchronous, the packet completion routine will be invoked by
     * the device layer when the HIF layer completes the request */

    return status;
}

/* try to send the current packet or a packet at the head of the TX queue,
 * if there are no credits, the packet remains in the queue. */
static void HTCTrySend(HTC_TARGET      *target,
                       HTC_PACKET      *pPacketToSend,
                       HTC_ENDPOINT_ID ep)
{
    HTC_PACKET   *pPacket;
    HTC_ENDPOINT *pEndpoint;
    int          creditsRequired;
    A_UINT8      sendFlags;

    AR_DEBUG_PRINTF(ATH_DEBUG_SEND,("+HTCTrySend (pPkt:0x%X)\n",(A_UINT32)pPacketToSend));

    pEndpoint = &target->EndPoint[ep];

    LOCK_HTC_TX(target);

    if (pPacketToSend != NULL) {
        /* caller supplied us a packet to queue to the tail of the HTC TX queue before
         * we check the tx queue */
        HTC_PACKET_ENQUEUE(&pEndpoint->TxQueue,pPacketToSend);
        pEndpoint->CurrentTxQueueDepth++;
    }

        /* now drain the TX queue for transmission as long as we have enough
         * credits */

    while (1) {

        if (HTC_QUEUE_EMPTY(&pEndpoint->TxQueue)) {
                /* nothing in the queue */
            break;
        }

        sendFlags = 0;

            /* get packet at head, but don't remove it */
        pPacket = HTC_GET_PKT_AT_HEAD(&pEndpoint->TxQueue);
        AR_DEBUG_PRINTF(ATH_DEBUG_SEND,(" Got head packet:0x%X , Queue Depth: %d\n",
                (A_UINT32)pPacket, pEndpoint->CurrentTxQueueDepth));

            /* figure out how many credits this message requires */
        creditsRequired  = pPacket->ActualLength + HTC_HDR_LENGTH;
        creditsRequired += target->TargetCreditSize - 1;
        creditsRequired /= target->TargetCreditSize;

        AR_DEBUG_PRINTF(ATH_DEBUG_SEND,(" Creds Required:%d   Got:%d\n",
                            creditsRequired, pEndpoint->CreditDist.TxCredits));

        if (pEndpoint->CreditDist.TxCredits < creditsRequired) {

            /* not enough credits */

            if (pPacket->Endpoint == ENDPOINT_0) {
                    /* leave it in the queue */
                break;
            }
                /* invoke the registered distribution function only if this is not
                 * endpoint 0, we let the driver layer provide more credits if it can.
                 * We pass the credit distribution list starting at the endpoint in question
                 * */

                /* set how many credits we need  */
            pEndpoint->CreditDist.TxCreditsSeek =
                                    creditsRequired - pEndpoint->CreditDist.TxCredits;
            DO_DISTRIBUTION(target,
                            HTC_CREDIT_DIST_SEEK_CREDITS,
                            "Seek Credits",
                            &pEndpoint->CreditDist);

            pEndpoint->CreditDist.TxCreditsSeek = 0;

            if (pEndpoint->CreditDist.TxCredits < creditsRequired) {
                    /* still not enough credits to send, leave packet in the queue */
                AR_DEBUG_PRINTF(ATH_DEBUG_SEND,
                    (" Not enough credits for ep %d leaving packet in queue..\n",
                    pPacket->Endpoint));
                break;
            }

        }

        pEndpoint->CreditDist.TxCredits -= creditsRequired;
        INC_HTC_EP_STAT(pEndpoint, TxCreditsConsummed, creditsRequired);

            /* check if we need credits */
        if (pEndpoint->CreditDist.TxCredits < pEndpoint->CreditDist.TxCreditsPerMaxMsg) {
            sendFlags |= HTC_FLAGS_NEED_CREDIT_UPDATE;
            INC_HTC_EP_STAT(pEndpoint, TxCreditLowIndications, 1);
            AR_DEBUG_PRINTF(ATH_DEBUG_SEND,(" Host Needs Credits  \n"));
        }

            /* now we can fully dequeue */
        pPacket = HTC_PACKET_DEQUEUE(&pEndpoint->TxQueue);
        pEndpoint->CurrentTxQueueDepth--;

        INC_HTC_EP_STAT(pEndpoint, TxIssued, 1);

        UNLOCK_HTC_TX(target);

        HTCIssueSend(target, pPacket, sendFlags);

        LOCK_HTC_TX(target);

        /* go back and check for more messages */
    }

    if (pEndpoint->CurrentTxQueueDepth >= pEndpoint->MaxTxQueueDepth) {
        AR_DEBUG_PRINTF(ATH_DEBUG_SEND, (" Endpoint %d, TX queue is full, Depth:%d, Max:%d \n",
                        ep, pEndpoint->CurrentTxQueueDepth, pEndpoint->MaxTxQueueDepth));
        UNLOCK_HTC_TX(target);
            /* queue is now full, let caller know */
        if (pEndpoint->EpCallBacks.EpSendFull != NULL) {
            AR_DEBUG_PRINTF(ATH_DEBUG_SEND, (" Calling driver's send full callback.... \n"));
            pEndpoint->EpCallBacks.EpSendFull(pEndpoint->EpCallBacks.pContext, ep);
        }
    } else {
        UNLOCK_HTC_TX(target);
            /* queue is now available for new packet, let caller know */
        if (pEndpoint->EpCallBacks.EpSendAvail)
            pEndpoint->EpCallBacks.EpSendAvail(pEndpoint->EpCallBacks.pContext, ep);
    }

    AR_DEBUG_PRINTF(ATH_DEBUG_SEND,("-HTCTrySend:  \n"));
}

/* HTC API - HTCSendPkt */
A_STATUS HTCSendPkt(HTC_HANDLE HTCHandle, HTC_PACKET *pPacket)
{
    HTC_TARGET      *target = GET_HTC_TARGET_FROM_HANDLE(HTCHandle);
    HTC_ENDPOINT    *pEndpoint;
    HTC_ENDPOINT_ID ep;
    A_STATUS        status = A_OK;

    AR_DEBUG_PRINTF(ATH_DEBUG_SEND,
                    ("+HTCSendPkt: Enter endPointId: %d, buffer: 0x%X, length: %d \n",
                    pPacket->Endpoint, (A_UINT32)pPacket->pBuffer, pPacket->ActualLength));

    ep = pPacket->Endpoint;
    AR_DEBUG_ASSERT(ep < ENDPOINT_MAX);
    pEndpoint = &target->EndPoint[ep];

    do {

        if (HTC_STOPPING(target)) {
            status = A_ECANCELED;
            pPacket->Status = status;
            DO_EP_TX_COMPLETION(pEndpoint,pPacket);
            break;
        }
            /* everything sent through this interface is asynchronous */
            /* fill in HTC completion routines */
        pPacket->Completion = HTCSendPktCompletionHandler;
        pPacket->pContext = target;

        HTCTrySend(target, pPacket, ep);

    } while (FALSE);

    AR_DEBUG_PRINTF(ATH_DEBUG_SEND, ("-HTCSendPkt \n"));

    return status;
}


/* check TX queues to drain because of credit distribution update */
static INLINE void HTCCheckEndpointTxQueues(HTC_TARGET *target)
{
    HTC_ENDPOINT                *pEndpoint;
    HTC_ENDPOINT_CREDIT_DIST    *pDistItem;

    AR_DEBUG_PRINTF(ATH_DEBUG_SEND, ("+HTCCheckEndpointTxQueues \n"));
    pDistItem = target->EpCreditDistributionListHead;

        /* run through the credit distribution list to see
         * if there are packets queued
         * NOTE: no locks need to be taken since the distribution list
         * is not dynamic (cannot be re-ordered) and we are not modifying any state */
    while (pDistItem != NULL) {
        pEndpoint = (HTC_ENDPOINT *)pDistItem->pHTCReserved;

        if (pEndpoint->CurrentTxQueueDepth > 0) {
            AR_DEBUG_PRINTF(ATH_DEBUG_SEND, (" Ep %d has %d credits and %d Packets in TX Queue \n",
                    pDistItem->Endpoint, pEndpoint->CreditDist.TxCredits, pEndpoint->CurrentTxQueueDepth));
                /* try to start the stalled queue, this list is ordered by priority.
                 * Highest priority queue get's processed first, if there are credits available the
                 * highest priority queue will get a chance to reclaim credits from lower priority
                 * ones */
            HTCTrySend(target, NULL, pDistItem->Endpoint);
        }

        pDistItem = pDistItem->pNext;
    }

    AR_DEBUG_PRINTF(ATH_DEBUG_SEND, ("-HTCCheckEndpointTxQueues \n"));
}

/* process credit reports and call distribution function */
void HTCProcessCreditRpt(HTC_TARGET *target, HTC_CREDIT_REPORT *pRpt, int NumEntries, HTC_ENDPOINT_ID FromEndpoint)
{
    int             i;
    HTC_ENDPOINT    *pEndpoint;
    int             totalCredits = 0;
    A_BOOL          doDist = FALSE;

    AR_DEBUG_PRINTF(ATH_DEBUG_SEND, ("+HTCProcessCreditRpt, Credit Report Entries:%d \n", NumEntries));

        /* lock out TX while we update credits */
    LOCK_HTC_TX(target);

    for (i = 0; i < NumEntries; i++, pRpt++) {
        if (pRpt->EndpointID >= ENDPOINT_MAX) {
            AR_DEBUG_ASSERT(FALSE);
            break;
        }

        pEndpoint = &target->EndPoint[pRpt->EndpointID];

        AR_DEBUG_PRINTF(ATH_DEBUG_SEND, ("  Endpoint %d got %d credits \n",
                pRpt->EndpointID, pRpt->Credits));


#ifdef HTC_EP_STAT_PROFILING

        INC_HTC_EP_STAT(pEndpoint, TxCreditRpts, 1);
        INC_HTC_EP_STAT(pEndpoint, TxCreditsReturned, pRpt->Credits);

        if (FromEndpoint == pRpt->EndpointID) {
                /* this credit report arrived on the same endpoint indicating it arrived in an RX
                 * packet */
            INC_HTC_EP_STAT(pEndpoint, TxCreditsFromRx, pRpt->Credits);
            INC_HTC_EP_STAT(pEndpoint, TxCreditRptsFromRx, 1);
        } else if (FromEndpoint == ENDPOINT_0) {
                /* this credit arrived on endpoint 0 as a NULL message */
            INC_HTC_EP_STAT(pEndpoint, TxCreditsFromEp0, pRpt->Credits);
            INC_HTC_EP_STAT(pEndpoint, TxCreditRptsFromEp0, 1);
        } else {
                /* arrived on another endpoint */
            INC_HTC_EP_STAT(pEndpoint, TxCreditsFromOther, pRpt->Credits);
            INC_HTC_EP_STAT(pEndpoint, TxCreditRptsFromOther, 1);
        }

#endif

        if (ENDPOINT_0 == pRpt->EndpointID) {
                /* always give endpoint 0 credits back */
            pEndpoint->CreditDist.TxCredits += pRpt->Credits;
        } else {
                /* for all other endpoints, update credits to distribute, the distribution function
                 * will handle giving out credits back to the endpoints */
            pEndpoint->CreditDist.TxCreditsToDist += pRpt->Credits;
                /* flag that we have to do the distribution */
            doDist = TRUE;
        }

        totalCredits += pRpt->Credits;
    }

    AR_DEBUG_PRINTF(ATH_DEBUG_SEND, ("  Report indicated %d credits to distribute \n", totalCredits));

    if (doDist) {
            /* this was a credit return based on a completed send operations
             * note, this is done with the lock held */
        DO_DISTRIBUTION(target,
                        HTC_CREDIT_DIST_SEND_COMPLETE,
                        "Send Complete",
                        target->EpCreditDistributionListHead->pNext);
    }

    UNLOCK_HTC_TX(target);

    if (totalCredits) {
        HTCCheckEndpointTxQueues(target);
    }

    AR_DEBUG_PRINTF(ATH_DEBUG_SEND, ("-HTCProcessCreditRpt \n"));
}

/* flush endpoint TX queue */
static void HTCFlushEndpointTX(HTC_TARGET *target, HTC_ENDPOINT *pEndpoint, HTC_TX_TAG Tag)
{
    HTC_PACKET          *pPacket;
    HTC_PACKET_QUEUE    discardQueue;

        /* initialize the discard queue */
    INIT_HTC_PACKET_QUEUE(&discardQueue);

    LOCK_HTC_TX(target);

        /* interate from the front of the TX queue and flush out packets */
    ITERATE_OVER_LIST_ALLOW_REMOVE(&pEndpoint->TxQueue, pPacket, HTC_PACKET, ListLink) {

            /* check for removal */
        if ((HTC_TX_PACKET_TAG_ALL == Tag) || (Tag == pPacket->PktInfo.AsTx.Tag)) {
                /* remove from queue */
            HTC_PACKET_REMOVE(pPacket);
                /* add it to the discard pile */
            HTC_PACKET_ENQUEUE(&discardQueue, pPacket);
            pEndpoint->CurrentTxQueueDepth--;
        }

    } ITERATE_END;

    UNLOCK_HTC_TX(target);

        /* empty the discard queue */
    while (1) {
        pPacket = HTC_PACKET_DEQUEUE(&discardQueue);
        if (NULL == pPacket) {
            break;
        }
        pPacket->Status = A_ECANCELED;
        AR_DEBUG_PRINTF(ATH_DEBUG_TRC, ("  Flushing TX packet:0x%X, length:%d, ep:%d tag:0x%X \n",
                (A_UINT32)pPacket, pPacket->ActualLength, pPacket->Endpoint, pPacket->PktInfo.AsTx.Tag));
        DO_EP_TX_COMPLETION(pEndpoint,pPacket);
    }

}

void DumpCreditDist(HTC_ENDPOINT_CREDIT_DIST *pEPDist)
{
#ifdef DEBUG
    HTC_ENDPOINT *pEndpoint = (HTC_ENDPOINT *)pEPDist->pHTCReserved;
#endif

    AR_DEBUG_PRINTF(ATH_DEBUG_ANY, ("--- EP : %d  ServiceID: 0x%X    --------------\n",
                        pEPDist->Endpoint, pEPDist->ServiceID));
    AR_DEBUG_PRINTF(ATH_DEBUG_ANY, (" this:0x%X next:0x%X prev:0x%X\n",
                (A_UINT32)pEPDist, (A_UINT32)pEPDist->pNext, (A_UINT32)pEPDist->pPrev));
    AR_DEBUG_PRINTF(ATH_DEBUG_ANY, (" DistFlags          : 0x%X \n", pEPDist->DistFlags));
    AR_DEBUG_PRINTF(ATH_DEBUG_ANY, (" TxCreditsNorm      : %d \n", pEPDist->TxCreditsNorm));
    AR_DEBUG_PRINTF(ATH_DEBUG_ANY, (" TxCreditsMin       : %d \n", pEPDist->TxCreditsMin));
    AR_DEBUG_PRINTF(ATH_DEBUG_ANY, (" TxCredits          : %d \n", pEPDist->TxCredits));
    AR_DEBUG_PRINTF(ATH_DEBUG_ANY, (" TxCreditsAssigned  : %d \n", pEPDist->TxCreditsAssigned));
    AR_DEBUG_PRINTF(ATH_DEBUG_ANY, (" TxCreditsSeek      : %d \n", pEPDist->TxCreditsSeek));
    AR_DEBUG_PRINTF(ATH_DEBUG_ANY, (" TxCreditSize       : %d \n", pEPDist->TxCreditSize));
    AR_DEBUG_PRINTF(ATH_DEBUG_ANY, (" TxCreditsPerMaxMsg : %d \n", pEPDist->TxCreditsPerMaxMsg));
    AR_DEBUG_PRINTF(ATH_DEBUG_ANY, (" TxCreditsToDist    : %d \n", pEPDist->TxCreditsToDist));
    AR_DEBUG_PRINTF(ATH_DEBUG_ANY, (" TxQueueDepth       : %d \n", pEndpoint->CurrentTxQueueDepth));
    AR_DEBUG_PRINTF(ATH_DEBUG_ANY, ("----------------------------------------------------\n"));
}

void DumpCreditDistStates(HTC_TARGET *target)
{
    HTC_ENDPOINT_CREDIT_DIST *pEPList = target->EpCreditDistributionListHead;

    while (pEPList != NULL) {
        DumpCreditDist(pEPList);
        pEPList = pEPList->pNext;
    }

    if (target->DistributeCredits != NULL) {
        DO_DISTRIBUTION(target,
                        HTC_DUMP_CREDIT_STATE,
                        "Dump State",
                        NULL);
    }
}

/* flush all send packets from all endpoint queues */
void HTCFlushSendPkts(HTC_TARGET *target)
{
    HTC_ENDPOINT    *pEndpoint;
    int             i;

    DumpCreditDistStates(target);

    for (i = ENDPOINT_0; i < ENDPOINT_MAX; i++) {
        pEndpoint = &target->EndPoint[i];
        if (pEndpoint->ServiceID == 0) {
                /* not in use.. */
            continue;
        }
        HTCFlushEndpointTX(target,pEndpoint,HTC_TX_PACKET_TAG_ALL);
    }

}

/* HTC API to flush an endpoint's TX queue*/
void HTCFlushEndpoint(HTC_HANDLE HTCHandle, HTC_ENDPOINT_ID Endpoint, HTC_TX_TAG Tag)
{
    HTC_TARGET      *target = GET_HTC_TARGET_FROM_HANDLE(HTCHandle);
    HTC_ENDPOINT    *pEndpoint = &target->EndPoint[Endpoint];

    if (pEndpoint->ServiceID == 0) {
        AR_DEBUG_ASSERT(FALSE);
        /* not in use.. */
        return;
    }

    HTCFlushEndpointTX(target, pEndpoint, Tag);
}

/* HTC API to indicate activity to the credit distribution function */
void HTCIndicateActivityChange(HTC_HANDLE      HTCHandle,
                               HTC_ENDPOINT_ID Endpoint,
                               A_BOOL          Active)
{
    HTC_TARGET      *target = GET_HTC_TARGET_FROM_HANDLE(HTCHandle);
    HTC_ENDPOINT    *pEndpoint = &target->EndPoint[Endpoint];
    A_BOOL          doDist = FALSE;

    if (pEndpoint->ServiceID == 0) {
        AR_DEBUG_ASSERT(FALSE);
        /* not in use.. */
        return;
    }

    LOCK_HTC_TX(target);

    if (Active) {
        if (!(pEndpoint->CreditDist.DistFlags & HTC_EP_ACTIVE)) {
                /* mark active now */
            pEndpoint->CreditDist.DistFlags |= HTC_EP_ACTIVE;
            doDist = TRUE;
        }
    } else {
        if (pEndpoint->CreditDist.DistFlags & HTC_EP_ACTIVE) {
                /* mark inactive now */
            pEndpoint->CreditDist.DistFlags &= ~HTC_EP_ACTIVE;
            doDist = TRUE;
        }
    }

    if (doDist) {
        /* do distribution again based on activity change
         * note, this is done with the lock held */
        DO_DISTRIBUTION(target,
                        HTC_CREDIT_DIST_ACTIVITY_CHANGE,
                        "Activity Change",
                        target->EpCreditDistributionListHead->pNext);
    }

    UNLOCK_HTC_TX(target);

}
