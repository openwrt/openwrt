/*===========================================================================

Copyright (c) 2011-2012 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

Thermal Daemon
~~~~~~~~~~~~~~

This daemon monitors thermal/temperature sensor data and performs
actions based on a configuration file (default is /etc/thermald.conf).

A sample configuration file is provided (thermald.conf_sample).



Configuration file format:
~~~~~~~~~~~~~~~~~~~~~~~~~~
{debug}
sampling <default sampling rate in ms>

[<temperature sensor section>]
sampling       <sampling rate in ms>
thresholds     <threshold values in degree Celsius> ...
thresholds_clr <temperature value to clear thresholds> ...
actions        <action to perform at threshold;
                multiple actions separated by '+'> ...
action_info    <additional info for action;
                multiple action_info separated by '+'> ...

* Optional 'debug' flag determines if debug logging is enabled.
* 'thresholds'/'thresholds_clr'/'actions'/'action_info' accepts a list of
space separated values for up to 8 thresholds.

'actions' field
---------------
* 'none' ;
- ACTION - Do nothing
- ACTION_INFO - ignored

* 'report'
- ACTION - Report threshold crossing to UI
- ACTION_INFO - ignored
- NOTES - The threshold crossing information is sent across an
          abstract local socket "THERMALD_UI" in newline-separated
          string format.  Report action runs upon clearing or
	  triggering a level, slightly different from the other
	  actions which are run upon reaching the level, e.g.
	  clearing level n+1 or triggering level n.

	  Parameters are sent in the following order.
          * sensorname - Name of sensor reporting
          * temperature - Current temperature
          * current_threshold_level - current threshold level triggered or cleared
          * is_trigger - "true" on level trigger, "false" on level clearing

	  Example Android code to listen to this notification is
          provided in the documentation folder.

* 'cpu'
- ACTION - CPU frequency scaling
- ACTION_INFO - Max CPU frequency in KHz

* 'lcd'
- ACTION - LCD brightness throttling
- ACTION_INFO - 0-255 value for max LCD brightness

* 'modem'
- ACTION - Request throttling of modem functionality
- ACTION_INFO - 0-3 throttling level for modem mitigation

* 'fusion'
- ACTION - Request throttling of fusion modem functionality
- ACTION_INFO - 0-3 throttling level for fusion modem mitigation

* 'battery'
- ACTION - Battery charging current throttling
- ACTION_INFO - 0-3 throttling level for battery charging current

* 'gpu'
- ACTION - GPU frequency scaling
- ACTION_INFO - Max GPU frequency in Hz

* 'wlan'
- ACTION - WLAN throttling
- ACTION_INFO - 0-4 throttling level for WLAN mitigation

* 'camera'
- ACTION - CAMERA throttling
- ACTION_INFO - 0-3 throttling level for CAMERA mitigation

* 'camcorder'
- ACTION - CAMCORDER throttling
- ACTION_INFO - 0-3 throttling level for CAMCORDER mitigation

* 'shutdown'
- ACTION - Shutdown target
- ACTION_INFO - Shutdown delay in ms

Example 1:
----------
sampling         1000

[PMIC_THERM]
sampling         5000
thresholds       40.2        45      50
thresholds_clr   38          43      48
actions          cpu+report  cpu     cpu
action_info      1188000+0    368640  245760

Description:
1) Default sampling rate of 1 second.
For sensor 'PMIC_THERM', sample at 5 second rate (overrides default):
2) Threshold level 1 triggered at 40.2 deg C; clears if temperature drops
   to/below 38 deg C.  When triggered, adjust maximum allowed CPU to
   1188000 KHz, which in this example is already the current maximum
   frequency, and results in no action.
   Also generate a report, action_info value 0 is ignored.
3) Threshold level 2 triggered at 45 deg C; clears if temperature drops
   to/below 43 deg C.  When triggered, adjust maximum allowed CPU to
   368640 KHz.
4) When threshold level 2 cleared at 43 deg C; adjust CPU back to maximum
   frequency 1188000 KHz.
5) When threshold level 1 cleared at 38 deg C; generate a report for level 1
   clearing, all mitigation is reset.

Example 2:
----------
debug
sampling         2000

[PMIC_THERM]
sampling         5000
thresholds       40.2          45      50
thresholds_clr   38            43      48
actions          cpu+report    cpu     report+shutdown
action_info      768000+0      368640  0+6000

Description:
1) Debug logging output is enabled.
2) Default sampling rate of 2 second.
For sensor 'PMIC_THERM', sample at 5 second rate (overrides default):
3) Threshold level 1 triggered at 40.2 deg C; clears if temperature drops
   to/below 38 deg C.  When triggered, adjust maximum allowed CPU to 768000 KHz,
   and generate a report (action_info value 0 is ignored).
4) Threshold level 2 triggered at 45 deg C; clears if temperature drops
   to/below 43 deg C.  When triggered, adjust maximum allowed CPU to 368640 KHz.
5) Threshold level 3 triggered at 50 deg C; clears if temperature drops
   to/below 48 deg C. When triggered, generate a report and shutdown the
   target after a delay of 6 seconds.

Example 3:
----------
debug
sampling         2000

[bcl]
sampling         1000
thresholds       100      0
thresholds_clr   500      100
actions          report   report
action_info      0        0

Description:
1) Debug logging output is enabled.
2) Default sampling rate of 2 second.
For battery current limit 'bcl', sample at 1 second rate (overrides default):
3) Threshold level 1 triggered if ibat is at (imax - 100mA); clears if ibat drops
   to/below (imax - 500mA).  When triggered, generate a report (action_info value 0 is ignored).
4) Threshold level 2 triggered if ibat is at imax; clears if ibat drops
   to/below (imax - 100mA).  When triggered, generate a report (action_info value 0 is ignored).

