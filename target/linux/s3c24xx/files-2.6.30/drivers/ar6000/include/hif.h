/*
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
 * HIF specific declarations and prototypes
 */

#ifndef _HIF_H_
#define _HIF_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Header files */
#include "a_config.h"
#include "athdefs.h"
#include "a_types.h"
#include "a_osapi.h"

typedef struct htc_callbacks HTC_CALLBACKS;
typedef struct hif_device HIF_DEVICE;

/*
 * direction - Direction of transfer (HIF_READ/HIF_WRITE).
 */
#define HIF_READ                    0x00000001
#define HIF_WRITE                   0x00000002
#define HIF_DIR_MASK                (HIF_READ | HIF_WRITE)

/*
 *     type - An interface may support different kind of read/write commands.
 *            The command type is divided into a basic and an extended command
 *            and can be specified using HIF_BASIC_IO/HIF_EXTENDED_IO.
 */
#define HIF_BASIC_IO                0x00000004
#define HIF_EXTENDED_IO             0x00000008
#define HIF_TYPE_MASK               (HIF_BASIC_IO | HIF_EXTENDED_IO)

/*
 *     emode - This indicates the whether the command is to be executed in a
 *             blocking or non-blocking fashion (HIF_SYNCHRONOUS/
 *             HIF_ASYNCHRONOUS). The read/write data paths in HTC have been
 *             implemented using the asynchronous mode allowing the the bus
 *             driver to indicate the completion of operation through the
 *             registered callback routine. The requirement primarily comes
 *             from the contexts these operations get called from (a driver's
 *             transmit context or the ISR context in case of receive).
 *             Support for both of these modes is essential.
 */
#define HIF_SYNCHRONOUS             0x00000010
#define HIF_ASYNCHRONOUS            0x00000020
#define HIF_EMODE_MASK              (HIF_SYNCHRONOUS | HIF_ASYNCHRONOUS)

/*
 *     dmode - An interface may support different kinds of commands based on
 *             the tradeoff between the amount of data it can carry and the
 *             setup time. Byte and Block modes are supported (HIF_BYTE_BASIS/
 *             HIF_BLOCK_BASIS). In case of latter, the data is rounded off
 *             to the nearest block size by padding. The size of the block is
 *             configurable at compile time using the HIF_BLOCK_SIZE and is
 *             negotiated with the target during initialization after the
 *             dragon interrupts are enabled.
 */
#define HIF_BYTE_BASIS              0x00000040
#define HIF_BLOCK_BASIS             0x00000080
#define HIF_DMODE_MASK              (HIF_BYTE_BASIS | HIF_BLOCK_BASIS)

/*
 *     amode - This indicates if the address has to be incremented on dragon
 *             after every read/write operation (HIF?FIXED_ADDRESS/
 *             HIF_INCREMENTAL_ADDRESS).
 */
#define HIF_FIXED_ADDRESS           0x00000100
#define HIF_INCREMENTAL_ADDRESS     0x00000200
#define HIF_AMODE_MASK              (HIF_FIXED_ADDRESS | HIF_INCREMENTAL_ADDRESS)

#define HIF_WR_ASYNC_BYTE_FIX   \
    (HIF_WRITE | HIF_ASYNCHRONOUS | HIF_EXTENDED_IO | HIF_BYTE_BASIS | HIF_FIXED_ADDRESS)
#define HIF_WR_ASYNC_BYTE_INC   \
    (HIF_WRITE | HIF_ASYNCHRONOUS | HIF_EXTENDED_IO | HIF_BYTE_BASIS | HIF_INCREMENTAL_ADDRESS)
#define HIF_WR_ASYNC_BLOCK_INC  \
    (HIF_WRITE | HIF_ASYNCHRONOUS | HIF_EXTENDED_IO | HIF_BLOCK_BASIS | HIF_INCREMENTAL_ADDRESS)
#define HIF_WR_SYNC_BYTE_FIX    \
    (HIF_WRITE | HIF_SYNCHRONOUS | HIF_EXTENDED_IO | HIF_BYTE_BASIS | HIF_FIXED_ADDRESS)
#define HIF_WR_SYNC_BYTE_INC    \
    (HIF_WRITE | HIF_SYNCHRONOUS | HIF_EXTENDED_IO | HIF_BYTE_BASIS | HIF_INCREMENTAL_ADDRESS)
#define HIF_WR_SYNC_BLOCK_INC  \
    (HIF_WRITE | HIF_SYNCHRONOUS | HIF_EXTENDED_IO | HIF_BLOCK_BASIS | HIF_INCREMENTAL_ADDRESS)
