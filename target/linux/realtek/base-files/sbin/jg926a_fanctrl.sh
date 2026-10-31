#!/bin/sh

# HPE 1920-24G-PoE+ 370W (JG926A) fan control script 

# Find PWM file - this check must run since the location will vary based on the number of SFP transcievers in use
PWM=$(find /sys/devices/platform/gpio_fan_array/hwmon/hwmon[0-9] -name "pwm1" 2>/dev/null)

if [ -f "$PWM" ]; then
    logger -t fan.control $(echo Found PWM Input at $PWM)
else
    logger -t fan.control PWM File not found. Fans will remain running high. Script Exiting.
    exit 0
fi

#Find fault indicator - this check must run since the location will vary based on the number of SFP transcievers in use
FAULT=$(find /sys/devices/platform/gpio_fan_array/hwmon/hwmon[0-9] -name "fan1_alarm" 2>/dev/null)
if [ -f "$FAULT" ]; then
    logger -t fan.control $(echo Found Fan Failure Indicator at $FAULT)
else
    logger -t fan.control Fan Failure Indicator not found. Fans will remain running high. Script Exiting
    exit 0
fi

TEMP_HIGH=65
LOAD_HIGH=300

while true; do
TEMP=$(cut -c1-2 /sys/class/hwmon/hwmon0/temp1_input)
LOAD=$(ubus call poe info | grep consumption | head -n 1 | awk -F'"consumption": ' '{print $2}' | awk -F',' '{printf "%.1f\n", $1}')
FAULT=$(find /sys/devices/platform/gpio_fan_array/hwmon/hwmon[0-9] -name "fan1_alarm" 2>/dev/null)
if [ $FAULT -eq 1 ]; then
    echo 100 > $PWM
    logger -t fan.control FAN FAULT DETECTED! FANS SET TO HIGH! FAN CONTROL SCRIPT TERMINATING! 
    exit 0
elif [ $TEMP -gt $TEMP_HIGH -o $LOAD -gt $LOAD_HIGH ]; then
    echo 100 > $PWM
else
    echo 0 > $PWM
fi
sleep 15
done
