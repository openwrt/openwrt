#ifndef _BMI_H_
#define _BMI_H_
/*
 * Copyright (c) 2004-2005 Atheros Communications Inc.
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
 * BMI declarations and prototypes
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Header files */
#include "a_config.h"
#include "athdefs.h"
#include "a_types.h"
#include "hif.h"
#include "a_osapi.h"
#include "bmi_msg.h"

void
BMIInit(void);

A_STATUS
BMIDone(HIF_DEVICE *device);

A_STATUS
BMIGetTargetInfo(HIF_DEVICE *device, struct bmi_target_info *targ_info);

A_STATUS
BMIReadMemory(HIF_DEVICE *device,
              A_UINT32 address,
              A_UCHAR *buffer,
              A_UINT32 length);

A_STATUS
BMIWriteMemory(HIF_DEVICE *device,
               A_UINT32 address,
               A_UCHAR *buffer,
               A_UINT32 length);

A_STATUS
BMIExecute(HIF_DEVICE *device,
           A_UINT32 address,
           A_UINT32 *param);

A_STATUS
BMISetAppStart(HIF_DEVICE *device,
               A_UINT32 address);

A_STATUS
BMIReadSOCRegister(HIF_DEVICE *device,
                   A_UINT32 address,
                   A_UINT32 *param);

A_STATUS
BMIWriteSOCRegister(HIF_DEVICE *device,
                    A_UINT32 address,
                    A_UINT32 param);

A_STATUS
BMIrompatchInstall(HIF_DEVICE *device,
                   A_UINT32 ROM_addr,
                   A_UINT32 RAM_addr,
                   A_UINT32 nbytes,
                   A_UINT32 do_activate,
                   A_UINT32 *patch_id);

A_STATUS
BMIrompatchUninstall(HIF_DEVICE *device,
                     A_UINT32 rompatch_id);

A_STATUS
BMIrompatchActivate(HIF_DEVICE *device,
                    A_UINT32 rompatch_count,
                    A_UINT32 *rompatch_list);

A_STATUS
BMIrompatchDeactivate(HIF_DEVICE *device,
                      A_UINT32 rompatch_count,
                      A_UINT32 *rompatch_list);

#ifdef __cplusplus
}
#endif

#endif /* _BMI_H_ */
