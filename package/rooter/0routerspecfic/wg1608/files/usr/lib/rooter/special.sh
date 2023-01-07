#!/bin/sh

LED=0
SM=$(uci get system.wifi)
if [ -z $SM ]; then
	uci set system.wifi=led
	uci set system.wifi.name="5Ghzwifi"
	uci set system.wifi.sysfs="wifi"
	uci set system.wifi.trigger="netdev"
	uci set system.wifi.dev="wlan1"
	uci set system.wifi.mode="link tx rx"
	uci commit system
	/etc/init.d/led restart
fi

