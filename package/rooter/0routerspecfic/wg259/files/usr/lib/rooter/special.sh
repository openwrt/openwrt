#!/bin/sh

SM=$(uci get system.4g5g)
if [ -z $SM ]; then
	uci set system.4g5g=led
	uci set system.4g5g.name="4G5G"
	uci set system.4g5g.sysfs="green:usb"
	uci set system.4g5g.trigger="netdev"
	uci set system.4g5g.dev="wwan0"
	uci set system.4g5g.mode="link tx rx"
	uci set system.4g5g.default='0'
	
	uci set system.sys=led
	uci set system.sys.name="SYS"
	uci set system.sys.sysfs="green:status"
	uci set system.sys.trigger="netdev"
	uci set system.sys.dev="br-lan"
	uci set system.sys.mode="link tx rx"
	uci set system.sys.default='0'

	uci commit system
	/etc/init.d/led restart
fi
