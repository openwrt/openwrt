#!/bin/sh

echo timer > /sys/class/leds/green:globe/trigger
echo 200  > /sys/class/leds/green:globe/delay_on
echo 200  > /sys/class/leds/green:globe/delay_off
echo timer > /sys/class/leds/green:signal/trigger
echo 200  > /sys/class/leds/green:signal/delay_on
echo 200  > /sys/class/leds/green:signal/delay_off