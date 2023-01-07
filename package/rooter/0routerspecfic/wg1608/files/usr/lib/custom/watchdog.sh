#!/bin/sh

dis=493
echo $dis > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio$dis/direction
echo 255 >/sys/class/gpio/gpio$dis/value

echo none > /sys/class/leds/watchdog/trigger
while [ 1 ]
do
	echo 1 >/sys/class/leds/watchdog/brightness
	sleep 1
	echo 0 >/sys/class/leds/watchdog/brightness
	sleep 1
	echo 1 >/sys/class/leds/watchdog/brightness
	sleep 1
	echo 0 >/sys/class/leds/watchdog/brightness
	sleep 1
	echo 1 >/sys/class/leds/watchdog/brightness
	sleep 1
	echo 0 >/sys/class/leds/watchdog/brightness
	sleep 5
done
