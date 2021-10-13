/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/
#ifndef __PM8821_SENSOR_H__
#define __PM8821_SENSOR_H__

#include "thermal.h"

int pm8821_setup(sensor_setting_t *setting, sensor_t *sensor);
void pm8821_shutdown(sensor_setting_t *setting);
int pm8821_get_temperature(sensor_setting_t *setting);
void pm8821_interrupt_wait(sensor_setting_t *setting);

extern hotplug_map_t *hotplug_map;

#endif  /* __PM8821_SENSOR_H__ */
