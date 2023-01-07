#!/bin/sh 
. /lib/functions.sh

MODEM_DEVICES=`cat /sys/kernel/debug/usb/devices | grep 'Sierra' -A3 | grep '^C:' | awk '{print $3}'`
if [ $MODEM_DEVICES -eq 1 ]; then
    echo "Sierra Modem in BOOTHOLD! Resetting pcie0_vcc state." > /dev/kmsg
    echo "0" > /sys/class/gpio/gpio9/value
    echo "Toggled pcie0_vcc: OFF" > /dev/kmsg
    echo "1" > /sys/class/gpio/gpio9/value
    echo "Toggled pcie0_vcc: ON" > /dev/kmsg
fi