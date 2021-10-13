/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/
#ifndef __TSENS_SENSOR_H__
#define __TSENS_SENSOR_H__

#include "thermal.h"

int tsens_sensors_setup(sensor_setting_t *settings, sensor_t * sensor);
void tsens_sensors_shutdown(sensor_setting_t *setting);
int tsens_sensor_get_temperature(sensor_setting_t *setting);
void tsens_sensor_interrupt_wait(sensor_setting_t *setting);
void tsens_sensor_update_thresholds(sensor_setting_t *setting, int threshold_type, int level);
void tsens_sensor_enable_thresholds(sensor_setting_t *setting, int hi_enabled, int lo_enabled);

#ifndef IPQ_806x
void tsens_sensor_enable_sensor(sensor_setting_t *setting, int enabled);
#else
void tsens_sensor_enable_sensor(int tzn, int enabled);
#endif

#endif  /* __TSENS_SENSOR_H__ */
