/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/
#ifndef __BCL_SENSOR_H__
#define __BCL_SENSOR_H__

#include "thermal.h"

int bcl_setup(sensor_setting_t *settings, sensor_t * sensor);
void bcl_shutdown(sensor_setting_t *setting);
int bcl_get_imax(sensor_setting_t *setting);
int bcl_get_ibat(sensor_setting_t *setting);
int bcl_get_diff_imax_ibat(sensor_setting_t *setting);
void bcl_enable(sensor_setting_t *setting, int enabled);
void bcl_interrupt_wait(sensor_setting_t *setting);
void bcl_update_thresholds(sensor_setting_t *setting, int threshold_triggered, int level);

#endif  /* __BCL_SENSOR_H__ */