#define HIF_RD_SYNC_BYTE_INC    \
    (HIF_READ | HIF_SYNCHRONOUS | HIF_EXTENDED_IO | HIF_BYTE_BASIS | HIF_INCREMENTAL_ADDRESS)
#define HIF_RD_SYNC_BYTE_FIX    \
    (HIF_READ | HIF_SYNCHRONOUS | HIF_EXTENDED_IO | HIF_BYTE_BASIS | HIF_FIXED_ADDRESS)
#define HIF_RD_ASYNC_BYTE_FIX   \
    (HIF_READ | HIF_ASYNCHRONOUS | HIF_EXTENDED_IO | HIF_BYTE_BASIS | HIF_FIXED_ADDRESS)
#define HIF_RD_ASYNC_BLOCK_FIX  \
    (HIF_READ | HIF_ASYNCHRONOUS | HIF_EXTENDED_IO | HIF_BLOCK_BASIS | HIF_FIXED_ADDRESS)
#define HIF_RD_ASYNC_BYTE_INC   \
    (HIF_READ | HIF_ASYNCHRONOUS | HIF_EXTENDED_IO | HIF_BYTE_BASIS | HIF_INCREMENTAL_ADDRESS)
#define HIF_RD_ASYNC_BLOCK_INC  \
    (HIF_READ | HIF_ASYNCHRONOUS | HIF_EXTENDED_IO | HIF_BLOCK_BASIS | HIF_INCREMENTAL_ADDRESS)
#define HIF_RD_SYNC_BLOCK_INC  \
    (HIF_READ | HIF_SYNCHRONOUS | HIF_EXTENDED_IO | HIF_BLOCK_BASIS | HIF_INCREMENTAL_ADDRESS)


typedef enum {
    HIF_DEVICE_POWER_STATE = 0,
    HIF_DEVICE_GET_MBOX_BLOCK_SIZE,
    HIF_DEVICE_GET_MBOX_ADDR,
    HIF_DEVICE_GET_PENDING_EVENTS_FUNC,
    HIF_DEVICE_GET_IRQ_PROC_MODE,
    HIF_DEVICE_GET_RECV_EVENT_MASK_UNMASK_FUNC,
} HIF_DEVICE_CONFIG_OPCODE;

/*
 * HIF CONFIGURE definitions:
 *
 *   HIF_DEVICE_GET_MBOX_BLOCK_SIZE
 *   input : none
 *   output : array of 4 A_UINT32s
 *   notes: block size is returned for each mailbox (4)
 *
 *   HIF_DEVICE_GET_MBOX_ADDR
 *   input : none
 *   output : array of 4 A_UINT32
 *   notes: address is returned for each mailbox (4) in the array
 *
 *   HIF_DEVICE_GET_PENDING_EVENTS_FUNC
 *   input : none
 *   output: HIF_PENDING_EVENTS_FUNC function pointer
 *   notes: this is optional for the HIF layer, if the request is
 *          not handled then it indicates that the upper layer can use
 *          the standard device methods to get pending events (IRQs, mailbox messages etc..)
 *          otherwise it can call the function pointer to check pending events.
 *
 *   HIF_DEVICE_GET_IRQ_PROC_MODE
 *   input : none
 *   output : HIF_DEVICE_IRQ_PROCESSING_MODE (interrupt processing mode)
 *   note: the hif layer interfaces with the underlying OS-specific bus driver. The HIF
 *         layer can report whether IRQ processing is requires synchronous behavior or
 *         can be processed using asynchronous bus requests (typically faster).
 *
 *   HIF_DEVICE_GET_RECV_EVENT_MASK_UNMASK_FUNC
 *   input :
 *   output : HIF_MASK_UNMASK_RECV_EVENT function pointer
 *   notes: this is optional for the HIF layer.  The HIF layer may require a special mechanism
 *          to mask receive message events.  The upper layer can call this pointer when it needs
 *          to mask/unmask receive events (in case it runs out of buffers).
 *
 *
 */

typedef enum {
    HIF_DEVICE_IRQ_SYNC_ONLY,   /* for HIF implementations that require the DSR to process all
                                   interrupts before returning */
    HIF_DEVICE_IRQ_ASYNC_SYNC,  /* for HIF implementations that allow DSR to process interrupts
                                   using ASYNC I/O (that is HIFAckInterrupt can be called at a
                                   later time */
} HIF_DEVICE_IRQ_PROCESSING_MODE;

#define HIF_MAX_DEVICES                 1

