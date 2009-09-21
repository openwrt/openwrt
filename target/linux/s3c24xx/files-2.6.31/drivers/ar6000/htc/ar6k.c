/*
 * AR6K device layer that handles register level I/O
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
#include "a_config.h"
#include "athdefs.h"
#include "a_types.h"
#include "AR6Khwreg.h"
#include "a_osapi.h"
#include "a_debug.h"
#include "hif.h"
#include "htc_packet.h"
#include "ar6k.h"

#define MAILBOX_FOR_BLOCK_SIZE 1

extern A_UINT32 resetok;

static A_STATUS DevEnableInterrupts(AR6K_DEVICE *pDev);
static A_STATUS DevDisableInterrupts(AR6K_DEVICE *pDev);

#define LOCK_AR6K(p)      A_MUTEX_LOCK(&(p)->Lock);
#define UNLOCK_AR6K(p)    A_MUTEX_UNLOCK(&(p)->Lock);

void AR6KFreeIOPacket(AR6K_DEVICE *pDev, HTC_PACKET *pPacket)
{
    LOCK_AR6K(pDev);
    HTC_PACKET_ENQUEUE(&pDev->RegisterIOList,pPacket);
    UNLOCK_AR6K(pDev);
}

HTC_PACKET *AR6KAllocIOPacket(AR6K_DEVICE *pDev)
{
    HTC_PACKET *pPacket;

    LOCK_AR6K(pDev);
    pPacket = HTC_PACKET_DEQUEUE(&pDev->RegisterIOList);
    UNLOCK_AR6K(pDev);

    return pPacket;
}

A_STATUS DevSetup(AR6K_DEVICE *pDev)
{
    A_UINT32 mailboxaddrs[AR6K_MAILBOXES];
    A_UINT32 blocksizes[AR6K_MAILBOXES];
    A_STATUS status = A_OK;
    int      i;

    AR_DEBUG_ASSERT(AR6K_IRQ_PROC_REGS_SIZE == 16);
    AR_DEBUG_ASSERT(AR6K_IRQ_ENABLE_REGS_SIZE == 4);

    do {
            /* give a handle to HIF for this target */
        HIFSetHandle(pDev->HIFDevice, (void *)pDev);
            /* initialize our free list of IO packets */
        INIT_HTC_PACKET_QUEUE(&pDev->RegisterIOList);
        A_MUTEX_INIT(&pDev->Lock);

            /* get the addresses for all 4 mailboxes */
        status = HIFConfigureDevice(pDev->HIFDevice, HIF_DEVICE_GET_MBOX_ADDR,
                                    mailboxaddrs, sizeof(mailboxaddrs));

        if (status != A_OK) {
            AR_DEBUG_ASSERT(FALSE);
            break;
        }

            /* carve up register I/O packets (these are for ASYNC register I/O ) */
        for (i = 0; i < AR6K_MAX_REG_IO_BUFFERS; i++) {
            HTC_PACKET *pIOPacket;
            pIOPacket = &pDev->RegIOBuffers[i].HtcPacket;
            SET_HTC_PACKET_INFO_RX_REFILL(pIOPacket,
                                          pDev,
                                          pDev->RegIOBuffers[i].Buffer,
                                          AR6K_REG_IO_BUFFER_SIZE,
                                          0); /* don't care */
            AR6KFreeIOPacket(pDev,pIOPacket);
        }

            /* get the address of the mailbox we are using */
        pDev->MailboxAddress = mailboxaddrs[HTC_MAILBOX];

            /* get the block sizes */
        status = HIFConfigureDevice(pDev->HIFDevice, HIF_DEVICE_GET_MBOX_BLOCK_SIZE,
                                    blocksizes, sizeof(blocksizes));

        if (status != A_OK) {
            AR_DEBUG_ASSERT(FALSE);
            break;
        }

            /* note: we actually get the block size of a mailbox other than 0, for SDIO the block
             * size on mailbox 0 is artificially set to 1.  So we use the block size that is set
             * for the other 3 mailboxes */
        pDev->BlockSize = blocksizes[MAILBOX_FOR_BLOCK_SIZE];
            /* must be a power of 2 */
        AR_DEBUG_ASSERT((pDev->BlockSize & (pDev->BlockSize - 1)) == 0);

            /* assemble mask, used for padding to a block */
        pDev->BlockMask = pDev->BlockSize - 1;

        AR_DEBUG_PRINTF(ATH_DEBUG_TRC,("BlockSize: %d, MailboxAddress:0x%X \n",
                    pDev->BlockSize, pDev->MailboxAddress));

        pDev->GetPendingEventsFunc = NULL;
            /* see if the HIF layer implements the get pending events function  */
        HIFConfigureDevice(pDev->HIFDevice,
                           HIF_DEVICE_GET_PENDING_EVENTS_FUNC,
                           &pDev->GetPendingEventsFunc,
                           sizeof(pDev->GetPendingEventsFunc));

            /* assume we can process HIF interrupt events asynchronously */
        pDev->HifIRQProcessingMode = HIF_DEVICE_IRQ_ASYNC_SYNC;

            /* see if the HIF layer overrides this assumption */
        HIFConfigureDevice(pDev->HIFDevice,
                           HIF_DEVICE_GET_IRQ_PROC_MODE,
                           &pDev->HifIRQProcessingMode,
                           sizeof(pDev->HifIRQProcessingMode));

        switch (pDev->HifIRQProcessingMode) {
            case HIF_DEVICE_IRQ_SYNC_ONLY:
                AR_DEBUG_PRINTF(ATH_DEBUG_TRC,("HIF Interrupt processing is SYNC ONLY\n"));
                break;
            case HIF_DEVICE_IRQ_ASYNC_SYNC:
                AR_DEBUG_PRINTF(ATH_DEBUG_TRC,("HIF Interrupt processing is ASYNC and SYNC\n"));
                break;
            default:
                AR_DEBUG_ASSERT(FALSE);
        }

        pDev->HifMaskUmaskRecvEvent = NULL;

            /* see if the HIF layer implements the mask/unmask recv events function  */
        HIFConfigureDevice(pDev->HIFDevice,
                           HIF_DEVICE_GET_RECV_EVENT_MASK_UNMASK_FUNC,
                           &pDev->HifMaskUmaskRecvEvent,
                           sizeof(pDev->HifMaskUmaskRecvEvent));

        AR_DEBUG_PRINTF(ATH_DEBUG_TRC,("HIF special overrides : 0x%X , 0x%X\n",
                 (A_UINT32)pDev->GetPendingEventsFunc, (A_UINT32)pDev->HifMaskUmaskRecvEvent));

        status = DevDisableInterrupts(pDev);

    } while (FALSE);

    if (A_FAILED(status)) {
            /* make sure handle is cleared */
        HIFSetHandle(pDev->HIFDevice, NULL);
    }

    return status;

}

