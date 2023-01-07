#!/bin/sh

#for example WE825-Q watchdog gpio is 2
wd_gpio="2"

echo $wd_gpio > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio$wd_gpio/direction

while [ 1 ]
do
    echo 1 >/sys/class/gpio/gpio$wd_gpio/value
    sleep 1
    echo 0 >/sys/class/gpio/gpio$wd_gpio/value
    sleep 1
	
	echo 1 >/sys/class/gpio/gpio$wd_gpio/value
    sleep 1
    echo 0 >/sys/class/gpio/gpio$wd_gpio/value
    sleep 1
	
	echo 1 >/sys/class/gpio/gpio$wd_gpio/value
    sleep 1
    echo 0 >/sys/class/gpio/gpio$wd_gpio/value
    sleep 1
done