struct htc_callbacks {
    A_UCHAR *name;
    A_UINT32 id;
    A_STATUS (* deviceInsertedHandler)(void *hif_handle);
    A_STATUS (* deviceRemovedHandler)(void *htc_handle, A_STATUS status);
    A_STATUS (* deviceSuspendHandler)(void *htc_handle);
    A_STATUS (* deviceResumeHandler)(void *htc_handle);
    A_STATUS (* deviceWakeupHandler)(void *htc_handle);
    A_STATUS (* rwCompletionHandler)(void *context, A_STATUS status);
    A_STATUS (* dsrHandler)(void *htc_handle);
};


#define HIF_OTHER_EVENTS     (1 << 0)   /* other interrupts (non-Recv) are pending, host
                                           needs to read the register table to figure out what */
#define HIF_RECV_MSG_AVAIL   (1 << 1)   /* pending recv packet */

typedef struct _HIF_PENDING_EVENTS_INFO {
    A_UINT32 Events;
    A_UINT32 LookAhead;
} HIF_PENDING_EVENTS_INFO;

    /* function to get pending events , some HIF modules use special mechanisms
     * to detect packet available and other interrupts */
typedef A_STATUS ( *HIF_PENDING_EVENTS_FUNC)(HIF_DEVICE              *device,
                                             HIF_PENDING_EVENTS_INFO *pEvents,
                                             void                    *AsyncContext);

#define HIF_MASK_RECV    TRUE
#define HIF_UNMASK_RECV  FALSE
    /* function to mask recv events */
typedef A_STATUS ( *HIF_MASK_UNMASK_RECV_EVENT)(HIF_DEVICE  *device,
                                                A_BOOL      Mask,
                                                void        *AsyncContext);


/*
 * This API is used by the HTC layer to initialize the HIF layer and to
 * register different callback routines. Support for following events has
 * been captured - DSR, Read/Write completion, Device insertion/removal,
 * Device suspension/resumption/wakeup. In addition to this, the API is
 * also used to register the name and the revision of the chip. The latter
 * can be used to verify the revision of the chip read from the device
 * before reporting it to HTC.
 */
int HIFInit(HTC_CALLBACKS *callbacks);

/*
 * This API is used to provide the read/write interface over the specific bus
 * interface.
 * address - Starting address in the dragon's address space. For mailbox
 *           writes, it refers to the start of the mbox boundary. It should
 *           be ensured that the last byte falls on the mailbox's EOM. For
 *           mailbox reads, it refers to the end of the mbox boundary.
 * buffer - Pointer to the buffer containg the data to be transmitted or
 *          received.
 * length - Amount of data to be transmitted or received.
 * request - Characterizes the attributes of the command.
 */
A_STATUS
HIFReadWrite(HIF_DEVICE    *device,
             A_UINT32       address,
             A_UCHAR       *buffer,
             A_UINT32       length,
             A_UINT32       request,
             void          *context);

/*
 * This can be initiated from the unload driver context ie when the HTCShutdown
 * routine is called.
 */
void HIFShutDownDevice(HIF_DEVICE *device);

/*
 * This should translate to an acknowledgment to the bus driver indicating that
 * the previous interrupt request has been serviced and the all the relevant
 * sources have been cleared. HTC is ready to process more interrupts.
 * This should prevent the bus driver from raising an interrupt unless the
 * previous one has been serviced and acknowledged using the previous API.
 */
void HIFAckInterrupt(HIF_DEVICE *device);

void HIFMaskInterrupt(HIF_DEVICE *device);

void HIFUnMaskInterrupt(HIF_DEVICE *device);

/*
 * This set of functions are to be used by the bus driver to notify
 * the HIF module about various events.
 * These are not implemented if the bus driver provides an alternative
 * way for this notification though callbacks for instance.
 */
int HIFInsertEventNotify(void);

int HIFRemoveEventNotify(void);

int HIFIRQEventNotify(void);

int HIFRWCompleteEventNotify(void);

/*
 * This function associates a opaque handle with the HIF layer
 * to be used in communication with upper layer i.e. HTC.
 * This would normaly be a pointer to htc_target data structure.
 */
void HIFSetHandle(void *hif_handle, void *handle);

A_STATUS
HIFConfigureDevice(HIF_DEVICE *device, HIF_DEVICE_CONFIG_OPCODE opcode,
                   void *config, A_UINT32 configLen);


struct device;
struct device*
HIFGetOSDevice(HIF_DEVICE *device);


#ifdef __cplusplus
}
#endif

#endif /* _HIF_H_ */