static A_STATUS DevEnableInterrupts(AR6K_DEVICE *pDev)
{
    A_STATUS                  status;
    AR6K_IRQ_ENABLE_REGISTERS regs;

    LOCK_AR6K(pDev);

        /* Enable all the interrupts except for the dragon interrupt */
    pDev->IrqEnableRegisters.int_status_enable = INT_STATUS_ENABLE_ERROR_SET(0x01) |
                                      INT_STATUS_ENABLE_CPU_SET(0x01) |
                                      INT_STATUS_ENABLE_COUNTER_SET(0x01);

    if (NULL == pDev->GetPendingEventsFunc) {
        pDev->IrqEnableRegisters.int_status_enable |= INT_STATUS_ENABLE_MBOX_DATA_SET(0x01);
    } else {
        /* The HIF layer provided us with a pending events function which means that
         * the detection of pending mbox messages is handled in the HIF layer.
         * This is the case for the SPI2 interface.
         * In the normal case we enable MBOX interrupts, for the case
         * with HIFs that offer this mechanism, we keep these interrupts
         * masked */
        pDev->IrqEnableRegisters.int_status_enable &= ~INT_STATUS_ENABLE_MBOX_DATA_SET(0x01);
    }


    /* Set up the CPU Interrupt Status Register */
    pDev->IrqEnableRegisters.cpu_int_status_enable = CPU_INT_STATUS_ENABLE_BIT_SET(0x00);

    /* Set up the Error Interrupt Status Register */
    pDev->IrqEnableRegisters.error_status_enable =
                                  ERROR_STATUS_ENABLE_RX_UNDERFLOW_SET(0x01) |
                                  ERROR_STATUS_ENABLE_TX_OVERFLOW_SET(0x01);

    /* Set up the Counter Interrupt Status Register (only for debug interrupt to catch fatal errors) */
    pDev->IrqEnableRegisters.counter_int_status_enable =
        COUNTER_INT_STATUS_ENABLE_BIT_SET(AR6K_TARGET_DEBUG_INTR_MASK);

        /* copy into our temp area */
    A_MEMCPY(&regs,&pDev->IrqEnableRegisters,AR6K_IRQ_ENABLE_REGS_SIZE);

    UNLOCK_AR6K(pDev);

        /* always synchronous */
    status = HIFReadWrite(pDev->HIFDevice,
                          INT_STATUS_ENABLE_ADDRESS,
                          &regs.int_status_enable,
                          AR6K_IRQ_ENABLE_REGS_SIZE,
                          HIF_WR_SYNC_BYTE_INC,
                          NULL);

    if (status != A_OK) {
        /* Can't write it for some reason */
        AR_DEBUG_PRINTF(ATH_DEBUG_ERR,
                        ("Failed to update interrupt control registers err: %d\n", status));

    }

    return status;
}

