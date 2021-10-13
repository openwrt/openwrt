/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#ifndef __QMI_TS_SENSOR_H__
#define __QMI_TS_SENSOR_H__

#include "thermal.h"

int  qmi_ts_setup(sensor_setting_t *settings, sensor_t * sensor);
void qmi_ts_shutdown(sensor_setting_t *setting);
int  qmi_ts_get_temperature(sensor_setting_t *setting);
void qmi_ts_interrupt_wait(sensor_setting_t *setting);
void qmi_ts_update_thresholds(sensor_setting_t *setting, int threshold_type, int level);
void qmi_ts_update_temperature(const char *sensor, int temperature);

#endif  /* __QMI_TS_SENSOR_H__ */
