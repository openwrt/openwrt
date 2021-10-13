/*===========================================================================

  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#ifndef __ADC_SENSOR_H__
#define __ADC_SENSOR_H__

#include "thermal.h"

int adc_sensors_setup(sensor_setting_t *settings, sensor_t * sensor);
void adc_sensors_shutdown(sensor_setting_t *setting);
int adc_sensor_get_temperature(sensor_setting_t *setting);

#endif  /* __ADC_SENSOR_H__ */