static A_STATUS DevDisableInterrupts(AR6K_DEVICE *pDev)
{
    AR6K_IRQ_ENABLE_REGISTERS regs;

    LOCK_AR6K(pDev);
        /* Disable all interrupts */
    pDev->IrqEnableRegisters.int_status_enable = 0;
    pDev->IrqEnableRegisters.cpu_int_status_enable = 0;
    pDev->IrqEnableRegisters.error_status_enable = 0;
    pDev->IrqEnableRegisters.counter_int_status_enable = 0;
        /* copy into our temp area */
    A_MEMCPY(&regs,&pDev->IrqEnableRegisters,AR6K_IRQ_ENABLE_REGS_SIZE);

    UNLOCK_AR6K(pDev);

        /* always synchronous */
    return HIFReadWrite(pDev->HIFDevice,
                        INT_STATUS_ENABLE_ADDRESS,
                        &regs.int_status_enable,
                        AR6K_IRQ_ENABLE_REGS_SIZE,
                        HIF_WR_SYNC_BYTE_INC,
                        NULL);
}

/* enable device interrupts */
A_STATUS DevUnmaskInterrupts(AR6K_DEVICE *pDev)
{
        /* Unmask the host controller interrupts */
    HIFUnMaskInterrupt(pDev->HIFDevice);

    return DevEnableInterrupts(pDev);
}

/* disable all device interrupts */
A_STATUS DevMaskInterrupts(AR6K_DEVICE *pDev)
{
    A_STATUS status;

    status = DevDisableInterrupts(pDev);

    if (A_SUCCESS(status)) {
            /* Disable the interrupt at the HIF layer */
        HIFMaskInterrupt(pDev->HIFDevice);
    }

    return status;
}

/* callback when our fetch to enable/disable completes */
static void DevDoEnableDisableRecvAsyncHandler(void *Context, HTC_PACKET *pPacket)
{
    AR6K_DEVICE *pDev = (AR6K_DEVICE *)Context;

    AR_DEBUG_PRINTF(ATH_DEBUG_IRQ,("+DevDoEnableDisableRecvAsyncHandler: (dev: 0x%X)\n", (A_UINT32)pDev));

    if (A_FAILED(pPacket->Status)) {
        AR_DEBUG_PRINTF(ATH_DEBUG_ERR,
                (" Failed to disable receiver, status:%d \n", pPacket->Status));
    }
        /* free this IO packet */
    AR6KFreeIOPacket(pDev,pPacket);
    AR_DEBUG_PRINTF(ATH_DEBUG_IRQ,("-DevDoEnableDisableRecvAsyncHandler \n"));
}

/* disable packet reception (used in case the host runs out of buffers)
 * this is the "override" method when the HIF reports another methods to
 * disable recv events */
static A_STATUS DevDoEnableDisableRecvOverride(AR6K_DEVICE *pDev, A_BOOL EnableRecv, A_BOOL AsyncMode)
{
    A_STATUS                  status = A_OK;
    HTC_PACKET                *pIOPacket = NULL;

    AR_DEBUG_PRINTF(ATH_DEBUG_TRC,("DevDoEnableDisableRecvOverride: Enable:%d Mode:%d\n",
            EnableRecv,AsyncMode));

    do {

        if (AsyncMode) {

            pIOPacket = AR6KAllocIOPacket(pDev);

            if (NULL == pIOPacket) {
                status = A_NO_MEMORY;
                AR_DEBUG_ASSERT(FALSE);
                break;
            }

                /* stick in our completion routine when the I/O operation completes */
            pIOPacket->Completion = DevDoEnableDisableRecvAsyncHandler;
            pIOPacket->pContext = pDev;

                /* call the HIF layer override and do this asynchronously */
            status = pDev->HifMaskUmaskRecvEvent(pDev->HIFDevice,
                                                 EnableRecv ? HIF_UNMASK_RECV : HIF_MASK_RECV,
                                                 pIOPacket);
            break;
        }

            /* if we get here we are doing it synchronously */
        status = pDev->HifMaskUmaskRecvEvent(pDev->HIFDevice,
                                             EnableRecv ? HIF_UNMASK_RECV : HIF_MASK_RECV,
                                             NULL);

    } while (FALSE);

    if (A_FAILED(status) && (pIOPacket != NULL)) {
        AR6KFreeIOPacket(pDev,pIOPacket);
    }

    return status;
}

/* disable packet reception (used in case the host runs out of buffers)
 * this is the "normal" method using the interrupt enable registers through
 * the host I/F */
