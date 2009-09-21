/*
 *
 * Copyright (c) 2004-2007 Atheros Communications Inc.
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

#include "ar6000_drv.h"

#ifdef HTC_RAW_INTERFACE

static void
ar6000_htc_raw_read_cb(void *Context, HTC_PACKET *pPacket)
{
    AR_SOFTC_T        *ar = (AR_SOFTC_T *)Context;
    raw_htc_buffer    *busy;
    HTC_RAW_STREAM_ID streamID;

    busy = (raw_htc_buffer *)pPacket->pPktContext;
    A_ASSERT(busy != NULL);

    if (pPacket->Status == A_ECANCELED) {
        /*
         * HTC provides A_ECANCELED status when it doesn't want to be refilled
         * (probably due to a shutdown)
         */
        return;
    }

    streamID = arEndpoint2RawStreamID(ar,pPacket->Endpoint);
    A_ASSERT(streamID != HTC_RAW_STREAM_NOT_MAPPED);

#ifdef CF
   if (down_trylock(&ar->raw_htc_read_sem[streamID])) {
#else
    if (down_interruptible(&ar->raw_htc_read_sem[streamID])) {
#endif /* CF */
        AR_DEBUG2_PRINTF("Unable to down the semaphore\n");
    }

    A_ASSERT((pPacket->Status != A_OK) ||
             (pPacket->pBuffer == (busy->data + HTC_HEADER_LEN)));

    busy->length = pPacket->ActualLength + HTC_HEADER_LEN;
    busy->currPtr = HTC_HEADER_LEN;
    ar->read_buffer_available[streamID] = TRUE;
    //AR_DEBUG_PRINTF("raw read cb:  0x%X 0x%X \n", busy->currPtr,busy->length);
    up(&ar->raw_htc_read_sem[streamID]);

    /* Signal the waiting process */
    AR_DEBUG2_PRINTF("Waking up the StreamID(%d) read process\n", streamID);
    wake_up_interruptible(&ar->raw_htc_read_queue[streamID]);
}

