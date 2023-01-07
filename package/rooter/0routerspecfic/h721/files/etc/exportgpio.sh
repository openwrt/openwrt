#!/bin/sh

for i in 502 503 
do
echo $i > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio${i}/direction
echo 1  > /sys/class/gpio/gpio${i}/value
done


sleep 1

for i in 502 503
do
echo 0 > /sys/class/gpio/gpio${i}/value
sleep 8
done

echo timer > /sys/class/leds/h761:green:4g5g/trigger
echo 0  > /sys/class/leds/h761:green:4g5g/delay_on
echo 1000  > /sys/class/leds/h761:green:4g5g/delay_off
echo timer > /sys/class/leds/h761:green:4g/trigger
echo 0  > /sys/class/leds/h761:green:4g/delay_on
echo 1000  > /sys/class/leds/h761:green:4g/delay_off