static A_STATUS DevDoEnableDisableRecvNormal(AR6K_DEVICE *pDev, A_BOOL EnableRecv, A_BOOL AsyncMode)
{
    A_STATUS                  status = A_OK;
    HTC_PACKET                *pIOPacket = NULL;
    AR6K_IRQ_ENABLE_REGISTERS regs;

        /* take the lock to protect interrupt enable shadows */
    LOCK_AR6K(pDev);

    if (EnableRecv) {
        pDev->IrqEnableRegisters.int_status_enable |= INT_STATUS_ENABLE_MBOX_DATA_SET(0x01);
    } else {
        pDev->IrqEnableRegisters.int_status_enable &= ~INT_STATUS_ENABLE_MBOX_DATA_SET(0x01);
    }

        /* copy into our temp area */
    A_MEMCPY(&regs,&pDev->IrqEnableRegisters,AR6K_IRQ_ENABLE_REGS_SIZE);
    UNLOCK_AR6K(pDev);

    do {

        if (AsyncMode) {

            pIOPacket = AR6KAllocIOPacket(pDev);

            if (NULL == pIOPacket) {
                status = A_NO_MEMORY;
                AR_DEBUG_ASSERT(FALSE);
                break;
            }

                /* copy values to write to our async I/O buffer */
            A_MEMCPY(pIOPacket->pBuffer,&regs,AR6K_IRQ_ENABLE_REGS_SIZE);

                /* stick in our completion routine when the I/O operation completes */
            pIOPacket->Completion = DevDoEnableDisableRecvAsyncHandler;
            pIOPacket->pContext = pDev;

                /* write it out asynchronously */
            HIFReadWrite(pDev->HIFDevice,
                         INT_STATUS_ENABLE_ADDRESS,
                         pIOPacket->pBuffer,
                         AR6K_IRQ_ENABLE_REGS_SIZE,
                         HIF_WR_ASYNC_BYTE_INC,
                         pIOPacket);
            break;
        }

        /* if we get here we are doing it synchronously */

        status = HIFReadWrite(pDev->HIFDevice,
                              INT_STATUS_ENABLE_ADDRESS,
                              &regs.int_status_enable,
                              AR6K_IRQ_ENABLE_REGS_SIZE,
                              HIF_WR_SYNC_BYTE_INC,
                              NULL);

    } while (FALSE);

    if (A_FAILED(status) && (pIOPacket != NULL)) {
        AR6KFreeIOPacket(pDev,pIOPacket);
    }

    return status;
}


A_STATUS DevStopRecv(AR6K_DEVICE *pDev, A_BOOL AsyncMode)
{
    if (NULL == pDev->HifMaskUmaskRecvEvent) {
        return DevDoEnableDisableRecvNormal(pDev,FALSE,AsyncMode);
    } else {
        return DevDoEnableDisableRecvOverride(pDev,FALSE,AsyncMode);
    }
}

A_STATUS DevEnableRecv(AR6K_DEVICE *pDev, A_BOOL AsyncMode)
{
    if (NULL == pDev->HifMaskUmaskRecvEvent) {
        return DevDoEnableDisableRecvNormal(pDev,TRUE,AsyncMode);
    } else {
        return DevDoEnableDisableRecvOverride(pDev,TRUE,AsyncMode);
    }
}

void DevDumpRegisters(AR6K_IRQ_PROC_REGISTERS   *pIrqProcRegs,
                      AR6K_IRQ_ENABLE_REGISTERS *pIrqEnableRegs)
{

    AR_DEBUG_PRINTF(ATH_DEBUG_DUMP, ("\n<------- Register Table -------->\n"));

    if (pIrqProcRegs != NULL) {
        AR_DEBUG_PRINTF(ATH_DEBUG_DUMP,
            ("Int Status:               0x%x\n",pIrqProcRegs->host_int_status));
        AR_DEBUG_PRINTF(ATH_DEBUG_DUMP,
            ("CPU Int Status:            0x%x\n",pIrqProcRegs->cpu_int_status));
        AR_DEBUG_PRINTF(ATH_DEBUG_DUMP,
            ("Error Int Status:          0x%x\n",pIrqProcRegs->error_int_status));
        AR_DEBUG_PRINTF(ATH_DEBUG_DUMP,
            ("Counter Int Status:        0x%x\n",pIrqProcRegs->counter_int_status));
        AR_DEBUG_PRINTF(ATH_DEBUG_DUMP,
            ("Mbox Frame:                0x%x\n",pIrqProcRegs->mbox_frame));
        AR_DEBUG_PRINTF(ATH_DEBUG_DUMP,
            ("Rx Lookahead Valid:        0x%x\n",pIrqProcRegs->rx_lookahead_valid));
        AR_DEBUG_PRINTF(ATH_DEBUG_DUMP,
            ("Rx Lookahead 0:            0x%x\n",pIrqProcRegs->rx_lookahead[0]));
        AR_DEBUG_PRINTF(ATH_DEBUG_DUMP,
            ("Rx Lookahead 1:            0x%x\n",pIrqProcRegs->rx_lookahead[1]));
    }

    if (pIrqEnableRegs != NULL) {
        AR_DEBUG_PRINTF(ATH_DEBUG_DUMP,
            ("Int Status Enable:         0x%x\n",pIrqEnableRegs->int_status_enable));
        AR_DEBUG_PRINTF(ATH_DEBUG_DUMP,
            ("Counter Int Status Enable: 0x%x\n",pIrqEnableRegs->counter_int_status_enable));
        AR_DEBUG_PRINTF(ATH_DEBUG_DUMP, ("<------------------------------->\n"));
    }
}