static void
ar6000_htc_raw_write_cb(void *Context, HTC_PACKET *pPacket)
{
    AR_SOFTC_T          *ar = (AR_SOFTC_T  *)Context;
    raw_htc_buffer      *free;
    HTC_RAW_STREAM_ID   streamID;

    free = (raw_htc_buffer *)pPacket->pPktContext;
    A_ASSERT(free != NULL);

    if (pPacket->Status == A_ECANCELED) {
        /*
         * HTC provides A_ECANCELED status when it doesn't want to be refilled
         * (probably due to a shutdown)
         */
        return;
    }

    streamID = arEndpoint2RawStreamID(ar,pPacket->Endpoint);
    A_ASSERT(streamID != HTC_RAW_STREAM_NOT_MAPPED);

#ifdef CF
    if (down_trylock(&ar->raw_htc_write_sem[streamID])) {
#else
    if (down_interruptible(&ar->raw_htc_write_sem[streamID])) {
#endif
        AR_DEBUG2_PRINTF("Unable to down the semaphore\n");
    }

    A_ASSERT(pPacket->pBuffer == (free->data + HTC_HEADER_LEN));

    free->length = 0;
    ar->write_buffer_available[streamID] = TRUE;
    up(&ar->raw_htc_write_sem[streamID]);

    /* Signal the waiting process */
    AR_DEBUG2_PRINTF("Waking up the StreamID(%d) write process\n", streamID);
    wake_up_interruptible(&ar->raw_htc_write_queue[streamID]);
}

/* connect to a service */
static A_STATUS ar6000_connect_raw_service(AR_SOFTC_T        *ar,
                                           HTC_RAW_STREAM_ID StreamID)
{
    A_STATUS                 status;
    HTC_SERVICE_CONNECT_RESP response;
    A_UINT8                  streamNo;
    HTC_SERVICE_CONNECT_REQ  connect;

    do {

        A_MEMZERO(&connect,sizeof(connect));
            /* pass the stream ID as meta data to the RAW streams service */
        streamNo = (A_UINT8)StreamID;
        connect.pMetaData = &streamNo;
        connect.MetaDataLength = sizeof(A_UINT8);
            /* these fields are the same for all endpoints */
        connect.EpCallbacks.pContext = ar;
        connect.EpCallbacks.EpTxComplete = ar6000_htc_raw_write_cb;
        connect.EpCallbacks.EpRecv = ar6000_htc_raw_read_cb;
            /* simple interface, we don't need these optional callbacks */
        connect.EpCallbacks.EpRecvRefill = NULL;
        connect.EpCallbacks.EpSendFull = NULL;
        connect.EpCallbacks.EpSendAvail = NULL;
        connect.MaxSendQueueDepth = RAW_HTC_WRITE_BUFFERS_NUM;

            /* connect to the raw streams service, we may be able to get 1 or more
             * connections, depending on WHAT is running on the target */
        connect.ServiceID = HTC_RAW_STREAMS_SVC;

        A_MEMZERO(&response,sizeof(response));

            /* try to connect to the raw stream, it is okay if this fails with
             * status HTC_SERVICE_NO_MORE_EP */
        status = HTCConnectService(ar->arHtcTarget,
                                   &connect,
                                   &response);

        if (A_FAILED(status)) {
            if (response.ConnectRespCode == HTC_SERVICE_NO_MORE_EP) {
                AR_DEBUG_PRINTF("HTC RAW , No more streams allowed \n");
                status = A_OK;
            }
            break;
        }

            /* set endpoint mapping for the RAW HTC streams */
        arSetRawStream2EndpointIDMap(ar,StreamID,response.Endpoint);

        AR_DEBUG_PRINTF("HTC RAW : stream ID: %d, endpoint: %d\n",
                        StreamID, arRawStream2EndpointID(ar,StreamID));

    } while (FALSE);

    return status;
}

int ar6000_htc_raw_open(AR_SOFTC_T *ar)
{
    A_STATUS status;
    int streamID, endPt, count2;
    raw_htc_buffer *buffer;
    HTC_SERVICE_ID servicepriority;

    A_ASSERT(ar->arHtcTarget != NULL);

        /* wait for target */
    status = HTCWaitTarget(ar->arHtcTarget);

    if (A_FAILED(status)) {
        AR_DEBUG_PRINTF("HTCWaitTarget failed (%d)\n", status);
        return -ENODEV;
    }

    for (endPt = 0; endPt < ENDPOINT_MAX; endPt++) {
        ar->arEp2RawMapping[endPt] = HTC_RAW_STREAM_NOT_MAPPED;
    }

    for (streamID = HTC_RAW_STREAM_0; streamID < HTC_RAW_STREAM_NUM_MAX; streamID++) {
        /* Initialize the data structures */
        init_MUTEX(&ar->raw_htc_read_sem[streamID]);
        init_MUTEX(&ar->raw_htc_write_sem[streamID]);
        init_waitqueue_head(&ar->raw_htc_read_queue[streamID]);
        init_waitqueue_head(&ar->raw_htc_write_queue[streamID]);

            /* try to connect to the raw service */
        status = ar6000_connect_raw_service(ar,streamID);

        if (A_FAILED(status)) {
            break;
        }

        if (arRawStream2EndpointID(ar,streamID) == 0) {
            break;
        }

        for (count2 = 0; count2 < RAW_HTC_READ_BUFFERS_NUM; count2 ++) {
            /* Initialize the receive buffers */
            buffer = ar->raw_htc_write_buffer[streamID][count2];
            memset(buffer, 0, sizeof(raw_htc_buffer));
            buffer = ar->raw_htc_read_buffer[streamID][count2];
            memset(buffer, 0, sizeof(raw_htc_buffer));

            SET_HTC_PACKET_INFO_RX_REFILL(&buffer->HTCPacket,
                                          buffer,
                                          buffer->data,
                                          AR6000_BUFFER_SIZE,
                                          arRawStream2EndpointID(ar,streamID));

            /* Queue buffers to HTC for receive */
            if ((status = HTCAddReceivePkt(ar->arHtcTarget, &buffer->HTCPacket)) != A_OK)
            {
                BMIInit();
                return -EIO;
            }
        }

        for (count2 = 0; count2 < RAW_HTC_WRITE_BUFFERS_NUM; count2 ++) {
            /* Initialize the receive buffers */
            buffer = ar->raw_htc_write_buffer[streamID][count2];
            memset(buffer, 0, sizeof(raw_htc_buffer));
        }

        ar->read_buffer_available[streamID] = FALSE;
        ar->write_buffer_available[streamID] = TRUE;
    }

    if (A_FAILED(status)) {
        return -EIO;
    }

    AR_DEBUG_PRINTF("HTC RAW, number of streams the target supports: %d \n", streamID);

    servicepriority = HTC_RAW_STREAMS_SVC;  /* only 1 */

        /* set callbacks and priority list */
    HTCSetCreditDistribution(ar->arHtcTarget,
                             ar,
                             NULL,  /* use default */
                             NULL,  /* use default */
                             &servicepriority,
                             1);

    /* Start the HTC component */
    if ((status = HTCStart(ar->arHtcTarget)) != A_OK) {
        BMIInit();
        return -EIO;
    }

    (ar)->arRawIfInit = TRUE;

    return 0;
}

int ar6000_htc_raw_close(AR_SOFTC_T *ar)
{
    A_PRINTF("ar6000_htc_raw_close called \n");
    HTCStop(ar->arHtcTarget);

        /* reset the device */
    ar6000_reset_device(ar->arHifDevice, ar->arTargetType);
    /* Initialize the BMI component */
    BMIInit();

    return 0;
}

raw_htc_buffer *
get_filled_buffer(AR_SOFTC_T *ar, HTC_RAW_STREAM_ID StreamID)
{
    int count;
    raw_htc_buffer *busy;

    /* Check for data */
    for (count = 0; count < RAW_HTC_READ_BUFFERS_NUM; count ++) {
        busy = ar->raw_htc_read_buffer[StreamID][count];
        if (busy->length) {
            break;
        }
    }
    if (busy->length) {
        ar->read_buffer_available[StreamID] = TRUE;
    } else {
        ar->read_buffer_available[StreamID] = FALSE;
    }

    return busy;
}

ssize_t ar6000_htc_raw_read(AR_SOFTC_T *ar, HTC_RAW_STREAM_ID StreamID,
                            char __user *buffer, size_t length)
{
    int readPtr;
    raw_htc_buffer *busy;

    if (arRawStream2EndpointID(ar,StreamID) == 0) {
        AR_DEBUG_PRINTF("StreamID(%d) not connected! \n", StreamID);
        return -EFAULT;
    }

    if (down_interruptible(&ar->raw_htc_read_sem[StreamID])) {
        return -ERESTARTSYS;
    }

    busy = get_filled_buffer(ar,StreamID);
    while (!ar->read_buffer_available[StreamID]) {
        up(&ar->raw_htc_read_sem[StreamID]);

        /* Wait for the data */
        AR_DEBUG2_PRINTF("Sleeping StreamID(%d) read process\n", StreamID);
        if (wait_event_interruptible(ar->raw_htc_read_queue[StreamID],
                                     ar->read_buffer_available[StreamID]))
        {
            return -EINTR;
        }
        if (down_interruptible(&ar->raw_htc_read_sem[StreamID])) {
            return -ERESTARTSYS;
        }
        busy = get_filled_buffer(ar,StreamID);
    }

    /* Read the data */
    readPtr = busy->currPtr;
    if (length > busy->length - HTC_HEADER_LEN) {
        length = busy->length - HTC_HEADER_LEN;
    }
    if (copy_to_user(buffer, &busy->data[readPtr], length)) {
        up(&ar->raw_htc_read_sem[StreamID]);
        return -EFAULT;
    }

    busy->currPtr += length;

    //AR_DEBUG_PRINTF("raw read ioctl:  currPTR : 0x%X 0x%X \n", busy->currPtr,busy->length);

    if (busy->currPtr == busy->length)
    {
        busy->currPtr = 0;
        busy->length = 0;
        HTC_PACKET_RESET_RX(&busy->HTCPacket);
        //AR_DEBUG_PRINTF("raw read ioctl:  ep for packet:%d \n", busy->HTCPacket.Endpoint);
        HTCAddReceivePkt(ar->arHtcTarget, &busy->HTCPacket);
    }
    ar->read_buffer_available[StreamID] = FALSE;
    up(&ar->raw_htc_read_sem[StreamID]);

    return length;
}

static raw_htc_buffer *
get_free_buffer(AR_SOFTC_T *ar, HTC_ENDPOINT_ID StreamID)
{
    int count;
    raw_htc_buffer *free;

    free = NULL;
    for (count = 0; count < RAW_HTC_WRITE_BUFFERS_NUM; count ++) {
        free = ar->raw_htc_write_buffer[StreamID][count];
        if (free->length == 0) {
            break;
        }
    }
    if (!free->length) {
        ar->write_buffer_available[StreamID] = TRUE;
    } else {
        ar->write_buffer_available[StreamID] = FALSE;
    }

    return free;
}

ssize_t ar6000_htc_raw_write(AR_SOFTC_T *ar, HTC_RAW_STREAM_ID StreamID,
                     char __user *buffer, size_t length)
{
    int writePtr;
    raw_htc_buffer *free;

    if (arRawStream2EndpointID(ar,StreamID) == 0) {
        AR_DEBUG_PRINTF("StreamID(%d) not connected! \n", StreamID);
        return -EFAULT;
    }

    if (down_interruptible(&ar->raw_htc_write_sem[StreamID])) {
        return -ERESTARTSYS;
    }

    /* Search for a free buffer */
    free = get_free_buffer(ar,StreamID);

    /* Check if there is space to write else wait */
    while (!ar->write_buffer_available[StreamID]) {
        up(&ar->raw_htc_write_sem[StreamID]);

        /* Wait for buffer to become free */
        AR_DEBUG2_PRINTF("Sleeping StreamID(%d) write process\n", StreamID);
        if (wait_event_interruptible(ar->raw_htc_write_queue[StreamID],
                                     ar->write_buffer_available[StreamID]))
        {
            return -EINTR;
        }
        if (down_interruptible(&ar->raw_htc_write_sem[StreamID])) {
            return -ERESTARTSYS;
        }
        free = get_free_buffer(ar,StreamID);
    }

    /* Send the data */
    writePtr = HTC_HEADER_LEN;
    if (length > (AR6000_BUFFER_SIZE - HTC_HEADER_LEN)) {
        length = AR6000_BUFFER_SIZE - HTC_HEADER_LEN;
    }

    if (copy_from_user(&free->data[writePtr], buffer, length)) {
        up(&ar->raw_htc_read_sem[StreamID]);
        return -EFAULT;
    }

    free->length = length;

    SET_HTC_PACKET_INFO_TX(&free->HTCPacket,
                           free,
                           &free->data[writePtr],
                           length,
                           arRawStream2EndpointID(ar,StreamID),
                           AR6K_DATA_PKT_TAG);

    HTCSendPkt(ar->arHtcTarget,&free->HTCPacket);

    ar->write_buffer_available[StreamID] = FALSE;
    up(&ar->raw_htc_write_sem[StreamID]);

    return length;
}
#endif /* HTC_RAW_INTERFACE */
