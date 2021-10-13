#!/bin/sh

timeout=$1

/etc/init.d/led blink
sleep $1
/etc/init.d/led turnon
exit 0
