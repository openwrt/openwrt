/*===========================================================================

  Copyright (c) 2011-2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

#ifndef __SENSORS_HW_H__
#define __SENSORS_HW_H__

#define SENSOR_NAME_MAX      (20)
#define SENSOR(x)     (((x >= 0) && (x < SENSOR_IDX_MAX)) ? \
				sensor_names[x] : "unknown")

enum sensor_list {
#ifdef SENSORS_7630
	PMIC_THERM,
	XO_THERM,
	XO_THERM_GPS,
	POP_MEM_TZ,
#endif /* SENSORS_7630 */

#ifdef SENSORS_8660
	PMIC8058_TZ,
	TSENS_TZ_SENSOR0,
#endif /* SENSORS_8660 */

#ifdef SENSORS_8960
	PM8921_TZ,
	PA_THERM0,
	PA_THERM1,
	TSENS_TZ_SENSOR0,
	TSENS_TZ_SENSOR1,
	TSENS_TZ_SENSOR2,
	TSENS_TZ_SENSOR3,
	TSENS_TZ_SENSOR4,
	TSENS_TZ_SENSOR5,
	TSENS_TZ_SENSOR6,
	TSENS_TZ_SENSOR7,
	TSENS_TZ_SENSOR8,
	TSENS_TZ_SENSOR9,
	TSENS_TZ_SENSOR10,
	BCL,
	PM8821_TZ,
#endif /* SENSORS_8960 */

#ifdef SENSORS_8974
	TSENS_TZ_SENSOR0,
	TSENS_TZ_SENSOR1,
	TSENS_TZ_SENSOR2,
	TSENS_TZ_SENSOR3,
	TSENS_TZ_SENSOR4,
	TSENS_TZ_SENSOR5,
	TSENS_TZ_SENSOR6,
	TSENS_TZ_SENSOR7,
	TSENS_TZ_SENSOR8,
	TSENS_TZ_SENSOR9,
	TSENS_TZ_SENSOR10,
	BCL,
#endif /* SENSORS_8974 */

#ifdef SENSORS_TEST
	TEST0,
#endif /* SENSORS_TEST */
#ifdef SENSORS_8625
	PMIC_THERM,
	MSM_THERM,
#endif

#ifdef IPQ_806x
        TSENS_TZ_SENSOR0,
        TSENS_TZ_SENSOR1,
        TSENS_TZ_SENSOR2,
        TSENS_TZ_SENSOR3,
        TSENS_TZ_SENSOR4,
        TSENS_TZ_SENSOR5,
        TSENS_TZ_SENSOR6,
        TSENS_TZ_SENSOR7,
        TSENS_TZ_SENSOR8,
        TSENS_TZ_SENSOR9,
        TSENS_TZ_SENSOR10,
	TSENS_TZ_SENSOR11,
	TSENS_TZ_SENSOR12,
	TSENS_TZ_SENSOR13,
	TSENS_TZ_SENSOR14,
	TSENS_TZ_SENSOR15,
#endif /*IPQ_806x */

	SENSOR_IDX_MAX
};

extern char *sensor_names[]; /* Sensor names are defined in sensors-*.c */

#endif  /* __SENSORS_HW_H__ */

