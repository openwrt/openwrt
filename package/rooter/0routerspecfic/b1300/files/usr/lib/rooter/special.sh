#!/bin/sh

LED=0
SM=$(uci get system.led_wifi0)
if [ -z $SM ]; then
	uci set system.led_wifi0=led
	uci set system.led_wifi0.default="0"  
	uci set system.led_wifi0.name="WIFI0"
	uci set system.led_wifi0.sysfs="b1300:green:wlan"
	uci set system.led_wifi0.trigger="netdev"
	uci set system.led_wifi0.dev="wlan0"
	uci set system.led_wifi0.mode="link tx rx"
	LED=1
fi
SM=$(uci get system.led_wifi1)
if [ -z $SM ]; then
	uci set system.led_wifi1=led
	uci set system.led_wifi1.default="0"  
	uci set system.led_wifi1.name="WIFI1"
	uci set system.led_wifi1.sysfs="b1300:green:wlan"
	uci set system.led_wifi1.trigger="netdev"
	uci set system.led_wifi1.dev="wlan1"
	uci set system.led_wifi1.mode="link tx rx"
	LED=1
fi

if [ $LED -eq 1 ]; then
	uci commit system
	/etc/init.d/led restart
fi