#ifdef MBOXHW_UNIT_TEST


/* This is a mailbox hardware unit test that must be called in a schedulable context
 * This test is very simple, it will send a list of buffers with a counting pattern
 * and the target will invert the data and send the message back
 *
 * the unit test has the following constraints:
 *
 * The target has at least 8 buffers of 256 bytes each. The host will send
 * the following pattern of buffers in rapid succession :
 *
 * 1 buffer - 128 bytes
 * 1 buffer - 256 bytes
 * 1 buffer - 512 bytes
 * 1 buffer - 1024 bytes
 *
 * The host will send the buffers to one mailbox and wait for buffers to be reflected
 * back from the same mailbox. The target sends the buffers FIFO order.
 * Once the final buffer has been received for a mailbox, the next mailbox is tested.
 *
 *
 * Note:  To simplifythe test , we assume that the chosen buffer sizes
 *        will fall on a nice block pad
 *
 * It is expected that higher-order tests will be written to stress the mailboxes using
 * a message-based protocol (with some performance timming) that can create more
 * randomness in the packets sent over mailboxes.
 *
 * */

#define A_ROUND_UP_PWR2(x, align)    (((int) (x) + ((align)-1)) & ~((align)-1))

#define BUFFER_BLOCK_PAD 128

#if 0
#define BUFFER1 128
#define BUFFER2 256
#define BUFFER3 512
#define BUFFER4 1024
#endif

#if 1
#define BUFFER1 80
#define BUFFER2 200
#define BUFFER3 444
#define BUFFER4 800
#endif

#define TOTAL_BYTES (A_ROUND_UP_PWR2(BUFFER1,BUFFER_BLOCK_PAD) + \
                     A_ROUND_UP_PWR2(BUFFER2,BUFFER_BLOCK_PAD) + \
                     A_ROUND_UP_PWR2(BUFFER3,BUFFER_BLOCK_PAD) + \
                     A_ROUND_UP_PWR2(BUFFER4,BUFFER_BLOCK_PAD) )

#define TEST_BYTES (BUFFER1 +  BUFFER2 + BUFFER3 + BUFFER4)

#define TEST_CREDITS_RECV_TIMEOUT 100

static A_UINT8  g_Buffer[TOTAL_BYTES];
static A_UINT32 g_MailboxAddrs[AR6K_MAILBOXES];
static A_UINT32 g_BlockSizes[AR6K_MAILBOXES];

#define BUFFER_PROC_LIST_DEPTH 4

typedef struct _BUFFER_PROC_LIST{
    A_UINT8  *pBuffer;
    A_UINT32 length;
}BUFFER_PROC_LIST;


#define PUSH_BUFF_PROC_ENTRY(pList,len,pCurrpos) \
{                                                   \
    (pList)->pBuffer = (pCurrpos);                  \
    (pList)->length = (len);                        \
    (pCurrpos) += (len);                            \
    (pList)++;                                      \
}

/* a simple and crude way to send different "message" sizes */
static void AssembleBufferList(BUFFER_PROC_LIST *pList)
{
    A_UINT8 *pBuffer = g_Buffer;

#if BUFFER_PROC_LIST_DEPTH < 4
#error "Buffer processing list depth is not deep enough!!"
#endif

    PUSH_BUFF_PROC_ENTRY(pList,BUFFER1,pBuffer);
    PUSH_BUFF_PROC_ENTRY(pList,BUFFER2,pBuffer);
    PUSH_BUFF_PROC_ENTRY(pList,BUFFER3,pBuffer);
    PUSH_BUFF_PROC_ENTRY(pList,BUFFER4,pBuffer);

}

#define FILL_ZERO     TRUE
#define FILL_COUNTING FALSE
static void InitBuffers(A_BOOL Zero)
{
    A_UINT16 *pBuffer16 = (A_UINT16 *)g_Buffer;
    int      i;

        /* fill buffer with 16 bit counting pattern or zeros */
    for (i = 0; i <  (TOTAL_BYTES / 2) ; i++) {
        if (!Zero) {
            pBuffer16[i] = (A_UINT16)i;
        } else {
            pBuffer16[i] = 0;
        }
    }
}


