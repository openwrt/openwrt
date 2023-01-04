#!/bin/sh

CPU_TEMP=$(cat /sys/class/thermal/thermal_zone0/temp)
WIFI0_TEMP=$(cat /sys/class/hwmon/hwmon0/temp1_input 2>/dev/null) || WIFI0_TEMP=0
WIFI1_TEMP=$(cat /sys/class/hwmon/hwmon1/temp1_input 2>/dev/null) || WIFI1_TEMP=0
WIFI2_TEMP=$(cat /sys/class/hwmon/hwmon2/temp1_input 2>/dev/null) || WIFI2_TEMP=0

CPU_LOW=65000
CPU_HIGH=70000
WIFI_LOW=65000
WIFI_HIGH=70000

if [ -d /sys/class/gpio/fan/value ];then
	FAN_CTRL=/sys/class/gpio/fan/value
else
	exit 0
fi

if [ "$CPU_TEMP" -ge "$CPU_HIGH" -o "$WIFI0_TEMP" -ge "$WIFI0_HIGH" -o "$WIFI1_TEMP" -ge "$WIFI_HIGH" -o "$WIFI2_TEMP" -ge "$WIFI_HIGH" ];then
	echo "1" > $FAN_CTRL
elif [ "$CPU_TEMP" -lt "$CPU_LOW1" -o "$WIFI0_TEMP" -lt "$WIFI0_LOW" -o "$WIFI1_TEMP" -lt "$WIFI_LOW" -o "$WIFI2_TEMP" -lt "$WIFI_LOW" ];then
	echo "0" > $FAN_CTRL
fi
