/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#ifndef __GEN_SENSOR_H__
#define __GEN_SENSOR_H__

#include "thermal.h"

int gen_sensors_setup(sensor_setting_t *settings, sensor_t * sensor);
void gen_sensors_shutdown(sensor_setting_t *setting);
int gen_sensor_get_temperature(sensor_setting_t *setting);

#endif  /* __GEN_SENSOR_H__ */