static A_BOOL CheckOneBuffer(A_UINT16 *pBuffer16, int Length)
{
    int      i;
    A_UINT16 startCount;
    A_BOOL   success = TRUE;

        /* get the starting count */
    startCount = pBuffer16[0];
        /* invert it, this is the expected value */
    startCount = ~startCount;
        /* scan the buffer and verify */
    for (i = 0; i < (Length / 2) ; i++,startCount++) {
            /* target will invert all the data */
        if ((A_UINT16)pBuffer16[i] != (A_UINT16)~startCount) {
            success = FALSE;
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("Invalid Data Got:0x%X, Expecting:0x%X (offset:%d, total:%d) \n",
                        pBuffer16[i], ((A_UINT16)~startCount), i, Length));
             AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("0x%X 0x%X 0x%X 0x%X \n",
                        pBuffer16[i], pBuffer16[i + 1], pBuffer16[i + 2],pBuffer16[i+3]));
            break;
        }
    }

    return success;
}

static A_BOOL CheckBuffers(void)
{
    int      i;
    A_BOOL   success = TRUE;
    BUFFER_PROC_LIST checkList[BUFFER_PROC_LIST_DEPTH];

        /* assemble the list */
    AssembleBufferList(checkList);

        /* scan the buffers and verify */
    for (i = 0; i < BUFFER_PROC_LIST_DEPTH ; i++) {
        success = CheckOneBuffer((A_UINT16 *)checkList[i].pBuffer, checkList[i].length);
        if (!success) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("Buffer : 0x%X, Length:%d failed verify \n",
                        (A_UINT32)checkList[i].pBuffer, checkList[i].length));
            break;
        }
    }

    return success;
}

    /* find the end marker for the last buffer we will be sending */
static A_UINT16 GetEndMarker(void)
{
    A_UINT8  *pBuffer;
    BUFFER_PROC_LIST checkList[BUFFER_PROC_LIST_DEPTH];

        /* fill up buffers with the normal counting pattern */
    InitBuffers(FILL_COUNTING);

        /* assemble the list we will be sending down */
    AssembleBufferList(checkList);
        /* point to the last 2 bytes of the last buffer */
    pBuffer = &(checkList[BUFFER_PROC_LIST_DEPTH - 1].pBuffer[(checkList[BUFFER_PROC_LIST_DEPTH - 1].length) - 2]);

        /* the last count in the last buffer is the marker */
    return (A_UINT16)pBuffer[0] | ((A_UINT16)pBuffer[1] << 8);
}

#define ATH_PRINT_OUT_ZONE ATH_DEBUG_ERR

/* send the ordered buffers to the target */
static A_STATUS SendBuffers(AR6K_DEVICE *pDev, int mbox)
{
    A_STATUS         status = A_OK;
    A_UINT32         request = HIF_WR_SYNC_BLOCK_INC;
    BUFFER_PROC_LIST sendList[BUFFER_PROC_LIST_DEPTH];
    int              i;
    int              totalBytes = 0;
    int              paddedLength;
    int              totalwPadding = 0;

    AR_DEBUG_PRINTF(ATH_PRINT_OUT_ZONE, ("Sending buffers on mailbox : %d \n",mbox));

        /* fill buffer with counting pattern */
    InitBuffers(FILL_COUNTING);

        /* assemble the order in which we send */
    AssembleBufferList(sendList);

    for (i = 0; i < BUFFER_PROC_LIST_DEPTH; i++) {

            /* we are doing block transfers, so we need to pad everything to a block size */
        paddedLength = (sendList[i].length + (g_BlockSizes[mbox] - 1)) &
                       (~(g_BlockSizes[mbox] - 1));

            /* send each buffer synchronously */
        status = HIFReadWrite(pDev->HIFDevice,
                              g_MailboxAddrs[mbox],
                              sendList[i].pBuffer,
                              paddedLength,
                              request,
                              NULL);
        if (status != A_OK) {
            break;
        }
        totalBytes += sendList[i].length;
        totalwPadding += paddedLength;
    }

    AR_DEBUG_PRINTF(ATH_PRINT_OUT_ZONE, ("Sent %d bytes (%d padded bytes) to mailbox : %d \n",totalBytes,totalwPadding,mbox));

    return status;
}

