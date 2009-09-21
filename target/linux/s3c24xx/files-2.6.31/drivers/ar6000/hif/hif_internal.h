/*
 * @file: hif_internal.h
 *
 * @abstract: internal header file for hif layer
 *
 * @notice: Copyright (c) 2004-2006 Atheros Communications Inc.
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

#include <linux/sdio/ctsystem.h>
#include <linux/sdio/sdio_busdriver.h>
#include <linux/sdio/_sdio_defs.h>
#include <linux/sdio/sdio_lib.h>
#include "a_config.h"
#include "athdefs.h"
#include "a_types.h"
#include "a_osapi.h"
#include "hif.h"

#define MANUFACTURER_ID_AR6001_BASE        0x100
#define MANUFACTURER_ID_AR6002_BASE        0x200
#define FUNCTION_CLASS                     0x0
#define MANUFACTURER_CODE                  0x271

#define BUS_REQUEST_MAX_NUM                64

#define SDIO_CLOCK_FREQUENCY_DEFAULT       25000000
#define SDWLAN_ENABLE_DISABLE_TIMEOUT      20
#define FLAGS_CARD_ENAB                    0x02
#define FLAGS_CARD_IRQ_UNMSK               0x04

#define HIF_MBOX_BLOCK_SIZE                128
#define HIF_MBOX_BASE_ADDR                 0x800
#define HIF_MBOX_WIDTH                     0x800
#define HIF_MBOX0_BLOCK_SIZE               1
#define HIF_MBOX1_BLOCK_SIZE               HIF_MBOX_BLOCK_SIZE
#define HIF_MBOX2_BLOCK_SIZE               HIF_MBOX_BLOCK_SIZE
#define HIF_MBOX3_BLOCK_SIZE               HIF_MBOX_BLOCK_SIZE

#define HIF_MBOX_START_ADDR(mbox)                        \
    HIF_MBOX_BASE_ADDR + mbox * HIF_MBOX_WIDTH

#define HIF_MBOX_END_ADDR(mbox)	                         \
    HIF_MBOX_START_ADDR(mbox) + HIF_MBOX_WIDTH - 1

struct hif_device {
    SDDEVICE *handle;
    void *htc_handle;
    OSKERNEL_HELPER insert_helper;
    BOOL  helper_started;
};

typedef struct target_function_context {
    SDFUNCTION           function; /* function description of the bus driver */
    OS_SEMAPHORE         instanceSem; /* instance lock. Unused */
    SDLIST               instanceList; /* list of instances. Unused */
} TARGET_FUNCTION_CONTEXT;

typedef struct bus_request {
    struct bus_request *next;
    SDREQUEST *request;
    void *context;
} BUS_REQUEST;

BOOL
hifDeviceInserted(SDFUNCTION *function, SDDEVICE *device);

void
hifDeviceRemoved(SDFUNCTION *function, SDDEVICE *device);

SDREQUEST *
hifAllocateDeviceRequest(SDDEVICE *device);

void
hifFreeDeviceRequest(SDREQUEST *request);

void
hifRWCompletionHandler(SDREQUEST *request);

void
hifIRQHandler(void *context);

HIF_DEVICE *
addHifDevice(SDDEVICE *handle);

HIF_DEVICE *
getHifDevice(SDDEVICE *handle);

void
delHifDevice(SDDEVICE *handle);
