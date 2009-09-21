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


#ifndef COMMON_DRV_H_
#define COMMON_DRV_H_

#include "hif.h"
#include "htc_packet.h"



/* structure that is the state information for the default credit distribution callback
 * drivers should instantiate (zero-init as well) this structure in their driver instance
 * and pass it as a context to the HTC credit distribution functions */
typedef struct _COMMON_CREDIT_STATE_INFO {
    int TotalAvailableCredits;      /* total credits in the system at startup */
    int CurrentFreeCredits;         /* credits available in the pool that have not been
                                       given out to endpoints */
    HTC_ENDPOINT_CREDIT_DIST *pLowestPriEpDist;  /* pointer to the lowest priority endpoint dist struct */
} COMMON_CREDIT_STATE_INFO;


/* HTC TX packet tagging definitions */
#define AR6K_CONTROL_PKT_TAG    HTC_TX_PACKET_TAG_USER_DEFINED
#define AR6K_DATA_PKT_TAG       (AR6K_CONTROL_PKT_TAG + 1)

#ifdef __cplusplus
extern "C" {
#endif

/* OS-independent APIs */
A_STATUS ar6000_setup_credit_dist(HTC_HANDLE HTCHandle, COMMON_CREDIT_STATE_INFO *pCredInfo);
A_STATUS ar6000_ReadRegDiag(HIF_DEVICE *hifDevice, A_UINT32 *address, A_UINT32 *data);
A_STATUS ar6000_WriteRegDiag(HIF_DEVICE *hifDevice, A_UINT32 *address, A_UINT32 *data);
A_STATUS ar6000_ReadDataDiag(HIF_DEVICE *hifDevice, A_UINT32 address,  A_UCHAR *data, A_UINT32 length);
A_STATUS ar6000_reset_device(HIF_DEVICE *hifDevice, A_UINT32 TargetType);
void ar6000_dump_target_assert_info(HIF_DEVICE *hifDevice, A_UINT32 TargetType);
A_STATUS ar6000_reset_device_skipflash(HIF_DEVICE *hifDevice);

#ifdef __cplusplus
}
#endif

#endif /*COMMON_DRV_H_*/