/* poll the mailbox credit counter until we get a credit or timeout */
static A_STATUS GetCredits(AR6K_DEVICE *pDev, int mbox, int *pCredits)
{
    A_STATUS status = A_OK;
    int      timeout = TEST_CREDITS_RECV_TIMEOUT;
    A_UINT8  credits = 0;
    A_UINT32 address;

    while (TRUE) {

            /* Read the counter register to get credits, this auto-decrements  */
        address = COUNT_DEC_ADDRESS + (AR6K_MAILBOXES + mbox) * 4;
        status = HIFReadWrite(pDev->HIFDevice, address, &credits, sizeof(credits),
                              HIF_RD_SYNC_BYTE_FIX, NULL);
        if (status != A_OK) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR,
                ("Unable to decrement the command credit count register (mbox=%d)\n",mbox));
            status = A_ERROR;
            break;
        }

        if (credits) {
            break;
        }

        timeout--;

        if (timeout <= 0) {
              AR_DEBUG_PRINTF(ATH_DEBUG_ERR,
                (" Timeout reading credit registers (mbox=%d, address:0x%X) \n",mbox,address));
            status = A_ERROR;
            break;
        }

         /* delay a little, target may not be ready */
         msleep(1000);

    }

    if (status == A_OK) {
        *pCredits = credits;
    }

    return status;
}


/* wait for the buffers to come back */
static A_STATUS RecvBuffers(AR6K_DEVICE *pDev, int mbox)
{
    A_STATUS         status = A_OK;
    A_UINT32         request = HIF_RD_SYNC_BLOCK_INC;
    BUFFER_PROC_LIST recvList[BUFFER_PROC_LIST_DEPTH];
    int              curBuffer;
    int              credits;
    int              i;
    int              totalBytes = 0;
    int              paddedLength;
    int              totalwPadding = 0;

    AR_DEBUG_PRINTF(ATH_PRINT_OUT_ZONE, ("Waiting for buffers on mailbox : %d \n",mbox));

        /* zero the buffers */
    InitBuffers(FILL_ZERO);

        /* assemble the order in which we should receive */
    AssembleBufferList(recvList);

    curBuffer = 0;

    while (curBuffer < BUFFER_PROC_LIST_DEPTH) {

            /* get number of buffers that have been completed, this blocks
             * until we get at least 1 credit or it times out */
        status = GetCredits(pDev, mbox, &credits);

        if (status != A_OK) {
            break;
        }

        AR_DEBUG_PRINTF(ATH_PRINT_OUT_ZONE, ("Got %d messages on mailbox : %d \n",credits, mbox));

            /* get all the buffers that are sitting on the queue */
        for (i = 0; i < credits; i++) {
            AR_DEBUG_ASSERT(curBuffer < BUFFER_PROC_LIST_DEPTH);
                /* recv the current buffer synchronously, the buffers should come back in
                 * order... with padding applied by the target */
            paddedLength = (recvList[curBuffer].length + (g_BlockSizes[mbox] - 1)) &
                       (~(g_BlockSizes[mbox] - 1));

            status = HIFReadWrite(pDev->HIFDevice,
                                  g_MailboxAddrs[mbox],
                                  recvList[curBuffer].pBuffer,
                                  paddedLength,
                                  request,
                                  NULL);
            if (status != A_OK) {
                AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("Failed to read %d bytes on mailbox:%d : address:0x%X \n",
                        recvList[curBuffer].length, mbox, g_MailboxAddrs[mbox]));
                break;
            }

            totalwPadding += paddedLength;
            totalBytes += recvList[curBuffer].length;
            curBuffer++;
        }

        if (status != A_OK) {
            break;
        }
            /* go back and get some more */
        credits = 0;
    }

    if (totalBytes != TEST_BYTES) {
        AR_DEBUG_ASSERT(FALSE);
    }  else {
        AR_DEBUG_PRINTF(ATH_PRINT_OUT_ZONE, ("Got all buffers on mbox:%d total recv :%d (w/Padding : %d) \n",
            mbox, totalBytes, totalwPadding));
    }

    return status;


}

static A_STATUS DoOneMboxHWTest(AR6K_DEVICE *pDev, int mbox)
{
    A_STATUS status;

    do {
            /* send out buffers */
        status = SendBuffers(pDev,mbox);

        if (status != A_OK) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("Sending buffers Failed : %d mbox:%d\n",status,mbox));
            break;
        }

            /* go get them, this will block */
        status =  RecvBuffers(pDev, mbox);

        if (status != A_OK) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("Recv buffers Failed : %d mbox:%d\n",status,mbox));
            break;
        }

            /* check the returned data patterns */
        if (!CheckBuffers()) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("Buffer Verify Failed : mbox:%d\n",mbox));
            status = A_ERROR;
            break;
        }

        AR_DEBUG_PRINTF(ATH_PRINT_OUT_ZONE, (" Send/Recv success! mailbox : %d \n",mbox));

    }  while (FALSE);

    return status;
}

