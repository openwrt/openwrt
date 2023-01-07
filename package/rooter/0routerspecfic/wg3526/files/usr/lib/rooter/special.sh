#!/bin/sh


echo none > /sys/class/leds/green:status/trigger
echo 0  > /sys/class/leds/green:status/brightness

SM=$(uci get system.4g5g)
if [ -z $SM ]; then
	uci set system.4g5g=led
	uci set system.4g5g.name="4G5G"
	uci set system.4g5g.sysfs="green:status"
	uci set system.4g5g.trigger="netdev"
	uci set system.4g5g.dev="wwan0"
	uci set system.4g5g.mode="link tx rx"
	uci set system.4g5g.default='0'

	uci commit system
	/etc/init.d/led restart
fi