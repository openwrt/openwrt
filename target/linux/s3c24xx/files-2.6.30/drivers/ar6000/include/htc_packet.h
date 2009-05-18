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

#ifndef HTC_PACKET_H_
#define HTC_PACKET_H_


#include "dl_list.h"

struct _HTC_PACKET;

typedef void (* HTC_PACKET_COMPLETION)(void *,struct _HTC_PACKET *);

typedef A_UINT16 HTC_TX_TAG;

typedef struct _HTC_TX_PACKET_INFO {
    HTC_TX_TAG    Tag;            /* tag used to selective flush packets */
} HTC_TX_PACKET_INFO;

#define HTC_TX_PACKET_TAG_ALL          0    /* a tag of zero is reserved and used to flush ALL packets */
#define HTC_TX_PACKET_TAG_INTERNAL     1                                /* internal tags start here */
#define HTC_TX_PACKET_TAG_USER_DEFINED (HTC_TX_PACKET_TAG_INTERNAL + 9) /* user-defined tags start here */

typedef struct _HTC_RX_PACKET_INFO {
    A_UINT32    Unused;          /* for future use and to make compilers happy */
} HTC_RX_PACKET_INFO;

/* wrapper around endpoint-specific packets */
typedef struct _HTC_PACKET {
    DL_LIST         ListLink;       /* double link */
    void            *pPktContext;   /* caller's per packet specific context */

    A_UINT8         *pBufferStart;  /* the true buffer start , the caller can
                                       store the real buffer start here.  In
                                       receive callbacks, the HTC layer sets pBuffer
                                       to the start of the payload past the header. This
                                       field allows the caller to reset pBuffer when it
                                       recycles receive packets back to HTC */
    /*
     * Pointer to the start of the buffer. In the transmit
     * direction this points to the start of the payload. In the
     * receive direction, however, the buffer when queued up
     * points to the start of the HTC header but when returned
     * to the caller points to the start of the payload
     */
    A_UINT8         *pBuffer;       /* payload start (RX/TX) */
    A_UINT32        BufferLength;   /* length of buffer */
    A_UINT32        ActualLength;   /* actual length of payload */
    int             Endpoint;       /* endpoint that this packet was sent/recv'd from */
    A_STATUS        Status;         /* completion status */
    union {
        HTC_TX_PACKET_INFO  AsTx;   /* Tx Packet specific info */
        HTC_RX_PACKET_INFO  AsRx;   /* Rx Packet specific info */
    } PktInfo;

    /* the following fields are for internal HTC use */
    HTC_PACKET_COMPLETION Completion;   /* completion */
    void                  *pContext;    /* HTC private completion context */
    A_UINT32              HTCReserved;  /* reserved */
} HTC_PACKET;



#define COMPLETE_HTC_PACKET(p,status)        \
{                                            \
    (p)->Status = (status);                  \
    (p)->Completion((p)->pContext,(p));      \
}

#define INIT_HTC_PACKET_INFO(p,b,len)             \
{                                                 \
    (p)->pBufferStart = (b);                      \
    (p)->BufferLength = (len);                    \
}

/* macro to set an initial RX packet for refilling HTC */
#define SET_HTC_PACKET_INFO_RX_REFILL(p,c,b,len,ep) \
{                                                 \
    (p)->pPktContext = (c);                       \
    (p)->pBuffer = (b);                           \
    (p)->pBufferStart = (b);                      \
    (p)->BufferLength = (len);                    \
    (p)->Endpoint = (ep);                         \
}

/* fast macro to recycle an RX packet that will be re-queued to HTC */
#define HTC_PACKET_RESET_RX(p)              \
    (p)->pBuffer = (p)->pBufferStart

/* macro to set packet parameters for TX */
#define SET_HTC_PACKET_INFO_TX(p,c,b,len,ep,tag)  \
{                                                 \
    (p)->pPktContext = (c);                       \
    (p)->pBuffer = (b);                           \
    (p)->ActualLength = (len);                    \
    (p)->Endpoint = (ep);                         \
    (p)->PktInfo.AsTx.Tag = (tag);                \
}

/* HTC Packet Queueing Macros */
typedef DL_LIST HTC_PACKET_QUEUE;
/* initialize queue */
#define INIT_HTC_PACKET_QUEUE(pQ) DL_LIST_INIT((pQ))
/* enqueue HTC packet to the tail of the queue */
#define HTC_PACKET_ENQUEUE(pQ,p)  DL_ListInsertTail((pQ),&(p)->ListLink)
/* test if a queue is empty */
#define HTC_QUEUE_EMPTY(pQ)       DL_LIST_IS_EMPTY((pQ))
/* get packet at head without removing it */
#define HTC_GET_PKT_AT_HEAD(pQ)   A_CONTAINING_STRUCT((DL_LIST_GET_ITEM_AT_HEAD(pQ)),HTC_PACKET,ListLink);
/* remove a packet from the current list it is linked to */
#define HTC_PACKET_REMOVE(p)      DL_ListRemove(&(p)->ListLink)

/* dequeue an HTC packet from the head of the queue */
static INLINE HTC_PACKET *HTC_PACKET_DEQUEUE(HTC_PACKET_QUEUE *queue) {
    DL_LIST    *pItem = DL_ListRemoveItemFromHead(queue);
    if (pItem != NULL) {
        return A_CONTAINING_STRUCT(pItem, HTC_PACKET, ListLink);
    }
    return NULL;
}

#endif /*HTC_PACKET_H_*/