/* here is where the test starts */
A_STATUS DoMboxHWTest(AR6K_DEVICE *pDev)
{
    int      i;
    A_STATUS status;
    int      credits = 0;
    A_UINT8  params[4];
    int      numBufs;
    int      bufferSize;
    A_UINT16 temp;


    AR_DEBUG_PRINTF(ATH_PRINT_OUT_ZONE, (" DoMboxHWTest START -  \n"));

    do {
            /* get the addresses for all 4 mailboxes */
        status = HIFConfigureDevice(pDev->HIFDevice, HIF_DEVICE_GET_MBOX_ADDR,
                                    g_MailboxAddrs, sizeof(g_MailboxAddrs));

        if (status != A_OK) {
            AR_DEBUG_ASSERT(FALSE);
            break;
        }

            /* get the block sizes */
        status = HIFConfigureDevice(pDev->HIFDevice, HIF_DEVICE_GET_MBOX_BLOCK_SIZE,
                                    g_BlockSizes, sizeof(g_BlockSizes));

        if (status != A_OK) {
            AR_DEBUG_ASSERT(FALSE);
            break;
        }

            /* note, the HIF layer usually reports mbox 0 to have a block size of
             * 1, but our test wants to run in block-mode for all mailboxes, so we treat all mailboxes
             * the same. */
        g_BlockSizes[0] = g_BlockSizes[1];
        AR_DEBUG_PRINTF(ATH_PRINT_OUT_ZONE, ("Block Size to use: %d \n",g_BlockSizes[0]));

        if (g_BlockSizes[1] > BUFFER_BLOCK_PAD) {
            AR_DEBUG_PRINTF(ATH_PRINT_OUT_ZONE, ("%d Block size is too large for buffer pad %d\n",
                g_BlockSizes[1], BUFFER_BLOCK_PAD));
            break;
        }

        AR_DEBUG_PRINTF(ATH_PRINT_OUT_ZONE, ("Waiting for target.... \n"));

            /* the target lets us know it is ready by giving us 1 credit on
             * mailbox 0 */
        status = GetCredits(pDev, 0, &credits);

        if (status != A_OK) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("Failed to wait for target ready \n"));
            break;
        }

        AR_DEBUG_PRINTF(ATH_PRINT_OUT_ZONE, ("Target is ready ...\n"));

            /* read the first 4 scratch registers */
        status = HIFReadWrite(pDev->HIFDevice,
                              SCRATCH_ADDRESS,
                              params,
                              4,
                              HIF_RD_SYNC_BYTE_INC,
                              NULL);

        if (status != A_OK) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("Failed to wait get parameters \n"));
            break;
        }

        numBufs = params[0];
        bufferSize = (int)(((A_UINT16)params[2] << 8) | (A_UINT16)params[1]);

        AR_DEBUG_PRINTF(ATH_PRINT_OUT_ZONE,
            ("Target parameters: bufs per mailbox:%d, buffer size:%d bytes (total space: %d, minimum required space (w/padding): %d) \n",
            numBufs, bufferSize, (numBufs * bufferSize), TOTAL_BYTES));

        if ((numBufs * bufferSize) < TOTAL_BYTES) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("Not Enough buffer space to run test! need:%d, got:%d \n",
                TOTAL_BYTES, (numBufs*bufferSize)));
            status = A_ERROR;
            break;
        }

        temp = GetEndMarker();

        status = HIFReadWrite(pDev->HIFDevice,
                              SCRATCH_ADDRESS + 4,
                              (A_UINT8 *)&temp,
                              2,
                              HIF_WR_SYNC_BYTE_INC,
                              NULL);

        if (status != A_OK) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("Failed to write end marker \n"));
            break;
        }

        AR_DEBUG_PRINTF(ATH_PRINT_OUT_ZONE, ("End Marker: 0x%X \n",temp));

        temp = (A_UINT16)g_BlockSizes[1];
            /* convert to a mask */
        temp = temp - 1;
        status = HIFReadWrite(pDev->HIFDevice,
                              SCRATCH_ADDRESS + 6,
                              (A_UINT8 *)&temp,
                              2,
                              HIF_WR_SYNC_BYTE_INC,
                              NULL);

        if (status != A_OK) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("Failed to write block mask \n"));
            break;
        }

        AR_DEBUG_PRINTF(ATH_PRINT_OUT_ZONE, ("Set Block Mask: 0x%X \n",temp));

            /* execute the test on each mailbox */
        for (i = 0; i < AR6K_MAILBOXES; i++) {
            status = DoOneMboxHWTest(pDev, i);
            if (status != A_OK) {
                break;
            }
        }

    } while (FALSE);

    if (status == A_OK) {
        AR_DEBUG_PRINTF(ATH_PRINT_OUT_ZONE, (" DoMboxHWTest DONE - SUCCESS! -  \n"));
    } else {
        AR_DEBUG_PRINTF(ATH_PRINT_OUT_ZONE, (" DoMboxHWTest DONE - FAILED! -  \n"));
    }
        /* don't let HTC_Start continue, the target is actually not running any HTC code */
    return A_ERROR;
}
#endif



